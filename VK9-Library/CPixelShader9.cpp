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
 
#include "CPixelShader9.h"
#include "CDevice9.h"
#include "LogManager.h"
//#include "PrivateTypes.h"

CPixelShader9::CPixelShader9(CDevice9* device,const DWORD* pFunction)
	: mDevice(device),
	mFunction((DWORD*)pFunction)
{
	Log(info) << "CPixelShader9::CPixelShader9" << std::endl;
}

CPixelShader9::~CPixelShader9()
{
	Log(info) << "CPixelShader9::~CPixelShader9" << std::endl;



	free(mFunction);
}

ULONG STDMETHODCALLTYPE CPixelShader9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CPixelShader9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DPixelShader9))
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

ULONG STDMETHODCALLTYPE CPixelShader9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0 && mPrivateReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CPixelShader9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return S_OK; 
}

HRESULT STDMETHODCALLTYPE CPixelShader9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	Log(warning) << "CPixelShader9::FreePrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CPixelShader9::GetPriority()
{
	//TODO: Implement.

	Log(warning) << "CPixelShader9::GetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CPixelShader9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	Log(warning) << "CPixelShader9::GetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CPixelShader9::GetType()
{
	//TODO: Implement.

	Log(warning) << "CPixelShader9::GetType is not implemented!" << std::endl;

	return D3DRTYPE_SURFACE;
}

void STDMETHODCALLTYPE CPixelShader9::PreLoad()
{
	//TODO: Implement.

	Log(warning) << "CPixelShader9::PreLoad is not implemented!" << std::endl;

	return; 
}

DWORD STDMETHODCALLTYPE CPixelShader9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	Log(warning) << "CPixelShader9::SetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CPixelShader9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	Log(warning) << "CPixelShader9::SetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CPixelShader9::GetFunction(void* pData, UINT* pSizeOfData)
{
	(*pSizeOfData) = mSize;

	if (pData == nullptr)
	{
		return S_OK;
	}

	memcpy(pData, mFunction, mSize);

	return S_OK;
}
