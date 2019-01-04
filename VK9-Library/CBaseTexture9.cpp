// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
Copyright(c) 2016 Christopher Joseph Dean Schaefer

This software is provided 'as-is', without any express or implied
warranty.In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions :

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.If you use this software
in a product, an acknowledgement in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/
 
#include "CBaseTexture9.h"
#include "CDevice9.h"

#include "Utilities.h"

CBaseTexture9::CBaseTexture9()
	: mReferenceCount(1),
	mDevice(nullptr)
{

}

CBaseTexture9::~CBaseTexture9()
{
	
}

ULONG STDMETHODCALLTYPE CBaseTexture9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CBaseTexture9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
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

ULONG STDMETHODCALLTYPE CBaseTexture9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CBaseTexture9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return S_OK; 
}

HRESULT STDMETHODCALLTYPE CBaseTexture9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	Log(warning) << "CBaseTexture9::FreePrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CBaseTexture9::GetPriority()
{
	//TODO: Implement.

	Log(warning) << "CBaseTexture9::GetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CBaseTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	Log(warning) << "CBaseTexture9::GetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

void STDMETHODCALLTYPE CBaseTexture9::PreLoad()
{
	//TODO: Implement.

	Log(warning) << "CBaseTexture9::PreLoad is not implemented!" << std::endl;

	return; 
}

DWORD STDMETHODCALLTYPE CBaseTexture9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	Log(warning) << "CBaseTexture9::SetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CBaseTexture9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	Log(warning) << "CBaseTexture9::SetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

VOID STDMETHODCALLTYPE CBaseTexture9::GenerateMipSubLevels()
{
	//TODO: Implement.

	Log(warning) << "CBaseTexture9::GenerateMipSubLevels is not implemented!" << std::endl;

	return;
}

D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE CBaseTexture9::GetAutoGenFilterType()
{
	//TODO: Implement.

	Log(warning) << "CBaseTexture9::GetAutoGenFilterType is not implemented!" << std::endl;

	return D3DTEXF_NONE;
}

DWORD STDMETHODCALLTYPE CBaseTexture9::GetLOD()
{
	//TODO: Implement.

	Log(warning) << "CBaseTexture9::GetLOD is not implemented!" << std::endl;

	return 0;
}


DWORD STDMETHODCALLTYPE CBaseTexture9::GetLevelCount()
{
	//TODO: Implement.

	Log(warning) << "CBaseTexture9::GetLevelCount is not implemented!" << std::endl;

	return 0;
}


HRESULT STDMETHODCALLTYPE CBaseTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	//TODO: Implement.

	Log(warning) << "CBaseTexture9::SetAutoGenFilterType is not implemented!" << std::endl;

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CBaseTexture9::SetLOD(DWORD LODNew)
{
	//TODO: Implement.

	Log(warning) << "CBaseTexture9::SetLOD is not implemented!" << std::endl;

	return 0;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CBaseTexture9::GetType()
{
	//return D3DRTYPE_SURFACE;
	//return D3DRTYPE_VOLUME;
	return D3DRTYPE_TEXTURE;
	//return D3DRTYPE_VOLUMETEXTURE;
	//return D3DRTYPE_CUBETEXTURE;
	//return D3DRTYPE_VERTEXBUFFER;
	//return D3DRTYPE_INDEXBUFFER;
}