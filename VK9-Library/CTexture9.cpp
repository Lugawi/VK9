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

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#include <math.h>
#include <algorithm>

#include "CTexture9.h"
#include "CSurface9.h"
#include "CDevice9.h"

#include "Utilities.h"

CTexture9::CTexture9(CDevice9* device, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE *pSharedHandle)
	: mDevice(device),
	mWidth(Width),
	mHeight(Height),
	mLevels(Levels),
	mUsage(Usage),
	mFormat(Format),
	mPool(Pool),
	mSharedHandle(pSharedHandle),
	mId(0)
{
	Log(info) << "CTexture9::CTexture9" << std::endl;

	this->mCommandStreamManager = this->mDevice->mCommandStreamManager;
	//mDevice->AddRef();

	if (!mLevels)
	{
		mLevels = std::log2( std::max(mWidth, mHeight) ) + 1;
	}

	//mLevels = 1; //workaround

	if (mUsage == 0)
	{
		mUsage = D3DUSAGE_RENDERTARGET;
	}

	mSurfaces.reserve(mLevels);
	UINT width = mWidth, height = mHeight;
	for (size_t i = 0; i < mLevels; i++)
	{
		CSurface9* ptr = new CSurface9(mDevice, this, width, height, mLevels, mUsage, mFormat, mPool, mSharedHandle);

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
}

CTexture9::~CTexture9()
{
	Log(info) << "CTexture9::~CTexture9" << std::endl;

	for (size_t i = 0; i < mSurfaces.size(); i++)
	{
		mSurfaces[i]->Release();
	}

	for (size_t id : mIds)
	{
		WorkItem* workItem = mCommandStreamManager->GetWorkItem(nullptr);
		workItem->WorkItemType = WorkItemType::Texture_Destroy;
		workItem->Id = id;
		mCommandStreamManager->RequestWorkAndWait(workItem);
	}
}

void CTexture9::Init()
{
	//Work-around for games using strange formats that don't support sampling.
	mFormat = D3DFMT_A8R8G8B8;

	WorkItem* workItem = mCommandStreamManager->GetWorkItem(this);
	workItem->Id = this->mDevice->mId;
	workItem->WorkItemType = WorkItemType::Texture_Create;
	workItem->Argument1 = (void*)this;
	this->mId = mCommandStreamManager->RequestWorkAndWait(workItem);
	this->mIds.push_back(this->mId); //Added so it won't be lost.
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

	if (ref == 0)
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
	WorkItem* workItem = mCommandStreamManager->GetWorkItem(this);
	workItem->WorkItemType = WorkItemType::Texture_GenerateMipSubLevels;
	workItem->Id = mId;
	workItem->Argument1 = this;
	mCommandStreamManager->RequestWorkAndWait(workItem);
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
	/*
	Unless the caller indicates that they have not overwritten anything used by a previous draw call then we have to assume they have.
	Because the draw may not have happened we need to check to see if presentation has occured and if not flip to the next vertex
	*/

	//if (((Flags & D3DLOCK_NOOVERWRITE) == D3DLOCK_NOOVERWRITE) || ((Flags & D3DLOCK_READONLY) == D3DLOCK_READONLY))
	//{
	//	//This is best case the caller says they didn't modify anything used in a draw call.
	//}
	//else
	//{
	//	if (mFrameBit != mCommandStreamManager->mFrameBit)
	//	{
	//		mIndex = 0;
	//		mFrameBit = mCommandStreamManager->mFrameBit;
	//	}
	//	else
	//	{
	//		mLastIndex = mIndex;
	//		mIndex++;
	//	}
	//}

	//if (mIndex > mIds.size() - 1)
	//{
	//	Init();
	//}
	//else
	//{
	//	mId = mIds[mIndex];
	//}

	//mSurfaces[Level]->mTextureId = mId;
	//mSurfaces[Level]->mLastTextureId = mIds[mLastIndex];

	HRESULT result = mSurfaces[Level]->LockRect(pLockedRect, pRect, Flags);

	if ((Flags & D3DLOCK_DISCARD) == D3DLOCK_DISCARD)
	{
		Log(warning) << "CTexture9::LockRect D3DLOCK_DISCARD is not implemented!" << std::endl;
	}

	//mLastIndex = mIndex;

	return result;
}

HRESULT STDMETHODCALLTYPE CTexture9::UnlockRect(UINT Level)
{
	HRESULT result = mSurfaces[Level]->UnlockRect();

	return result;
}

void CTexture9::Flush()
{
	for (size_t i = 0; i < mSurfaces.size(); i++)
	{
		mSurfaces[i]->Flush();
	}
}
