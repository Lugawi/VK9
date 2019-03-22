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

CSurface9::CSurface9(CDevice9* Device, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DFORMAT Format)
	: mDevice(Device)
{
	mWidth = pPresentationParameters->BackBufferWidth;
	mHeight = pPresentationParameters->BackBufferHeight;
	mFormat = Format;
	mMultiSample = pPresentationParameters->MultiSampleType;
	mMultisampleQuality = pPresentationParameters->MultiSampleQuality;
	
	if (mFormat == D3DFMT_D16_LOCKABLE || mFormat == D3DFMT_D32 || mFormat == D3DFMT_D15S1 || mFormat == D3DFMT_D24S8 || mFormat == D3DFMT_D24X8 || mFormat == D3DFMT_D24X4S4 || mFormat == D3DFMT_D16)
	{
		mUsage = D3DUSAGE_DEPTHSTENCIL;
	}

	//Init();
}

CSurface9::CSurface9(CDevice9* Device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	: mDevice(Device)
{
	mWidth = pPresentationParameters->BackBufferWidth;
	mHeight = pPresentationParameters->BackBufferHeight;
	mFormat = pPresentationParameters->BackBufferFormat;
	mMultiSample = pPresentationParameters->MultiSampleType;
	mMultisampleQuality = pPresentationParameters->MultiSampleQuality;

	if (mFormat == D3DFMT_D16_LOCKABLE || mFormat == D3DFMT_D32 || mFormat == D3DFMT_D15S1 || mFormat == D3DFMT_D24S8 || mFormat == D3DFMT_D24X8 || mFormat == D3DFMT_D24X4S4 || mFormat == D3DFMT_D16)
	{
		mUsage = D3DUSAGE_DEPTHSTENCIL;
	}

	//Init();
}

CSurface9::CSurface9(CDevice9* Device, CTexture9* Texture, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, HANDLE *pSharedHandle)
	: mDevice(Device),
	mTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mFormat(Format),
	mMultiSample(MultiSample),
	mMultisampleQuality(MultisampleQuality),
	mDiscard(Discard),
	mSharedHandle(pSharedHandle)
{
	//Init();

	if (mFormat == D3DFMT_D16_LOCKABLE || mFormat == D3DFMT_D32 || mFormat == D3DFMT_D15S1 || mFormat == D3DFMT_D24S8 || mFormat == D3DFMT_D24X8 || mFormat == D3DFMT_D24X4S4 || mFormat == D3DFMT_D16)
	{
		mUsage = D3DUSAGE_DEPTHSTENCIL;
	}

}

CSurface9::CSurface9(CDevice9* Device, CCubeTexture9* Texture, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, HANDLE *pSharedHandle)
	: mDevice(Device),
	mCubeTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mFormat(Format),
	mMultiSample(MultiSample),
	mMultisampleQuality(MultisampleQuality),
	mDiscard(Discard),
	mSharedHandle(pSharedHandle)
{
	//Init();

	if (mFormat == D3DFMT_D16_LOCKABLE || mFormat == D3DFMT_D32 || mFormat == D3DFMT_D15S1 || mFormat == D3DFMT_D24S8 || mFormat == D3DFMT_D24X8 || mFormat == D3DFMT_D24X4S4 || mFormat == D3DFMT_D16)
	{
		mUsage = D3DUSAGE_DEPTHSTENCIL;
	}

}

CSurface9::CSurface9(CDevice9* Device, CTexture9* Texture, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, HANDLE *pSharedHandle, int32_t filler)
	: mDevice(Device),
	mTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mFormat(Format),
	mMultiSample(MultiSample),
	mMultisampleQuality(MultisampleQuality),
	mLockable(Lockable),
	mSharedHandle(pSharedHandle)
{
	//Init();

	if (mFormat == D3DFMT_D16_LOCKABLE || mFormat == D3DFMT_D32 || mFormat == D3DFMT_D15S1 || mFormat == D3DFMT_D24S8 || mFormat == D3DFMT_D24X8 || mFormat == D3DFMT_D24X4S4 || mFormat == D3DFMT_D16)
	{
		mUsage = D3DUSAGE_DEPTHSTENCIL;
	}

}

CSurface9::CSurface9(CDevice9* Device, CCubeTexture9* Texture, UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, HANDLE *pSharedHandle, int32_t filler)
	: mDevice(Device),
	mCubeTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mFormat(Format),
	mMultiSample(MultiSample),
	mMultisampleQuality(MultisampleQuality),
	mLockable(Lockable),
	mSharedHandle(pSharedHandle)
{
	//Init();

	if (mFormat == D3DFMT_D16_LOCKABLE || mFormat == D3DFMT_D32 || mFormat == D3DFMT_D15S1 || mFormat == D3DFMT_D24S8 || mFormat == D3DFMT_D24X8 || mFormat == D3DFMT_D24X4S4 || mFormat == D3DFMT_D16)
	{
		mUsage = D3DUSAGE_DEPTHSTENCIL;
	}

}

CSurface9::CSurface9(CDevice9* Device, CTexture9* Texture, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE *pSharedHandle)
	: mDevice(Device),
	mTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mUsage(Usage),
	mFormat(Format),
	mPool(Pool),
	mSharedHandle(pSharedHandle)
{
	//Init();
}

CSurface9::CSurface9(CDevice9* Device, CCubeTexture9* Texture, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE *pSharedHandle)
	: mDevice(Device),
	mCubeTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mUsage(Usage),
	mFormat(Format),
	mPool(Pool),
	mSharedHandle(pSharedHandle)
{
	//Init();
}

void CSurface9::Init()
{
	Log(info) << "CSurface9::CSurface9" << std::endl;

	//mDevice->AddRef();

	/*
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172611(v=vs.85).aspx
	Only these two are valid usage values for surface per the documentation.
	*/
	switch (mUsage)
	{
	case D3DUSAGE_DEPTHSTENCIL:
		break;
	case D3DUSAGE_RENDERTARGET:
		break;
	default:
		mUsage = D3DUSAGE_DEPTHSTENCIL;
		break;
	}

	if (mCubeTexture != nullptr)
	{
		mCubeTexture->AddRef();
	}
	else if (mTexture!=nullptr)
	{
		mTexture->AddRef();
	}



}

CSurface9::~CSurface9()
{
	Log(info) << "CSurface9::~CSurface9" << std::endl;



	if (mCubeTexture != nullptr)
	{
		mCubeTexture->Release();
	}
	else if (mTexture != nullptr)
	{
		mTexture->Release();
	}
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
	mFlags = Flags;
	


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


	this->Flush();

	return S_OK;
}

void CSurface9::Flush()
{

}