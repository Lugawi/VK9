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

#include "CSurface9.h"
#include "CDevice9.h"
#include "CTexture9.h"
#include "CCubeTexture9.h"
#include "LogManager.h"
//#include "PrivateTypes.h"

vk::Format ConvertFormat(D3DFORMAT format) noexcept
{
	/*
	https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#VkFormat
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172558(v=vs.85).aspx
	*/
	switch (format)
	{
	case D3DFMT_UNKNOWN:
		return  (vk::Format)VK_FORMAT_UNDEFINED;
	case D3DFMT_R8G8B8:
		return (vk::Format)VK_FORMAT_R8G8B8_UNORM;
	case D3DFMT_A8R8G8B8:
		return (vk::Format)VK_FORMAT_B8G8R8A8_UNORM;
	case D3DFMT_X8R8G8B8:
		/*
		There is technically no alpha so this may cause problems but I don't see anything that is a 100% match for this format in Vulkan.
		*/
		return (vk::Format)VK_FORMAT_B8G8R8A8_UNORM; //B8G8R8X8_UNORM
	case D3DFMT_R5G6B5:
		return (vk::Format)VK_FORMAT_R5G6B5_UNORM_PACK16; //Reall this is VK_FORMAT_B5G6R5_UNORM_PACK16 but Nvidia doesn't support that format as an attachment so we'll do it the other way around and remap.
	case D3DFMT_X1R5G5B5:
		return (vk::Format)VK_FORMAT_B5G5R5A1_UNORM_PACK16; //B5G5R5X1_UNORM
	case D3DFMT_A1R5G5B5:
		return (vk::Format)VK_FORMAT_B5G5R5A1_UNORM_PACK16;
	case D3DFMT_A4R4G4B4:
		return (vk::Format)VK_FORMAT_B4G4R4A4_UNORM_PACK16;
	case D3DFMT_R3G3B2:
		return (vk::Format)VK_FORMAT_UNDEFINED; //B2G3R3_UNORM:
	case D3DFMT_A8:
		return (vk::Format)VK_FORMAT_R8_UNORM; //A8_UNORM
	case D3DFMT_A8R3G3B2:
		return (vk::Format)VK_FORMAT_UNDEFINED; //B2G3R3A8_UNORM
	case D3DFMT_X4R4G4B4:
		return (vk::Format)VK_FORMAT_UNDEFINED; //B4G4R4X4_UNORM
	case D3DFMT_A2B10G10R10:
		return (vk::Format)VK_FORMAT_UNDEFINED; //R10G10B10A2_UNORM
	case D3DFMT_A8B8G8R8:
		return (vk::Format)VK_FORMAT_R8G8B8A8_UNORM;
	case D3DFMT_X8B8G8R8:
		return (vk::Format)VK_FORMAT_R8G8B8A8_UNORM; //R8G8B8X8_UNORM
	case D3DFMT_G16R16:
		return (vk::Format)VK_FORMAT_R16G16_UNORM;
	case D3DFMT_A2R10G10B10:
		return (vk::Format)VK_FORMAT_UNDEFINED; //B10G10R10A2_UNORM
	case D3DFMT_A16B16G16R16:
		return (vk::Format)VK_FORMAT_R16G16B16A16_UNORM;
	case D3DFMT_A8P8:
		return (vk::Format)VK_FORMAT_R8G8_UNORM; //P8_UINT_A8_UNORM
	case D3DFMT_P8:
		return (vk::Format)VK_FORMAT_R8_UINT; //P8_UINT
	case D3DFMT_L8:
		return (vk::Format)VK_FORMAT_R8_UNORM;
	case D3DFMT_A8L8:
		return (vk::Format)VK_FORMAT_R8G8_UNORM; //L8A8_UNORM
	case D3DFMT_A4L4:
		return (vk::Format)VK_FORMAT_UNDEFINED; //L4A4_UNORM
	case D3DFMT_V8U8:
		return (vk::Format)VK_FORMAT_R8G8_SNORM;
	case D3DFMT_L6V5U5:
		return (vk::Format)VK_FORMAT_UNDEFINED; //R5G5_SNORM_L6_UNORM
	case D3DFMT_X8L8V8U8:
		return (vk::Format)VK_FORMAT_UNDEFINED; //R8G8_SNORM_L8X8_UNORM
	case D3DFMT_Q8W8V8U8:
		return (vk::Format)VK_FORMAT_R8G8B8A8_SNORM;
	case D3DFMT_V16U16:
		return (vk::Format)VK_FORMAT_R16G16_SNORM;
	case D3DFMT_A2W10V10U10:
		return (vk::Format)VK_FORMAT_UNDEFINED; //R10G10B10_SNORM_A2_UNORM
	case D3DFMT_UYVY:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	case D3DFMT_R8G8_B8G8:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	case D3DFMT_YUY2:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	case D3DFMT_G8R8_G8B8:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	case D3DFMT_DXT1:
		return (vk::Format)VK_FORMAT_BC1_RGB_UNORM_BLOCK; //VK_FORMAT_BC1_RGB_UNORM_BLOCK
	case D3DFMT_DXT2:
		return (vk::Format)VK_FORMAT_BC2_UNORM_BLOCK; //VK_FORMAT_BC2_UNORM_BLOCK
	case D3DFMT_DXT3:
		return (vk::Format)VK_FORMAT_BC3_UNORM_BLOCK; //VK_FORMAT_BC3_UNORM_BLOCK
	case D3DFMT_DXT4:
		return (vk::Format)VK_FORMAT_BC4_UNORM_BLOCK; //VK_FORMAT_BC4_UNORM_BLOCK
	case D3DFMT_DXT5:
		return (vk::Format)VK_FORMAT_BC5_UNORM_BLOCK; //VK_FORMAT_BC5_UNORM_BLOCK
	case D3DFMT_D16_LOCKABLE:
		return (vk::Format)VK_FORMAT_D16_UNORM; //D16_LOCKABLE
	case D3DFMT_D32:
		return (vk::Format)VK_FORMAT_D32_SFLOAT; //D32_UNORM
	case D3DFMT_D15S1:
		return (vk::Format)VK_FORMAT_UNDEFINED; //S1_UINT_D15_UNORM
	case D3DFMT_D24S8:
		return (vk::Format)VK_FORMAT_D24_UNORM_S8_UINT;
	case D3DFMT_D24X8:
		return (vk::Format)VK_FORMAT_D24_UNORM_S8_UINT; //X8D24_UNORM
	case D3DFMT_D24X4S4:
		return (vk::Format)VK_FORMAT_UNDEFINED; //S4X4_UINT_D24_UNORM
	case D3DFMT_D16:
		return (vk::Format)VK_FORMAT_D16_UNORM;
	case D3DFMT_D32F_LOCKABLE:
		return (vk::Format)VK_FORMAT_D32_SFLOAT;
	case D3DFMT_D24FS8:
		return (vk::Format)VK_FORMAT_D24_UNORM_S8_UINT; //S8_UINT_D24_SFLOAT
#if !defined(D3D_DISABLE_9EX)
	case D3DFMT_D32_LOCKABLE:
		return (vk::Format)VK_FORMAT_D32_SFLOAT;
	case D3DFMT_S8_LOCKABLE:
		return (vk::Format)VK_FORMAT_S8_UINT;
#endif // !D3D_DISABLE_9EX
	case D3DFMT_L16:
		return (vk::Format)VK_FORMAT_R16_UNORM; //L16_UNORM
	case D3DFMT_VERTEXDATA:
		return (vk::Format)VK_FORMAT_UNDEFINED; //VERTEXDATA
	case D3DFMT_INDEX16:
		return (vk::Format)VK_FORMAT_R16_UINT;
	case D3DFMT_INDEX32:
		return (vk::Format)VK_FORMAT_R32_UINT;
	case D3DFMT_Q16W16V16U16:
		return (vk::Format)VK_FORMAT_R16G16B16A16_SNORM;
	case D3DFMT_MULTI2_ARGB8:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	case D3DFMT_R16F:
		return (vk::Format)VK_FORMAT_R16_SFLOAT;
	case D3DFMT_G16R16F:
		return (vk::Format)VK_FORMAT_R16G16_SFLOAT;
	case D3DFMT_A16B16G16R16F:
		return (vk::Format)VK_FORMAT_R16G16B16A16_SFLOAT;
	case D3DFMT_R32F:
		return (vk::Format)VK_FORMAT_R32_SFLOAT;
	case D3DFMT_G32R32F:
		return (vk::Format)VK_FORMAT_R32G32_SFLOAT;
	case D3DFMT_A32B32G32R32F:
		return (vk::Format)VK_FORMAT_R32G32B32A32_SFLOAT;
	case D3DFMT_CxV8U8:
		return (vk::Format)VK_FORMAT_R8G8_SNORM;
#if !defined(D3D_DISABLE_9EX)
	case D3DFMT_A1:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	case D3DFMT_A2B10G10R10_XR_BIAS:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	case D3DFMT_BINARYBUFFER:
		return (vk::Format)VK_FORMAT_UNDEFINED;
#endif // !D3D_DISABLE_9EX
	case D3DFMT_FORCE_DWORD:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	default:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	}
}

CSurface9::CSurface9(CDevice9* Device, CTexture9* Texture, UINT Width, UINT Height, DWORD Usage, UINT Levels, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, BOOL Lockable, D3DPOOL pool, HANDLE *pSharedHandle)
	: mDevice(Device),
	mTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mUsage(Usage),
	mLevels(Levels),
	mFormat(Format),
	mMultiSample(MultiSample),
	mMultisampleQuality(MultisampleQuality),
	mDiscard(Discard),
	mLockable(Lockable),
	mPool(pool),
	mSharedHandle(pSharedHandle)
{
	if (mFormat == D3DFMT_D16_LOCKABLE || mFormat == D3DFMT_D32 || mFormat == D3DFMT_D15S1 || mFormat == D3DFMT_D24S8 || mFormat == D3DFMT_D24X8 || mFormat == D3DFMT_D24X4S4 || mFormat == D3DFMT_D16)
	{
		mUsage = D3DUSAGE_DEPTHSTENCIL;
	}

	if (mCubeTexture != nullptr)
	{
		mCubeTexture->AddRef();
	}
	else if (mTexture != nullptr)
	{
		mTexture->AddRef();
	}

	const vk::ImageTiling tiling = vk::ImageTiling::eOptimal;
	const vk::ImageUsageFlags usage = ((mUsage == D3DUSAGE_DEPTHSTENCIL) ? vk::ImageUsageFlagBits::eDepthStencilAttachment : vk::ImageUsageFlagBits::eColorAttachment) | vk::ImageUsageFlagBits::eTransferDst;
	const vk::MemoryPropertyFlags required_props = vk::MemoryPropertyFlagBits::eDeviceLocal;

	const vk::ImageCreateInfo imageCreateInfo = vk::ImageCreateInfo()
		.setImageType(vk::ImageType::e2D)
		.setFormat(ConvertFormat(mFormat))
		.setExtent({ mWidth, mHeight, 1 })
		.setMipLevels(1)
		.setArrayLayers(1)
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
	//SetImageLayout(image.get(), vk::ImageAspectFlagBits::eColor, vk::ImageLayout::ePreinitialized, mSharedState.mImageLayout, vk::AccessFlagBits(), vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eFragmentShader);

	auto const viewInfo = vk::ImageViewCreateInfo()
		.setImage(mImage.get())
		.setViewType(vk::ImageViewType::e2D)
		.setFormat(ConvertFormat(mFormat))
		.setSubresourceRange(vk::ImageSubresourceRange(((mUsage == D3DUSAGE_DEPTHSTENCIL) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor), 0, 1, 0, 1));
	mImageView = mDevice->mDevice->createImageViewUnique(viewInfo);

}

CSurface9::CSurface9(CDevice9* Device, CCubeTexture9* Texture, UINT Width, UINT Height, DWORD Usage, UINT Levels, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, BOOL Lockable, D3DPOOL pool, HANDLE *pSharedHandle)
	: mDevice(Device),
	mCubeTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mUsage(Usage),
	mLevels(Levels),
	mFormat(Format),
	mMultiSample(MultiSample),
	mMultisampleQuality(MultisampleQuality),
	mDiscard(Discard),
	mLockable(Lockable),
	mPool(pool),
	mSharedHandle(pSharedHandle)
{
	if (mFormat == D3DFMT_D16_LOCKABLE || mFormat == D3DFMT_D32 || mFormat == D3DFMT_D15S1 || mFormat == D3DFMT_D24S8 || mFormat == D3DFMT_D24X8 || mFormat == D3DFMT_D24X4S4 || mFormat == D3DFMT_D16)
	{
		mUsage = D3DUSAGE_DEPTHSTENCIL;
	}

	if (mCubeTexture != nullptr)
	{
		mCubeTexture->AddRef();
	}
	else if (mTexture != nullptr)
	{
		mTexture->AddRef();
	}

	const vk::ImageTiling tiling = vk::ImageTiling::eOptimal;
	const vk::ImageUsageFlags usage = ((mUsage == D3DUSAGE_DEPTHSTENCIL) ? vk::ImageUsageFlagBits::eDepthStencilAttachment : vk::ImageUsageFlagBits::eColorAttachment) | vk::ImageUsageFlagBits::eTransferDst;
	const vk::MemoryPropertyFlags required_props = vk::MemoryPropertyFlagBits::eDeviceLocal;

	const vk::ImageCreateInfo imageCreateInfo = vk::ImageCreateInfo()
		.setImageType(vk::ImageType::e2D)
		.setFormat(ConvertFormat(mFormat))
		.setExtent({ mWidth, mHeight, 1 })
		.setMipLevels(1)
		.setArrayLayers(1)
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
	//SetImageLayout(image.get(), vk::ImageAspectFlagBits::eColor, vk::ImageLayout::ePreinitialized, mSharedState.mImageLayout, vk::AccessFlagBits(), vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eFragmentShader);

	auto const viewInfo = vk::ImageViewCreateInfo()
		.setImage(mImage.get())
		.setViewType(vk::ImageViewType::e2D)
		.setFormat(ConvertFormat(mFormat))
		.setSubresourceRange(vk::ImageSubresourceRange(((mUsage == D3DUSAGE_DEPTHSTENCIL) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor), 0, 1, 0, 1));
	mImageView = mDevice->mDevice->createImageViewUnique(viewInfo);
}

CSurface9::~CSurface9()
{
	if (mCubeTexture != nullptr)
	{
		mCubeTexture->Release();
	}
	else if (mTexture != nullptr)
	{
		mTexture->Release();
	}
}

ULONG CSurface9::PrivateAddRef(void)
{
	return InterlockedIncrement(&mPrivateReferenceCount);
}

ULONG CSurface9::PrivateRelease(void)
{
	ULONG ref = InterlockedDecrement(&mPrivateReferenceCount);

	if (ref == 0 && mReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

ULONG STDMETHODCALLTYPE CSurface9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CSurface9::QueryInterface(REFIID riid, void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DSurface9))
	{
		(*ppv) = this;
		this->AddRef();
		return S_OK;
	}

	if (IsEqualGUID(riid, IID_IDirect3DResource9))
	{
		(*ppv) = this;
		this->AddRef();
		return S_OK;
	}

	if (IsEqualGUID(riid, IID_IUnknown))
	{
		(*ppv) = this;
		this->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CSurface9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0 && mPrivateReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return S_OK; 
}

HRESULT STDMETHODCALLTYPE CSurface9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	Log(warning) << "CSurface9::FreePrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CSurface9::GetPriority()
{
	//TODO: Implement.

	Log(warning) << "CSurface9::GetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	Log(warning) << "CSurface9::GetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CSurface9::GetType()
{
	return D3DRTYPE_SURFACE;
}

void STDMETHODCALLTYPE CSurface9::PreLoad()
{
	//TODO: Implement.

	Log(warning) << "CSurface9::PreLoad is not implemented!" << std::endl;

	return;
}

DWORD STDMETHODCALLTYPE CSurface9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	Log(warning) << "CSurface9::SetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CSurface9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	Log(warning) << "CSurface9::SetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetContainer(REFIID riid, void** ppContainer)
{
	this->AddRef();

	//TODO: Implement.

	Log(warning) << "CSurface9::GetContainer is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetDC(HDC* phdc)
{
	//TODO: Implement.

	Log(warning) << "CSurface9::GetDC is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetDesc(D3DSURFACE_DESC* pDesc)
{
	pDesc->Format = this->mFormat;
	pDesc->Type = D3DRTYPE_SURFACE;
	pDesc->Usage = this->mUsage;
	pDesc->Pool = this->mPool;
	pDesc->MultiSampleType = this->mMultiSample;
	pDesc->MultiSampleQuality = this->mMultisampleQuality;
	pDesc->Width = this->mWidth;
	pDesc->Height = this->mHeight;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CSurface9::LockRect(D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags)
{
	


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CSurface9::ReleaseDC(HDC hdc)
{
	//TODO: Implement.

	Log(warning) << "CSurface9::ReleaseDC is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSurface9::UnlockRect()
{


	return S_OK;
}