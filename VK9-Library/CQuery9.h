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

class CQuery9 : public IDirect3DQuery9
{
public:
	CQuery9(CDevice9* device, D3DQUERYTYPE Type);
	~CQuery9();
		
	void* mData = nullptr;
	size_t mSize = 4;

	CDevice9* mDevice;
	D3DQUERYTYPE mType;
	ULONG mReferenceCount=1;
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
public:
	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void  **ppv);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);	
	virtual ULONG STDMETHODCALLTYPE Release(void);

	//IDirect3DQuery9
	virtual HRESULT STDMETHODCALLTYPE GetData(void* pData, DWORD dwSize, DWORD dwGetDataFlags);
	virtual DWORD STDMETHODCALLTYPE GetDataSize();
	virtual HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice);
	virtual D3DQUERYTYPE STDMETHODCALLTYPE GetType();
	virtual HRESULT STDMETHODCALLTYPE Issue(DWORD dwIssueFlags);
};
