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

#include <vector>

#include "renderdoc_app.h"

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* layerPrefix, const char* message, void* userData);

static PFN_vkCmdPushDescriptorSetKHR pfn_vkCmdPushDescriptorSetKHR;
VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSetKHR(
	VkCommandBuffer                             commandBuffer,
	VkPipelineBindPoint                         pipelineBindPoint,
	VkPipelineLayout                            layout,
	uint32_t                                    set,
	uint32_t                                    descriptorWriteCount,
	const VkWriteDescriptorSet*                 pDescriptorWrites);

static PFN_vkCreateDebugReportCallbackEXT pfn_vkCreateDebugReportCallbackEXT;
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(
	VkInstance                                  instance,
	const VkDebugReportCallbackCreateInfoEXT*   pCreateInfo,
	const VkAllocationCallbacks*                pAllocator,
	VkDebugReportCallbackEXT*                   pCallback);

static PFN_vkDestroyDebugReportCallbackEXT pfn_vkDestroyDebugReportCallbackEXT;
VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(
	VkInstance                                  instance,
	VkDebugReportCallbackEXT                    callback,
	const VkAllocationCallbacks*                pAllocator);

static PFN_vkDebugReportMessageEXT pfn_vkDebugReportMessageEXT;
VKAPI_ATTR void VKAPI_CALL vkDebugReportMessageEXT(
	VkInstance                                  instance,
	VkDebugReportFlagsEXT                       flags,
	VkDebugReportObjectTypeEXT                  objectType,
	uint64_t                                    object,
	size_t                                      location,
	int32_t                                     messageCode,
	const char*                                 pLayerPrefix,
	const char*                                 pMessage);

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

void LTrim(std::string &s);
void RTrim(std::string &s);
void Trim(std::string &s);

struct Monitor
{
	HMONITOR hMonitor = NULL;
	HDC hdcMonitor = NULL;
	uint32_t Height = 0;
	uint32_t Width = 0;
	uint32_t RefreshRate = 0;
	uint32_t PixelBits = 0;
	uint32_t ColorPlanes = 0;

	LUID LUID = {};

	std::vector<D3DDISPLAYMODE> Modes;
	std::vector<D3DDISPLAYMODEEX> ExModes;
};

struct CustomGUID
{
	DWORD Data1;
	WORD  Data2;
	WORD  Data3;
	WORD  Data4;
	uint64_t  Data5; //really this is 6 but we'll just truncate.
};

class C9 : public IDirect3D9Ex
{
public:
	C9();
	~C9();

	//Reference Counting
	ULONG mReferenceCount = 1;
	ULONG mPrivateReferenceCount = 0;

	ULONG PrivateAddRef(void);
	ULONG PrivateRelease(void);

	//Monitor stuff
	uint32_t mDisplayCount = 0;	
	std::vector<Monitor> mMonitors;	

	//Vulkan
	vk::UniqueInstance mInstance;
	vk::UniqueDebugReportCallbackEXT mCallback;
	std::vector<vk::PhysicalDevice> mPhysicalDevices;
	size_t mPhysicalDeviceIndex;
	vk::PhysicalDeviceProperties mPhysicalDeviceProperties;
	vk::PhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties;
	std::vector<vk::QueueFamilyProperties> mQueueFamilyProperties;
	size_t mGraphicsQueueFamilyIndex;

	//RenderDoc
	HMODULE mRenderDocDll = nullptr;
	RENDERDOC_API_1_1_1* mRenderDocApi = nullptr;

	//Configuration
	std::map<std::string, std::string> mConfiguration;
	void LoadConfiguration(std::string filename);
	std::string mGameName;

public:
	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void  **ppv);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);	
	virtual ULONG STDMETHODCALLTYPE Release(void);

	//IDirect3D9
	virtual HRESULT STDMETHODCALLTYPE CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat);
	virtual HRESULT STDMETHODCALLTYPE CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat);
	virtual HRESULT STDMETHODCALLTYPE CheckDeviceFormatConversion(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat);
	virtual HRESULT STDMETHODCALLTYPE CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD *pQualityLevels);
	virtual HRESULT STDMETHODCALLTYPE CheckDeviceType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL Windowed);
	virtual HRESULT STDMETHODCALLTYPE CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface);
	virtual HRESULT STDMETHODCALLTYPE EnumAdapterModes(UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE *pMode);
	virtual UINT STDMETHODCALLTYPE GetAdapterCount();
	virtual HRESULT STDMETHODCALLTYPE GetAdapterDisplayMode(UINT Adapter,D3DDISPLAYMODE *pMode);
	virtual HRESULT STDMETHODCALLTYPE GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9 *pIdentifier);
	virtual UINT STDMETHODCALLTYPE GetAdapterModeCount(UINT Adapter,D3DFORMAT Format);
	virtual HMONITOR STDMETHODCALLTYPE GetAdapterMonitor(UINT Adapter);
	virtual HRESULT STDMETHODCALLTYPE GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9 *pCaps);
	virtual HRESULT STDMETHODCALLTYPE RegisterSoftwareDevice(void *pInitializeFunction);

	//IDirect3D9Ex
	virtual UINT STDMETHODCALLTYPE GetAdapterModeCountEx(UINT Adapter, const D3DDISPLAYMODEFILTER *pFilter);
	virtual HRESULT STDMETHODCALLTYPE EnumAdapterModesEx(UINT Adapter, const D3DDISPLAYMODEFILTER *pFilter, UINT Mode, D3DDISPLAYMODEEX *pMode);
	virtual HRESULT STDMETHODCALLTYPE GetAdapterDisplayModeEx(UINT Adapter, D3DDISPLAYMODEEX *pMode, D3DDISPLAYROTATION *pRotation);
	virtual HRESULT STDMETHODCALLTYPE CreateDeviceEx(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode, IDirect3DDevice9Ex **ppReturnedDeviceInterface);
	virtual HRESULT STDMETHODCALLTYPE GetAdapterLUID(UINT Adapter, LUID *pLUID);
};
