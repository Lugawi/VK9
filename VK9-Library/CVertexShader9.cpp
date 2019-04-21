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
 
#include "CVertexShader9.h"
#include "CDevice9.h"
#include "LogManager.h"
#include "ShaderConverter.h"
//#include "PrivateTypes.h"

#include <malloc.h>

CVertexShader9::CVertexShader9(CDevice9* device, const DWORD* pFunction)
	: mDevice(device),
	mFunction(nullptr)
{
	Log(info) << "CVertexShader9::CVertexShader9" << std::endl;

	ShaderConverter converter(mDevice->mDevice.get());
	mShader = converter.Convert((uint32_t*)pFunction);

	mSize = converter.mSize;

	if (mSize)
	{
		mFunction = (DWORD*)malloc(mSize);
		memcpy(mFunction, pFunction, mSize);
	}	
}

CVertexShader9::~CVertexShader9()
{
	Log(info) << "CVertexShader9::~CVertexShader9" << std::endl;

	free(mFunction);
}

ULONG CVertexShader9::PrivateAddRef(void)
{
	return InterlockedIncrement(&mPrivateReferenceCount);
}

ULONG CVertexShader9::PrivateRelease(void)
{
	ULONG ref = InterlockedDecrement(&mPrivateReferenceCount);

	if (ref == 0 && mReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

ULONG STDMETHODCALLTYPE CVertexShader9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CVertexShader9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DVertexShader9))
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

ULONG STDMETHODCALLTYPE CVertexShader9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0 && mPrivateReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CVertexShader9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return D3D_OK; 
}

HRESULT STDMETHODCALLTYPE CVertexShader9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	Log(warning) << "CVertexShader9::FreePrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CVertexShader9::GetPriority()
{
	//TODO: Implement.

	Log(warning) << "CVertexShader9::GetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CVertexShader9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	Log(warning) << "CVertexShader9::GetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CVertexShader9::GetType()
{
	//TODO: Implement.

	Log(warning) << "CVertexShader9::GetType is not implemented!" << std::endl;

	return D3DRTYPE_SURFACE;
}

void STDMETHODCALLTYPE CVertexShader9::PreLoad()
{
	//TODO: Implement.

	Log(warning) << "CVertexShader9::PreLoad is not implemented!" << std::endl;

	return; 
}

DWORD STDMETHODCALLTYPE CVertexShader9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	Log(warning) << "CVertexShader9::SetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CVertexShader9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	Log(warning) << "CVertexShader9::SetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

//HRESULT STDMETHODCALLTYPE CVertexShader9::GetDevice(IDirect3DDevice9** ppDevice)
//{
//	return E_NOTIMPL;
//}

HRESULT STDMETHODCALLTYPE CVertexShader9::GetFunction(void* pData, UINT* pSizeOfData)
{
	(*pSizeOfData) = mSize;

	if (pData==nullptr)
	{
		return D3D_OK;
	}

	//Log(warn) << "CVertexShader9::GetFunction" << std::endl;

	memcpy(pData, mFunction, mSize);

	return D3D_OK;
}
