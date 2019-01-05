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

HRESULT STDMETHODCALLTYPE C9::QueryInterface(REFIID riid,void  **ppv)
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

HRESULT STDMETHODCALLTYPE C9::CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat)
{
	Log(info) << "C9::CheckDepthStencilMatch Adapter: " << Adapter << std::endl;

	if (DepthStencilFormat == D3DFMT_UNKNOWN || (ConvertFormat(AdapterFormat) != vk::Format::eUndefined && ConvertFormat(DepthStencilFormat) != vk::Format::eUndefined))
	{
		Log(warning) << "C9::CheckDepthStencilMatch (D3D_OK) AdapterFormat: " << AdapterFormat << " CheckFormat: " << DepthStencilFormat << std::endl;

		return D3D_OK;
	}
	else
	{
		Log(warning) << "C9::CheckDepthStencilMatch (D3DERR_NOTAVAILABLE) AdapterFormat: " << AdapterFormat << " CheckFormat: " << DepthStencilFormat << std::endl;

		return D3DERR_NOTAVAILABLE;
	}
}

HRESULT STDMETHODCALLTYPE C9::CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat)
{
	Log(info) << "C9::CheckDeviceFormat Adapter: " << Adapter << std::endl;

	if (CheckFormat == D3DFMT_UNKNOWN || (ConvertFormat(AdapterFormat) != vk::Format::eUndefined && ConvertFormat(CheckFormat) != vk::Format::eUndefined))
	{
		Log(warning) << "C9::CheckDeviceFormat (D3D_OK) AdapterFormat: " << AdapterFormat << " CheckFormat: " << CheckFormat << std::endl;

		return D3D_OK;
	}
	else
	{
		Log(warning) << "C9::CheckDeviceFormat (D3DERR_NOTAVAILABLE) AdapterFormat: " << AdapterFormat << " CheckFormat: " << CheckFormat << std::endl;

		return D3DERR_NOTAVAILABLE;
	}
}


HRESULT STDMETHODCALLTYPE C9::CheckDeviceFormatConversion(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat)
{
	Log(info) << "C9::CheckDeviceFormatConversion Adapter: " << Adapter << std::endl;

	HRESULT result = S_OK;

	//TODO: Implement.

	Log(warning) << "C9::CheckDeviceFormatConversion is not implemented!" << std::endl;

	return result;
}


HRESULT STDMETHODCALLTYPE C9::CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD *pQualityLevels)
{
	Log(info) << "C9::CheckDeviceMultiSampleType Adapter: " << Adapter << std::endl;

	if (Adapter >= mMonitors.size())
	{
		return D3DERR_INVALIDCALL;
	}

	if (MultiSampleType > 16)
	{
		return D3DERR_NOTAVAILABLE;
	}

	if (ConvertFormat(SurfaceFormat) == vk::Format::eUndefined)
	{
		return D3DERR_NOTAVAILABLE;
	}

	//TODO: Implement.

	Log(warning) << "C9::CheckDeviceMultiSampleType is not implemented!" << std::endl;

	if (pQualityLevels!= nullptr)
	{
		(*pQualityLevels) = 1;
	}

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE C9::CheckDeviceType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL Windowed)
{		
	//TODO: Implement.

	Log(warning) << "C9::CheckDeviceType is not implemented!" << std::endl;

	return S_OK;	
}

HRESULT STDMETHODCALLTYPE C9::CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface)
{
	Log(info) << "C9::CreateDevice Adapter: " << Adapter << std::endl;

	HRESULT result = S_OK;

	CDevice9* obj = new CDevice9(this,Adapter,DeviceType,hFocusWindow,BehaviorFlags,pPresentationParameters, nullptr);

	(*ppReturnedDeviceInterface) = (IDirect3DDevice9*)obj;

	obj->Init();

	pPresentationParameters->BackBufferWidth = obj->mPresentationParameters.BackBufferWidth;
	pPresentationParameters->BackBufferHeight = obj->mPresentationParameters.BackBufferHeight;

	return result;	
}

HRESULT STDMETHODCALLTYPE C9::EnumAdapterModes(UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE *pMode)
{
	Log(info) << "C9::EnumAdapterModes Adapter: " << Adapter << std::endl;

	if (Adapter >= mMonitors.size())
	{
		return D3DERR_INVALIDCALL;
	}

	if (Format != D3DFMT_X8R8G8B8)
	{
		return D3DERR_NOTAVAILABLE;
	}

	Monitor& monitor = mMonitors[Adapter];


	pMode->RefreshRate = monitor.RefreshRate;
	pMode->Format = D3DFMT_X8R8G8B8;

	switch (Mode)
	{
	case 0:
		pMode->Width = 1024;
		pMode->Height = 768;
		break;
	case 1:
		pMode->Width = 1152;
		pMode->Height = 768;
		break;
	case 2:
		pMode->Width = 1280;
		pMode->Height = 720;
		break;
	case 3:
		pMode->Width = 1280;
		pMode->Height = 768;
		break;
	case 4:
		pMode->Width = 1280;
		pMode->Height = 800;
		break;
	case 5:
		pMode->Width = 1280;
		pMode->Height = 1024;
		break;
	case 6:
		pMode->Width = 1366;
		pMode->Height = 768;
		break;
	case 7:
		pMode->Width = 1440;
		pMode->Height = 960;
		break;
	case 8:
		pMode->Width = 1440;
		pMode->Height = 1050;
		break;
	case 9:
		pMode->Width = 1600;
		pMode->Height = 1200;
		break;
	case 10:
		pMode->Width = 1680;
		pMode->Height = 1050;
		break;
	case 11:
		pMode->Width = 1920;
		pMode->Height = 1080;
		break;
	case 12:
		pMode->Width = 1920;
		pMode->Height = 1200;
		break;
	case 13:
		pMode->Width = 2048;
		pMode->Height = 1536;
		break;
	case 14:
		pMode->Width = 2560;
		pMode->Height = 1440;
		break;
	case 15:
		pMode->Width = 2560;
		pMode->Height = 1600;
		break;
	case 16:
		pMode->Width = 2560;
		pMode->Height = 2048;
		break;
	case 17:
		pMode->Width = 3840;
		pMode->Height = 2160;
		break;
	case 18:
		pMode->Width = 7680;
		pMode->Height = 4320;
		break;
	default:
		pMode->Width = monitor.Width;
		pMode->Height = monitor.Height;
		break;
	}

	return D3D_OK;
}

UINT STDMETHODCALLTYPE C9::GetAdapterCount()
{
	size_t monitorCount = mMonitors.size();

	Log(warning) << "C9::GetAdapterCount MonitorCount: " << monitorCount << std::endl;

	return monitorCount;
}

HRESULT STDMETHODCALLTYPE C9::GetAdapterDisplayMode(UINT Adapter,D3DDISPLAYMODE *pMode)
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

HRESULT STDMETHODCALLTYPE C9::GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9 *pIdentifier)
{		
	Log(info) << "C9::GetAdapterIdentifier Adapter: " << Adapter << std::endl;

	WorkItem* workItem = mCommandStreamManager->GetWorkItem(this);
	workItem->WorkItemType = WorkItemType::Instance_GetAdapterIdentifier;
	workItem->Id = mId;
	workItem->Argument1 = (void*)Adapter;
	workItem->Argument2 = (void*)Flags;
	workItem->Argument3 = (void*)pIdentifier;
	mCommandStreamManager->RequestWorkAndWait(workItem);

	return S_OK;	
}

UINT STDMETHODCALLTYPE C9::GetAdapterModeCount(UINT Adapter,D3DFORMAT Format)
{	
	Log(info) << "C9::GetAdapterModeCount Adapter: " << Adapter << std::endl;

	if (Format == D3DFMT_X8R8G8B8)
	{
		return 19;
	}

	return 0;	
}


HMONITOR STDMETHODCALLTYPE C9::GetAdapterMonitor(UINT Adapter)
{
	Log(info) << "C9::GetAdapterMonitor Adapter: " << Adapter << std::endl;

	HMONITOR returnValue;
	if ((mMonitors.size()-1) < Adapter)
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


HRESULT STDMETHODCALLTYPE C9::GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9 *pCaps)
{
	Log(info) << "C9::GetDeviceCaps Adapter: " << Adapter << std::endl;

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

	if (pFilter!=nullptr && pFilter->Format == D3DFMT_X8R8G8B8)
	{
		return 19;
	}

	return 0;
}

HRESULT STDMETHODCALLTYPE C9::EnumAdapterModesEx(UINT Adapter, const D3DDISPLAYMODEFILTER *pFilter, UINT Mode, D3DDISPLAYMODEEX *pMode)
{
	Log(info) << "C9::EnumAdapterModesEx Adapter: " << Adapter << std::endl;

	if (Adapter >= mMonitors.size())
	{
		return D3DERR_INVALIDCALL;
	}

	if (pFilter==nullptr || pFilter->Format != D3DFMT_X8R8G8B8)
	{
		return D3DERR_NOTAVAILABLE;
	}

	Monitor& monitor = mMonitors[Adapter];

	pMode->Size = sizeof(D3DDISPLAYMODEEX);
	pMode->RefreshRate = monitor.RefreshRate;
	pMode->Format = D3DFMT_X8R8G8B8;
	pMode->ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;

	switch (Mode)
	{
	case 0:
		pMode->Width = 1024;
		pMode->Height = 768;
		break;
	case 1:
		pMode->Width = 1152;
		pMode->Height = 768;
		break;
	case 2:
		pMode->Width = 1280;
		pMode->Height = 720;
		break;
	case 3:
		pMode->Width = 1280;
		pMode->Height = 768;
		break;
	case 4:
		pMode->Width = 1280;
		pMode->Height = 800;
		break;
	case 5:
		pMode->Width = 1280;
		pMode->Height = 1024;
		break;
	case 6:
		pMode->Width = 1366;
		pMode->Height = 768;
		break;
	case 7:
		pMode->Width = 1440;
		pMode->Height = 960;
		break;
	case 8:
		pMode->Width = 1440;
		pMode->Height = 1050;
		break;
	case 9:
		pMode->Width = 1600;
		pMode->Height = 1200;
		break;
	case 10:
		pMode->Width = 1680;
		pMode->Height = 1050;
		break;
	case 11:
		pMode->Width = 1920;
		pMode->Height = 1080;
		break;
	case 12:
		pMode->Width = 1920;
		pMode->Height = 1200;
		break;
	case 13:
		pMode->Width = 2048;
		pMode->Height = 1536;
		break;
	case 14:
		pMode->Width = 2560;
		pMode->Height = 1440;
		break;
	case 15:
		pMode->Width = 2560;
		pMode->Height = 1600;
		break;
	case 16:
		pMode->Width = 2560;
		pMode->Height = 2048;
		break;
	case 17:
		pMode->Width = 3840;
		pMode->Height = 2160;
		break;
	case 18:
		pMode->Width = 7680;
		pMode->Height = 4320;
		break;
	default:
		pMode->Width = monitor.Width;
		pMode->Height = monitor.Height;
		break;
	}

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

	if (dwData != NULL)
	{
		if (hMonitor==0)
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