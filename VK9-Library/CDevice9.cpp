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

#ifndef MAX_DESCRIPTOR
#define MAX_DESCRIPTOR 2048u
#endif // !MAX_DESCRIPTOR

#ifndef MAX_BUFFERUPDATE
#define MAX_BUFFERUPDATE 65536u
#endif // !MAX_BUFFERUPDATE

#include <wingdi.h> //used for gamma ramp

#include "C9.h"
#include "CDevice9.h"
#include "CCubeTexture9.h"
#include "CBaseTexture9.h"
#include "CTexture9.h"
#include "CVolumeTexture9.h"
#include "CSurface9.h"
#include "CVertexDeclaration9.h"
#include "CVertexShader9.h"
#include "CStateBlock9.h"
#include "CSwapChain9.h"
#include "CVertexBuffer9.h"
#include "CIndexBuffer9.h"
#include "CPixelShader9.h"
#include "CVertexShader9.h"
#include "CQuery9.h"
#include "CRenderTargetSurface9.h"
#include "CVolume9.h"
#include "LogManager.h"
//#include "PrivateTypes.h"

const uint32_t VERTEX_BUFFER_XYZRHW_VERT[] =
#include "VertexBuffer_XYZRHW.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_VERT[] =
#include "VertexBuffer_XYZ.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_FRAG[] =
#include "VertexBuffer_XYZ.frag.h"
;

const uint32_t VERTEX_BUFFER_XYZRHW_DIFFUSE_VERT[] =
#include "VertexBuffer_XYZRHW_DIFFUSE.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_DIFFUSE_VERT[] =
#include "VertexBuffer_XYZ_DIFFUSE.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_DIFFUSE_FRAG[] =
#include "VertexBuffer_XYZ_DIFFUSE.frag.h"
;
const uint32_t VERTEX_BUFFER_XYZ_DIFFUSE_GEOM[] =
#include "VertexBuffer_XYZ_DIFFUSE.geom.h"
;

const uint32_t VERTEX_BUFFER_XYZRHW_TEX1_VERT[] =
#include "VertexBuffer_XYZRHW_TEX1.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_TEX1_VERT[] =
#include "VertexBuffer_XYZ_TEX1.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_TEX1_FRAG[] =
#include "VertexBuffer_XYZ_TEX1.frag.h"
;

const uint32_t VERTEX_BUFFER_XYZRHW_TEX2_VERT[] =
#include "VertexBuffer_XYZRHW_TEX2.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_TEX2_VERT[] =
#include "VertexBuffer_XYZ_TEX2.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_TEX2_FRAG[] =
#include "VertexBuffer_XYZ_TEX2.frag.h"
;

const uint32_t VERTEX_BUFFER_XYZRHW_DIFFUSE_TEX1_VERT[] =
#include "VertexBuffer_XYZRHW_DIFFUSE_TEX1.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_DIFFUSE_TEX1_VERT[] =
#include "VertexBuffer_XYZ_DIFFUSE_TEX1.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_DIFFUSE_TEX1_FRAG[] =
#include "VertexBuffer_XYZ_DIFFUSE_TEX1.frag.h"
;

const uint32_t VERTEX_BUFFER_XYZRHW_DIFFUSE_TEX2_VERT[] =
#include "VertexBuffer_XYZRHW_DIFFUSE_TEX2.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_DIFFUSE_TEX2_VERT[] =
#include "VertexBuffer_XYZ_DIFFUSE_TEX2.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_DIFFUSE_TEX2_FRAG[] =
#include "VertexBuffer_XYZ_DIFFUSE_TEX2.frag.h"
;

const uint32_t VERTEX_BUFFER_XYZ_NORMAL_VERT[] =
#include "VertexBuffer_XYZ_NORMAL.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_NORMAL_FRAG[] =
#include "VertexBuffer_XYZ_NORMAL.frag.h"
;

const uint32_t VERTEX_BUFFER_XYZ_NORMAL_TEX1_VERT[] =
#include "VertexBuffer_XYZ_NORMAL_TEX1.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_NORMAL_TEX1_FRAG[] =
#include "VertexBuffer_XYZ_NORMAL_TEX1.frag.h"
;

const uint32_t VERTEX_BUFFER_XYZ_NORMAL_TEX2_VERT[] =
#include "VertexBuffer_XYZ_NORMAL_TEX2.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_NORMAL_TEX2_FRAG[] =
#include "VertexBuffer_XYZ_NORMAL_TEX2.frag.h"
;

const uint32_t VERTEX_BUFFER_XYZ_NORMAL_DIFFUSE_VERT[] =
#include "VertexBuffer_XYZ_NORMAL_DIFFUSE.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_NORMAL_DIFFUSE_FRAG[] =
#include "VertexBuffer_XYZ_NORMAL_DIFFUSE.frag.h"
;

const uint32_t VERTEX_BUFFER_XYZ_NORMAL_DIFFUSE_TEX1_VERT[] =
#include "VertexBuffer_XYZ_NORMAL_DIFFUSE_TEX1.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_NORMAL_DIFFUSE_TEX1_FRAG[] =
#include "VertexBuffer_XYZ_NORMAL_DIFFUSE_TEX1.frag.h"
;

const uint32_t VERTEX_BUFFER_XYZ_NORMAL_DIFFUSE_TEX2_VERT[] =
#include "VertexBuffer_XYZ_NORMAL_DIFFUSE_TEX2.vert.h"
;
const uint32_t VERTEX_BUFFER_XYZ_NORMAL_DIFFUSE_TEX2_FRAG[] =
#include "VertexBuffer_XYZ_NORMAL_DIFFUSE_TEX2.frag.h"
;

const uint32_t PIXEL_PASSTHROUGH_FRAG[] =
#include "PixelPassthrough.frag.h"
;

CDevice9::CDevice9(C9* c9, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode)
	:
	mC9(c9),
	mAdapter(Adapter),
	mDeviceType(DeviceType),
	mFocusWindow(hFocusWindow),
	mBehaviorFlags(BehaviorFlags)
{
	Log(info) << "CDevice9::CDevice9" << std::endl;

	if (pPresentationParameters != nullptr)
	{
		if (pPresentationParameters->BackBufferFormat == D3DFMT_UNKNOWN)
		{
			pPresentationParameters->BackBufferFormat = D3DFMT_X8R8G8B8;
		}
		memcpy(&mPresentationParameters, pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS));
	}

	if (pFullscreenDisplayMode != nullptr)
	{
		memcpy(&mFullscreenDisplayMode, pFullscreenDisplayMode, sizeof(D3DDISPLAYMODEEX));
	}

	//Setup Windows window
	if (!mPresentationParameters.BackBufferWidth)
	{
		RECT  rect;
		GetWindowRect(mFocusWindow, &rect);
		mPresentationParameters.BackBufferWidth = rect.right;
		mPresentationParameters.BackBufferHeight = rect.bottom;
	}

	if (!mPresentationParameters.Windowed)
	{
		DEVMODE newSettings = {};
		EnumDisplaySettings(0, 0, &newSettings);
		newSettings.dmPelsWidth = mPresentationParameters.BackBufferWidth;
		newSettings.dmPelsHeight = mPresentationParameters.BackBufferHeight;
		newSettings.dmBitsPerPel = 32;
		newSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings(&newSettings, CDS_FULLSCREEN);
		SetWindowPos(mFocusWindow, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		InvalidateRect(mFocusWindow, 0, true);
	}

	//Create a device and command pool (unique device will auto destroy)
	{
		float queuePriority = 0.0f;
		const vk::DeviceQueueCreateInfo deviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), static_cast<uint32_t>(mC9->mGraphicsQueueFamilyIndex), 1, &queuePriority);
		std::vector<char const*> deviceExtensionNames;
		deviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		mDevice = mC9->mPhysicalDevices[mC9->mPhysicalDeviceIndex].createDeviceUnique(vk::DeviceCreateInfo({}, 1, &deviceQueueCreateInfo, 0, nullptr, static_cast<uint32_t>(deviceExtensionNames.size()), deviceExtensionNames.data()));

		mCommandPool = mDevice->createCommandPoolUnique(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, deviceQueueCreateInfo.queueFamilyIndex));
	}

	//Create a descriptor pool that should be able to allocate enough of any type.
	{
		const vk::DescriptorPoolSize descriptorPoolSizes[11] =
		{
			vk::DescriptorPoolSize(vk::DescriptorType::eSampler,std::min((uint32_t)MAX_DESCRIPTOR, mC9->mPhysicalDeviceProperties.limits.maxDescriptorSetSamplers)),
			vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler,std::min((uint32_t)MAX_DESCRIPTOR, mC9->mPhysicalDeviceProperties.limits.maxPerStageDescriptorSamplers)),
			vk::DescriptorPoolSize(vk::DescriptorType::eSampledImage,std::min((uint32_t)MAX_DESCRIPTOR, mC9->mPhysicalDeviceProperties.limits.maxDescriptorSetSampledImages)),
			vk::DescriptorPoolSize(vk::DescriptorType::eStorageImage,std::min((uint32_t)MAX_DESCRIPTOR, mC9->mPhysicalDeviceProperties.limits.maxDescriptorSetStorageImages)),
			vk::DescriptorPoolSize(vk::DescriptorType::eUniformTexelBuffer,std::min((uint32_t)MAX_DESCRIPTOR, mC9->mPhysicalDeviceProperties.limits.maxPerStageDescriptorSampledImages)),
			vk::DescriptorPoolSize(vk::DescriptorType::eStorageTexelBuffer,std::min((uint32_t)MAX_DESCRIPTOR, mC9->mPhysicalDeviceProperties.limits.maxPerStageDescriptorStorageImages)),
			vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer,std::min((uint32_t)MAX_DESCRIPTOR, mC9->mPhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffers)),
			vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer,std::min((uint32_t)MAX_DESCRIPTOR, mC9->mPhysicalDeviceProperties.limits.maxDescriptorSetStorageBuffers)),
			vk::DescriptorPoolSize(vk::DescriptorType::eUniformBufferDynamic,std::min((uint32_t)MAX_DESCRIPTOR, mC9->mPhysicalDeviceProperties.limits.maxDescriptorSetUniformBuffersDynamic)),
			vk::DescriptorPoolSize(vk::DescriptorType::eStorageBufferDynamic,std::min((uint32_t)MAX_DESCRIPTOR, mC9->mPhysicalDeviceProperties.limits.maxDescriptorSetStorageBuffersDynamic)),
			vk::DescriptorPoolSize(vk::DescriptorType::eInputAttachment,std::min((uint32_t)MAX_DESCRIPTOR, mC9->mPhysicalDeviceProperties.limits.maxDescriptorSetInputAttachments))
		};
		mDescriptorPool = mDevice->createDescriptorPoolUnique(vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlags(), std::min(MAX_DESCRIPTOR, mC9->mPhysicalDeviceProperties.limits.maxDescriptorSetSamplers), 11, &descriptorPoolSizes[0]));
	}

	//Setup buffers for up draw methods
	{
		auto const upVertexBufferInfo = vk::BufferCreateInfo().setSize(MAX_BUFFERUPDATE).setUsage(vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst);
		auto const upIndexBufferInfo = vk::BufferCreateInfo().setSize(MAX_BUFFERUPDATE).setUsage(vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst);

		mUpVertexBuffer = mDevice->createBufferUnique(upVertexBufferInfo);
		mUpIndexBuffer = mDevice->createBufferUnique(upIndexBufferInfo);

		vk::MemoryRequirements mem_reqs;
		mDevice->getBufferMemoryRequirements(mUpVertexBuffer.get(), &mem_reqs);

		auto mem_alloc = vk::MemoryAllocateInfo().setAllocationSize(mem_reqs.size).setMemoryTypeIndex(0);
		FindMemoryTypeFromProperties(mem_reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &mem_alloc.memoryTypeIndex);

		mUpVertexBufferMemory = mDevice->allocateMemoryUnique(mem_alloc);
		mUpIndexBufferMemory = mDevice->allocateMemoryUnique(mem_alloc);

		mDevice->bindBufferMemory(mUpVertexBuffer.get(), mUpVertexBufferMemory.get(), 0);
		mDevice->bindBufferMemory(mUpIndexBuffer.get(), mUpIndexBufferMemory.get(), 0);
	}

	//Setup FF Buffers

	//RenderState
	{
		auto const renderStateBufferInfo = vk::BufferCreateInfo().setSize(sizeof(uint32_t) /*FIX!*/).setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);
		mRenderStateBuffer = mDevice->createBufferUnique(renderStateBufferInfo);
		vk::MemoryRequirements renderStateMemoryRequirements;
		mDevice->getBufferMemoryRequirements(mRenderStateBuffer.get(), &renderStateMemoryRequirements);
		auto renderStateMemoryAllocateInfo = vk::MemoryAllocateInfo().setAllocationSize(renderStateMemoryRequirements.size).setMemoryTypeIndex(0);
		FindMemoryTypeFromProperties(renderStateMemoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &renderStateMemoryAllocateInfo.memoryTypeIndex);
		mRenderStateBufferMemory = mDevice->allocateMemoryUnique(renderStateMemoryAllocateInfo);
		mDevice->bindBufferMemory(mRenderStateBuffer.get(), mRenderStateBufferMemory.get(), 0);
	}

	//TextureStage
	{
		auto const textureStageBufferInfo = vk::BufferCreateInfo().setSize(sizeof(uint32_t) /*FIX!*/).setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);
		mTextureStageBuffer = mDevice->createBufferUnique(textureStageBufferInfo);
		vk::MemoryRequirements textureStageMemoryRequirements;
		mDevice->getBufferMemoryRequirements(mTextureStageBuffer.get(), &textureStageMemoryRequirements);
		auto textureStageMemoryAllocateInfo = vk::MemoryAllocateInfo().setAllocationSize(textureStageMemoryRequirements.size).setMemoryTypeIndex(0);
		FindMemoryTypeFromProperties(textureStageMemoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &textureStageMemoryAllocateInfo.memoryTypeIndex);
		mTextureStageBufferMemory = mDevice->allocateMemoryUnique(textureStageMemoryAllocateInfo);
		mDevice->bindBufferMemory(mTextureStageBuffer.get(), mTextureStageBufferMemory.get(), 0);
	}

	//Light
	{
		auto const lightBufferInfo = vk::BufferCreateInfo().setSize(sizeof(uint32_t) /*FIX!*/).setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);
		mLightBuffer = mDevice->createBufferUnique(lightBufferInfo);
		vk::MemoryRequirements lightMemoryRequirements;
		mDevice->getBufferMemoryRequirements(mLightBuffer.get(), &lightMemoryRequirements);
		auto lightMemoryAllocateInfo = vk::MemoryAllocateInfo().setAllocationSize(lightMemoryRequirements.size).setMemoryTypeIndex(0);
		FindMemoryTypeFromProperties(lightMemoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &lightMemoryAllocateInfo.memoryTypeIndex);
		mLightBufferMemory = mDevice->allocateMemoryUnique(lightMemoryAllocateInfo);
		mDevice->bindBufferMemory(mLightBuffer.get(), mLightBufferMemory.get(), 0);
	}

	//Material
	{
		auto const materialBufferInfo = vk::BufferCreateInfo().setSize(sizeof(D3DMATERIAL9)).setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);
		mMaterialBuffer = mDevice->createBufferUnique(materialBufferInfo);
		vk::MemoryRequirements materialMemoryRequirements;
		mDevice->getBufferMemoryRequirements(mMaterialBuffer.get(), &materialMemoryRequirements);
		auto materialMemoryAllocateInfo = vk::MemoryAllocateInfo().setAllocationSize(materialMemoryRequirements.size).setMemoryTypeIndex(0);
		FindMemoryTypeFromProperties(materialMemoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &materialMemoryAllocateInfo.memoryTypeIndex);
		mMaterialBufferMemory = mDevice->allocateMemoryUnique(materialMemoryAllocateInfo);
		mDevice->bindBufferMemory(mMaterialBuffer.get(), mMaterialBufferMemory.get(), 0);
	}

	//Transformation
	{
		auto const transformationBufferInfo = vk::BufferCreateInfo().setSize(sizeof(uint32_t) /*FIX!*/).setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);
		mTransformationBuffer = mDevice->createBufferUnique(transformationBufferInfo);
		vk::MemoryRequirements transformationMemoryRequirements;
		mDevice->getBufferMemoryRequirements(mTransformationBuffer.get(), &transformationMemoryRequirements);
		auto transformationMemoryAllocateInfo = vk::MemoryAllocateInfo().setAllocationSize(transformationMemoryRequirements.size).setMemoryTypeIndex(0);
		FindMemoryTypeFromProperties(transformationMemoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &transformationMemoryAllocateInfo.memoryTypeIndex);
		mTransformationBufferMemory = mDevice->allocateMemoryUnique(transformationMemoryAllocateInfo);
		mDevice->bindBufferMemory(mTransformationBuffer.get(), mTransformationBufferMemory.get(), 0);
	}

	//Setup Shader buffers

	//VertexConstant
	{
		auto const vertexConstantBufferInfo = vk::BufferCreateInfo().setSize(sizeof(uint32_t) /*FIX!*/).setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);
		mVertexConstantBuffer = mDevice->createBufferUnique(vertexConstantBufferInfo);
		vk::MemoryRequirements vertexConstantMemoryRequirements;
		mDevice->getBufferMemoryRequirements(mVertexConstantBuffer.get(), &vertexConstantMemoryRequirements);
		auto vertexConstantMemoryAllocateInfo = vk::MemoryAllocateInfo().setAllocationSize(vertexConstantMemoryRequirements.size).setMemoryTypeIndex(0);
		FindMemoryTypeFromProperties(vertexConstantMemoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &vertexConstantMemoryAllocateInfo.memoryTypeIndex);
		mVertexConstantBufferMemory = mDevice->allocateMemoryUnique(vertexConstantMemoryAllocateInfo);
		mDevice->bindBufferMemory(mVertexConstantBuffer.get(), mVertexConstantBufferMemory.get(), 0);
	}

	//PixelConstant
	{
		auto const pixelConstantBufferInfo = vk::BufferCreateInfo().setSize(sizeof(uint32_t) /*FIX!*/).setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);
		mPixelConstantBuffer = mDevice->createBufferUnique(pixelConstantBufferInfo);
		vk::MemoryRequirements pixelConstantMemoryRequirements;
		mDevice->getBufferMemoryRequirements(mPixelConstantBuffer.get(), &pixelConstantMemoryRequirements);
		auto pixelConstantMemoryAllocateInfo = vk::MemoryAllocateInfo().setAllocationSize(pixelConstantMemoryRequirements.size).setMemoryTypeIndex(0);
		FindMemoryTypeFromProperties(pixelConstantMemoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &pixelConstantMemoryAllocateInfo.memoryTypeIndex);
		mPixelConstantBufferMemory = mDevice->allocateMemoryUnique(pixelConstantMemoryAllocateInfo);
		mDevice->bindBufferMemory(mPixelConstantBuffer.get(), mPixelConstantBufferMemory.get(), 0);
	}

	//Create Descriptor layout.
	{
		const uint32_t textureCount = 16;
		const vk::DescriptorSetLayoutBinding layoutBindings[8] =
		{
			vk::DescriptorSetLayoutBinding() /*Render State*/
				.setBinding(0)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount(1)
				.setStageFlags(vk::ShaderStageFlagBits::eAllGraphics)
				.setPImmutableSamplers(nullptr),
			vk::DescriptorSetLayoutBinding() /*Texture Stages*/
				.setBinding(1)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount(1)
				.setStageFlags(vk::ShaderStageFlagBits::eFragment)
				.setPImmutableSamplers(nullptr),
			vk::DescriptorSetLayoutBinding() /*Lights*/
				.setBinding(2)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount(1)
				.setStageFlags(vk::ShaderStageFlagBits::eVertex)
				.setPImmutableSamplers(nullptr),
			vk::DescriptorSetLayoutBinding() /*Material*/
				.setBinding(3)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount(1)
				.setStageFlags(vk::ShaderStageFlagBits::eVertex)
				.setPImmutableSamplers(nullptr),
			vk::DescriptorSetLayoutBinding() /*Matrix*/
				.setBinding(4)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount(1)
				.setStageFlags(vk::ShaderStageFlagBits::eVertex)
				.setPImmutableSamplers(nullptr),
			vk::DescriptorSetLayoutBinding() /*Vertex Shader Const*/
				.setBinding(5)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount(1)
				.setStageFlags(vk::ShaderStageFlagBits::eFragment)
				.setPImmutableSamplers(nullptr),
			vk::DescriptorSetLayoutBinding() /*Pixel Shader Const*/
				.setBinding(6)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount(1)
				.setStageFlags(vk::ShaderStageFlagBits::eFragment)
				.setPImmutableSamplers(nullptr),
			vk::DescriptorSetLayoutBinding() /*Image/Sampler*/
				.setBinding(7)
				.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
				.setDescriptorCount(textureCount)
				.setStageFlags(vk::ShaderStageFlagBits::eFragment)
				.setPImmutableSamplers(nullptr)
		};
		auto const descriptorLayout = vk::DescriptorSetLayoutCreateInfo().setBindingCount(8).setPBindings(layoutBindings);
		mDescriptorLayout = mDevice->createDescriptorSetLayoutUnique(descriptorLayout);
	}

	//Create Pipeline layout.
	{
		std::array<vk::PushConstantRange, 1> ranges =
		{
			vk::PushConstantRange
			{
				vk::ShaderStageFlagBits::eVertex,
				0,
				sizeof(uint32_t) * 4
			}
		};

		auto const pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
			.setSetLayoutCount(1)
			.setPPushConstantRanges(ranges.data())
			.setPushConstantRangeCount(1)
			.setPSetLayouts(&mDescriptorLayout.get());
		mPipelineLayout = mDevice->createPipelineLayoutUnique(pipelineLayoutCreateInfo);
	}

	//Load fixed function shaders.
	mVertShaderModule_XYZRHW = LoadShaderFromConst(VERTEX_BUFFER_XYZRHW_VERT);
	mVertShaderModule_XYZ = LoadShaderFromConst(VERTEX_BUFFER_XYZ_VERT);
	mFragShaderModule_XYZ = LoadShaderFromConst(VERTEX_BUFFER_XYZ_FRAG);

	mVertShaderModule_XYZRHW_DIFFUSE = LoadShaderFromConst(VERTEX_BUFFER_XYZRHW_DIFFUSE_VERT);
	mVertShaderModule_XYZ_DIFFUSE = LoadShaderFromConst(VERTEX_BUFFER_XYZ_DIFFUSE_VERT);
	mFragShaderModule_XYZ_DIFFUSE = LoadShaderFromConst(VERTEX_BUFFER_XYZ_DIFFUSE_FRAG);
	mGeomShaderModule_XYZ_DIFFUSE = LoadShaderFromConst(VERTEX_BUFFER_XYZ_DIFFUSE_GEOM);

	mVertShaderModule_XYZRHW_TEX1 = LoadShaderFromConst(VERTEX_BUFFER_XYZRHW_TEX1_VERT);
	mVertShaderModule_XYZ_TEX1 = LoadShaderFromConst(VERTEX_BUFFER_XYZ_TEX1_VERT);
	mFragShaderModule_XYZ_TEX1 = LoadShaderFromConst(VERTEX_BUFFER_XYZ_TEX1_FRAG);

	mVertShaderModule_XYZRHW_TEX2 = LoadShaderFromConst(VERTEX_BUFFER_XYZRHW_TEX2_VERT);
	mVertShaderModule_XYZ_TEX2 = LoadShaderFromConst(VERTEX_BUFFER_XYZ_TEX2_VERT);
	mFragShaderModule_XYZ_TEX2 = LoadShaderFromConst(VERTEX_BUFFER_XYZ_TEX2_FRAG);

	mVertShaderModule_XYZRHW_DIFFUSE_TEX1 = LoadShaderFromConst(VERTEX_BUFFER_XYZRHW_DIFFUSE_TEX1_VERT);
	mVertShaderModule_XYZ_DIFFUSE_TEX1 = LoadShaderFromConst(VERTEX_BUFFER_XYZ_DIFFUSE_TEX1_VERT);
	mFragShaderModule_XYZ_DIFFUSE_TEX1 = LoadShaderFromConst(VERTEX_BUFFER_XYZ_DIFFUSE_TEX1_FRAG);

	mVertShaderModule_XYZRHW_DIFFUSE_TEX2 = LoadShaderFromConst(VERTEX_BUFFER_XYZRHW_DIFFUSE_TEX2_VERT);
	mVertShaderModule_XYZ_DIFFUSE_TEX2 = LoadShaderFromConst(VERTEX_BUFFER_XYZ_DIFFUSE_TEX2_VERT);
	mFragShaderModule_XYZ_DIFFUSE_TEX2 = LoadShaderFromConst(VERTEX_BUFFER_XYZ_DIFFUSE_TEX2_FRAG);

	mVertShaderModule_XYZ_NORMAL = LoadShaderFromConst(VERTEX_BUFFER_XYZ_NORMAL_VERT);
	mFragShaderModule_XYZ_NORMAL = LoadShaderFromConst(VERTEX_BUFFER_XYZ_NORMAL_FRAG);

	mVertShaderModule_XYZ_NORMAL_TEX1 = LoadShaderFromConst(VERTEX_BUFFER_XYZ_NORMAL_TEX1_VERT);
	mFragShaderModule_XYZ_NORMAL_TEX1 = LoadShaderFromConst(VERTEX_BUFFER_XYZ_NORMAL_TEX1_FRAG);

	mVertShaderModule_XYZ_NORMAL_TEX2 = LoadShaderFromConst(VERTEX_BUFFER_XYZ_NORMAL_TEX2_VERT);
	mFragShaderModule_XYZ_NORMAL_TEX2 = LoadShaderFromConst(VERTEX_BUFFER_XYZ_NORMAL_TEX2_FRAG);

	mVertShaderModule_XYZ_NORMAL_DIFFUSE = LoadShaderFromConst(VERTEX_BUFFER_XYZ_NORMAL_DIFFUSE_VERT);
	mFragShaderModule_XYZ_NORMAL_DIFFUSE = LoadShaderFromConst(VERTEX_BUFFER_XYZ_NORMAL_DIFFUSE_FRAG);

	mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX1 = LoadShaderFromConst(VERTEX_BUFFER_XYZ_NORMAL_DIFFUSE_TEX1_VERT);
	mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX1 = LoadShaderFromConst(VERTEX_BUFFER_XYZ_NORMAL_DIFFUSE_TEX1_FRAG);

	mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = LoadShaderFromConst(VERTEX_BUFFER_XYZ_NORMAL_DIFFUSE_TEX2_VERT);
	mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = LoadShaderFromConst(VERTEX_BUFFER_XYZ_NORMAL_DIFFUSE_TEX2_FRAG);

	mFragShaderModule_Passthrough = LoadShaderFromConst(PIXEL_PASSTHROUGH_FRAG);

	//Add implicit swap chain.
	CSwapChain9* ptr = nullptr;
	CreateAdditionalSwapChain(&mPresentationParameters, (IDirect3DSwapChain9**)&ptr);
	mSwapChains.push_back(ptr);

	//Add implicit render target
	SetRenderTarget(0, ptr->mBackBuffer);

	//Add implicit stencil buffer surface.
	auto depth = new CSurface9(this, &mPresentationParameters, mPresentationParameters.AutoDepthStencilFormat);
	depth->Init();
	SetDepthStencilSurface(depth);
	depth->Release();
}

CDevice9::~CDevice9()
{
	mDevice->waitIdle();
}

ULONG CDevice9::PrivateAddRef(void)
{
	return InterlockedIncrement(&mPrivateReferenceCount);
}

ULONG CDevice9::PrivateRelease(void)
{
	ULONG ref = InterlockedDecrement(&mPrivateReferenceCount);

	if (ref == 0 && mReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

bool CDevice9::FindMemoryTypeFromProperties(uint32_t typeBits, vk::MemoryPropertyFlags requirements_mask, uint32_t* typeIndex)
{
	// Search memtypes to find first index with those properties
	for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++)
	{
		if ((typeBits & 1) == 1)
		{
			// Type is available, does it match user properties?
			if ((mC9->mPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask)
			{
				*typeIndex = i;
				return true;
			}
		}
		typeBits >>= 1;
	}

	// No memory types matched, return failure
	return false;
}

HRESULT STDMETHODCALLTYPE CDevice9::Clear(DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	//Flags |= D3DCLEAR_TARGET;


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::BeginScene() //
{
	//According to a tip from the Nine team games don't always use the begin/end scene functions correctly.

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::EndScene()
{
	//According to a tip from the Nine team games don't always use the begin/end scene functions correctly.

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::Present(const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{

	//if (mCommandStreamManager->mResult == vk::Result::eErrorDeviceLost) { return D3DERR_DEVICELOST; }

	return D3D_OK;
}

ULONG STDMETHODCALLTYPE CDevice9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CDevice9::QueryInterface(REFIID riid, void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DDevice9))
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

ULONG STDMETHODCALLTYPE CDevice9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0 && mPrivateReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}


HRESULT STDMETHODCALLTYPE CDevice9::BeginStateBlock()
{



	return S_OK;
}



HRESULT STDMETHODCALLTYPE CDevice9::ColorFill(IDirect3DSurface9 *pSurface, const RECT *pRect, D3DCOLOR color)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::ColorFill is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain9 **ppSwapChain)
{
	auto ptr = new CSwapChain9(this, pPresentationParameters);
	ptr->Init();
	(*ppSwapChain) = ptr;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9 **ppCubeTexture, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CCubeTexture9* obj = new CCubeTexture9(this, EdgeLength, Levels, Usage, Format, Pool, pSharedHandle);

	obj->Init();

	for (size_t i = 0; i < 6; i++)
	{
		for (size_t j = 0; j < obj->mLevels; j++)
		{
			obj->mSurfaces[i][j]->Init();
		}
	}

	(*ppCubeTexture) = (IDirect3DCubeTexture9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CSurface9* obj = new CSurface9(this, (CTexture9*)nullptr, Width, Height, Format, MultiSample, MultisampleQuality, Discard, pSharedHandle);

	obj->Init();

	(*ppSurface) = (IDirect3DSurface9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9 **ppIndexBuffer, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CIndexBuffer9* obj = new CIndexBuffer9(this, Length, Usage, Format, Pool, pSharedHandle);

	obj->Init();

	(*ppIndexBuffer) = (IDirect3DIndexBuffer9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CSurface9* ptr = new CSurface9(this, (CTexture9*)nullptr, Width, Height, 1, 0, Format, Pool, pSharedHandle);

	ptr->Init();

	(*ppSurface) = (IDirect3DSurface9*)ptr;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreatePixelShader(const DWORD *pFunction, IDirect3DPixelShader9 **ppShader)
{
	HRESULT result = S_OK;

	CPixelShader9* obj = new CPixelShader9(this, pFunction);



	//The application is allowed to dispose of the shader data it passes in after this call returns but can request it later so we need to make a copy.
	obj->mFunction = (DWORD*)malloc(obj->mSize);
	if (obj->mFunction != nullptr)
	{
		memcpy(obj->mFunction, pFunction, obj->mSize);
	}

	(*ppShader) = (IDirect3DPixelShader9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9 **ppQuery)
{
	/*
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb174360(v=vs.85).aspx
	*/

	//If null is passed the call is checking to see if a query type is supported.
	if (ppQuery == nullptr)
	{
		if (Type == D3DQUERYTYPE_OCCLUSION || Type == D3DQUERYTYPE_TIMESTAMP || Type == D3DQUERYTYPE_TIMESTAMPDISJOINT || Type == D3DQUERYTYPE_TIMESTAMPFREQ)
		{
			return S_OK;
		}
		else
		{
			return D3DERR_NOTAVAILABLE;
		}
	}

	if (Type == D3DQUERYTYPE_OCCLUSION || Type == D3DQUERYTYPE_TIMESTAMP || Type == D3DQUERYTYPE_TIMESTAMPDISJOINT || Type == D3DQUERYTYPE_TIMESTAMPFREQ)
	{
		HRESULT result = S_OK;

		CQuery9* obj = new CQuery9(this, Type);

		(*ppQuery) = (IDirect3DQuery9*)obj;



		return result;
	}
	else
	{
		return D3DERR_NOTAVAILABLE;
	}
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	//I added an extra int at the end so the signature would be different for this version. Locakable/Discard are both BOOL.
	CRenderTargetSurface9* obj = new CRenderTargetSurface9(this, Width, Height, Format);

	(*ppSurface) = (IDirect3DSurface9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9 **ppSB)
{
	HRESULT result = S_OK;

	CStateBlock9* obj = new CStateBlock9(this, Type);



	(*ppSB) = (IDirect3DStateBlock9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9 **ppTexture, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CTexture9* obj = new CTexture9(this, Width, Height, Levels, Usage, Format, Pool, pSharedHandle);

	obj->Init();

	for (size_t i = 0; i < obj->mLevels; i++)
	{
		obj->mSurfaces[i]->Init();
	}

	(*ppTexture) = (IDirect3DTexture9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9 **ppVertexBuffer, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CVertexBuffer9* obj = new CVertexBuffer9(this, Length, Usage, FVF, Pool, pSharedHandle);

	obj->Init();

	(*ppVertexBuffer) = (IDirect3DVertexBuffer9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexDeclaration(const D3DVERTEXELEMENT9 *pVertexElements, IDirect3DVertexDeclaration9 **ppDecl)
{
	HRESULT result = S_OK;

	CVertexDeclaration9* obj = new CVertexDeclaration9(this, pVertexElements);

	(*ppDecl) = (IDirect3DVertexDeclaration9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexShader(const DWORD *pFunction, IDirect3DVertexShader9 **ppShader)
{
	HRESULT result = S_OK;

	CVertexShader9* obj = new CVertexShader9(this, pFunction);


	//The application is allowed to dispose of the shader data it passes in after this call returns but can request it later so we need to make a copy.
	obj->mFunction = (DWORD*)malloc(obj->mSize);
	if (obj->mFunction != nullptr)
	{
		memcpy(obj->mFunction, pFunction, obj->mSize);
	}

	(*ppShader) = (IDirect3DVertexShader9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9 **ppVolumeTexture, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CVolumeTexture9* obj = new CVolumeTexture9(this, Width, Height, Depth, Levels, Usage, Format, Pool, pSharedHandle);



	for (size_t i = 0; i < obj->mLevels; i++)
	{
		obj->mVolumes[i]->Init();
	}

	(*ppVolumeTexture) = (IDirect3DVolumeTexture9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::DeletePatch(UINT Handle)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::DeletePatch is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, const void *pIndexData, D3DFORMAT IndexDataFormat, const void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	////Figure out the current buffer setup.
	//IDirect3DIndexBuffer9* oldIndexBuffer = nullptr;

	//GetIndices(&oldIndexBuffer);

	//IDirect3DVertexBuffer9* oldVertexBuffer = nullptr;
	//UINT oldOffsetInBytes = 0;
	//UINT oldStride = 0;

	//GetStreamSource(0, &oldVertexBuffer, &oldOffsetInBytes, &oldStride);

	////Setup temp buffers
	//UINT indexLength = 0;
	//UINT vertexLength = NumVertices * VertexStreamZeroStride;
	//DWORD Usage = 0;

	//switch (PrimitiveType)
	//{
	//case D3DPT_POINTLIST:
	//	indexLength = (PrimitiveCount) * ((IndexDataFormat == D3DFMT_INDEX16) ? 2 : 4);
	//	break;
	//case D3DPT_LINELIST:
	//	indexLength = (PrimitiveCount * 2) * ((IndexDataFormat == D3DFMT_INDEX16) ? 2 : 4);
	//	break;
	//case D3DPT_LINESTRIP:
	//	indexLength = (PrimitiveCount + 1) * ((IndexDataFormat == D3DFMT_INDEX16) ? 2 : 4);
	//	break;
	//case D3DPT_TRIANGLELIST:
	//	indexLength = (PrimitiveCount * 3) * ((IndexDataFormat == D3DFMT_INDEX16) ? 2 : 4);
	//	break;
	//case D3DPT_TRIANGLESTRIP:
	//	indexLength = (PrimitiveCount + 2) * ((IndexDataFormat == D3DFMT_INDEX16) ? 2 : 4);
	//	break;
	//case D3DPT_TRIANGLEFAN:
	//	indexLength = (PrimitiveCount + 2) * ((IndexDataFormat == D3DFMT_INDEX16) ? 2 : 4);
	//	break;
	//default:
	//	//indexLength = PrimitiveCount;
	//	break;
	//}

	//CIndexBuffer9* indexBuffer = nullptr;
	//CVertexBuffer9* vertexBuffer = nullptr;
	//void* buffer = nullptr;

	//for (auto& buffer : mTempIndexBuffers)
	//{
	//	if (!buffer->mIsUsed && buffer->mLength >= indexLength)
	//	{
	//		indexBuffer = buffer;
	//		break;
	//	}
	//}
	//if (indexBuffer == nullptr)
	//{
	//	CreateIndexBuffer(indexLength, Usage, IndexDataFormat, D3DPOOL_DEFAULT, (IDirect3DIndexBuffer9**)&indexBuffer, nullptr);
	//	mTempIndexBuffers.push_back(indexBuffer);
	//}

	//for (auto& buffer : mTempVertexBuffers)
	//{
	//	if (!buffer->mIsUsed && buffer->mLength >= vertexLength)
	//	{
	//		vertexBuffer = buffer;
	//		break;
	//	}
	//}
	//if (vertexBuffer == nullptr)
	//{
	//	CreateVertexBuffer(vertexLength, Usage, D3DFVF_XYZ, D3DPOOL_DEFAULT, (IDirect3DVertexBuffer9**)&vertexBuffer, nullptr);
	//	mTempVertexBuffers.push_back(vertexBuffer);
	//}

	//vertexBuffer->mIsUsed = true;
	//vertexBuffer->mSize = NumVertices;


	//vertexBuffer->Lock(0, 0, (void**)&buffer, 0);
	//memcpy(buffer, pVertexStreamZeroData, vertexLength);
	//vertexBuffer->Unlock();

	//indexBuffer->Lock(0, 0, (void**)&buffer, 0);
	//memcpy(buffer, pIndexData, indexLength);
	//indexBuffer->Unlock();

	//SetIndices(indexBuffer);
	//indexBuffer->Release();
	//SetStreamSource(0, vertexBuffer, 0, VertexStreamZeroStride);
	//vertexBuffer->Release();

	////Queue draw command
	////DrawIndexedPrimitive(PrimitiveType, 0, MinVertexIndex, NumVertices, 0, PrimitiveCount);


	////Switch the buffers back.
	//SetIndices(oldIndexBuffer);
	//SetStreamSource(0, oldVertexBuffer, oldOffsetInBytes, oldStride);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	////Figure out the current buffer setup.
	//IDirect3DVertexBuffer9* oldVertexBuffer = nullptr;
	//UINT oldOffsetInBytes = 0;
	//UINT oldStride = 0;

	//GetStreamSource(0, &oldVertexBuffer, &oldOffsetInBytes, &oldStride);

	////Setup temp buffers
	//UINT vertexLength = 0;
	//DWORD Usage = 0;
	//UINT NumVertices = 0;

	//switch (PrimitiveType)
	//{
	//case D3DPT_POINTLIST:
	//	NumVertices = (PrimitiveCount);
	//	vertexLength = NumVertices * VertexStreamZeroStride;
	//	break;
	//case D3DPT_LINELIST:
	//	NumVertices = (PrimitiveCount * 2);
	//	vertexLength = NumVertices * VertexStreamZeroStride;
	//	break;
	//case D3DPT_LINESTRIP:
	//	NumVertices = (PrimitiveCount + 1);
	//	vertexLength = NumVertices * VertexStreamZeroStride;
	//	break;
	//case D3DPT_TRIANGLELIST:
	//	NumVertices = (PrimitiveCount * 3);
	//	vertexLength = NumVertices * VertexStreamZeroStride;
	//	break;
	//case D3DPT_TRIANGLESTRIP:
	//	NumVertices = (PrimitiveCount + 2);
	//	vertexLength = NumVertices * VertexStreamZeroStride;
	//	break;
	//case D3DPT_TRIANGLEFAN:
	//	NumVertices = (PrimitiveCount + 2);
	//	vertexLength = NumVertices * VertexStreamZeroStride;
	//	break;
	//default:
	//	//NumVertices = PrimitiveCount;
	//	//vertexLength = NumVertices * VertexStreamZeroStride;
	//	break;
	//}

	//CVertexBuffer9* vertexBuffer = nullptr;
	//void* buffer = nullptr;

	//for (auto& buffer : mTempVertexBuffers)
	//{
	//	if (!buffer->mIsUsed && buffer->mLength >= vertexLength)
	//	{
	//		vertexBuffer = buffer;
	//		break;
	//	}
	//}
	//if (vertexBuffer == nullptr)
	//{
	//	CreateVertexBuffer(vertexLength, Usage, D3DFVF_XYZ, D3DPOOL_DEFAULT, (IDirect3DVertexBuffer9**)&vertexBuffer, nullptr);
	//	mTempVertexBuffers.push_back(vertexBuffer);
	//}

	//vertexBuffer->mIsUsed = true;
	//vertexBuffer->mSize = NumVertices;


	//vertexBuffer->Lock(0, 0, (void**)&buffer, 0);
	//memcpy(buffer, pVertexStreamZeroData, vertexLength);
	//vertexBuffer->Unlock();

	//SetStreamSource(0, vertexBuffer, 0, VertexStreamZeroStride);
	////vertexBuffer->Release();

	////Queue draw command
	////DrawPrimitive(PrimitiveType, 0, PrimitiveCount);


	////Switch the buffers back.
	//SetStreamSource(0, oldVertexBuffer, oldOffsetInBytes, oldStride);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawRectPatch(UINT Handle, const float *pNumSegs, const D3DRECTPATCH_INFO *pRectPatchInfo)
{
	//if (!mIsSceneStarted)
	//{
	//	this->StartScene();
	//}

	//TODO: Implement.

	Log(warning) << "CDevice9::DrawRectPatch is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawTriPatch(UINT Handle, const float *pNumSegs, const D3DTRIPATCH_INFO *pTriPatchInfo)
{
	//if (!mIsSceneStarted)
	//{
	//	this->StartScene();
	//}

	//TODO: Implement.

	Log(warning) << "CDevice9::DrawTriPatch is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::EndStateBlock(IDirect3DStateBlock9** ppSB)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::EvictManagedResources()
{
	//TODO: Implement.

	Log(warning) << "CDevice9::EvictManagedResources is not implemented!" << std::endl;

	return S_OK;
}

UINT STDMETHODCALLTYPE CDevice9::GetAvailableTextureMem()
{


	return mAvailableTextureMemory;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetBackBuffer(UINT  iSwapChain, UINT BackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9 **ppBackBuffer)
{
	return mSwapChains[iSwapChain]->GetBackBuffer(BackBuffer, Type, ppBackBuffer);
}

HRESULT STDMETHODCALLTYPE CDevice9::GetClipPlane(DWORD Index, float *pPlane)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::GetClipPlane is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetClipStatus(D3DCLIPSTATUS9 *pClipStatus)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::GetClipStatus is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	pParameters->AdapterOrdinal = this->mAdapter;
	pParameters->DeviceType = this->mDeviceType;
	pParameters->hFocusWindow = this->mFocusWindow;
	pParameters->BehaviorFlags = this->mBehaviorFlags;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetCurrentTexturePalette(UINT *pPaletteNumber)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::GetCurrentTexturePalette is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDepthStencilSurface(IDirect3DSurface9 **ppZStencilSurface)
{
	(*ppZStencilSurface) = mDepthStencilSurface;

	if (mDepthStencilSurface != nullptr)
	{
		mDepthStencilSurface->AddRef();
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDeviceCaps(D3DCAPS9 *pCaps)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDirect3D(IDirect3D9 **ppD3D9)
{
	(*ppD3D9) = (IDirect3D9*)mC9;

	mC9->AddRef();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDisplayMode(UINT  iSwapChain, D3DDISPLAYMODE *pMode)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetFrontBufferData(UINT  iSwapChain, IDirect3DSurface9 *pDestSurface)
{
	return mSwapChains[iSwapChain]->GetFrontBufferData(pDestSurface);
}

HRESULT STDMETHODCALLTYPE CDevice9::GetFVF(DWORD *pFVF)
{


	return S_OK;
}

void STDMETHODCALLTYPE CDevice9::GetGammaRamp(UINT  iSwapChain, D3DGAMMARAMP *pRamp)
{
	auto& swapChain = mSwapChains[iSwapChain];

	HDC windowHdc = GetDC(swapChain->mPresentationParameters->hDeviceWindow);

	if (!GetDeviceGammaRamp(windowHdc, (LPVOID)pRamp))
	{
		(*pRamp) = {};
	}
}

HRESULT STDMETHODCALLTYPE CDevice9::GetIndices(IDirect3DIndexBuffer9 **ppIndexData) //,UINT *pBaseVertexIndex ?
{


	if ((*ppIndexData) != nullptr)
	{
		(*ppIndexData)->AddRef();
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetLight(DWORD Index, D3DLIGHT9 *pLight)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetLightEnable(DWORD Index, BOOL *pEnable)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetMaterial(D3DMATERIAL9 *pMaterial)
{


	return S_OK;
}

FLOAT STDMETHODCALLTYPE CDevice9::GetNPatchMode()
{
	FLOAT output = {};



	return output;
}

UINT STDMETHODCALLTYPE CDevice9::GetNumberOfSwapChains()
{
	//TODO: Implement.

	Log(warning) << "CDevice9::GetNumberOfSwapChains is not implemented!" << std::endl;

	return 0;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY *pEntries)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::GetPaletteEntries is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShader(IDirect3DPixelShader9 **ppShader)
{


	if ((*ppShader) != nullptr)
	{
		(*ppShader)->AddRef();
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantB(UINT StartRegister, BOOL *pConstantData, UINT BoolCount)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantF(UINT StartRegister, float *pConstantData, UINT Vector4fCount)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantI(UINT StartRegister, int *pConstantData, UINT Vector4iCount)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRasterStatus(UINT  iSwapChain, D3DRASTER_STATUS *pRasterStatus)
{
	return mSwapChains[iSwapChain]->GetRasterStatus(pRasterStatus);
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
{
	mRenderTargets[RenderTargetIndex]->AddRef();

	(*ppRenderTarget) = mRenderTargets[RenderTargetIndex];

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderTargetData(IDirect3DSurface9 *pRenderTarget, IDirect3DSurface9 *pDestSurface)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::GetRenderTargetData is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetScissorRect(RECT* pRect)
{


	return S_OK;
}

BOOL STDMETHODCALLTYPE CDevice9::GetSoftwareVertexProcessing()
{
	//TODO: Implement.

	Log(warning) << "CDevice9::GetSoftwareVertexProcessing is not implemented!" << std::endl;

	return true;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetStreamSourceFreq(UINT StreamNumber, UINT *pDivider)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::GetStreamSourceFreq is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** ppSwapChain)
{
	(*ppSwapChain) = (IDirect3DSwapChain9*)mSwapChains[iSwapChain];

	if ((*ppSwapChain) != nullptr)
	{
		(*ppSwapChain)->AddRef();
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture)
{


	if ((*ppTexture) != nullptr)
	{
		(*ppTexture)->AddRef();
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl)
{


	if ((*ppDecl) != nullptr)
	{
		(*ppDecl)->AddRef();
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShader(IDirect3DVertexShader9** ppShader)
{


	if ((*ppShader) != nullptr)
	{
		(*ppShader)->AddRef();
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetViewport(D3DVIEWPORT9* pViewport)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::LightEnable(DWORD LightIndex, BOOL bEnable)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX *pMatrix)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::MultiplyTransform is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9 *pDestBuffer, IDirect3DVertexDeclaration9 *pVertexDecl, DWORD Flags)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::ProcessVertices is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::Reset(D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	//if (mCommandStreamManager->mResult == vk::Result::eErrorDeviceLost)
	//{
	//	/*
	//	All of the the d3d9 "device state" is attached to the RealDevice object so just destroy the current one and make a new one.
	//	The application is responsible for destroying any handles it has before calling Reset but I doubt games really do that.
	//	*/
	//	memcpy(&mPresentationParameters, pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS));

	//	Destroy();
	//	Init();
	//}
	//else
	//{

	//}

	return S_OK;
}

HRESULT CDevice9::SetClipPlane(DWORD Index, const float *pPlane)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::SetClipPlane is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetClipStatus(const D3DCLIPSTATUS9 *pClipStatus)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::SetClipStatus is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetCurrentTexturePalette(UINT PaletteNumber)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::SetCurrentTexturePalette is not implemented!" << std::endl;

	return S_OK;
}

void STDMETHODCALLTYPE CDevice9::SetCursorPosition(INT X, INT Y, DWORD Flags)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::SetCursorPosition is not implemented!" << std::endl;

	return;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9 *pCursorBitmap)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::SetCursorProperties is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil)
{
	if (pNewZStencil == nullptr)
	{
		Log(warning) << "CDevice9::SetDepthStencilSurface passing null should disable the stencil operation but this isn't supported yet." << std::endl;
		return S_OK;
	}

	((CSurface9*)pNewZStencil)->PrivateAddRef();

	if (mDepthStencilSurface != nullptr)
	{
		mDepthStencilSurface->PrivateRelease();
	}

	mDepthStencilSurface = (CSurface9*)pNewZStencil;



	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetDialogBoxMode(BOOL bEnableDialogs)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::SetDialogBoxMode is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetFVF(DWORD FVF)
{


	return S_OK;
}

void STDMETHODCALLTYPE CDevice9::SetGammaRamp(UINT  iSwapChain, DWORD Flags, const D3DGAMMARAMP *pRamp)
{
	auto& swapChain = mSwapChains[iSwapChain];

	HDC windowHdc = GetDC(swapChain->mPresentationParameters->hDeviceWindow);
	SetDeviceGammaRamp(windowHdc, (LPVOID)pRamp);

	if (Flags != D3DSGR_NO_CALIBRATION)
	{
		//TODO: Implement.
		Log(warning) << "CDevice9::SetGammaRamp D3DSGR_CALIBRATE is not implemented!" << std::endl;
	}
}

HRESULT STDMETHODCALLTYPE CDevice9::SetIndices(IDirect3DIndexBuffer9* pIndexData)
{


	if (pIndexData != nullptr)
	{
		((CIndexBuffer9*)pIndexData)->PrivateAddRef();
	}
	if (mDeviceState.mIndexBuffer != nullptr)
	{
		mDeviceState.mIndexBuffer->PrivateRelease();
	}
	mDeviceState.mIndexBuffer = (CIndexBuffer9*)pIndexData;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetLight(DWORD Index, const D3DLIGHT9* pLight)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetMaterial(const D3DMATERIAL9* pMaterial)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetNPatchMode(float nSegments)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY *pEntries)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::SetPaletteEntries is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShader(IDirect3DPixelShader9* pShader)
{


	if (pShader != nullptr)
	{
		((CPixelShader9*)pShader)->PrivateAddRef();
	}
	if (mDeviceState.mPixelShader != nullptr)
	{
		mDeviceState.mPixelShader->PrivateRelease();
	}
	mDeviceState.mPixelShader = (CPixelShader9*)pShader;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantF(UINT StartRegister, const float *pConstantData, UINT Vector4fCount)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantI(UINT StartRegister, const int *pConstantData, UINT Vector4iCount)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
	if (mRenderTargets[RenderTargetIndex] != nullptr)
	{
		mRenderTargets[RenderTargetIndex]->PrivateRelease();
	}
	mRenderTargets[RenderTargetIndex] = (CSurface9*)pRenderTarget;
	if (mRenderTargets[RenderTargetIndex] != nullptr)
	{
		mRenderTargets[RenderTargetIndex]->PrivateAddRef();
	}



	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetScissorRect(const RECT* pRect)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetSoftwareVertexProcessing(BOOL bSoftware)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::SetSoftwareVertexProcessing is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
	auto& streamSource = mDeviceState.mStreamSource[StreamNumber];



	if (pStreamData != nullptr)
	{
		((CVertexBuffer9*)pStreamData)->PrivateAddRef();
	}
	if (streamSource.vertexBuffer != nullptr)
	{
		streamSource.vertexBuffer->PrivateRelease();
	}
	streamSource.vertexBuffer = (CVertexBuffer9*)pStreamData;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetStreamSourceFreq(UINT StreamNumber, UINT FrequencyParameter)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::SetStreamSourceFreq is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTexture(DWORD Sampler, IDirect3DBaseTexture9* pTexture)
{


	if (pTexture != nullptr)
	{
		switch (pTexture->GetType())
		{
		case D3DRTYPE_VOLUMETEXTURE:
			((CVolumeTexture9*)pTexture)->PrivateAddRef();
			break;
		case D3DRTYPE_CUBETEXTURE:
			((CCubeTexture9*)pTexture)->PrivateAddRef();
			break;
		case D3DRTYPE_TEXTURE:
			((CTexture9*)pTexture)->PrivateAddRef();
			break;
		default:
			break;
		}	
	}
	if (mDeviceState.mTexture[Sampler] != nullptr)
	{
		switch (mDeviceState.mTexture[Sampler]->GetType())
		{
		case D3DRTYPE_VOLUMETEXTURE:
			((CVolumeTexture9*)mDeviceState.mTexture[Sampler])->PrivateRelease();
			break;
		case D3DRTYPE_CUBETEXTURE:
			((CCubeTexture9*)mDeviceState.mTexture[Sampler])->PrivateRelease();
			break;
		case D3DRTYPE_TEXTURE:
			((CTexture9*)mDeviceState.mTexture[Sampler])->PrivateRelease();
			break;
		default:
			break;
		}
	}
	mDeviceState.mTexture[Sampler] = pTexture;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
{


	if (pDecl != nullptr)
	{
		((CVertexDeclaration9*)pDecl)->PrivateAddRef();
	}
	if (mDeviceState.mVertexDeclaration != nullptr)
	{
		mDeviceState.mVertexDeclaration->PrivateRelease();
	}
	mDeviceState.mVertexDeclaration = (CVertexDeclaration9*)pDecl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShader(IDirect3DVertexShader9* pShader)
{


	if (pShader != nullptr)
	{
		((CVertexShader9*)pShader)->PrivateAddRef();
	}
	if (mDeviceState.mVertexShader != nullptr)
	{
		mDeviceState.mVertexShader->PrivateRelease();
	}
	mDeviceState.mVertexShader = (CVertexShader9*)pShader;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetViewport(const D3DVIEWPORT9* pViewport)
{


	return S_OK;
}

BOOL STDMETHODCALLTYPE CDevice9::ShowCursor(BOOL bShow)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::ShowCursor is not implemented!" << std::endl;

	return TRUE;
}

HRESULT STDMETHODCALLTYPE CDevice9::StretchRect(IDirect3DSurface9 *pSourceSurface, const RECT *pSourceRect, IDirect3DSurface9 *pDestSurface, const RECT *pDestRect, D3DTEXTUREFILTERTYPE Filter)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::TestCooperativeLevel()
{
	//if (mCommandStreamManager->mResult == vk::Result::eErrorDeviceLost) { return D3DERR_DEVICELOST; }

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::UpdateSurface(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, const POINT* pDestinationPoint)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::ValidateDevice(DWORD *pNumPasses)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::ValidateDevice is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetConvolutionMonoKernel(UINT width, UINT height, float *rows, float *columns)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::SetConvolutionMonoKernel is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::ComposeRects(IDirect3DSurface9 *pSrc, IDirect3DSurface9 *pDst, IDirect3DVertexBuffer9 *pSrcRectDescs, UINT NumRects, IDirect3DVertexBuffer9 *pDstRectDescs, D3DCOMPOSERECTSOP Operation, int Xoffset, int Yoffset)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::ComposeRects is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::PresentEx(const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion, DWORD dwFlags)
{






	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetGPUThreadPriority(INT *pPriority)
{
	(*pPriority) = mPriority;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetGPUThreadPriority(INT Priority)
{
	mPriority = Priority;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::WaitForVBlank(UINT iSwapChain)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::WaitForVBlank is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CheckResourceResidency(IDirect3DResource9 **pResourceArray, UINT32 NumResources)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::CheckResourceResidency is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetMaximumFrameLatency(UINT MaxLatency)
{
	mMaxLatency = MaxLatency;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetMaximumFrameLatency(UINT *pMaxLatency)
{
	(*pMaxLatency) = mMaxLatency;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CheckDeviceState(HWND hDestinationWindow)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::CheckDeviceState is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateRenderTargetEx(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle, DWORD Usage)
{
	HRESULT result = S_OK;

	//I added an extra int at the end so the signature would be different for this version. Locakable/Discard are both BOOL.
	CRenderTargetSurface9* obj = new CRenderTargetSurface9(this, Width, Height, Format);

	(*ppSurface) = (IDirect3DSurface9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateOffscreenPlainSurfaceEx(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle, DWORD Usage)
{
	HRESULT result = S_OK;

	CSurface9* ptr = new CSurface9(this, (CTexture9*)nullptr, Width, Height, 1, 0, Format, Pool, pSharedHandle);

	ptr->Init();

	(*ppSurface) = (IDirect3DSurface9*)ptr;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateDepthStencilSurfaceEx(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle, DWORD Usage)
{
	HRESULT result = S_OK;

	CSurface9* obj = new CSurface9(this, (CTexture9*)nullptr, Width, Height, Format, MultiSample, MultisampleQuality, Discard, pSharedHandle);

	obj->Init();

	(*ppSurface) = (IDirect3DSurface9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::ResetEx(D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode)
{
	//if (mCommandStreamManager->mResult == vk::Result::eErrorDeviceLost)
	//{
	//	/*
	//	All of the the d3d9 "device state" is attached to the RealDevice object so just destroy the current one and make a new one.
	//	The application is responsible for destroying any handles it has before calling Reset but I doubt games really do that.
	//	*/
	//	memcpy(&mPresentationParameters, pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS));

	//	Destroy();
	//	Init();
	//}
	//else
	//{

	//}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDisplayModeEx(UINT iSwapChain, D3DDISPLAYMODEEX *pMode, D3DDISPLAYROTATION *pRotation)
{


	(*pRotation) = D3DDISPLAYROTATION_IDENTITY;

	return S_OK;
}