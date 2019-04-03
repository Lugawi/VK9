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

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#include <math.h>
#include <algorithm>

#include "CTexture9.h"
#include "CSurface9.h"
#include "CDevice9.h"
#include "LogManager.h"
//#include "PrivateTypes.h"

CTexture9::CTexture9(CDevice9* device, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE *pSharedHandle)
	: mDevice(device),
	mWidth(Width),
	mHeight(Height),
	mLevels(Levels),
	mUsage(Usage),
	mFormat(Format),
	mPool(Pool),
	mSharedHandle(pSharedHandle)
{
	Log(info) << "CTexture9::CTexture9" << std::endl;

	//mDevice->AddRef();

	if (!mLevels)
	{
		mLevels = (UINT)std::log2( std::max(mWidth, mHeight) ) + 1;
	}

	if (mUsage == 0)
	{
		mUsage = D3DUSAGE_RENDERTARGET;
	}

	mSurfaces.reserve(mLevels);
	UINT width = mWidth, height = mHeight;
	for (size_t i = 0; i < mLevels; i++)
	{
		CSurface9* ptr = new CSurface9(mDevice, this, width, height, mUsage, 1 /*Levels*/, mFormat, D3DMULTISAMPLE_NONE, 0 /*MultisampleQuality*/, false /*Discard*/, false /*Lockable*/, mPool, mSharedHandle);
		ptr->mMipIndex = i;

		mSurfaces.push_back(ptr);

		width /= 2;
		height /= 2;

		if (height == 0)
		{
			height = 1;
		}
		if (width == 0 && Width > 0)
		{
			width = 1;
		}
	}

	const vk::ImageTiling tiling = vk::ImageTiling::eOptimal;
	const vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
	const vk::MemoryPropertyFlags required_props = vk::MemoryPropertyFlagBits::eDeviceLocal;

	const vk::ImageCreateInfo imageCreateInfo = vk::ImageCreateInfo()
		.setImageType(vk::ImageType::e2D)
		.setFormat(ConvertFormat(mFormat))
		.setExtent({ mWidth, mHeight, 1 })
		.setMipLevels(mLevels)
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
		.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, mLevels, 0, 1));
	mImageView = mDevice->mDevice->createImageViewUnique(viewInfo);
}

CTexture9::~CTexture9()
{
	Log(info) << "CTexture9::~CTexture9" << std::endl;

	for (size_t i = 0; i < mSurfaces.size(); i++)
	{
		mSurfaces[i]->Release();
	}
}

ULONG CTexture9::PrivateAddRef(void)
{
	return InterlockedIncrement(&mPrivateReferenceCount);
}

ULONG CTexture9::PrivateRelease(void)
{
	ULONG ref = InterlockedDecrement(&mPrivateReferenceCount);

	if (ref == 0 && mReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

ULONG STDMETHODCALLTYPE CTexture9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CTexture9::QueryInterface(REFIID riid, void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DTexture9))
	{
		(*ppv) = this;
		this->AddRef();
		return S_OK;
	}

	if (IsEqualGUID(riid, IID_IDirect3DBaseTexture9))
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

ULONG STDMETHODCALLTYPE CTexture9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0 && mPrivateReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CTexture9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return S_OK; 
}

HRESULT STDMETHODCALLTYPE CTexture9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	Log(warning) << "CTexture9::FreePrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CTexture9::GetPriority()
{
	//TODO: Implement.

	Log(warning) << "CTexture9::GetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	Log(warning) << "CTexture9::GetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

void STDMETHODCALLTYPE CTexture9::PreLoad()
{
	//TODO: Implement.

	Log(warning) << "CTexture9::PreLoad is not implemented!" << std::endl;

	return;
}

DWORD STDMETHODCALLTYPE CTexture9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	Log(warning) << "CTexture9::SetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CTexture9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	Log(warning) << "CTexture9::SetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

VOID STDMETHODCALLTYPE CTexture9::GenerateMipSubLevels()
{

}

D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE CTexture9::GetAutoGenFilterType()
{
	return mMipFilter;
}

DWORD STDMETHODCALLTYPE CTexture9::GetLOD()
{
	//TODO: Implement.

	Log(warning) << "CTexture9::GetLOD is not implemented!" << std::endl;

	return 0;
}


DWORD STDMETHODCALLTYPE CTexture9::GetLevelCount()
{
	return mLevels;
}


HRESULT STDMETHODCALLTYPE CTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	mMipFilter = FilterType; //revisit

	return S_OK;
}

DWORD STDMETHODCALLTYPE CTexture9::SetLOD(DWORD LODNew)
{
	//TODO: Implement.

	Log(warning) << "CTexture9::SetLOD is not implemented!" << std::endl;

	return 0;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CTexture9::GetType()
{
	//return D3DRTYPE_SURFACE;
	//return D3DRTYPE_VOLUME;
	return D3DRTYPE_TEXTURE;
	//return D3DRTYPE_VOLUMETEXTURE;
	//return D3DRTYPE_CUBETEXTURE;
	//return D3DRTYPE_VERTEXBUFFER;
	//return D3DRTYPE_INDEXBUFFER;
}

HRESULT STDMETHODCALLTYPE CTexture9::AddDirtyRect(const RECT* pDirtyRect)
{
	//TODO: Implement.

	Log(warning) << "CTexture9::AddDirtyRect is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTexture9::GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc)
{
	return mSurfaces[Level]->GetDesc(pDesc);
}

HRESULT STDMETHODCALLTYPE CTexture9::GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel)
{
	IDirect3DSurface9* surface = (IDirect3DSurface9*)this->mSurfaces[Level];

	/*
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb205912(v=vs.85).aspx
	"Calling this method will increase the internal reference count on the IDirect3DSurface9 interface."
	*/
	surface->AddRef();

	(*ppSurfaceLevel) = surface;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CTexture9::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags)
{	
	HRESULT result = mSurfaces[Level]->LockRect(pLockedRect, pRect, Flags);

	if ((Flags & D3DLOCK_DISCARD) == D3DLOCK_DISCARD)
	{
		Log(warning) << "CTexture9::LockRect D3DLOCK_DISCARD is not implemented!" << std::endl;
	}

	return result;
}

HRESULT STDMETHODCALLTYPE CTexture9::UnlockRect(UINT Level)
{
	HRESULT result = mSurfaces[Level]->UnlockRect();

	return result;
}