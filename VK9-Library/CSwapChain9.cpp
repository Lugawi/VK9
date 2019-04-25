/*
Copyright(c) 2016-2019 Christopher Joseph Dean Schaefer

This software is provided 'as-is', without any express or implied
warranty.In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions :

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "CSwapChain9.h"
#include "CSurface9.h"
#include "CDevice9.h"
#include "C9.h"
#include "LogManager.h"
//#include "PrivateTypes.h"

#include <math.h>
#include <algorithm>

CSwapChain9::CSwapChain9(CDevice9* Device, D3DPRESENT_PARAMETERS *pPresentationParameters)
	: mDevice(Device)
{
	Log(info) << "CSwapChain9::CSwapChain9" << std::endl;

	if (pPresentationParameters != nullptr)
	{
		if (pPresentationParameters->BackBufferFormat == D3DFMT_UNKNOWN)
		{
			pPresentationParameters->BackBufferFormat = D3DFMT_X8R8G8B8;
		}
		else if (pPresentationParameters->BackBufferFormat == D3DFMT_A8R8G8B8)
		{
			pPresentationParameters->BackBufferFormat = D3DFMT_X8R8G8B8;
		}
		memcpy(&mPresentationParameters, pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS));
	}

	auto const createInfo = vk::Win32SurfaceCreateInfoKHR().setHinstance(GetModuleHandle(nullptr)).setHwnd(mPresentationParameters.hDeviceWindow);
	mSurface = mDevice->mC9->mInstance->createWin32SurfaceKHRUnique(createInfo);

	/*
	Now we need to find a queue.
	We would prefer to get a queue that can do graphics and presentation so that is what we'll search for.
	This requires us to loop over all of the queue properties and check them.
	*/
	mPresentQueueFamilyIndex = mDevice->mC9->mPhysicalDevices[mDevice->mC9->mPhysicalDeviceIndex].getSurfaceSupportKHR(static_cast<uint32_t>(mDevice->mC9->mGraphicsQueueFamilyIndex), mSurface.get()) ? mDevice->mC9->mGraphicsQueueFamilyIndex : mDevice->mC9->mQueueFamilyProperties.size();
	if (mPresentQueueFamilyIndex == mDevice->mC9->mQueueFamilyProperties.size())
	{
		//No present support here move along.
		for (int32_t i = 0; i < (int32_t)mDevice->mC9->mQueueFamilyProperties.size(); i++)
		{
			if ((mDevice->mC9->mQueueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) && mDevice->mC9->mPhysicalDevices[mDevice->mC9->mPhysicalDeviceIndex].getSurfaceSupportKHR(static_cast<uint32_t>(i), mSurface.get()))
			{
				mPresentQueueFamilyIndex = i;
				break;
			}
		}
		if (mPresentQueueFamilyIndex == mDevice->mC9->mQueueFamilyProperties.size())
		{
			//Couldn't find graphics and present so we'll just grab two different queue families.
			for (int32_t i = 0; i < (int32_t)mDevice->mC9->mQueueFamilyProperties.size(); i++)
			{
				if (mDevice->mC9->mPhysicalDevices[mDevice->mC9->mPhysicalDeviceIndex].getSurfaceSupportKHR(static_cast<uint32_t>(i), mSurface.get()))
				{
					mPresentQueueFamilyIndex = i;
					break;
				}
			}
		}
	}

	mFormats = mDevice->mC9->mPhysicalDevices[mDevice->mC9->mPhysicalDeviceIndex].getSurfaceFormatsKHR(mSurface.get());
	mSurfaceCapabilities = mDevice->mC9->mPhysicalDevices[mDevice->mC9->mPhysicalDeviceIndex].getSurfaceCapabilitiesKHR(mSurface.get());

	if (mSurfaceCapabilities.currentExtent.width == std::numeric_limits<uint32_t>::max())
	{
		// If the surface size is undefined, the size is set to the size of the images requested.
		mSwapchainExtent.width = std::clamp(mPresentationParameters.BackBufferWidth, mSurfaceCapabilities.minImageExtent.width, mSurfaceCapabilities.maxImageExtent.width);
		mSwapchainExtent.height = std::clamp(mPresentationParameters.BackBufferHeight, mSurfaceCapabilities.minImageExtent.height, mSurfaceCapabilities.maxImageExtent.height);
	}
	else
	{
		// If the surface size is defined, the swap chain size must match
		mSwapchainExtent = mSurfaceCapabilities.currentExtent;
	}

	/*
	Now we need to figure out what presentation mode to use. We'll fall back to Fifo if there is nothing better.
	Mailbox is prefered but immediate is second best.
	If the parameters say to use V-Sync then just roll with FIFO regardless of what is available.
	*/
	mSwapchainPresentMode = vk::PresentModeKHR::eFifo;
	bool useVsync = !(mPresentationParameters.PresentationInterval == D3DPRESENT_DONOTWAIT || mPresentationParameters.PresentationInterval == D3DPRESENT_FORCEIMMEDIATE || mPresentationParameters.PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE);
	if (!useVsync)
	{
		auto presentModes = mDevice->mC9->mPhysicalDevices[mDevice->mC9->mPhysicalDeviceIndex].getSurfacePresentModesKHR(mSurface.get());
		for (auto& presentMode : presentModes)
		{
			if (presentMode == vk::PresentModeKHR::eMailbox)
			{
				mSwapchainPresentMode = vk::PresentModeKHR::eMailbox;
				break;
			}
			else if (presentMode == vk::PresentModeKHR::eImmediate)
			{
				mSwapchainPresentMode = vk::PresentModeKHR::eImmediate;
			}
		}
	}

	vk::SurfaceTransformFlagBitsKHR preTransform = (mSurfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) ? vk::SurfaceTransformFlagBitsKHR::eIdentity : mSurfaceCapabilities.currentTransform;

	vk::CompositeAlphaFlagBitsKHR compositeAlpha =
		(mSurfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied :
		(mSurfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied :
		(mSurfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit) ? vk::CompositeAlphaFlagBitsKHR::eInherit : vk::CompositeAlphaFlagBitsKHR::eOpaque;

	vk::SwapchainCreateInfoKHR swapChainCreateInfo(vk::SwapchainCreateFlagsKHR(), mSurface.get(), mSurfaceCapabilities.minImageCount, ConvertFormat(mPresentationParameters.BackBufferFormat), vk::ColorSpaceKHR::eSrgbNonlinear,
		mSwapchainExtent, 1, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive, 0, nullptr, preTransform, compositeAlpha, mSwapchainPresentMode, true, nullptr);

	uint32_t queueFamilyIndices[2] = { static_cast<uint32_t>(mDevice->mC9->mGraphicsQueueFamilyIndex), static_cast<uint32_t>(mPresentQueueFamilyIndex) };
	if (mDevice->mC9->mGraphicsQueueFamilyIndex != mPresentQueueFamilyIndex)
	{
		// If the graphics and present queues are from different queue families, we either have to explicitly transfer ownership of images between
		// the queues, or we have to create the swapchain with imageSharingMode as VK_SHARING_MODE_CONCURRENT
		swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		swapChainCreateInfo.queueFamilyIndexCount = 2;
		swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;

		mDevice->mDevice->getQueue(static_cast<uint32_t>(mPresentQueueFamilyIndex), 0, &mQueue);
	}
	else
	{
		mQueue = mDevice->mQueue;
	}

	mSwapChain = mDevice->mDevice->createSwapchainKHRUnique(swapChainCreateInfo);
	mSwapChainImages = mDevice->mDevice->getSwapchainImagesKHR(mSwapChain.get());

	//I'd like to setup back/front with real swap images to avoid a copy but vulkan is much more strict about render targets.

	mBackBuffer = new CSurface9(mDevice, (CTexture9*)nullptr, mPresentationParameters.BackBufferWidth, mPresentationParameters.BackBufferHeight, D3DUSAGE_RENDERTARGET, 1, mPresentationParameters.BackBufferFormat, mPresentationParameters.MultiSampleType, mPresentationParameters.MultiSampleQuality, false, false, D3DPOOL_DEFAULT, nullptr);
	mFrontBuffer = new CSurface9(mDevice, (CTexture9*)nullptr, mPresentationParameters.BackBufferWidth, mPresentationParameters.BackBufferHeight, D3DUSAGE_RENDERTARGET, 1, mPresentationParameters.BackBufferFormat, mPresentationParameters.MultiSampleType, mPresentationParameters.MultiSampleQuality, false, false, D3DPOOL_DEFAULT, nullptr);
}

CSwapChain9::~CSwapChain9()
{
	Log(info) << "CSwapChain9::~CSwapChain9" << std::endl;

	delete mBackBuffer;
	delete mFrontBuffer;
}

ULONG CSwapChain9::PrivateAddRef(void)
{
	return InterlockedIncrement(&mPrivateReferenceCount);
}

ULONG CSwapChain9::PrivateRelease(void)
{
	ULONG ref = InterlockedDecrement(&mPrivateReferenceCount);

	if (ref == 0 && mReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

ULONG STDMETHODCALLTYPE CSwapChain9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CSwapChain9::QueryInterface(REFIID riid, void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DSurface9))
	{
		(*ppv) = this;
		this->AddRef();
		return D3D_OK;
	}

	if (IsEqualGUID(riid, IID_IDirect3DResource9))
	{
		(*ppv) = this;
		this->AddRef();
		return D3D_OK;
	}

	if (IsEqualGUID(riid, IID_IUnknown))
	{
		(*ppv) = this;
		this->AddRef();
		return D3D_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CSwapChain9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0 && mPrivateReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetBackBuffer(UINT BackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9  **ppBackBuffer)
{
	switch (Type)
	{
	case D3DBACKBUFFER_TYPE_MONO:
		(*ppBackBuffer) = mBackBuffer;
		mBackBuffer->AddRef();
		return D3D_OK;
		break;
	case D3DBACKBUFFER_TYPE_LEFT:
		Log(warning) << "CSwapChain9::GetBackBuffer left type is not implemented!" << std::endl;
		return E_NOTIMPL;
	case D3DBACKBUFFER_TYPE_RIGHT:
		Log(warning) << "CSwapChain9::GetBackBuffer right type is not implemented!" << std::endl;
		return E_NOTIMPL;
	default:
		Log(warning) << "CSwapChain9::GetBackBuffer unknown type is not implemented!" << std::endl;
		return E_NOTIMPL;
	}
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetDevice(IDirect3DDevice9** ppDevice)
{
	mDevice->AddRef();
	(*ppDevice) = (IDirect3DDevice9*)mDevice;
	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetDisplayMode(D3DDISPLAYMODE *pMode)
{
	pMode->Format = mPresentationParameters.BackBufferFormat;
	pMode->Height = mPresentationParameters.BackBufferHeight;
	pMode->Width = mPresentationParameters.BackBufferWidth;
	pMode->RefreshRate = mPresentationParameters.FullScreen_RefreshRateInHz; //Revsit

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetFrontBufferData(IDirect3DSurface9 *pDestSurface)
{
	Log(warning) << "CSurface9::GetFrontBufferData is not implemented!" << std::endl;

	return E_NOTIMPL; //TODO: Implement.
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetPresentParameters(D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	memcpy(pPresentationParameters, &mPresentationParameters, sizeof(D3DPRESENT_PARAMETERS)); //Revsit

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetRasterStatus(D3DRASTER_STATUS *pRasterStatus)
{
	(*pRasterStatus) = {};

	Log(warning) << "CSurface9::GetRasterStatus is not implemented!" << std::endl;

	return D3D_OK; //TODO: Implement.
}

HRESULT STDMETHODCALLTYPE CSwapChain9::Present(const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion, DWORD dwFlags)
{
	if (pSourceRect)
	{
		Log(warning) << "CSwapChain9::Present SourceRect is not implemented!" << std::endl;
	}
	if (pDestRect)
	{
		Log(warning) << "CSwapChain9::Present DestRect is not implemented!" << std::endl;
	}
	if (hDestWindowOverride != NULL && mPresentationParameters.hDeviceWindow != hDestWindowOverride)
	{
		Log(warning) << "CSwapChain9::Present window override is not implemented!" << std::endl;
	}
	if (pDirtyRegion)
	{
		Log(warning) << "CSwapChain9::Present DirtyRegion is not implemented!" << std::endl;
	}
	if (dwFlags)
	{
		Log(warning) << "CSwapChain9::Present dwFlags are not implemented!" << std::endl;
	}

	mDevice->StopDraw(); //Stop render pass if there is one open.

	mDevice->BeginRecordingCommands(); //recording should already be running by this point.

	vk::ImageMemoryBarrier prePresentBarrier;
	prePresentBarrier.srcAccessMask = vk::AccessFlags();
	prePresentBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
	prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	prePresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	mDevice->mDevice->acquireNextImageKHR(mSwapChain.get(), UINT64_MAX, mDevice->mImageAvailableSemaphores[mDevice->mFrameIndex].get(), vk::Fence(), &mImageIndex);

	prePresentBarrier.image = mBackBuffer->mImage.get();
	prePresentBarrier.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
	prePresentBarrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
	mDevice->mCurrentDrawCommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &prePresentBarrier);

	prePresentBarrier.image = mSwapChainImages[mImageIndex];
	prePresentBarrier.oldLayout = vk::ImageLayout::eUndefined;
	prePresentBarrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
	mDevice->mCurrentDrawCommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &prePresentBarrier);

	{
		const vk::ImageSubresourceLayers subResource1 = vk::ImageSubresourceLayers()
			.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseArrayLayer(0)
			.setMipLevel(0)
			.setLayerCount(1);

		const vk::ImageSubresourceLayers subResource2 = vk::ImageSubresourceLayers()
			.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseArrayLayer(0)
			.setMipLevel(0)
			.setLayerCount(1);

		const vk::ImageBlit region = vk::ImageBlit()
			.setSrcSubresource(subResource1)
			.setDstSubresource(subResource2)
			.setSrcOffsets({ vk::Offset3D(), vk::Offset3D(mBackBuffer->mWidth, mBackBuffer->mHeight, 1) })
			.setDstOffsets({ vk::Offset3D(), vk::Offset3D(mSwapchainExtent.width, mSwapchainExtent.height, 1) });

		/*
		Currently the format should back between the back buffer and surface so an imagecopy should work but 
		once window override is supported that may not be the case.
		*/
		mDevice->mCurrentDrawCommandBuffer.blitImage(
			mBackBuffer->mImage.get(), vk::ImageLayout::eTransferSrcOptimal,
			mSwapChainImages[mImageIndex], vk::ImageLayout::eTransferDstOptimal,
			1, &region, vk::Filter::eLinear);
	}

	prePresentBarrier.image = mBackBuffer->mImage.get();
	prePresentBarrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
	prePresentBarrier.newLayout = vk::ImageLayout::eColorAttachmentOptimal;
	mDevice->mCurrentDrawCommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &prePresentBarrier);

	prePresentBarrier.image = mSwapChainImages[mImageIndex];
	prePresentBarrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
	prePresentBarrier.newLayout = vk::ImageLayout::ePresentSrcKHR;
	mDevice->mCurrentDrawCommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &prePresentBarrier);

	mDevice->StopRecordingCommands();

	/*
	Take the swap chain image and display it to the screen.
	*/
	vk::PresentInfoKHR presentInfo;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &mDevice->mRenderFinishedSemaphores[mDevice->mFrameIndex].get();
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &mSwapChain.get();
	presentInfo.pImageIndices = &mImageIndex;
	mQueue.presentKHR(&presentInfo); //use present queue.

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetLastPresentCount(UINT *pLastPresentCount)
{
	(*pLastPresentCount) = {};

	Log(warning) << "CSurface9::GetLastPresentCount is not implemented!" << std::endl;

	return D3D_OK; //TODO: Implement.
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetPresentStats(D3DPRESENTSTATS *pPresentationStatistics)
{
	(*pPresentationStatistics) = {};

	Log(warning) << "CSurface9::GetPresentStats is not implemented!" << std::endl;

	return D3D_OK; //TODO: Implement.
}

HRESULT STDMETHODCALLTYPE CSwapChain9::GetDisplayModeEx(D3DDISPLAYMODEEX *pMode, D3DDISPLAYROTATION *pRotation)
{
	pMode->Format = mPresentationParameters.BackBufferFormat;
	pMode->Height = mPresentationParameters.BackBufferHeight;
	pMode->Width = mPresentationParameters.BackBufferWidth;
	pMode->RefreshRate = mPresentationParameters.FullScreen_RefreshRateInHz; //Revsit

	(*pRotation) = D3DDISPLAYROTATION_IDENTITY;

	return D3D_OK;
}