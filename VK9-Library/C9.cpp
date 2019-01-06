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

/*
"Honor the LORD with your wealth and with the firstfruits of all your produce; then your barns will be filled with plenty, and your vats will be bursting with wine." (Proverbs 3:9-10, ESV)
*/

#include <iostream>
#include "wtypes.h"

#include "C9.h"
#include "CDevice9.h"
#include "CSwapChain9.h"
#include "CSurface9.h"

#include "Utilities.h"

#define APP_SHORT_NAME "VK9"

#include "PrivateTypes.h"

C9::C9()
{
	mCommandStreamManager = std::make_shared<CommandStreamManager>();

	Log(info) << "C9::C9" << std::endl;
}

C9::~C9()
{
	WorkItem* workItem = mCommandStreamManager->GetWorkItem(nullptr);
	workItem->WorkItemType = WorkItemType::Instance_Destroy;
	workItem->Id = mId;
	mCommandStreamManager->RequestWorkAndWait(workItem);

	Log(info) << "C9::~C9" << std::endl;
}

//IUnknown

ULONG STDMETHODCALLTYPE C9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE C9::QueryInterface(REFIID riid, void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3D9))
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

	(*ppv) = nullptr;

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE C9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0)
	{
		delete this;
	}

	return ref;
}

//IDirect3D9

HRESULT STDMETHODCALLTYPE C9::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)
{
	Log(info) << "C9::CheckDepthStencilMatch Adapter: " << Adapter << std::endl;

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE C9::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
	Log(info) << "C9::CheckDeviceFormat Adapter: " << Adapter << std::endl;

	//Table based on DXUP which was based on SwiftShader.
	switch (RType)
	{
	case D3DRTYPE_SURFACE:
		if (Usage & D3DUSAGE_RENDERTARGET)
		{
			switch (CheckFormat)
			{
				//case D3DFMT_NULL:			return D3D_OK;
			case D3DFMT_R8G8B8:			return D3D_OK;
			case D3DFMT_R5G6B5:			return D3D_OK;
			case D3DFMT_X1R5G5B5:		return D3D_OK;
			case D3DFMT_A1R5G5B5:		return D3D_OK;
			case D3DFMT_A4R4G4B4:		return D3D_OK;
			case D3DFMT_R3G3B2:			return D3D_OK;
			case D3DFMT_A8R3G3B2:		return D3D_OK;
			case D3DFMT_X4R4G4B4:		return D3D_OK;
			case D3DFMT_A8R8G8B8:		return D3D_OK;
			case D3DFMT_X8R8G8B8:		return D3D_OK;
			case D3DFMT_A8B8G8R8:		return D3D_OK;
			case D3DFMT_X8B8G8R8:		return D3D_OK;
				// Integer HDR formats
			case D3DFMT_G16R16:			return D3D_OK;
			case D3DFMT_A2B10G10R10:	return D3D_OK;
			case D3DFMT_A2R10G10B10:	return D3D_OK;
			case D3DFMT_A16B16G16R16:	return D3D_OK;
				// Floating-point formats
			case D3DFMT_R16F:			return D3D_OK;
			case D3DFMT_G16R16F:		return D3D_OK;
			case D3DFMT_A16B16G16R16F:	return D3D_OK;
			case D3DFMT_R32F:			return D3D_OK;
			case D3DFMT_G32R32F:		return D3D_OK;
			case D3DFMT_A32B32G32R32F:	return D3D_OK;
			default:
				return D3DERR_NOTAVAILABLE;
			}
		}
		else if (Usage & D3DUSAGE_DEPTHSTENCIL)
		{
			switch (CheckFormat)
			{
			case D3DFMT_D32:			return D3D_OK;
			case D3DFMT_D24S8:			return D3D_OK;
			case D3DFMT_D24X8:			return D3D_OK;
			case D3DFMT_D16:			return D3D_OK;
			case D3DFMT_D24FS8:			return D3D_OK;
			case D3DFMT_D32F_LOCKABLE:	return D3D_OK;
			case D3DFMT_DF24:			return D3D_OK;
			case D3DFMT_DF16:			return D3D_OK;
			case D3DFMT_INTZ:			return D3D_OK;
			default:
				return D3DERR_NOTAVAILABLE;
			}
		}
		else
		{
			switch (CheckFormat)
			{
			case D3DFMT_A8:				return D3D_OK;
			case D3DFMT_R5G6B5:			return D3D_OK;
			case D3DFMT_X1R5G5B5:		return D3D_OK;
			case D3DFMT_A1R5G5B5:		return D3D_OK;
			case D3DFMT_A4R4G4B4:		return D3D_OK;
			case D3DFMT_R3G3B2:			return D3D_OK;
			case D3DFMT_A8R3G3B2:		return D3D_OK;
			case D3DFMT_X4R4G4B4:		return D3D_OK;
			case D3DFMT_R8G8B8:			return D3D_OK;
			case D3DFMT_X8R8G8B8:		return D3D_OK;
			case D3DFMT_A8R8G8B8:		return D3D_OK;
			case D3DFMT_X8B8G8R8:		return D3D_OK;
			case D3DFMT_A8B8G8R8:		return D3D_OK;
				// Paletted formats
			case D3DFMT_P8:				return D3D_OK;
			case D3DFMT_A8P8:			return D3D_OK;
				// Integer HDR formats
			case D3DFMT_G16R16:			return D3D_OK;
			case D3DFMT_A2R10G10B10:	return D3D_OK;
			case D3DFMT_A2B10G10R10:	return D3D_OK;
			case D3DFMT_A16B16G16R16:	return D3D_OK;
				// Compressed formats
			case D3DFMT_DXT1:			return D3D_OK;
			case D3DFMT_DXT2:			return D3D_OK;
			case D3DFMT_DXT3:			return D3D_OK;
			case D3DFMT_DXT4:			return D3D_OK;
			case D3DFMT_DXT5:			return D3D_OK;
				//case D3DFMT_ATI1:			return D3D_OK;
				//case D3DFMT_ATI2:			return D3D_OK;
				// Floating-point formats
			case D3DFMT_R16F:			return D3D_OK;
			case D3DFMT_G16R16F:		return D3D_OK;
			case D3DFMT_A16B16G16R16F:	return D3D_OK;
			case D3DFMT_R32F:			return D3D_OK;
			case D3DFMT_G32R32F:		return D3D_OK;
			case D3DFMT_A32B32G32R32F:	return D3D_OK;
				// Bump map formats
			case D3DFMT_V8U8:			return D3D_OK;
			case D3DFMT_L6V5U5:			return D3D_OK;
			case D3DFMT_X8L8V8U8:		return D3D_OK;
			case D3DFMT_Q8W8V8U8:		return D3D_OK;
			case D3DFMT_V16U16:			return D3D_OK;
			case D3DFMT_A2W10V10U10:	return D3D_OK;
			case D3DFMT_Q16W16V16U16:	return D3D_OK;
				// Luminance formats
			case D3DFMT_L8:				return D3D_OK;
			case D3DFMT_A4L4:			return D3D_OK;
			case D3DFMT_L16:			return D3D_OK;
			case D3DFMT_A8L8:			return D3D_OK;
				// Depth Bounds Test
				//case D3DFMT_NVDB:			return D3D_OK;
				// Transparency anti-aliasing
				//case D3DFMT_ATOC:			return D3D_OK;
			default:
				return D3DERR_NOTAVAILABLE;
			}
		}
	case D3DRTYPE_VOLUME:
		switch (CheckFormat)
		{
		case D3DFMT_A8:					return D3D_OK;
		case D3DFMT_R5G6B5:				return D3D_OK;
		case D3DFMT_X1R5G5B5:			return D3D_OK;
		case D3DFMT_A1R5G5B5:			return D3D_OK;
		case D3DFMT_A4R4G4B4:			return D3D_OK;
		case D3DFMT_R3G3B2:				return D3D_OK;
		case D3DFMT_A8R3G3B2:			return D3D_OK;
		case D3DFMT_X4R4G4B4:			return D3D_OK;
		case D3DFMT_R8G8B8:				return D3D_OK;
		case D3DFMT_X8R8G8B8:			return D3D_OK;
		case D3DFMT_A8R8G8B8:			return D3D_OK;
		case D3DFMT_X8B8G8R8:			return D3D_OK;
		case D3DFMT_A8B8G8R8:			return D3D_OK;
			// Paletted formats
		case D3DFMT_P8:					return D3D_OK;
		case D3DFMT_A8P8:				return D3D_OK;
			// Integer HDR formats
		case D3DFMT_G16R16:				return D3D_OK;
		case D3DFMT_A2R10G10B10:		return D3D_OK;
		case D3DFMT_A2B10G10R10:		return D3D_OK;
		case D3DFMT_A16B16G16R16:		return D3D_OK;
			// Compressed formats
		case D3DFMT_DXT1:				return D3D_OK;
		case D3DFMT_DXT2:				return D3D_OK;
		case D3DFMT_DXT3:				return D3D_OK;
		case D3DFMT_DXT4:				return D3D_OK;
		case D3DFMT_DXT5:				return D3D_OK;
			//case D3DFMT_ATI1:				return D3D_OK;
			//case D3DFMT_ATI2:				return D3D_OK;
			  // Floating-point formats
		case D3DFMT_R16F:				return D3D_OK;
		case D3DFMT_G16R16F:			return D3D_OK;
		case D3DFMT_A16B16G16R16F:		return D3D_OK;
		case D3DFMT_R32F:				return D3D_OK;
		case D3DFMT_G32R32F:			return D3D_OK;
		case D3DFMT_A32B32G32R32F:		return D3D_OK;
			// Bump map formats
		case D3DFMT_V8U8:				return D3D_OK;
		case D3DFMT_L6V5U5:				return D3D_OK;
		case D3DFMT_X8L8V8U8:			return D3D_OK;
		case D3DFMT_Q8W8V8U8:			return D3D_OK;
		case D3DFMT_V16U16:				return D3D_OK;
		case D3DFMT_A2W10V10U10:		return D3D_OK;
		case D3DFMT_Q16W16V16U16:		return D3D_OK;
			// Luminance formats
		case D3DFMT_L8:					return D3D_OK;
		case D3DFMT_A4L4:				return D3D_OK;
		case D3DFMT_L16:				return D3D_OK;
		case D3DFMT_A8L8:				return D3D_OK;
		default:
			return D3DERR_NOTAVAILABLE;
		}
	case D3DRTYPE_CUBETEXTURE:
		if (Usage & D3DUSAGE_RENDERTARGET)
		{
			switch (CheckFormat)
			{
				//case D3DFMT_NULL:			return D3D_OK;
			case D3DFMT_R8G8B8:			return D3D_OK;
			case D3DFMT_R5G6B5:			return D3D_OK;
			case D3DFMT_X1R5G5B5:		return D3D_OK;
			case D3DFMT_A1R5G5B5:		return D3D_OK;
			case D3DFMT_A4R4G4B4:		return D3D_OK;
			case D3DFMT_R3G3B2:			return D3D_OK;
			case D3DFMT_A8R3G3B2:		return D3D_OK;
			case D3DFMT_X4R4G4B4:		return D3D_OK;
			case D3DFMT_A8R8G8B8:		return D3D_OK;
			case D3DFMT_X8R8G8B8:		return D3D_OK;
			case D3DFMT_A8B8G8R8:		return D3D_OK;
			case D3DFMT_X8B8G8R8:		return D3D_OK;
				// Integer HDR formats
			case D3DFMT_G16R16:			return D3D_OK;
			case D3DFMT_A2B10G10R10:	return D3D_OK;
			case D3DFMT_A2R10G10B10:	return D3D_OK;
			case D3DFMT_A16B16G16R16:	return D3D_OK;
				// Floating-point formats
			case D3DFMT_R16F:			return D3D_OK;
			case D3DFMT_G16R16F:		return D3D_OK;
			case D3DFMT_A16B16G16R16F:	return D3D_OK;
			case D3DFMT_R32F:			return D3D_OK;
			case D3DFMT_G32R32F:		return D3D_OK;
			case D3DFMT_A32B32G32R32F:	return D3D_OK;
			default:
				return D3DERR_NOTAVAILABLE;
			}
		}
		else if (Usage & D3DUSAGE_DEPTHSTENCIL)
		{
			switch (CheckFormat)
			{
			case D3DFMT_D32:			return D3D_OK;
			case D3DFMT_D24S8:			return D3D_OK;
			case D3DFMT_D24X8:			return D3D_OK;
			case D3DFMT_D16:			return D3D_OK;
			case D3DFMT_D24FS8:			return D3D_OK;
			case D3DFMT_D32F_LOCKABLE:	return D3D_OK;
			case D3DFMT_DF24:			return D3D_OK;
			case D3DFMT_DF16:			return D3D_OK;
			case D3DFMT_INTZ:			return D3D_OK;
			default:
				return D3DERR_NOTAVAILABLE;
			}
		}
		else
		{
			switch (CheckFormat)
			{
			case D3DFMT_A8:				return D3D_OK;
			case D3DFMT_R5G6B5:			return D3D_OK;
			case D3DFMT_X1R5G5B5:		return D3D_OK;
			case D3DFMT_A1R5G5B5:		return D3D_OK;
			case D3DFMT_A4R4G4B4:		return D3D_OK;
			case D3DFMT_R3G3B2:			return D3D_OK;
			case D3DFMT_A8R3G3B2:		return D3D_OK;
			case D3DFMT_X4R4G4B4:		return D3D_OK;
			case D3DFMT_R8G8B8:			return D3D_OK;
			case D3DFMT_X8R8G8B8:		return D3D_OK;
			case D3DFMT_A8R8G8B8:		return D3D_OK;
			case D3DFMT_X8B8G8R8:		return D3D_OK;
			case D3DFMT_A8B8G8R8:		return D3D_OK;
				// Paletted formats
			case D3DFMT_P8:				return D3D_OK;
			case D3DFMT_A8P8:			return D3D_OK;
				// Integer HDR formats
			case D3DFMT_G16R16:			return D3D_OK;
			case D3DFMT_A2R10G10B10:	return D3D_OK;
			case D3DFMT_A2B10G10R10:	return D3D_OK;
			case D3DFMT_A16B16G16R16:	return D3D_OK;
				// Compressed formats
			case D3DFMT_DXT1:			return D3D_OK;
			case D3DFMT_DXT2:			return D3D_OK;
			case D3DFMT_DXT3:			return D3D_OK;
			case D3DFMT_DXT4:			return D3D_OK;
			case D3DFMT_DXT5:			return D3D_OK;
				//case D3DFMT_ATI1:			return D3D_OK;
				//case D3DFMT_ATI2:			return D3D_OK;
				// Floating-point formats
			case D3DFMT_R16F:			return D3D_OK;
			case D3DFMT_G16R16F:		return D3D_OK;
			case D3DFMT_A16B16G16R16F:	return D3D_OK;
			case D3DFMT_R32F:			return D3D_OK;
			case D3DFMT_G32R32F:		return D3D_OK;
			case D3DFMT_A32B32G32R32F:	return D3D_OK;
				// Bump map formats
			case D3DFMT_V8U8:			return D3D_OK;
			case D3DFMT_L6V5U5:			return D3D_OK;
			case D3DFMT_X8L8V8U8:		return D3D_OK;
			case D3DFMT_Q8W8V8U8:		return D3D_OK;
			case D3DFMT_V16U16:			return D3D_OK;
			case D3DFMT_A2W10V10U10:	return D3D_OK;
			case D3DFMT_Q16W16V16U16:	return D3D_OK;
				// Luminance formats
			case D3DFMT_L8:				return D3D_OK;
			case D3DFMT_A4L4:			return D3D_OK;
			case D3DFMT_L16:			return D3D_OK;
			case D3DFMT_A8L8:			return D3D_OK;
			default:
				return D3DERR_NOTAVAILABLE;
			}
		}
	case D3DRTYPE_VOLUMETEXTURE:
		switch (CheckFormat)
		{
		case D3DFMT_A8:					return D3D_OK;
		case D3DFMT_R5G6B5:				return D3D_OK;
		case D3DFMT_X1R5G5B5:			return D3D_OK;
		case D3DFMT_A1R5G5B5:			return D3D_OK;
		case D3DFMT_A4R4G4B4:			return D3D_OK;
		case D3DFMT_R3G3B2:				return D3D_OK;
		case D3DFMT_A8R3G3B2:			return D3D_OK;
		case D3DFMT_X4R4G4B4:			return D3D_OK;
		case D3DFMT_R8G8B8:				return D3D_OK;
		case D3DFMT_X8R8G8B8:			return D3D_OK;
		case D3DFMT_A8R8G8B8:			return D3D_OK;
		case D3DFMT_X8B8G8R8:			return D3D_OK;
		case D3DFMT_A8B8G8R8:			return D3D_OK;
			// Paletted formats
		case D3DFMT_P8:					return D3D_OK;
		case D3DFMT_A8P8:				return D3D_OK;
			// Integer HDR formats
		case D3DFMT_G16R16:				return D3D_OK;
		case D3DFMT_A2R10G10B10:		return D3D_OK;
		case D3DFMT_A2B10G10R10:		return D3D_OK;
		case D3DFMT_A16B16G16R16:		return D3D_OK;
			// Compressed formats
		case D3DFMT_DXT1:				return D3D_OK;
		case D3DFMT_DXT2:				return D3D_OK;
		case D3DFMT_DXT3:				return D3D_OK;
		case D3DFMT_DXT4:				return D3D_OK;
		case D3DFMT_DXT5:				return D3D_OK;
			//case D3DFMT_ATI1:				return D3D_OK;
			//case D3DFMT_ATI2:				return D3D_OK;
			// Floating-point formats
		case D3DFMT_R16F:				return D3D_OK;
		case D3DFMT_G16R16F:			return D3D_OK;
		case D3DFMT_A16B16G16R16F:		return D3D_OK;
		case D3DFMT_R32F:				return D3D_OK;
		case D3DFMT_G32R32F:			return D3D_OK;
		case D3DFMT_A32B32G32R32F:		return D3D_OK;
			// Bump map formats
		case D3DFMT_V8U8:				return D3D_OK;
		case D3DFMT_L6V5U5:				return D3D_OK;
		case D3DFMT_X8L8V8U8:			return D3D_OK;
		case D3DFMT_Q8W8V8U8:			return D3D_OK;
		case D3DFMT_V16U16:				return D3D_OK;
		case D3DFMT_A2W10V10U10:		return D3D_OK;
		case D3DFMT_Q16W16V16U16:		return D3D_OK;
			// Luminance formats
		case D3DFMT_L8:					return D3D_OK;
		case D3DFMT_A4L4:				return D3D_OK;
		case D3DFMT_L16:				return D3D_OK;
		case D3DFMT_A8L8:				return D3D_OK;
		default:
			return D3DERR_NOTAVAILABLE;
		}
	case D3DRTYPE_TEXTURE:
		if (Usage & D3DUSAGE_RENDERTARGET)
		{
			switch (CheckFormat)
			{
				//case D3DFMT_NULL:			return D3D_OK;
			case D3DFMT_R8G8B8:			return D3D_OK;
			case D3DFMT_R5G6B5:			return D3D_OK;
			case D3DFMT_X1R5G5B5:		return D3D_OK;
			case D3DFMT_A1R5G5B5:		return D3D_OK;
			case D3DFMT_A4R4G4B4:		return D3D_OK;
			case D3DFMT_R3G3B2:			return D3D_OK;
			case D3DFMT_A8R3G3B2:		return D3D_OK;
			case D3DFMT_X4R4G4B4:		return D3D_OK;
			case D3DFMT_A8R8G8B8:		return D3D_OK;
			case D3DFMT_X8R8G8B8:		return D3D_OK;
			case D3DFMT_A8B8G8R8:		return D3D_OK;
			case D3DFMT_X8B8G8R8:		return D3D_OK;
				// Integer HDR formats
			case D3DFMT_G16R16:			return D3D_OK;
			case D3DFMT_A2B10G10R10:	return D3D_OK;
			case D3DFMT_A2R10G10B10:	return D3D_OK;
			case D3DFMT_A16B16G16R16:	return D3D_OK;
				// Floating-point formats
			case D3DFMT_R16F:			return D3D_OK;
			case D3DFMT_G16R16F:		return D3D_OK;
			case D3DFMT_A16B16G16R16F:	return D3D_OK;
			case D3DFMT_R32F:			return D3D_OK;
			case D3DFMT_G32R32F:		return D3D_OK;
			case D3DFMT_A32B32G32R32F:	return D3D_OK;
			default:
				return D3DERR_NOTAVAILABLE;
			}
		}
		else if (Usage & D3DUSAGE_DEPTHSTENCIL)
		{
			switch (CheckFormat)
			{
			case D3DFMT_D32:			return D3D_OK;
			case D3DFMT_D24S8:			return D3D_OK;
			case D3DFMT_D24X8:			return D3D_OK;
			case D3DFMT_D16:			return D3D_OK;
			case D3DFMT_D24FS8:			return D3D_OK;
			case D3DFMT_D32F_LOCKABLE:	return D3D_OK;
			case D3DFMT_DF24:			return D3D_OK;
			case D3DFMT_DF16:			return D3D_OK;
			case D3DFMT_INTZ:			return D3D_OK;
			default:
				return D3DERR_NOTAVAILABLE;
			}
		}
		else
		{
			switch (CheckFormat)
			{
				//case D3DFMT_NULL:			return D3D_OK;
			case D3DFMT_A8:				return D3D_OK;
			case D3DFMT_R5G6B5:			return D3D_OK;
			case D3DFMT_X1R5G5B5:		return D3D_OK;
			case D3DFMT_A1R5G5B5:		return D3D_OK;
			case D3DFMT_A4R4G4B4:		return D3D_OK;
			case D3DFMT_R3G3B2:			return D3D_OK;
			case D3DFMT_A8R3G3B2:		return D3D_OK;
			case D3DFMT_X4R4G4B4:		return D3D_OK;
			case D3DFMT_R8G8B8:			return D3D_OK;
			case D3DFMT_X8R8G8B8:		return D3D_OK;
			case D3DFMT_A8R8G8B8:		return D3D_OK;
			case D3DFMT_X8B8G8R8:		return D3D_OK;
			case D3DFMT_A8B8G8R8:		return D3D_OK;
				// Paletted formats
			case D3DFMT_P8:				return D3D_OK;
			case D3DFMT_A8P8:			return D3D_OK;
				// Integer HDR formats
			case D3DFMT_G16R16:			return D3D_OK;
			case D3DFMT_A2R10G10B10:	return D3D_OK;
			case D3DFMT_A2B10G10R10:	return D3D_OK;
			case D3DFMT_A16B16G16R16:	return D3D_OK;
				// Compressed formats
			case D3DFMT_DXT1:			return D3D_OK;
			case D3DFMT_DXT2:			return D3D_OK;
			case D3DFMT_DXT3:			return D3D_OK;
			case D3DFMT_DXT4:			return D3D_OK;
			case D3DFMT_DXT5:			return D3D_OK;
				//case D3DFMT_ATI1:			return D3D_OK;
				//case D3DFMT_ATI2:			return D3D_OK;
				// Floating-point formats
			case D3DFMT_R16F:			return D3D_OK;
			case D3DFMT_G16R16F:		return D3D_OK;
			case D3DFMT_A16B16G16R16F:	return D3D_OK;
			case D3DFMT_R32F:			return D3D_OK;
			case D3DFMT_G32R32F:		return D3D_OK;
			case D3DFMT_A32B32G32R32F:	return D3D_OK;
				// Bump map formats
			case D3DFMT_V8U8:			return D3D_OK;
			case D3DFMT_L6V5U5:			return D3D_OK;
			case D3DFMT_X8L8V8U8:		return D3D_OK;
			case D3DFMT_Q8W8V8U8:		return D3D_OK;
			case D3DFMT_V16U16:			return D3D_OK;
			case D3DFMT_A2W10V10U10:	return D3D_OK;
			case D3DFMT_Q16W16V16U16:	return D3D_OK;
				// Luminance formats
			case D3DFMT_L8:				return D3D_OK;
			case D3DFMT_A4L4:			return D3D_OK;
			case D3DFMT_L16:			return D3D_OK;
			case D3DFMT_A8L8:			return D3D_OK;
				// Depth formats
			case D3DFMT_D32:			return D3D_OK;
			case D3DFMT_D24S8:			return D3D_OK;
			case D3DFMT_D24X8:			return D3D_OK;
			case D3DFMT_D16:			return D3D_OK;
			case D3DFMT_D24FS8:			return D3D_OK;
			case D3DFMT_D32F_LOCKABLE:	return D3D_OK;
			case D3DFMT_DF24:			return D3D_OK;
			case D3DFMT_DF16:			return D3D_OK;
			case D3DFMT_INTZ:			return D3D_OK;
			default:
				return D3DERR_NOTAVAILABLE;
			}
		}
	case D3DRTYPE_VERTEXBUFFER:
		if (CheckFormat == D3DFMT_VERTEXDATA)
		{
			return D3D_OK;
		}
		else
		{
			return D3DERR_NOTAVAILABLE;
		}
	case D3DRTYPE_INDEXBUFFER:
		switch (CheckFormat)
		{
		case D3DFMT_INDEX16:
		case D3DFMT_INDEX32:
			return D3D_OK;
		default:
			return D3DERR_NOTAVAILABLE;
		}
	default:
		return D3DERR_NOTAVAILABLE;
	}
}


HRESULT STDMETHODCALLTYPE C9::CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat)
{
	Log(info) << "C9::CheckDeviceFormatConversion Adapter: " << Adapter << std::endl;

	return D3D_OK;
}


HRESULT STDMETHODCALLTYPE C9::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD *pQualityLevels)
{
	Log(info) << "C9::CheckDeviceMultiSampleType Adapter: " << Adapter << std::endl;

	//Multi-Sample check based on DXUP.
	if (Adapter >= GetAdapterCount())
	{
		return D3DERR_INVALIDCALL;
	}

	if (pQualityLevels) 
	{
		if (MultiSampleType == D3DMULTISAMPLE_NONMASKABLE)
		{
			(*pQualityLevels) = 4;
		}
		else
		{
			(*pQualityLevels) = 1;
		}
	}

	if (MultiSampleType == D3DMULTISAMPLE_NONE ||
		MultiSampleType == D3DMULTISAMPLE_NONMASKABLE ||
		MultiSampleType == D3DMULTISAMPLE_2_SAMPLES ||
		MultiSampleType == D3DMULTISAMPLE_4_SAMPLES ||
		MultiSampleType == D3DMULTISAMPLE_8_SAMPLES ||
		MultiSampleType == D3DMULTISAMPLE_16_SAMPLES) 
	{
		if (CheckDeviceFormat(Adapter, DeviceType, D3DFMT_X8R8G8B8, D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, SurfaceFormat) == D3D_OK ||
			CheckDeviceFormat(Adapter, DeviceType, D3DFMT_X8R8G8B8, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, SurfaceFormat) == D3D_OK) 
		{
			if (SurfaceFormat != D3DFMT_D32F_LOCKABLE && SurfaceFormat != D3DFMT_D16_LOCKABLE)
			{
				return D3D_OK;
			}
		}
	}

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE C9::CheckDeviceType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL Windowed)
{
	Log(info) << "C9::CheckDeviceType Adapter: " << Adapter << std::endl;

	if ((ConvertFormat(DisplayFormat) == vk::Format::eUndefined && DisplayFormat != D3DFMT_UNKNOWN))
	{
		return D3DERR_NOTAVAILABLE;
	}

	if ((ConvertFormat(BackBufferFormat) == vk::Format::eUndefined && BackBufferFormat != D3DFMT_UNKNOWN))
	{
		return D3DERR_NOTAVAILABLE;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE C9::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DDevice9 **ppReturnedDeviceInterface)
{
	Log(info) << "C9::CreateDevice Adapter: " << Adapter << std::endl;

	if (Adapter >= GetAdapterCount())
	{
		(*ppReturnedDeviceInterface) = {};
		return D3DERR_INVALIDCALL;
	}

	HRESULT result = S_OK;

	CDevice9* obj = new CDevice9(this, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, nullptr);

	(*ppReturnedDeviceInterface) = (IDirect3DDevice9*)obj;

	obj->Init();

	pPresentationParameters->BackBufferWidth = obj->mPresentationParameters.BackBufferWidth;
	pPresentationParameters->BackBufferHeight = obj->mPresentationParameters.BackBufferHeight;

	return result;
}

HRESULT STDMETHODCALLTYPE C9::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE *pMode)
{
	Log(info) << "C9::EnumAdapterModes Adapter: " << Adapter << std::endl;

	if (Adapter >= GetAdapterCount())
	{
		return D3DERR_INVALIDCALL;
	}

	if (Format != D3DFMT_X8R8G8B8)
	{
		return D3DERR_NOTAVAILABLE;
	}

	Monitor& monitor = mMonitors[Adapter];

	if (Mode >= monitor.Modes.size())
	{
		return D3DERR_INVALIDCALL;
	}

	(*pMode) = monitor.Modes[Mode];

	return D3D_OK;
}

UINT STDMETHODCALLTYPE C9::GetAdapterCount()
{
	size_t monitorCount = mMonitors.size();

	Log(info) << "C9::GetAdapterCount MonitorCount: " << monitorCount << std::endl;

	//return monitorCount;
	return 1;
}

HRESULT STDMETHODCALLTYPE C9::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE *pMode)
{
	Log(info) << "C9::GetAdapterDisplayMode Adapter: " << Adapter << std::endl;

	Monitor& monitor = mMonitors[Adapter];

	pMode->RefreshRate = monitor.RefreshRate;
	pMode->Format = D3DFMT_X8R8G8B8;
	pMode->Height = monitor.Height;
	pMode->Width = monitor.Width;

	if (monitor.PixelBits != 32)
	{
		Log(info) << Adapter << std::endl;
		Log(info) << monitor.RefreshRate << std::endl;
		Log(info) << monitor.Height << std::endl;
		Log(info) << monitor.Width << std::endl;

		Log(info) << "C9::GetAdapterDisplayMode Unknown pixel bit format : " << monitor.PixelBits << std::endl; //Revisit
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE C9::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9 *pIdentifier)
{
	Log(info) << "C9::GetAdapterIdentifier Adapter: " << Adapter << std::endl;

	if (pIdentifier == nullptr)
	{
		return D3DERR_INVALIDCALL;
	}

	WorkItem* workItem = mCommandStreamManager->GetWorkItem(this);
	workItem->WorkItemType = WorkItemType::Instance_GetAdapterIdentifier;
	workItem->Id = mId;
	workItem->Argument1 = (void*)Adapter;
	workItem->Argument2 = (void*)Flags;
	workItem->Argument3 = (void*)pIdentifier;
	mCommandStreamManager->RequestWorkAndWait(workItem);

	return S_OK;
}

UINT STDMETHODCALLTYPE C9::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format)
{
	Log(info) << "C9::GetAdapterModeCount Adapter: " << Adapter << std::endl;

	if (Adapter >= GetAdapterCount())
	{
		return 0;
	}

	if (Format != D3DFMT_X8R8G8B8)
	{
		return 0;
	}

	Monitor& monitor = mMonitors[Adapter];

	return monitor.Modes.size();
}


HMONITOR STDMETHODCALLTYPE C9::GetAdapterMonitor(UINT Adapter)
{
	Log(info) << "C9::GetAdapterMonitor Adapter: " << Adapter << std::endl;

	HMONITOR returnValue;
	if ((mMonitors.size() - 1) < Adapter)
	{
		returnValue = mMonitors[0].hMonitor;
	}
	else
	{
		returnValue = mMonitors[Adapter].hMonitor;
	}

	Log(info) << "C9::GetAdapterMonitor HMONITOR: " << returnValue << std::endl;

	return returnValue;
}


HRESULT STDMETHODCALLTYPE C9::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9 *pCaps)
{
	Log(info) << "C9::GetDeviceCaps Adapter: " << Adapter << std::endl;

	if (pCaps == nullptr)
	{
		return D3DERR_INVALIDCALL;
	}

	if (Adapter >= GetAdapterCount())
	{
		return D3DERR_INVALIDCALL;
	}

	WorkItem* workItem = mCommandStreamManager->GetWorkItem(this);
	workItem->WorkItemType = WorkItemType::Instance_GetDeviceCaps;
	workItem->Id = mId;
	workItem->Argument1 = (void*)Adapter;
	workItem->Argument2 = (void*)DeviceType;
	workItem->Argument3 = (void*)pCaps;
	mCommandStreamManager->RequestWorkAndWait(workItem);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE C9::RegisterSoftwareDevice(void *pInitializeFunction)
{
	//TODO: Implement.

	Log(warning) << "C9::RegisterSoftwareDevice is not implemented!" << std::endl;

	return S_OK;
}

UINT STDMETHODCALLTYPE C9::GetAdapterModeCountEx(UINT Adapter, const D3DDISPLAYMODEFILTER *pFilter)
{
	Log(info) << "C9::GetAdapterModeCountEx Adapter: " << Adapter << std::endl;

	if (Adapter >= GetAdapterCount())
	{
		return 0;
	}

	if (pFilter == nullptr || pFilter->Format != D3DFMT_X8R8G8B8)
	{
		return 0;
	}

	Monitor& monitor = mMonitors[Adapter];

	return monitor.ExModes.size();
}

HRESULT STDMETHODCALLTYPE C9::EnumAdapterModesEx(UINT Adapter, const D3DDISPLAYMODEFILTER *pFilter, UINT Mode, D3DDISPLAYMODEEX *pMode)
{
	Log(info) << "C9::EnumAdapterModesEx Adapter: " << Adapter << std::endl;

	if (Adapter >= GetAdapterCount())
	{
		return D3DERR_INVALIDCALL;
	}

	if (pFilter == nullptr || pFilter->Format != D3DFMT_X8R8G8B8)
	{
		return D3DERR_NOTAVAILABLE;
	}

	Monitor& monitor = mMonitors[Adapter];

	if (Mode >= monitor.ExModes.size())
	{
		return D3DERR_INVALIDCALL;
	}

	(*pMode) = monitor.ExModes[Mode];

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE C9::GetAdapterDisplayModeEx(UINT Adapter, D3DDISPLAYMODEEX *pMode, D3DDISPLAYROTATION *pRotation)
{
	Log(info) << "C9::GetAdapterDisplayModeEx Adapter: " << Adapter << std::endl;

	Monitor& monitor = mMonitors[Adapter];

	pMode->Size = sizeof(D3DDISPLAYMODEEX);
	pMode->RefreshRate = monitor.RefreshRate;
	pMode->Format = D3DFMT_X8R8G8B8;
	pMode->Height = monitor.Height;
	pMode->Width = monitor.Width;
	pMode->ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;

	if (monitor.PixelBits != 32)
	{
		Log(info) << Adapter << std::endl;
		Log(info) << monitor.RefreshRate << std::endl;
		Log(info) << monitor.Height << std::endl;
		Log(info) << monitor.Width << std::endl;

		Log(info) << "C9::GetAdapterDisplayModeEx Unknown pixel bit format : " << monitor.PixelBits << std::endl; //Revisit
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE C9::CreateDeviceEx(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode, IDirect3DDevice9Ex **ppReturnedDeviceInterface)
{
	Log(info) << "C9::CreateDeviceEx Adapter: " << Adapter << std::endl;

	HRESULT result = S_OK;

	CDevice9* obj = new CDevice9(this, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode);

	(*ppReturnedDeviceInterface) = (IDirect3DDevice9Ex*)obj;

	obj->Init();

	pPresentationParameters->BackBufferWidth = obj->mPresentationParameters.BackBufferWidth;
	pPresentationParameters->BackBufferHeight = obj->mPresentationParameters.BackBufferHeight;

	return result;
}

HRESULT STDMETHODCALLTYPE C9::GetAdapterLUID(UINT Adapter, LUID *pLUID)
{
	Log(info) << "C9::GetAdapterLUID Adapter: " << Adapter << std::endl;

	(*pLUID) = mLUID;

	return S_OK;
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	Monitor monitor;

	monitor.hMonitor = hMonitor;
	monitor.hdcMonitor = hdcMonitor;

	monitor.Height = ::GetDeviceCaps(monitor.hdcMonitor, HORZRES);
	monitor.Width = ::GetDeviceCaps(monitor.hdcMonitor, VERTRES);
	monitor.RefreshRate = ::GetDeviceCaps(monitor.hdcMonitor, VREFRESH);
	monitor.PixelBits = ::GetDeviceCaps(monitor.hdcMonitor, BITSPIXEL);
	monitor.ColorPlanes = ::GetDeviceCaps(monitor.hdcMonitor, PLANES);

	// Query more monitor info
	::MONITORINFOEXW monitorInfo;
	monitorInfo.cbSize = sizeof(monitorInfo);

	if (!::GetMonitorInfoW(monitor.hMonitor, reinterpret_cast<MONITORINFO*>(&monitorInfo)))
	{
		Log(warning) << "MonitorEnumProc Failed to get some of the monitor information!" << std::endl;
	}

	DEVMODEW displayDeviceMode;
	uint32_t modeIndex = 0;
	while (::EnumDisplaySettingsW(monitorInfo.szDevice, modeIndex++, &displayDeviceMode))
	{
		if (displayDeviceMode.dmDisplayFlags & DM_INTERLACED)
		{
			continue;
		}

		/*
		I'm only handling 32bit formats currently for display so I'll need to revisit this later.
		*/
		if (displayDeviceMode.dmBitsPerPel != 32)
		{
			continue;
		}

		D3DDISPLAYMODE mode;
		mode.Width = displayDeviceMode.dmPelsWidth;
		mode.Height = displayDeviceMode.dmPelsHeight;
		mode.RefreshRate = displayDeviceMode.dmDisplayFrequency;
		mode.Format = D3DFMT_X8R8G8B8;
		monitor.Modes.push_back(mode);

		D3DDISPLAYMODEEX exMode;
		exMode.Size = sizeof(D3DDISPLAYMODEEX);
		exMode.Width = displayDeviceMode.dmPelsWidth;
		exMode.Height = displayDeviceMode.dmPelsHeight;
		exMode.RefreshRate = displayDeviceMode.dmDisplayFrequency;
		exMode.Format = D3DFMT_X8R8G8B8;
		exMode.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
		monitor.ExModes.push_back(exMode);
	}

	//DXVK mentions that some games depend on order so we'll so a sort as well.
	std::sort(monitor.Modes.begin(), monitor.Modes.end(), [](const D3DDISPLAYMODE& a, const D3DDISPLAYMODE& b)
	{
		if (a.Width < b.Width) return true;
		if (a.Width > b.Width) return false;

		if (a.Height < b.Height) return true;
		if (a.Height > b.Height) return false;

		return (a.RefreshRate) < (b.RefreshRate);
	});

	std::sort(monitor.ExModes.begin(), monitor.ExModes.end(), [](const D3DDISPLAYMODEEX& a, const D3DDISPLAYMODEEX& b)
	{
		if (a.Width < b.Width) return true;
		if (a.Width > b.Width) return false;

		if (a.Height < b.Height) return true;
		if (a.Height > b.Height) return false;

		return (a.RefreshRate) < (b.RefreshRate);
	});

	//All done so shove it in the vector.
	if (dwData != NULL)
	{
		if (hMonitor == 0)
		{
			return false;
		}
		std::vector<Monitor>* monitors;
		monitors = (std::vector<Monitor>*)dwData;
		monitors->push_back(monitor);

		Log(info) << "MonitorEnumProc HMONITOR: " << monitor.hMonitor << std::endl;
		Log(info) << "MonitorEnumProc HDC: " << monitor.hdcMonitor << std::endl;

		Log(info) << "MonitorEnumProc Height: " << monitor.Height << std::endl;
		Log(info) << "MonitorEnumProc Width: " << monitor.Width << std::endl;
		Log(info) << "MonitorEnumProc RefreshRate: " << monitor.RefreshRate << std::endl;

		Log(info) << "MonitorEnumProc PixelBits: " << monitor.PixelBits << std::endl;
		Log(info) << "MonitorEnumProc ColorPlanes: " << monitor.ColorPlanes << std::endl;

		return true;
	}
	else
	{
		Log(error) << "MonitorEnumProc: monitor vector is null." << std::endl;
		return false;
	}
}