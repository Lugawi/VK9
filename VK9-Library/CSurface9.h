#pragma once

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
 
#include <vulkan/vulkan.hpp>
#include <vulkan/vk_sdk_platform.h>
#include "d3d9.h"

class CDevice9;
class CTexture9;
class CCubeTexture9;

vk::Format ConvertFormat(D3DFORMAT format) noexcept;
size_t SizeOf(vk::Format format) noexcept;

class CSurface9 : public IDirect3DSurface9
{
private:

public:
	CSurface9(CDevice9* Device, CTexture9* Texture, UINT Width, UINT Height, DWORD Usage, UINT Levels, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, BOOL Lockable, D3DPOOL pool, HANDLE *pSharedHandle);
	CSurface9(CDevice9* Device, CCubeTexture9* Texture, UINT Width, UINT Height, DWORD Usage, UINT Levels, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, BOOL Lockable, D3DPOOL pool, HANDLE *pSharedHandle);
	CSurface9(CDevice9* Device, vk::Image& image, UINT Width, UINT Height, DWORD Usage, UINT Levels, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, BOOL Lockable, D3DPOOL pool, HANDLE *pSharedHandle);
	~CSurface9();

	//Reference Counting
	ULONG mReferenceCount = 1;
	ULONG mPrivateReferenceCount = 0;

	ULONG PrivateAddRef(void);
	ULONG PrivateRelease(void);

	//Creation Parameters
	CTexture9* mTexture = nullptr;
	CCubeTexture9* mCubeTexture = nullptr;
	UINT mWidth = 0;
	UINT mHeight = 0;
	UINT mLevels = 0;
	DWORD mUsage = D3DUSAGE_RENDERTARGET;
	D3DFORMAT mFormat = D3DFMT_UNKNOWN;
	D3DMULTISAMPLE_TYPE mMultiSample = D3DMULTISAMPLE_NONE;
	DWORD mMultisampleQuality = 0;
	BOOL mDiscard = 0;
	BOOL mLockable = 0;
	D3DPOOL mPool = D3DPOOL_DEFAULT;
	HANDLE* mSharedHandle = nullptr;

	//Vulkan - Image
	vk::UniqueImage mImage;
	vk::UniqueDeviceMemory mImageDeviceMemory;
	vk::UniqueImageView mImageView;

	vk::ImageLayout mImageLayout{ vk::ImageLayout::eUndefined };
	vk::MemoryAllocateInfo mImageMemoryAllocateInfo;

	vk::UniqueBuffer mStagingBuffer;
	vk::UniqueDeviceMemory mStagingBufferMemory;

	//Misc
	uint32_t mMipIndex = 0;
	uint32_t mTargetLayer = 0;
	void* mData = nullptr;

	//Helper Functions
	void SetImageLayout(vk::ImageLayout newLayout);
	void ResetViewAndStagingBuffer();
private:
	CDevice9* mDevice = nullptr;
public:
	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void  **ppv);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);	
	virtual ULONG STDMETHODCALLTYPE Release(void);

	//IDirect3DResource9
	virtual HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice);
	virtual HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid);
	virtual DWORD STDMETHODCALLTYPE GetPriority();
	virtual HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData);
	virtual D3DRESOURCETYPE STDMETHODCALLTYPE GetType();
	virtual void STDMETHODCALLTYPE PreLoad();
	virtual DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew);
	virtual HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags);

	//IDirect3DSurface9
	virtual HRESULT STDMETHODCALLTYPE GetContainer(REFIID riid, void** ppContainer);
	virtual HRESULT STDMETHODCALLTYPE GetDC(HDC* phdc);
	virtual HRESULT STDMETHODCALLTYPE GetDesc(D3DSURFACE_DESC* pDesc);
	virtual HRESULT STDMETHODCALLTYPE LockRect(D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags);
	virtual HRESULT STDMETHODCALLTYPE ReleaseDC(HDC hdc);
	virtual HRESULT STDMETHODCALLTYPE UnlockRect();
};
