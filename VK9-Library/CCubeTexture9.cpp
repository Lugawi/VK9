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
 
#include "CCubeTexture9.h"
#include "CDevice9.h"
#include "CSurface9.h"
#include "LogManager.h"
//#include "PrivateTypes.h"

CCubeTexture9::CCubeTexture9(CDevice9* device, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE *pSharedHandle)
	: mDevice(device),
	mEdgeLength(EdgeLength),
	mLevels(Levels),
	mUsage(Usage),
	mFormat(Format),
	mPool(Pool),
	mSharedHandle(pSharedHandle),
	mReferenceCount(1)
{
	Log(info) << "CCubeTexture9::CCubeTexture9" << std::endl;

	if (!mLevels)
	{
		mLevels = (UINT)std::log2(mEdgeLength) + 1;
	}

	for (int32_t i = 0; i < 6; i++)
	{
		UINT width = mEdgeLength;
		UINT height = mEdgeLength;
		std::vector<CSurface9*> surfaces;
		for (int32_t j = 0; j < (int32_t)mLevels; j++)
		{
			CSurface9* ptr = new CSurface9(mDevice, this, width, height, mUsage, 1 /*Levels*/, mFormat, D3DMULTISAMPLE_NONE, 0 /*MultisampleQuality*/, false /*Discard*/, false /*Lockable*/, mPool, mSharedHandle);

			ptr->mMipIndex = j;
			ptr->mTargetLayer = i;

			surfaces.push_back(ptr);

			width /= 2;
			height /= 2;
		}
		mSurfaces[i] = surfaces;
	}

	const vk::ImageTiling tiling = vk::ImageTiling::eOptimal;
	const vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
	const vk::MemoryPropertyFlags required_props = vk::MemoryPropertyFlagBits::eDeviceLocal;

	const vk::ImageCreateInfo imageCreateInfo = vk::ImageCreateInfo()
		.setImageType(vk::ImageType::e2D)
		.setFormat(ConvertFormat(mFormat))
		.setExtent({ mEdgeLength, mEdgeLength, 1 })
		.setMipLevels(mLevels)
		.setArrayLayers(6)
		.setSamples(vk::SampleCountFlagBits::e1)
		.setTiling(tiling)
		.setUsage(usage)
		.setSharingMode(vk::SharingMode::eExclusive)
		.setQueueFamilyIndexCount(0)
		.setPQueueFamilyIndices(nullptr)
		.setInitialLayout(vk::ImageLayout::ePreinitialized);
	mImage = mDevice->mDevice->createImageUnique(imageCreateInfo);

	vk::MemoryRequirements memoryRequirements;
	mDevice->mDevice->getImageMemoryRequirements(mImage.get(), &memoryRequirements);

	mImageMemoryAllocateInfo.setAllocationSize(memoryRequirements.size);
	mImageMemoryAllocateInfo.setMemoryTypeIndex(0);

	auto pass = mDevice->FindMemoryTypeFromProperties(memoryRequirements.memoryTypeBits, required_props, &mImageMemoryAllocateInfo.memoryTypeIndex);

	mImageDeviceMemory = mDevice->mDevice->allocateMemoryUnique(mImageMemoryAllocateInfo);

	mDevice->mDevice->bindImageMemory(mImage.get(), mImageDeviceMemory.get(), 0);

	//Now transition this thing from init to shader ready.
	SetImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

	/*
	This block handles the luminance & x formats. They are converted to color formats but need a little mapping to make them work correctly.
	*/
	vk::ComponentMapping componentMapping;
	switch (mFormat)
	{
	case D3DFMT_R5G6B5:
		//Vulkan has a matching format but nvidia doesn't support using it as a color attachment so we just use the other one and re-map the components.
		componentMapping = vk::ComponentMapping(vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_A8:
		//TODO: Revisit A8 mapping.
		componentMapping = vk::ComponentMapping(vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eR);
		break;
	case D3DFMT_L8:
		componentMapping = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_L16:
		componentMapping = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_A8L8:
		componentMapping = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG);
		break;
	case D3DFMT_X8R8G8B8:
	case D3DFMT_X8B8G8R8:
		componentMapping = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eOne);
		break;
	default:
		break;
	}

	auto const viewInfo = vk::ImageViewCreateInfo()
		.setImage(mImage.get())
		.setViewType(vk::ImageViewType::e2D)
		.setFormat(ConvertFormat(mFormat))
		.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, mLevels, 0, 6))
		.setComponents(componentMapping);
	mImageView = mDevice->mDevice->createImageViewUnique(viewInfo);
}

CCubeTexture9::~CCubeTexture9()
{
	Log(info) << "CCubeTexture9::~CCubeTexture9" << std::endl;

	for (int32_t i = 0; i < 6; i++)
	{
		for (int32_t j = 0; j < (int32_t)mSurfaces[i].size(); j++)
		{
			mSurfaces[i][j]->Release();
		}		
	}


}

void CCubeTexture9::SetImageLayout(vk::ImageLayout newLayout)
{
	Log(info) << "CCubeTexture9::SetImageLayout test1" << std::endl;

	mDevice->BeginRecordingUtilityCommands();
	{
		const vk::PipelineStageFlags src_stages = ((mImageLayout == vk::ImageLayout::eTransferSrcOptimal || mImageLayout == vk::ImageLayout::eTransferDstOptimal) ? vk::PipelineStageFlagBits::eTransfer : vk::PipelineStageFlagBits::eTopOfPipe);
		const vk::PipelineStageFlags dest_stages = ((newLayout == vk::ImageLayout::eTransferSrcOptimal || newLayout == vk::ImageLayout::eTransferDstOptimal) ? vk::PipelineStageFlagBits::eTransfer : vk::PipelineStageFlagBits::eFragmentShader);

		auto DstAccessMask = [](vk::ImageLayout const &layout)
		{
			vk::AccessFlags flags;

			switch (layout) {
			case vk::ImageLayout::eTransferDstOptimal:
				// Make sure anything that was copying from this image has completed
				flags = vk::AccessFlagBits::eTransferWrite;
				break;
			case vk::ImageLayout::eColorAttachmentOptimal:
				flags = vk::AccessFlagBits::eColorAttachmentWrite;
				break;
			case vk::ImageLayout::eDepthStencilAttachmentOptimal:
				flags = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
				break;
			case vk::ImageLayout::eShaderReadOnlyOptimal:
				// Make sure any Copy or CPU writes to image are flushed
				flags = vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eInputAttachmentRead;
				break;
			case vk::ImageLayout::eTransferSrcOptimal:
				flags = vk::AccessFlagBits::eTransferRead;
				break;
			case vk::ImageLayout::ePresentSrcKHR:
				flags = vk::AccessFlagBits::eMemoryRead;
				break;
			default:
				break;
			}

			return flags;
		};

		auto const barrier = vk::ImageMemoryBarrier()
			.setSrcAccessMask(vk::AccessFlagBits())
			.setDstAccessMask(DstAccessMask(newLayout))
			.setOldLayout(mImageLayout)
			.setNewLayout(newLayout)
			.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setImage(mImage.get())
			.setSubresourceRange(vk::ImageSubresourceRange(((mUsage == D3DUSAGE_DEPTHSTENCIL) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor), 0, mLevels, 0, 6));

		mDevice->mCurrentUtilityCommandBuffer.pipelineBarrier(src_stages, dest_stages, vk::DependencyFlagBits(), 0, nullptr, 0, nullptr, 1, &barrier);

	}
	mDevice->StopRecordingUtilityCommands();

	mImageLayout = newLayout;
}

ULONG CCubeTexture9::PrivateAddRef(void)
{
	return InterlockedIncrement(&mPrivateReferenceCount);
}

ULONG CCubeTexture9::PrivateRelease(void)
{
	ULONG ref = InterlockedDecrement(&mPrivateReferenceCount);

	if (ref == 0 && mReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

ULONG STDMETHODCALLTYPE CCubeTexture9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DCubeTexture9))
	{
		(*ppv) = this;
		this->AddRef();
		return D3D_OK;
	}

	if (IsEqualGUID(riid, IID_IDirect3DBaseTexture9))
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

ULONG STDMETHODCALLTYPE CCubeTexture9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0 && mPrivateReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::GetDevice(IDirect3DDevice9** ppDevice)
{
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return D3D_OK; 
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	Log(warning) << "CCubeTexture9::FreePrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CCubeTexture9::GetPriority()
{
	//TODO: Implement.

	Log(warning) << "CCubeTexture9::GetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	Log(warning) << "CCubeTexture9::GetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

void STDMETHODCALLTYPE CCubeTexture9::PreLoad()
{
	//TODO: Implement.

	Log(warning) << "CCubeTexture9::PreLoad is not implemented!" << std::endl;

	return; 
}

DWORD STDMETHODCALLTYPE CCubeTexture9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	Log(warning) << "CCubeTexture9::SetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	Log(warning) << "CCubeTexture9::SetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

VOID STDMETHODCALLTYPE CCubeTexture9::GenerateMipSubLevels()
{
	//TODO: this needs to be updated to handle all of the faces.

	mDevice->BeginRecordingUtilityCommands();
	{
		vk::PipelineStageFlags sourceStages = vk::PipelineStageFlagBits::eTopOfPipe;
		vk::PipelineStageFlags destinationStages = vk::PipelineStageFlagBits::eTopOfPipe;
		vk::Filter realFilter = ConvertFilter(mMipFilter);

		vk::ImageMemoryBarrier imageMemoryBarrier;
		//imageMemoryBarrier.srcAccessMask = 0;
		//imageMemoryBarrier.dstAccessMask = 0;

		vk::ImageSubresourceRange mipSubRange;
		mipSubRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		mipSubRange.levelCount = 1;
		mipSubRange.layerCount = 1;

		//Transition zero mip level to transfer source
		mipSubRange.baseMipLevel = 0;

		imageMemoryBarrier.oldLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		imageMemoryBarrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
		imageMemoryBarrier.image = mImage.get();
		imageMemoryBarrier.subresourceRange = mipSubRange;
		mDevice->mCurrentUtilityCommandBuffer.pipelineBarrier(sourceStages, destinationStages, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

		for (UINT i = 1; i < mLevels; i++) //Changed to match mLevels datatype
		{
			vk::ImageBlit imageBlit;

			// Source
			imageBlit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			imageBlit.srcSubresource.layerCount = 1;
			imageBlit.srcSubresource.mipLevel = 0;
			imageBlit.srcOffsets[1].x = int32_t(mEdgeLength);
			imageBlit.srcOffsets[1].y = int32_t(mEdgeLength);
			imageBlit.srcOffsets[1].z = 1;

			// Destination
			imageBlit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			imageBlit.dstSubresource.layerCount = 1;
			imageBlit.dstSubresource.mipLevel = i;
			imageBlit.dstOffsets[1].x = int32_t(mEdgeLength >> i);
			imageBlit.dstOffsets[1].y = int32_t(mEdgeLength >> i);
			imageBlit.dstOffsets[1].z = 1;

			//Transition current mip level to transfer dest
			mipSubRange.baseMipLevel = i;

			imageMemoryBarrier.oldLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			imageMemoryBarrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
			imageMemoryBarrier.image = mImage.get();
			imageMemoryBarrier.subresourceRange = mipSubRange;
			mDevice->mCurrentUtilityCommandBuffer.pipelineBarrier(sourceStages, destinationStages, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

			// Blit from zero level
			mDevice->mCurrentUtilityCommandBuffer.blitImage(mImage.get(), vk::ImageLayout::eTransferSrcOptimal, mImage.get(), vk::ImageLayout::eTransferDstOptimal, 1, &imageBlit, realFilter /*vk::Filter::eLinear*/);

			//Transition back
			imageMemoryBarrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
			imageMemoryBarrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			imageMemoryBarrier.image = mImage.get();
			imageMemoryBarrier.subresourceRange = mipSubRange;
			mDevice->mCurrentUtilityCommandBuffer.pipelineBarrier(sourceStages, destinationStages, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
		}

		imageMemoryBarrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
		imageMemoryBarrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		imageMemoryBarrier.image = mImage.get();
		imageMemoryBarrier.subresourceRange = mipSubRange;
		mDevice->mCurrentUtilityCommandBuffer.pipelineBarrier(sourceStages, destinationStages, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}
	mDevice->StopRecordingUtilityCommands();
}

D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE CCubeTexture9::GetAutoGenFilterType()
{
	return mMipFilter;
}

DWORD STDMETHODCALLTYPE CCubeTexture9::GetLOD()
{
	//TODO: Implement.

	Log(warning) << "CCubeTexture9::GetLOD is not implemented!" << std::endl;

	return 0;
}

DWORD STDMETHODCALLTYPE CCubeTexture9::GetLevelCount()
{
	return mLevels;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	mMipFilter = FilterType; //revisit

	return D3D_OK;
}

DWORD STDMETHODCALLTYPE CCubeTexture9::SetLOD(DWORD LODNew)
{
	//TODO: Implement.

	Log(warning) << "CCubeTexture9::SetLOD is not implemented!" << std::endl;

	return 0;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CCubeTexture9::GetType()
{
	//return D3DRTYPE_SURFACE;
	//return D3DRTYPE_VOLUME;
	//return D3DRTYPE_TEXTURE;
	//return D3DRTYPE_VOLUMETEXTURE;
	return D3DRTYPE_CUBETEXTURE;
	//return D3DRTYPE_VERTEXBUFFER;
	//return D3DRTYPE_INDEXBUFFER;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::AddDirtyRect(D3DCUBEMAP_FACES FaceType, const RECT* pDirtyRect)
{
	//TODO: Implement.

	Log(warning) << "CCubeTexture9::AddDirtyRect is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::GetCubeMapSurface(D3DCUBEMAP_FACES FaceType, UINT Level, IDirect3DSurface9** ppCubeMapSurface)
{
	IDirect3DSurface9* surface = (IDirect3DSurface9*)this->mSurfaces[FaceType][Level];

	surface->AddRef();

	(*ppCubeMapSurface) = surface;

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CCubeTexture9::GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc)
{
	return mSurfaces[0][Level]->GetDesc(pDesc);
}

HRESULT CCubeTexture9::LockRect(D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags)
{
	//if (Level == 0)
	//{
	//	Log(info) << "CTexture9::LockRect Level:" << Level << " Handle: " << this << std::endl;
	//}
	HRESULT result = mSurfaces[FaceType][Level]->LockRect(pLockedRect, pRect, Flags);	
	return result;
}

HRESULT CCubeTexture9::UnlockRect(D3DCUBEMAP_FACES FaceType, UINT Level)
{
	//if (Level == 0)
	//{
	//	Log(info) << "CTexture9::UnlockRect Level:" << Level << " Handle: " << this << std::endl;
	//}
	HRESULT result = mSurfaces[FaceType][Level]->UnlockRect();
	return result;
}