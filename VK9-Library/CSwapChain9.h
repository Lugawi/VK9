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
class CSurface9;

class CSwapChain9 : public IDirect3DSwapChain9Ex
{
private:

public:
	ULONG mReferenceCount = 1;
	ULONG mPrivateReferenceCount = 0;

	ULONG PrivateAddRef(void)
	{
		return InterlockedIncrement(&mPrivateReferenceCount);
	}

	ULONG PrivateRelease(void)
	{
		ULONG ref = InterlockedDecrement(&mPrivateReferenceCount);

		if (ref == 0 && mReferenceCount == 0)
		{
			delete this;
		}

		return ref;
	}

	VkResult mResult = VK_SUCCESS;

	CDevice9* mDevice = nullptr;
	D3DPRESENT_PARAMETERS mPresentationParameters;
	CSurface9* mBackBuffer = nullptr;
	CSurface9* mFrontBuffer = nullptr;

	CSwapChain9(CDevice9* Device, D3DPRESENT_PARAMETERS *pPresentationParameters);
	~CSwapChain9();

	void Init();

public:
	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void  **ppv);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

	//IDirect3DSwapChain9
	virtual HRESULT STDMETHODCALLTYPE GetBackBuffer(UINT BackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9  **ppBackBuffer);
	virtual HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice);
	virtual HRESULT STDMETHODCALLTYPE GetDisplayMode(D3DDISPLAYMODE *pMode);
	virtual HRESULT STDMETHODCALLTYPE GetFrontBufferData(IDirect3DSurface9 *pDestSurface);
	virtual HRESULT STDMETHODCALLTYPE GetPresentParameters(D3DPRESENT_PARAMETERS *pPresentationParameters);
	virtual HRESULT STDMETHODCALLTYPE GetRasterStatus(D3DRASTER_STATUS *pRasterStatus);
	virtual HRESULT STDMETHODCALLTYPE Present(const RECT *pSourceRect,const RECT *pDestRect,HWND hDestWindowOverride,const RGNDATA *pDirtyRegion,DWORD dwFlags);

	//IDirect3DSwapChain9Ex
	virtual HRESULT STDMETHODCALLTYPE GetLastPresentCount(UINT *pLastPresentCount);
	virtual HRESULT STDMETHODCALLTYPE GetPresentStats(D3DPRESENTSTATS *pPresentationStatistics);
	virtual HRESULT STDMETHODCALLTYPE GetDisplayModeEx(D3DDISPLAYMODEEX *pMode, D3DDISPLAYROTATION *pRotation);
};
