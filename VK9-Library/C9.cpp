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

#include <iostream>
#include "wtypes.h"
#include <locale>

#include "C9.h"
#include "CDevice9.h"
#include "CSwapChain9.h"
#include "CSurface9.h"
#include "LogManager.h"
//#include "PrivateTypes.h"

#include "BitCast.h"

#define APP_SHORT_NAME "VK9"

#define D3DFMT_INTZ ((D3DFORMAT)(MAKEFOURCC('I','N','T','Z')))
#define D3DFMT_RAWZ ((D3DFORMAT)(MAKEFOURCC('R','A','W','Z')))
#define D3DFMT_DF24 ((D3DFORMAT)(MAKEFOURCC('D','F','2','4')))
#define D3DFMT_DF16 ((D3DFORMAT)(MAKEFOURCC('D','F','1','6')))
#define D3DFMT_INST ((D3DFORMAT)(MAKEFOURCC('I','N','S','T')))
#define D3DFMT_NULL ((D3DFORMAT)(MAKEFOURCC('N','U','L','L')))

#define SPIR_V_MAX_ID 4194303
#define ESTIMATED_SPIR_V_PER_DXBC_OP 5
#define MAX_DXBC_PHASES 2

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* layerPrefix, const char* message, void* userData)
{
	switch (flags)
	{
	case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
		Log(info) << "------DebugReport(Info)------" << std::endl;
		Log(info) << "ObjectType: " << objectType << std::endl;
		Log(info) << "Object: " << object << std::endl;
		Log(info) << "Location: " << location << std::endl;
		Log(info) << "MessageCode: " << messageCode << std::endl;
		Log(info) << "LayerPrefix: " << layerPrefix << std::endl;
		Log(info) << "Message: " << message << std::endl;
		Log(info) << "-----------------------------" << std::endl;
		break;
	case VK_DEBUG_REPORT_WARNING_BIT_EXT:
		Log(warning) << "------DebugReport(Warn)------" << std::endl;
		Log(warning) << "ObjectType: " << objectType << std::endl;
		Log(warning) << "Object: " << object << std::endl;
		Log(warning) << "Location: " << location << std::endl;
		Log(warning) << "MessageCode: " << messageCode << std::endl;
		Log(warning) << "LayerPrefix: " << layerPrefix << std::endl;
		Log(warning) << "Message: " << message << std::endl;
		Log(warning) << "-----------------------------" << std::endl;
		break;
	case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
		Log(warning) << "------DebugReport(Perf)------" << std::endl;
		Log(warning) << "ObjectType: " << objectType << std::endl;
		Log(warning) << "Object: " << object << std::endl;
		Log(warning) << "Location: " << location << std::endl;
		Log(warning) << "MessageCode: " << messageCode << std::endl;
		Log(warning) << "LayerPrefix: " << layerPrefix << std::endl;
		Log(warning) << "Message: " << message << std::endl;
		Log(warning) << "-----------------------------" << std::endl;
		break;
	case VK_DEBUG_REPORT_ERROR_BIT_EXT:
		Log(error) << "------DebugReport(Error)------" << std::endl;
		Log(error) << "ObjectType: " << objectType << std::endl;
		Log(error) << "Object: " << object << std::endl;
		Log(error) << "Location: " << location << std::endl;
		Log(error) << "MessageCode: " << messageCode << std::endl;
		Log(error) << "LayerPrefix: " << layerPrefix << std::endl;
		Log(error) << "Message: " << message << std::endl;
		Log(error) << "------------------------------" << std::endl;
		break;
	case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
		Log(warning) << "------DebugReport(Debug)------" << std::endl;
		Log(warning) << "ObjectType: " << objectType << std::endl;
		Log(warning) << "Object: " << object << std::endl;
		Log(warning) << "Location: " << location << std::endl;
		Log(warning) << "MessageCode: " << messageCode << std::endl;
		Log(warning) << "LayerPrefix: " << layerPrefix << std::endl;
		Log(warning) << "Message: " << message << std::endl;
		Log(warning) << "------------------------------" << std::endl;
		break;
	default:
		Log(error) << "------DebugReport(?)------" << std::endl;
		Log(error) << "ObjectType: " << objectType << std::endl;
		Log(error) << "Object: " << object << std::endl;
		Log(error) << "Location: " << location << std::endl;
		Log(error) << "MessageCode: " << messageCode << std::endl;
		Log(error) << "LayerPrefix: " << layerPrefix << std::endl;
		Log(error) << "Message: " << message << std::endl;
		Log(error) << "--------------------------" << std::endl;
		break;
	}

	return VK_FALSE;
}

VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSetKHR(
	VkCommandBuffer                             commandBuffer,
	VkPipelineBindPoint                         pipelineBindPoint,
	VkPipelineLayout                            layout,
	uint32_t                                    set,
	uint32_t                                    descriptorWriteCount,
	const VkWriteDescriptorSet*                 pDescriptorWrites)
{
	if (pfn_vkCmdPushDescriptorSetKHR == nullptr)
	{
		return;
	}

	pfn_vkCmdPushDescriptorSetKHR(
		commandBuffer,
		pipelineBindPoint,
		layout,
		set,
		descriptorWriteCount,
		pDescriptorWrites
	);
}

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(
	VkInstance                                  instance,
	const VkDebugReportCallbackCreateInfoEXT*   pCreateInfo,
	const VkAllocationCallbacks*                pAllocator,
	VkDebugReportCallbackEXT*                   pCallback)
{
	if (pfn_vkCreateDebugReportCallbackEXT == nullptr)
	{
		return VK_NOT_READY;
	}

	return pfn_vkCreateDebugReportCallbackEXT(
		instance,
		pCreateInfo,
		pAllocator,
		pCallback
	);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(
	VkInstance                                  instance,
	VkDebugReportCallbackEXT                    callback,
	const VkAllocationCallbacks*                pAllocator)
{
	if (pfn_vkDestroyDebugReportCallbackEXT == nullptr)
	{
		return;
	}

	pfn_vkDestroyDebugReportCallbackEXT(
		instance,
		callback,
		pAllocator
	);
}

VKAPI_ATTR void VKAPI_CALL vkDebugReportMessageEXT(
	VkInstance                                  instance,
	VkDebugReportFlagsEXT                       flags,
	VkDebugReportObjectTypeEXT                  objectType,
	uint64_t                                    object,
	size_t                                      location,
	int32_t                                     messageCode,
	const char*                                 pLayerPrefix,
	const char*                                 pMessage)
{
	if (pfn_vkDebugReportMessageEXT == nullptr)
	{
		return;
	}

	pfn_vkDebugReportMessageEXT(
		instance,
		flags,
		objectType,
		object,
		location,
		messageCode,
		pLayerPrefix,
		pMessage
	);
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	Monitor monitor;

	monitor.hMonitor = hMonitor;
	monitor.hdcMonitor = hdcMonitor;

	//Call GDI caps to get basic monitor information.
	monitor.Height = ::GetDeviceCaps(monitor.hdcMonitor, HORZRES);
	monitor.Width = ::GetDeviceCaps(monitor.hdcMonitor, VERTRES);
	monitor.RefreshRate = ::GetDeviceCaps(monitor.hdcMonitor, VREFRESH);
	monitor.PixelBits = ::GetDeviceCaps(monitor.hdcMonitor, BITSPIXEL);
	monitor.ColorPlanes = ::GetDeviceCaps(monitor.hdcMonitor, PLANES);

	//Query more monitor info
	::MONITORINFOEXW monitorInfo;
	monitorInfo.cbSize = sizeof(monitorInfo);

	if (!::GetMonitorInfoW(monitor.hMonitor, reinterpret_cast<MONITORINFO*>(&monitorInfo)))
	{
		Log(warning) << "MonitorEnumProc Failed to get some of the monitor information!" << std::endl;
	}

	//Get LUID
	//_D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME displayName;
	//strcpy(displayName.DeviceName, monitorInfo.szDevice);
	//D3DKMTOpenAdapterFromGdiDisplayName(&displayName);
	//monitor.LUID = displayName.AdapterLuid;

	//Get display modes
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

void LTrim(std::string &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch)
	{
		std::locale loc;
		return !std::isspace(ch, loc);
	}));
}

void RTrim(std::string &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch)
	{
		std::locale loc;
		return !std::isspace(ch, loc);
	}).base(), s.end());
}

void Trim(std::string &s)
{
	LTrim(s);
	RTrim(s);
}

C9::C9()
{
	//Load Configuration
	mConfiguration["LogFile"] = "VK9.log";
	mConfiguration["VSync"] = "1";
#ifdef _DEBUG
	mConfiguration["LogLevel"] = "0";
	mConfiguration["EnableDebugLayers"] = "1";
#else
	mConfiguration["LogLevel"] = "3";
	mConfiguration["EnableDebugLayers"] = "0";
#endif
	LoadConfiguration("VK9.conf");

	//Setup Logging.
	LogManager::Create(mConfiguration["LogFile"], (SeverityLevel)std::stoi(mConfiguration["LogLevel"]));

	//WINAPI to get monitor info
	EnumDisplayMonitors(GetDC(NULL), NULL, MonitorEnumProc, (LPARAM)&(mMonitors));

	//Load RenderDoc if it is available.
	HINSTANCE instance = LoadLibraryA("renderdoc.dll");
	mRenderDocDll = GetModuleHandleA("renderdoc.dll");
	if (mRenderDocDll != nullptr)
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mRenderDocDll, "RENDERDOC_GetAPI");
		if (RENDERDOC_GetAPI != nullptr)
		{
			int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void **)&mRenderDocApi);
			if (ret != 1)
			{
				Log(warning) << "C9::C9 unable to find RENDERDOC_API_Version_1_1_ !" << std::endl;
			}
			else
			{
				mRenderDocApi->SetLogFilePathTemplate("vk");
			}
		}
		else
		{
			Log(warning) << "C9::C9 unable to find RENDERDOC_GetAPI !" << std::endl;
		}

	}
	else
	{
		Log(warning) << "C9::C9 unable to find renderdoc.dll !" << std::endl;
	}

	//Figure out which layers and extensions we need.
#ifdef DEBUG
	bool enableDebugLayers = true; 
#else
	bool enableDebugLayers = false;
#endif

	if (!mConfiguration["EnableDebugLayers"].empty())
	{
		enableDebugLayers = std::stoi(mConfiguration["EnableDebugLayers"]);
	}

	std::vector<char*> extensionNames;
	std::vector<char*> layerNames;

	extensionNames.push_back("VK_KHR_surface");
	extensionNames.push_back("VK_KHR_win32_surface");
	//extensionNames.push_back("VK_KHR_get_physical_device_properties2");

	if (enableDebugLayers)
	{
		extensionNames.push_back("VK_EXT_debug_report");
		layerNames.push_back("VK_LAYER_LUNARG_standard_validation");
	}

	//Get the path to the executable.
	char fullPath[MAX_PATH];
	DWORD size = GetModuleFileNameA(nullptr, fullPath, MAX_PATH);
	mGameName = fullPath;
	
	//Remove the path from the name
	const size_t last_slash_idx = mGameName.find_last_of("\\/");
	if (std::string::npos != last_slash_idx)
	{
		mGameName.erase(0, last_slash_idx + 1);
	}

	//Remove extension from the name
	const size_t period_idx = mGameName.rfind('.');
	if (std::string::npos != period_idx)
	{
		mGameName.erase(period_idx);
	}

	//Create the Vulkan instance
	const vk::ApplicationInfo applicationInfo(mGameName.c_str(), 1, APP_SHORT_NAME, 1, VK_MAKE_VERSION(1, 1, 0));
	const vk::InstanceCreateInfo createInfo({}, &applicationInfo, static_cast<uint32_t>(layerNames.size()), layerNames.data(), static_cast<uint32_t>(extensionNames.size()), extensionNames.data());
	mInstance = vk::createInstanceUnique(createInfo);

	if (enableDebugLayers)
	{
		Log(info) << "C9::C9 Debug layers are on!"  << std::endl;

		/*
		If we are debugging then grab the function pointers for the debug methods.
		We have to do this because these are extensions so they aren't definded by default.
		*/
		pfn_vkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(mInstance->getProcAddr("vkCreateDebugReportCallbackEXT"));
		pfn_vkDebugReportMessageEXT = reinterpret_cast<PFN_vkDebugReportMessageEXT>(mInstance->getProcAddr("vkDebugReportMessageEXT"));
		pfn_vkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(mInstance->getProcAddr("vkDestroyDebugReportCallbackEXT"));

		/*
		Now we need to create our callback which will be called when the validation layers catch something.
		From there it's up to us to capture that information in some way. In our case we'll pass it along to the logger.
		*/
		const vk::DebugReportCallbackCreateInfoEXT callbackCreateInfo(
			vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::ePerformanceWarning,
			&DebugReportCallback,
			this);
		mCallback = mInstance->createDebugReportCallbackEXTUnique(callbackCreateInfo);
	}

	//Grab the physical devices so we can figure out which one to use.
	mPhysicalDevices = mInstance->enumeratePhysicalDevices();
	mPhysicalDeviceIndex = 0;

	//Count VRAM to try to to guess the most powerful GPU.
	vk::DeviceSize largestSize = 0;
	for (size_t i = 0; i < mPhysicalDevices.size(); i++)
	{
		vk::PhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
		mPhysicalDevices[mPhysicalDeviceIndex].getMemoryProperties(&physicalDeviceMemoryProperties);
		vk::DeviceSize size = 0;
		for (size_t j = 0; j < physicalDeviceMemoryProperties.memoryHeapCount; j++)
		{
			size += physicalDeviceMemoryProperties.memoryHeaps[j].size;
		}
		if (size > largestSize)
		{
			mPhysicalDeviceIndex = i;
		}
	}
	Log(info) << "C9::C9 Assumed Device Index: " << mPhysicalDeviceIndex << std::endl;

	//Grab the properties for GetAdapterIdentifier and other calls.
	mPhysicalDevices[mPhysicalDeviceIndex].getMemoryProperties(&mPhysicalDeviceMemoryProperties);
	mPhysicalDevices[mPhysicalDeviceIndex].getProperties(&mPhysicalDeviceProperties);

	Log(info) << "C9::C9 DeviceName: " << mPhysicalDeviceProperties.deviceName << std::endl;
	Log(info) << "C9::C9 APIVersion: " << mPhysicalDeviceProperties.apiVersion << std::endl;
	Log(info) << "C9::C9 DriverVersion: " << mPhysicalDeviceProperties.driverVersion << std::endl;

	//Look for a queue that supports graphics
	mQueueFamilyProperties = mPhysicalDevices[mPhysicalDeviceIndex].getQueueFamilyProperties();
	mGraphicsQueueFamilyIndex = std::distance(mQueueFamilyProperties.begin(), std::find_if(mQueueFamilyProperties.begin(), mQueueFamilyProperties.end(), [](vk::QueueFamilyProperties const& qfp) { return qfp.queueFlags & vk::QueueFlagBits::eGraphics; }));
}

C9::~C9()
{
	if (mRenderDocDll != nullptr)
	{
		FreeLibrary(mRenderDocDll);
	}
}

ULONG C9::PrivateAddRef(void)
{
	return InterlockedIncrement(&mPrivateReferenceCount);
}

ULONG C9::PrivateRelease(void)
{
	ULONG ref = InterlockedDecrement(&mPrivateReferenceCount);

	if (ref == 0 && mReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

void C9::LoadConfiguration(std::string filename)
{
	std::string key;
	std::string value;
	std::ifstream input(filename);

	while (input)
	{
		//load key/value pair
		std::getline(input, key, '=');
		std::getline(input, value, '\n');

		//scrub \r out just in case this is a DOS/Windows format file.
		key.erase(std::remove(key.begin(), key.end(), '\r'), key.end());
		value.erase(std::remove(value.begin(), value.end(), '\r'), value.end());

		//Handle leading and trailing spaces.
		Trim(key);
		Trim(value);

		mConfiguration[key] = value;
	}

	input.close();
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

	if (ref == 0 && mPrivateReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

//IDirect3D9

HRESULT STDMETHODCALLTYPE C9::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)
{
	Log(info) << "C9::CheckDepthStencilMatch Adapter: " << Adapter << std::endl;

	HRESULT result = D3D_OK;

	switch (RenderTargetFormat)
	{
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A2R10G10B10:
	case D3DFMT_A2B10G10R10:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_R5G6B5:
		//It's fine
	default:
		//It's not fine
		result = D3DERR_NOTAVAILABLE;
		break;
	}

	//TODO:Check depth format.

	return result;
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

	//At least for now we are assuming that any conversion is ok.

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

	/*
	This piece is pretty much the same as what d9vk does.
	The code is a bit different but the net result should be the same.
	*/

	if (DisplayFormat == D3DFMT_UNKNOWN)
	{
		if (Windowed)
		{
			return D3DERR_NOTAVAILABLE;		
		}
		else
		{
			return D3DERR_INVALIDCALL;
		}
	}

	if (BackBufferFormat == D3DFMT_UNKNOWN)
	{
		if (Windowed)
		{
			return D3DERR_NOTAVAILABLE;
		}
		else
		{
			return D3DERR_INVALIDCALL;
		}
	}

	HRESULT result = D3D_OK;

	switch (DisplayFormat)
	{
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A2R10G10B10:
	case D3DFMT_A2B10G10R10:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_R5G6B5:
		//It's fine
	default:
		//It's not fine
		result = D3DERR_NOTAVAILABLE;
		break;
	}

	switch (BackBufferFormat)
	{
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A2R10G10B10:
	case D3DFMT_A2B10G10R10:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_R5G6B5:
		//It's fine
	default:
		//It's not fine
		result = D3DERR_NOTAVAILABLE;
		break;
	}

	return result;
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

	//TODO: revisit - I'll need to test with some real titles to see if multiple adapters break it.

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

	if (Adapter >= GetAdapterCount())
	{
		return 0;
	}

	auto device = mPhysicalDevices[Adapter];
	vk::PhysicalDeviceProperties properties;
	device.getProperties(&properties);

	(*pIdentifier) = {}; //zero it out.

	//TODO: currently reporting a static card rather than detecting or giving the user an option.
	strcpy(pIdentifier->DeviceName, "Radeon (TM) RX 480 Graphics"); //properties.deviceName
	strcpy(pIdentifier->Driver, "aticfx64.dll,aticfx64.dll,aticfx64.dll,aticfx32,aticfx32,aticfx32,atiumd64.dll,atidxx64.dll,atidxx64.dll,atiumdag,atidxx32,atidxx32,atiumdva,atiumd6a.cap,atitmm64.dll"); //revisit
	strcpy(pIdentifier->Description, "VK9 Emulated Device"); //revisit
	pIdentifier->VendorId = 0x1002;  //properties.vendorID;
	pIdentifier->DeviceId = 0x67DF;  //properties.deviceID;
	pIdentifier->SubSysId = 0x94801682;
	pIdentifier->Revision = 0x00C7;
	pIdentifier->WHQLLevel = 1;

	pIdentifier->DriverVersion.QuadPart = properties.driverVersion;

	//little endian assumption.
	CustomGUID guid = { 0xD7B71EE2 , 0x249F , 0x11CF , 0x3275 , 0x17B57BC2D835 };
	pIdentifier->DeviceIdentifier = bit_cast<GUID>(guid);

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

	auto instance = mInstance.get();
	auto device = mPhysicalDevices[mPhysicalDeviceIndex];

	vk::PhysicalDeviceProperties properties;
	vk::PhysicalDeviceFeatures features;

	device.getProperties(&properties);
	device.getFeatures(&features);

	/*
	https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkPhysicalDeviceProperties
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172513(v=vs.85).aspx
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172635(v=vs.85).aspx
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172591(v=vs.85).aspx
	*/

	(*pCaps) = {};

	//Re-worked Caps based off of SwiftGL, ToGL, Nine, Wine, MS Docs, and local implementation.
	pCaps->DeviceType = DeviceType;

	pCaps->AdapterOrdinal = 0;

	pCaps->Caps = D3DCAPS_READ_SCANLINE; //Based on ToGL & Swift this is DX7 stuff.

	pCaps->Caps2 = D3DCAPS2_CANMANAGERESOURCE |
		/* D3DCAPS2_CANSHARERESOURCE | */
		/* D3DCAPS2_CANCALIBRATEGAMMA | */
		D3DCAPS2_DYNAMICTEXTURES |
		D3DCAPS2_FULLSCREENGAMMA |
		D3DCAPS2_CANAUTOGENMIPMAP;

	pCaps->Caps3 = D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD |
		D3DCAPS3_COPY_TO_VIDMEM |
		D3DCAPS3_COPY_TO_SYSTEMMEM |
		D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION;

	pCaps->Caps3 = D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD |
		D3DCAPS3_COPY_TO_VIDMEM |
		D3DCAPS3_COPY_TO_SYSTEMMEM |
		D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION;

	pCaps->PresentationIntervals = D3DPRESENT_INTERVAL_DEFAULT |
		D3DPRESENT_INTERVAL_ONE |
		D3DPRESENT_INTERVAL_TWO |
		D3DPRESENT_INTERVAL_THREE |
		D3DPRESENT_INTERVAL_FOUR |
		D3DPRESENT_INTERVAL_IMMEDIATE;

	pCaps->CursorCaps = D3DCURSORCAPS_COLOR | D3DCURSORCAPS_LOWRES;

	pCaps->DevCaps = D3DDEVCAPS_CANBLTSYSTONONLOCAL |
		D3DDEVCAPS_CANRENDERAFTERFLIP |
		D3DDEVCAPS_DRAWPRIMITIVES2 |
		D3DDEVCAPS_DRAWPRIMITIVES2EX |
		D3DDEVCAPS_DRAWPRIMTLVERTEX |
		D3DDEVCAPS_EXECUTESYSTEMMEMORY |
		D3DDEVCAPS_EXECUTEVIDEOMEMORY |
		D3DDEVCAPS_HWRASTERIZATION |
		D3DDEVCAPS_HWTRANSFORMANDLIGHT |
		/*D3DDEVCAPS_NPATCHES |*/
		D3DDEVCAPS_PUREDEVICE |
		/*D3DDEVCAPS_QUINTICRTPATCHES |*/
		/*D3DDEVCAPS_RTPATCHES |*/
		/*D3DDEVCAPS_RTPATCHHANDLEZERO |*/
		/*D3DDEVCAPS_SEPARATETEXTUREMEMORIES |*/
		D3DDEVCAPS_TEXTURENONLOCALVIDMEM |
		/* D3DDEVCAPS_TEXTURESYSTEMMEMORY |*/
		D3DDEVCAPS_TEXTUREVIDEOMEMORY |
		D3DDEVCAPS_TLVERTEXSYSTEMMEMORY |
		D3DDEVCAPS_TLVERTEXVIDEOMEMORY;

	pCaps->PrimitiveMiscCaps = D3DPMISCCAPS_MASKZ |	// Device can enable and disable modification of the depth buffer on pixel operations.
		D3DPMISCCAPS_CULLNONE |						// The driver does not perform triangle culling. This corresponds to the D3DCULL_NONE member of the D3DCULL enumerated type.
		D3DPMISCCAPS_CULLCW |						// The driver supports clockwise triangle culling through the D3DRS_CULLMODE state. (This applies only to triangle primitives.) This flag corresponds to the D3DCULL_CW member of the D3DCULL enumerated type.
		D3DPMISCCAPS_CULLCCW |						// The driver supports counterclockwise culling through the D3DRS_CULLMODE state. (This applies only to triangle primitives.) This flag corresponds to the D3DCULL_CCW member of the D3DCULL enumerated type.
		D3DPMISCCAPS_COLORWRITEENABLE |				// Device supports per-channel writes for the render-target color buffer through the D3DRS_COLORWRITEENABLE state.
		D3DPMISCCAPS_CLIPPLANESCALEDPOINTS |		// Device correctly clips scaled points of size greater than 1.0 to user-defined clipping planes.
		D3DPMISCCAPS_CLIPTLVERTS |					// Device clips post-transformed vertex primitives. Specify D3DUSAGE_DONOTCLIP when the pipeline should not do any clipping. For this case, additional software clipping may need to be performed at draw time, requiring the vertex buffer to be in system memory.
		D3DPMISCCAPS_TSSARGTEMP |					// Device supports D3DTA for temporary register.
		D3DPMISCCAPS_BLENDOP |						// Device supports alpha-blending operations other than D3DBLENDOP_ADD.
	//	D3DPMISCCAPS_NULLREFERENCE |				// A reference device that does not render.
		D3DPMISCCAPS_INDEPENDENTWRITEMASKS |		// Device supports independent write masks for multiple element textures or multiple render targets.
		D3DPMISCCAPS_PERSTAGECONSTANT |				// Device supports per-stage constants. See D3DTSS_CONSTANT in D3DTEXTURESTAGESTATETYPE.
		D3DPMISCCAPS_FOGANDSPECULARALPHA |			// Device supports separate fog and specular alpha. Many devices use the specular alpha channel to store the fog factor.
		D3DPMISCCAPS_SEPARATEALPHABLEND |			// Device supports separate blend settings for the alpha channel.
		D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS |		// Device supports different bit depths for multiple render targets.
		D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING |	// Device supports post-pixel shader operations for multiple render targets.
		D3DPMISCCAPS_FOGVERTEXCLAMPED;				// Device clamps fog blend factor per vertex.

	pCaps->RasterCaps = D3DPRASTERCAPS_ANISOTROPY |	// Device supports anisotropic filtering.
		D3DPRASTERCAPS_COLORPERSPECTIVE |			// Device iterates colors perspective correctly.
	//	D3DPRASTERCAPS_DITHER |						// Device can dither to improve color resolution.
		D3DPRASTERCAPS_DEPTHBIAS |					// Device supports legacy depth bias. For true depth bias, see D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS.
		D3DPRASTERCAPS_FOGRANGE |					// Device supports range-based fog. In range-based fog, the distance of an object from the viewer is used to compute fog effects, not the depth of the object (that is, the z-coordinate) in the scene.
		D3DPRASTERCAPS_FOGTABLE |					// Device calculates the fog value by referring to a lookup table containing fog values that are indexed to the depth of a given pixel.
		D3DPRASTERCAPS_FOGVERTEX |					// Device calculates the fog value during the lighting operation and interpolates the fog value during rasterization.
		D3DPRASTERCAPS_MIPMAPLODBIAS |				// Device supports level of detail (LOD) bias adjustments. These bias adjustments enable an application to make a mipmap appear crisper or less sharp than it normally would. For more information about LOD bias in mipmaps, see D3DSAMP_MIPMAPLODBIAS.
	//	D3DPRASTERCAPS_MULTISAMPLE_TOGGLE |			// Device supports toggling multisampling on and off between IDirect3DDevice9::BeginScene and IDirect3DDevice9::EndScene (using D3DRS_MULTISAMPLEANTIALIAS).
		D3DPRASTERCAPS_SCISSORTEST |				// Device supports scissor test. See Scissor Test.
		D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS |		// Device performs true slope-scale based depth bias. This is in contrast to the legacy style D3DPRASTERCAPS_DEPTHBIAS.
	//	D3DPRASTERCAPS_WBUFFER |					// Device supports depth buffering using w.
		D3DPRASTERCAPS_WFOG |						// Device supports w-based fog. W-based fog is used when a perspective projection matrix is specified, but affine projections still use z-based fog. The system considers a projection matrix that contains a nonzero value in the [3][4] element to be a perspective projection matrix.
	//	D3DPRASTERCAPS_ZBUFFERLESSHSR |				// Device can perform hidden-surface removal (HSR) without requiring the application to sort polygons and without requiring the allocation of a depth-buffer. This leaves more video memory for textures. The method used to perform HSR is hardware-dependent and is transparent to the application. Z-bufferless HSR is performed if no depth-buffer surface is associated with the rendering-target surface and the depth-buffer comparison test is enabled (that is, when the state value associated with the D3DRS_ZENABLE enumeration constant is set to TRUE).
		D3DPRASTERCAPS_ZFOG |						// Device supports z-based fog.
		D3DPRASTERCAPS_ZTEST;						// Device can perform z-test operations. This effectively renders a primitive and indicates whether any z pixels have been rendered.

	pCaps->ZCmpCaps = D3DPCMPCAPS_NEVER |
		D3DPCMPCAPS_LESS |
		D3DPCMPCAPS_EQUAL |
		D3DPCMPCAPS_LESSEQUAL |
		D3DPCMPCAPS_GREATER |
		D3DPCMPCAPS_NOTEQUAL |
		D3DPCMPCAPS_GREATEREQUAL |
		D3DPCMPCAPS_ALWAYS;

	pCaps->SrcBlendCaps = D3DPBLENDCAPS_BLENDFACTOR |		// The driver supports both D3DBLEND_BLENDFACTOR and D3DBLEND_INVBLENDFACTOR. See D3DBLEND.
		D3DPBLENDCAPS_BOTHINVSRCALPHA |	// Source blend factor is (1-As,1-As,1-As,1-As) and destination blend factor is (As,As,As,As); the destination blend selection is overridden.
		D3DPBLENDCAPS_BOTHSRCALPHA |	// The driver supports the D3DBLEND_BOTHSRCALPHA blend mode. (This blend mode is obsolete. For more information, see D3DBLEND.)
		D3DPBLENDCAPS_DESTALPHA |		// Blend factor is (Ad, Ad, Ad, Ad).
		D3DPBLENDCAPS_DESTCOLOR |		// Blend factor is (Rd, Gd, Bd, Ad).
		D3DPBLENDCAPS_INVDESTALPHA |	// Blend factor is (1-Ad, 1-Ad, 1-Ad, 1-Ad).
		D3DPBLENDCAPS_INVDESTCOLOR |	// Blend factor is (1-Rd, 1-Gd, 1-Bd, 1-Ad).
		D3DPBLENDCAPS_INVSRCALPHA |		// Blend factor is (1-As, 1-As, 1-As, 1-As).
		D3DPBLENDCAPS_INVSRCCOLOR |		// Blend factor is (1-Rs, 1-Gs, 1-Bs, 1-As).
		D3DPBLENDCAPS_ONE |				// Blend factor is (1, 1, 1, 1).
		D3DPBLENDCAPS_SRCALPHA |		// Blend factor is (As, As, As, As).
		D3DPBLENDCAPS_SRCALPHASAT |		// Blend factor is (f, f, f, 1); f = min(As, 1-Ad).
		D3DPBLENDCAPS_SRCCOLOR |		// Blend factor is (Rs, Gs, Bs, As).
		D3DPBLENDCAPS_ZERO;				// Blend factor is (0, 0, 0, 0).

	pCaps->DestBlendCaps = pCaps->SrcBlendCaps;

	pCaps->AlphaCmpCaps = D3DPCMPCAPS_NEVER |
		D3DPCMPCAPS_LESS |
		D3DPCMPCAPS_EQUAL |
		D3DPCMPCAPS_LESSEQUAL |
		D3DPCMPCAPS_GREATER |
		D3DPCMPCAPS_NOTEQUAL |
		D3DPCMPCAPS_GREATEREQUAL |
		D3DPCMPCAPS_ALWAYS;

	pCaps->ShadeCaps = D3DPSHADECAPS_COLORGOURAUDRGB |
		D3DPSHADECAPS_SPECULARGOURAUDRGB |
		D3DPSHADECAPS_ALPHAGOURAUDBLEND |
		D3DPSHADECAPS_FOGGOURAUD;

	pCaps->TextureCaps = D3DPTEXTURECAPS_ALPHA |	// Alpha in texture pixels is supported.
		D3DPTEXTURECAPS_ALPHAPALETTE |				// Device can draw alpha from texture palettes.
		D3DPTEXTURECAPS_CUBEMAP |					// Supports cube textures.
	//	D3DPTEXTURECAPS_CUBEMAP_POW2 |				// Device requires that cube texture maps have dimensions specified as powers of two.
		D3DPTEXTURECAPS_MIPCUBEMAP |				// Device supports mipmapped cube textures.
		D3DPTEXTURECAPS_MIPMAP |					// Device supports mipmapped textures.
		D3DPTEXTURECAPS_MIPVOLUMEMAP |				// Device supports mipmapped volume textures.
	//	D3DPTEXTURECAPS_NONPOW2CONDITIONAL |		// Conditionally supports the use of 2-D textures with dimensions that are not powers of two. A device that exposes this capability can use such a texture if all of the following requirements are met...
	//	D3DPTEXTURECAPS_NOPROJECTEDBUMPENV |		// Device does not support a projected bump-environment loopkup operation in programmable and fixed function shaders.
		D3DPTEXTURECAPS_PERSPECTIVE |				// Perspective correction texturing is supported.
	//	D3DPTEXTURECAPS_POW2 |						// All textures must have widths and heights specified as powers of two. This requirement does not apply to either cube textures or volume textures.
		D3DPTEXTURECAPS_PROJECTED |					// Supports the D3DTTFF_PROJECTED texture transformation flag. When applied, the device divides transformed texture coordinates by the last texture coordinate. If this capability is present, then the projective divide occurs per pixel. If this capability is not present, but the projective divide needs to occur anyway, then it is performed on a per-vertex basis by the Direct3D runtime.
	//	D3DPTEXTURECAPS_SQUAREONLY |				// All textures must be square.
		D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE |	// Texture indices are not scaled by the texture size prior to interpolation.
		D3DPTEXTURECAPS_VOLUMEMAP;					// Device supports volume textures.
	//	D3DPTEXTURECAPS_VOLUMEMAP_POW2;				// Device requires that volume texture maps have dimensions specified as powers of two.

	pCaps->TextureFilterCaps = D3DPTFILTERCAPS_MAGFPOINT |	// Device supports per-stage point-sample filtering for magnifying textures. The point-sample magnification filter is represented by the D3DTEXF_POINT member of the D3DTEXTUREFILTERTYPE enumerated type.
		D3DPTFILTERCAPS_MAGFLINEAR |						// Device supports per-stage bilinear interpolation filtering for magnifying mipmaps. The bilinear interpolation mipmapping filter is represented by the D3DTEXF_LINEAR member of the D3DTEXTUREFILTERTYPE enumerated type.
	//	D3DPTFILTERCAPS_MAGFANISOTROPIC |					// Device supports per-stage anisotropic filtering for magnifying textures. The anisotropic magnification filter is represented by the D3DTEXF_ANISOTROPIC member of the D3DTEXTUREFILTERTYPE enumerated type.
	//	D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD |					// Device supports per-stage pyramidal sample filtering for magnifying textures. The pyramidal magnifying filter is represented by the D3DTEXF_PYRAMIDALQUAD member of the D3DTEXTUREFILTERTYPE enumerated type.
	//	D3DPTFILTERCAPS_MAGFGAUSSIANQUAD |					// Device supports per-stage Gaussian quad filtering for magnifying textures.
		D3DPTFILTERCAPS_MINFPOINT |							// Device supports per-stage point-sample filtering for minifying textures. The point-sample minification filter is represented by the D3DTEXF_POINT member of the D3DTEXTUREFILTERTYPE enumerated type.
		D3DPTFILTERCAPS_MINFLINEAR |						// Device supports per-stage linear filtering for minifying textures. The linear minification filter is represented by the D3DTEXF_LINEAR member of the D3DTEXTUREFILTERTYPE enumerated type.
		D3DPTFILTERCAPS_MINFANISOTROPIC |					// Device supports per-stage anisotropic filtering for minifying textures. The anisotropic minification filter is represented by the D3DTEXF_ANISOTROPIC member of the D3DTEXTUREFILTERTYPE enumerated type.
	//	D3DPTFILTERCAPS_MINFPYRAMIDALQUAD |					// Device supports per-stage pyramidal sample filtering for minifying textures.
	//	D3DPTFILTERCAPS_MINFGAUSSIANQUAD |					// Device supports per-stage Gaussian quad filtering for minifying textures.
		D3DPTFILTERCAPS_MIPFPOINT |							// Device supports per-stage point-sample filtering for mipmaps. The point-sample mipmapping filter is represented by the D3DTEXF_POINT member of the D3DTEXTUREFILTERTYPE enumerated type.
		D3DPTFILTERCAPS_MIPFLINEAR;							// Device supports per-stage bilinear interpolation filtering for mipmaps. The bilinear interpolation mipmapping filter is represented by the D3DTEXF_LINEAR member of the D3DTEXTUREFILTERTYPE enumerated type.

	pCaps->CubeTextureFilterCaps = pCaps->TextureFilterCaps;

	pCaps->VolumeTextureFilterCaps = pCaps->TextureFilterCaps;

	pCaps->TextureAddressCaps = D3DPTADDRESSCAPS_BORDER |	// Device supports setting coordinates outside the range [0.0, 1.0] to the border color, as specified by the D3DSAMP_BORDERCOLOR texture-stage state.
		D3DPTADDRESSCAPS_CLAMP |							// Device can clamp textures to addresses.
		D3DPTADDRESSCAPS_INDEPENDENTUV |					// Device can separate the texture-addressing modes of the u and v coordinates of the texture. This ability corresponds to the D3DSAMP_ADDRESSU and D3DSAMP_ADDRESSV render-state values.
		D3DPTADDRESSCAPS_MIRROR |							// Device can mirror textures to addresses.
		D3DPTADDRESSCAPS_MIRRORONCE |						// Device can take the absolute value of the texture coordinate (thus, mirroring around 0) and then clamp to the maximum value.
		D3DPTADDRESSCAPS_WRAP;								// Device can wrap textures to addresses.

	pCaps->VolumeTextureAddressCaps = pCaps->TextureAddressCaps;

	pCaps->LineCaps =
		D3DLINECAPS_ALPHACMP |
		D3DLINECAPS_BLEND |
		D3DLINECAPS_TEXTURE |
		D3DLINECAPS_ZTEST |
		D3DLINECAPS_FOG |
		D3DLINECAPS_ANTIALIAS;

	pCaps->MaxTextureWidth = properties.limits.maxImageDimension2D;
	pCaps->MaxTextureHeight = pCaps->MaxTextureWidth;
	pCaps->MaxVolumeExtent = properties.limits.maxImageDimension3D;
	pCaps->MaxTextureRepeat = 32768;
	pCaps->MaxTextureAspectRatio = 0; //pCaps->MaxTextureWidth;
	pCaps->MaxAnisotropy = features.samplerAnisotropy;

	//TODO: revisit guard values.
	pCaps->MaxVertexW = 1e10f;
	pCaps->GuardBandLeft = -1e9f;
	pCaps->GuardBandTop = -1e9f;
	pCaps->GuardBandRight = 1e9f;
	pCaps->GuardBandBottom = 1e9f;
	pCaps->ExtentsAdjust = 0.0f;

	pCaps->StencilCaps =
		D3DSTENCILCAPS_KEEP |
		D3DSTENCILCAPS_ZERO |
		D3DSTENCILCAPS_REPLACE |
		D3DSTENCILCAPS_INCRSAT |
		D3DSTENCILCAPS_DECRSAT |
		D3DSTENCILCAPS_INVERT |
		D3DSTENCILCAPS_INCR |
		D3DSTENCILCAPS_DECR |
		D3DSTENCILCAPS_TWOSIDED;

	pCaps->FVFCaps =
		8 | /* 8 textures max */
		/*D3DFVFCAPS_DONOTSTRIPELEMENTS |*/
		D3DFVFCAPS_PSIZE;

	pCaps->TextureOpCaps = D3DTEXOPCAPS_DISABLE |
		D3DTEXOPCAPS_SELECTARG1 |
		D3DTEXOPCAPS_SELECTARG2 |
		D3DTEXOPCAPS_MODULATE |
		D3DTEXOPCAPS_MODULATE2X |
		D3DTEXOPCAPS_MODULATE4X |
		D3DTEXOPCAPS_ADD |
		D3DTEXOPCAPS_ADDSIGNED |
		D3DTEXOPCAPS_ADDSIGNED2X |
		D3DTEXOPCAPS_SUBTRACT |
		D3DTEXOPCAPS_ADDSMOOTH |
		D3DTEXOPCAPS_BLENDDIFFUSEALPHA |
		D3DTEXOPCAPS_BLENDTEXTUREALPHA |
		D3DTEXOPCAPS_BLENDFACTORALPHA |
		D3DTEXOPCAPS_BLENDTEXTUREALPHAPM |
		D3DTEXOPCAPS_BLENDCURRENTALPHA |
		D3DTEXOPCAPS_PREMODULATE |
		D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR |
		D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA |
		D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR |
		D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA |
		D3DTEXOPCAPS_BUMPENVMAP |
		D3DTEXOPCAPS_BUMPENVMAPLUMINANCE |
		D3DTEXOPCAPS_DOTPRODUCT3 |
		D3DTEXOPCAPS_MULTIPLYADD |
		D3DTEXOPCAPS_LERP;

	pCaps->MaxTextureBlendStages = 8;
	pCaps->MaxSimultaneousTextures = 8;

	pCaps->VertexProcessingCaps = D3DVTXPCAPS_TEXGEN |
		D3DVTXPCAPS_TEXGEN_SPHEREMAP |
		D3DVTXPCAPS_MATERIALSOURCE7 |
		D3DVTXPCAPS_DIRECTIONALLIGHTS |
		D3DVTXPCAPS_POSITIONALLIGHTS |
		D3DVTXPCAPS_LOCALVIEWER |
		D3DVTXPCAPS_TWEENING;
	/*D3DVTXPCAPS_NO_TEXGEN_NONLOCALVIEWER*/

	pCaps->MaxActiveLights = 8;
	pCaps->MaxUserClipPlanes = 6; //TODO: I need to look into this one.
	pCaps->MaxVertexBlendMatrices = 3; //Nine & Swift uses 4 but I only have 3 slots with my binding mapping.
	pCaps->MaxVertexBlendMatrixIndex = 8;
	pCaps->MaxPointSize = properties.limits.pointSizeRange[1]; //[0] min [1] max
	pCaps->MaxPrimitiveCount = 0x555555;
	pCaps->MaxVertexIndex = 0xFFFFFF;
	pCaps->MaxStreams = std::max((uint32_t)1, std::min(properties.limits.maxVertexInputBindings, (uint32_t)16));
	pCaps->MaxStreamStride = std::min(properties.limits.maxVertexInputBindingStride, (uint32_t)65536);
	pCaps->VertexShaderVersion = D3DVS_VERSION(3, 0);
	pCaps->PixelShaderVersion = D3DPS_VERSION(3, 0);
	pCaps->MaxVertexShaderConst = 256;
	pCaps->PixelShader1xMaxValue = 65504.f;

	pCaps->DevCaps2 = D3DDEVCAPS2_STREAMOFFSET |
		D3DDEVCAPS2_VERTEXELEMENTSCANSHARESTREAMOFFSET |
		D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES;
	/*D3DDEVCAPS2_DMAPNPATCH |*/
	/*D3DDEVCAPS2_ADAPTIVETESSRTPATCH |*/
	/*D3DDEVCAPS2_ADAPTIVETESSNPATCH |*/
	/*D3DDEVCAPS2_PRESAMPLEDDMAPNPATCH*/

	pCaps->MasterAdapterOrdinal = 0;
	pCaps->AdapterOrdinalInGroup = 0;
	pCaps->NumberOfAdaptersInGroup = 1; //TODO: need to look into groups.

	//TODO: need to look at these in a few more implementations.
	pCaps->MaxNpatchTessellationLevel = 0.0f;
	pCaps->Reserved5 = 0;

	pCaps->DeclTypes = D3DDTCAPS_UBYTE4 |
		D3DDTCAPS_UBYTE4N |
		D3DDTCAPS_SHORT2N |
		D3DDTCAPS_SHORT4N |
		D3DDTCAPS_USHORT2N |
		D3DDTCAPS_USHORT4N |
		D3DDTCAPS_UDEC3 |
		D3DDTCAPS_DEC3N |
		D3DDTCAPS_FLOAT16_2 |
		D3DDTCAPS_FLOAT16_4;

	//TODO: Look into real target limits. All of my cases use one or two targets.
	pCaps->NumSimultaneousRTs = std::max((uint32_t)1, properties.limits.maxColorAttachments);

	pCaps->StretchRectFilterCaps = D3DPTFILTERCAPS_MINFPOINT |
		D3DPTFILTERCAPS_MINFLINEAR |
		D3DPTFILTERCAPS_MAGFPOINT |
		D3DPTFILTERCAPS_MAGFLINEAR;

	pCaps->VS20Caps.Caps = D3DVS20CAPS_PREDICATION;
	pCaps->VS20Caps.DynamicFlowControlDepth = D3DVS20_MAX_DYNAMICFLOWCONTROLDEPTH;
	pCaps->VS20Caps.NumTemps = D3DVS20_MAX_NUMTEMPS;
	pCaps->VS20Caps.StaticFlowControlDepth = D3DVS20_MAX_STATICFLOWCONTROLDEPTH;

	pCaps->PS20Caps.Caps = D3DPS20CAPS_ARBITRARYSWIZZLE |
		D3DPS20CAPS_GRADIENTINSTRUCTIONS |
		D3DPS20CAPS_PREDICATION |
		D3DPS20CAPS_NOTEXINSTRUCTIONLIMIT |
		D3DPS20CAPS_NODEPENDENTREADLIMIT;
	pCaps->PS20Caps.DynamicFlowControlDepth = D3DPS20_MAX_DYNAMICFLOWCONTROLDEPTH;
	pCaps->PS20Caps.NumTemps = D3DPS20_MAX_NUMTEMPS;
	pCaps->PS20Caps.StaticFlowControlDepth = D3DPS20_MAX_STATICFLOWCONTROLDEPTH;
	pCaps->PS20Caps.NumInstructionSlots = D3DPS20_MAX_NUMINSTRUCTIONSLOTS;

	pCaps->VertexTextureFilterCaps = pCaps->TextureFilterCaps &
		~(D3DPTFILTERCAPS_MIPFPOINT |
			D3DPTFILTERCAPS_MIPFPOINT);

	//I divide by MAX_DXBC_PHASES because for 1.x they can use PHASE to upgrade to 2.0 and both phases have their own limit.
	DWORD estimatedMaxInstructions = (SPIR_V_MAX_ID / ESTIMATED_SPIR_V_PER_DXBC_OP / MAX_DXBC_PHASES);
	pCaps->MaxVertexShader30InstructionSlots = std::max((DWORD)D3DMIN30SHADERINSTRUCTIONS, std::min((DWORD)D3DMAX30SHADERINSTRUCTIONS, estimatedMaxInstructions));
	pCaps->MaxPixelShader30InstructionSlots = std::max((DWORD)D3DMIN30SHADERINSTRUCTIONS, std::min((DWORD)D3DMAX30SHADERINSTRUCTIONS, estimatedMaxInstructions));

	pCaps->MaxVShaderInstructionsExecuted = std::max((DWORD)65535, pCaps->MaxVertexShader30InstructionSlots * 32);
	pCaps->MaxPShaderInstructionsExecuted = std::max((DWORD)65535, pCaps->MaxPixelShader30InstructionSlots * 32);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE C9::RegisterSoftwareDevice(void *pInitializeFunction)
{
	//TODO: Do some research to see if any pluggable software devices even exist.

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

	return result;
}

HRESULT STDMETHODCALLTYPE C9::GetAdapterLUID(UINT Adapter, LUID *pLUID)
{
	Log(info) << "C9::GetAdapterLUID Adapter: " << Adapter << std::endl;

	if (Adapter >= GetAdapterCount())
	{
		return D3DERR_INVALIDCALL;
	}

	Monitor& monitor = mMonitors[Adapter];

	(*pLUID) = monitor.LUID;

	return S_OK;
}