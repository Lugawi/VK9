// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
Copyright(c) 2018 Christopher Joseph Dean Schaefer

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

#define MAX_DESCRIPTOR 2048

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#include "Perf_StateManager.h"

#include "C9.h"
#include "CDevice9.h"
#include "CStateBlock9.h"
#include "CTexture9.h"
#include "CVolume9.h"
#include "CCubeTexture9.h"
#include "CVolumeTexture9.h"
#include "CVertexBuffer9.h"
#include "CIndexBuffer9.h"
#include "CQuery9.h"

#include "Utilities.h"

typedef std::unordered_map<UINT, StreamSource> map_type;

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

StateManager::StateManager(std::map<std::string, std::string>& configuration)
	: mConfiguration(configuration)
{

}

StateManager::~StateManager()
{

}

void StateManager::DestroyDevice(size_t id)
{
	//TODO: figure out a way to stop everything from blowing up when doing reset.
	//if (mDevices.size() == 1)
	//{
	//	mSurfaces.clear();
	//	mSwapChains.clear();
	//}	
	//mDevices[id].reset();
}

void StateManager::CreateDevice(size_t id, void* argument1)
{
	CDevice9* device9 = (CDevice9*)argument1;
	auto instance = mInstances[id];
	auto physicalDevice = instance->mPhysicalDevices[device9->mAdapter];
	auto device = std::make_shared<RealDevice>(this,instance->mInstance, physicalDevice, device9->mPresentationParameters.BackBufferWidth, device9->mPresentationParameters.BackBufferHeight,(instance->mRenderDocApi!=nullptr));

	if (pfn_vkCmdPushDescriptorSetKHR == nullptr)
	{
		pfn_vkCmdPushDescriptorSetKHR = reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(device->mDevice.getProcAddr("vkCmdPushDescriptorSetKHR"));
	}

	//The user wants d3d9 to auto-detect so I'll go ahead and grab the swap and pull it's format.
	auto& presentationParameters = device9->mPresentationParameters;
	bool useVsync = !(presentationParameters.PresentationInterval == D3DPRESENT_DONOTWAIT || presentationParameters.PresentationInterval == D3DPRESENT_FORCEIMMEDIATE || presentationParameters.PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE);

	device->mUseVsync = useVsync;

	if (presentationParameters.BackBufferFormat == D3DFMT_UNKNOWN)
	{
		HWND handle = device9->mFocusWindow;
		auto swapChain = GetSwapChain(device, handle, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight, useVsync);
		presentationParameters.BackBufferFormat = ConvertFormat(swapChain->mSurfaceFormat);				   
	}

	if (presentationParameters.AutoDepthStencilFormat == D3DFMT_UNKNOWN)
	{
		presentationParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
	}

	mDevices.push_back(device);
}

void StateManager::DestroyInstance(size_t id)
{
	mInstances[id].reset();
}

void StateManager::CreateInstance()
{
#ifdef _DEBUG
	bool enableDebugLayers = true;
#else
	bool enableDebugLayers = false;
#endif

	if (!mConfiguration["EnableDebugLayers"].empty())
	{
		enableDebugLayers = std::stoi(mConfiguration["EnableDebugLayers"]);
	}

	auto ptr = std::make_shared<RealInstance>(enableDebugLayers);

	//Get an instance handle.
	auto& vulkanInstance = ptr->mInstance;
	
	if (enableDebugLayers)
	{
		pfn_vkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vulkanInstance.getProcAddr("vkCreateDebugReportCallbackEXT"));
		pfn_vkDebugReportMessageEXT = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vulkanInstance.getProcAddr("vkDebugReportMessageEXT"));
		pfn_vkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vulkanInstance.getProcAddr("vkDestroyDebugReportCallbackEXT"));

		vk::DebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
		callbackCreateInfo.flags = vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::ePerformanceWarning;
		callbackCreateInfo.pfnCallback = &DebugReportCallback;
		callbackCreateInfo.pUserData = this;

		vk::Result result = vulkanInstance.createDebugReportCallbackEXT(&callbackCreateInfo, nullptr, &ptr->mCallback);
		if (result == vk::Result::eSuccess)
		{
			Log(info) << "StateManager::CreateInstance vkCreateDebugReportCallbackEXT succeeded." << std::endl;
		}
		else
		{
			Log(fatal) << "StateManager::CreateInstance vkCreateDebugReportCallbackEXT failed with return code of " << GetResultString((VkResult)result) << std::endl;
			return;
		}
	}

	mInstances.push_back(ptr);


}

void StateManager::DestroyVertexBuffer(size_t id)
{
	mVertexBuffers[id].reset();
}

void StateManager::CreateVertexBuffer(size_t id, void* argument1)
{
	CVertexBuffer9* vertexBuffer9 = bit_cast<CVertexBuffer9*>(argument1);
	auto ptr = std::make_shared<RealVertexBuffer>(mDevices[id].get(), vertexBuffer9->mLength, (vertexBuffer9->mUsage & D3DUSAGE_DYNAMIC) == D3DUSAGE_DYNAMIC, vertexBuffer9->mFVF);

	if (ptr->mSize)
	{
		vertexBuffer9->mSize = ptr->mSize;
	}

	mVertexBuffers.push_back(ptr);
}

void StateManager::DestroyIndexBuffer(size_t id)
{
	mIndexBuffers[id].reset();
}

void StateManager::CreateIndexBuffer(size_t id, void* argument1)
{
	CIndexBuffer9* indexBuffer9 = bit_cast<CIndexBuffer9*>(argument1);
	auto ptr = std::make_shared<RealIndexBuffer>(mDevices[id].get(), indexBuffer9->mLength, (indexBuffer9->mUsage & D3DUSAGE_DYNAMIC) == D3DUSAGE_DYNAMIC, indexBuffer9->mFormat);

	if (ptr->mSize)
	{
		indexBuffer9->mSize = ptr->mSize;
	}

	mIndexBuffers.push_back(ptr);
}

void StateManager::DestroyTexture(size_t id)
{
	mTextures[id].reset();
}

void StateManager::CreateTexture(size_t id, void* argument1)
{
	auto device = mDevices[id];
	CTexture9* texture9 = bit_cast<CTexture9*>(argument1);

	std::shared_ptr<RealTexture> ptr = std::make_shared<RealTexture>(device.get(), texture9);

	mTextures.push_back(ptr);
}

void StateManager::DestroyCubeTexture(size_t id)
{
	mTextures[id].reset();
}

void StateManager::CreateCubeTexture(size_t id, void* argument1)
{
	auto device = mDevices[id];
	CCubeTexture9* texture9 = bit_cast<CCubeTexture9*>(argument1);

	std::shared_ptr<RealTexture> ptr = std::make_shared<RealTexture>(device.get(), texture9);

	mTextures.push_back(ptr);
}

void StateManager::DestroyVolumeTexture(size_t id)
{
	mTextures[id].reset();
}

void StateManager::CreateVolumeTexture(size_t id, void* argument1)
{
	std::shared_ptr<RealDevice> device = mDevices[id];
	CVolumeTexture9* texture9 = bit_cast<CVolumeTexture9*>(argument1);

	std::shared_ptr<RealTexture> ptr = std::make_shared<RealTexture>(device.get(), texture9);

	mTextures.push_back(ptr);
}

void StateManager::DestroySurface(size_t id)
{
	if (mSurfaces.size())
	{
		mSurfaces[id].reset();
	}
}

void StateManager::CreateSurface(size_t id, void* argument1)
{
	auto device = mDevices[id];
	CSurface9* surface9 = bit_cast<CSurface9*>(argument1);
	vk::Image* parentImage = nullptr;

	if (surface9->mTexture != nullptr)
	{
		parentImage = &mTextures[surface9->mTexture->mId]->mImage;
	}

	std::shared_ptr<RealSurface> ptr = std::make_shared<RealSurface>(device.get(), surface9, parentImage);

	mSurfaces.push_back(ptr);
}

void StateManager::DestroyVolume(size_t id)
{
	mVolumes[id].reset();
}

void StateManager::CreateVolume(size_t id, void* argument1)
{
	auto device = mDevices[id];
	CVolume9* volume9 = bit_cast<CVolume9*>(argument1);
	vk::Image* parentImage = nullptr;

	if (volume9->mTexture != nullptr)
	{
		parentImage = &mTextures[volume9->mTexture->mId]->mImage;
	}

	std::shared_ptr<RealVolume> ptr = std::make_shared<RealVolume>(device.get(), volume9, parentImage);

	mVolumes.push_back(ptr);
}

void StateManager::DestroyShader(size_t id)
{
	mShaderConverters[id].reset();
}

void StateManager::CreateShader(size_t id, void* argument1, void* argument2, void* argument3)
{
	//vk::Result result;
	auto device = mDevices[id];
	DWORD* pFunction = (DWORD*)(argument1);
	bool isVertex = (bool)(argument2);
	size_t* size = (size_t*)(argument3);

	if (isVertex)
	{
		std::shared_ptr<ShaderConverter> ptr = std::make_shared<ShaderConverter>(device->mDevice, device->mDeviceState.mVertexShaderConstantSlots);
		ptr->Convert((uint32_t*)pFunction);
		(*size) = ptr->mConvertedShader.Size;
		mShaderConverters.push_back(ptr);
	}
	else
	{
		std::shared_ptr<ShaderConverter> ptr = std::make_shared<ShaderConverter>(device->mDevice, device->mDeviceState.mPixelShaderConstantSlots);
		ptr->Convert((uint32_t*)pFunction);
		(*size) = ptr->mConvertedShader.Size;
		mShaderConverters.push_back(ptr);
	}

}

void StateManager::DestroyQuery(size_t id)
{
	mQueries[id].reset();
}

void StateManager::CreateQuery(size_t id, void* argument1)
{
	auto device = mDevices[id];
	CQuery9* query9 = bit_cast<CQuery9*>(argument1);
	std::shared_ptr<RealQuery> ptr = std::make_shared<RealQuery>(device.get());

	vk::QueryPoolCreateInfo createInfo;
	createInfo.queryCount = 1;

	/*
	case D3DQUERYTYPE_VCACHE:
	break;
	case D3DQUERYTYPE_RESOURCEMANAGER:
	break;
	case D3DQUERYTYPE_VERTEXSTATS:
	break;
	case D3DQUERYTYPE_PIPELINETIMINGS:
	break;
	case D3DQUERYTYPE_INTERFACETIMINGS:
	break;
	case D3DQUERYTYPE_VERTEXTIMINGS:
	break;
	case D3DQUERYTYPE_PIXELTIMINGS:
	break;
	case D3DQUERYTYPE_BANDWIDTHTIMINGS:
	break;
	case D3DQUERYTYPE_CACHEUTILIZATION:
	break;
	case D3DQUERYTYPE_EVENT:
	break;
	*/

	/*
	eInputAssemblyVertices = VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT,
	eInputAssemblyPrimitives = VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT,
	eVertexShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT,
	eGeometryShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_INVOCATIONS_BIT,
	eGeometryShaderPrimitives = VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_PRIMITIVES_BIT,
	eClippingInvocations = VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT,
	eClippingPrimitives = VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT,
	eFragmentShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT,
	eTessellationControlShaderPatches = VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT,
	eTessellationEvaluationShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT,
	eComputeShaderInvocations = VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT
	*/

	switch (query9->mType)
	{
	case D3DQUERYTYPE_OCCLUSION:
		createInfo.queryType = vk::QueryType::eOcclusion;
		break;
	case D3DQUERYTYPE_TIMESTAMP:
		createInfo.queryType = vk::QueryType::eTimestamp;
		break;
	case D3DQUERYTYPE_TIMESTAMPDISJOINT:
		createInfo.queryType = vk::QueryType::eTimestamp;
		break;
	case D3DQUERYTYPE_TIMESTAMPFREQ:
		createInfo.queryType = vk::QueryType::eTimestamp;
		break;
	default:
		createInfo.queryType = vk::QueryType::ePipelineStatistics;
		createInfo.pipelineStatistics = vk::QueryPipelineStatisticFlagBits::eVertexShaderInvocations;
		Log(info) << "StateManager::CreateQuery Unsupported query type " << query9->mType << std::endl;
		break;
	}

	vk::ResultValue<vk::QueryPool> poolResult = device->mDevice.createQueryPool(createInfo, nullptr); //&(ptr->mQueryPool)
	if (poolResult.result != vk::Result::eSuccess)
	{
		Log(fatal) << "StateManager::CreateQuery vkCreateQueryPool failed with return code of " << GetResultString((VkResult)poolResult.result) << std::endl;
		return;
	}
	ptr->mQueryPool = poolResult.value;

	mQueries.push_back(ptr);
}

std::shared_ptr<RealSwapChain> StateManager::GetSwapChain(std::shared_ptr<RealDevice> realDevice, HWND handle, uint32_t width, uint32_t height, bool useVsync)
{
	auto it = mSwapChains.find(handle);
	if (it != mSwapChains.end())
	{
		return (*it).second;
	}
	else
	{
		vk::Instance instance = realDevice->mInstance;
		vk::PhysicalDevice physicalDevice = realDevice->mPhysicalDevice;
		vk::Device device = realDevice->mDevice;
		HWND windowHandle = handle;
		auto& deviceState = realDevice->mDeviceState;

		if (deviceState.mRenderTarget != nullptr && deviceState.mRenderTarget->mColorSurface != nullptr)
		{
			width = deviceState.mRenderTarget->mColorSurface->mExtent.width;
			height = deviceState.mRenderTarget->mColorSurface->mExtent.height;
		}

		auto output = std::make_shared<RealSwapChain>(instance, physicalDevice, device, windowHandle, width, height, useVsync);
		mSwapChains[handle] = output;

		return output;
	}
}