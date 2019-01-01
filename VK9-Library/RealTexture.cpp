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

#include "RealTexture.h"
#include "CTexture9.h"
#include "CCubeTexture9.h"
#include "CVolumeTexture9.h"
#include "Utilities.h"

RealTexture::RealTexture(RealDevice* realDevice, CTexture9* texture9)
	: mRealDevice(realDevice),
	mSize(0),
	mTexture9(texture9)
{
	mRealFormat = ConvertFormat(texture9->mFormat);

	if (mRealFormat == vk::Format::eUndefined)//VK_FORMAT_UNDEFINED
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealTexture::RealTexture (CTexture9*) unknown format: " << texture9->mFormat;
	}

	mExtent = vk::Extent3D(texture9->mWidth, texture9->mHeight, 1);
	mLevels = texture9->mLevels;
	mLayers = 1;

	vk::ImageCreateInfo imageCreateInfo;
	imageCreateInfo.imageType = vk::ImageType::e2D;
	imageCreateInfo.format = mRealFormat; //VK_FORMAT_B8G8R8A8_UNORM
	imageCreateInfo.extent = mExtent;
	imageCreateInfo.mipLevels = texture9->mLevels;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
	imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
	imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eColorAttachment;
	//imageCreateInfo.flags = 0;
	imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined; //VK_IMAGE_LAYOUT_PREINITIALIZED;

	vk::Result result;
	auto& vulkanDevice = mRealDevice->mDevice;

	VmaAllocationCreateInfo imageAllocInfo = {};
	imageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	result = (vk::Result)vmaCreateImage(mRealDevice->mAllocator, (VkImageCreateInfo*)&imageCreateInfo, &imageAllocInfo, (VkImage*)&mImage, &mImageAllocation, &mImageAllocationInfo);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealTexture::RealTexture (CTexture9*) vmaCreateImage failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	vk::ImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.image = mImage;
	imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
	imageViewCreateInfo.format = mRealFormat;
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	imageViewCreateInfo.subresourceRange.levelCount = texture9->mLevels;

	/*
	This block handles the luminance & x formats. They are converted to color formats but need a little mapping to make them work correctly.
	*/
	switch (texture9->mFormat)
	{
	case D3DFMT_R5G6B5:
		//Vulkan has a matching format but nvidia doesn't support using it as a color attachment so we just use the other one and re-map the components.
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_A8:
		//TODO: Revisit A8 mapping.
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eR);
		break;
	case D3DFMT_L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_L16:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_A8L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG);
		break;
	case D3DFMT_X8R8G8B8:
	case D3DFMT_X8B8G8R8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eOne);
		break;
	default:
		break;
	}

	result = vulkanDevice.createImageView(&imageViewCreateInfo, nullptr, &mImageView);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealTexture::RealTexture (CTexture9*) vkCreateImageView failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	mRealDevice->SetImageLayout(mImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
}

RealTexture::RealTexture(RealDevice* realDevice, CCubeTexture9* texture9)
	: mRealDevice(realDevice),
	mSize(0),
	mCubeTexture9(texture9)
{
	mRealFormat = ConvertFormat(texture9->mFormat);

	if (mRealFormat == vk::Format::eUndefined)//VK_FORMAT_UNDEFINED
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealTexture::RealTexture (CCubeTexture9*) unknown format: " << texture9->mFormat;
	}

	mExtent = vk::Extent3D(texture9->mEdgeLength, texture9->mEdgeLength, 1);
	mLevels = texture9->mLevels;
	mLayers = 6;

	vk::ImageCreateInfo imageCreateInfo;
	imageCreateInfo.imageType = vk::ImageType::e2D;
	imageCreateInfo.format = mRealFormat; //VK_FORMAT_B8G8R8A8_UNORM
	imageCreateInfo.extent = vk::Extent3D(texture9->mEdgeLength, texture9->mEdgeLength, 1);
	imageCreateInfo.mipLevels = texture9->mLevels;
	imageCreateInfo.arrayLayers = 6;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
	imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
	imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc;
	imageCreateInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;
	imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined; //VK_IMAGE_LAYOUT_PREINITIALIZED;
	imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;

	vk::Result result;
	auto& vulkanDevice = mRealDevice->mDevice;

	VmaAllocationCreateInfo imageAllocInfo = {};
	imageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	result = (vk::Result)vmaCreateImage(mRealDevice->mAllocator, (VkImageCreateInfo*)&imageCreateInfo, &imageAllocInfo, (VkImage*)&mImage, &mImageAllocation, &mImageAllocationInfo);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealTexture::RealTexture (CCubeTexture9*) vmaCreateImage failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	vk::ImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.image = mImage;
	imageViewCreateInfo.viewType = vk::ImageViewType::eCube; //e2D
	imageViewCreateInfo.format = mRealFormat;
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	//imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.levelCount = texture9->mLevels;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 6;

	/*
	This block handles the luminance & x formats. They are converted to color formats but need a little mapping to make them work correctly.
	*/
	switch (texture9->mFormat)
	{
	case D3DFMT_A8:
		//TODO: Revisit A8 mapping.
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eR);
		break;
	case D3DFMT_L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_L16:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_A8L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG);
		break;
	case D3DFMT_X8R8G8B8:
	case D3DFMT_X8B8G8R8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eOne);
		break;
	default:
		break;
	}

	result = vulkanDevice.createImageView(&imageViewCreateInfo, nullptr, &mImageView);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealTexture::RealTexture (CCubeTexture9*) vkCreateImageView failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	mRealDevice->SetImageLayout(mImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
}

RealTexture::RealTexture(RealDevice* realDevice, CVolumeTexture9* texture9)
	: mRealDevice(realDevice),
	mSize(0),
	mVolumeTexture9(texture9)
{
	mRealFormat = ConvertFormat(texture9->mFormat);

	if (mRealFormat == vk::Format::eUndefined)//VK_FORMAT_UNDEFINED
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealTexture::RealTexture (CVolumeTexture9*) unknown format: " << texture9->mFormat;
	}

	mExtent = vk::Extent3D(texture9->mWidth, texture9->mHeight, 1);
	mLevels = texture9->mLevels;
	mLayers = 1;

	vk::ImageCreateInfo imageCreateInfo;
	imageCreateInfo.imageType = vk::ImageType::e3D;
	imageCreateInfo.format = mRealFormat;
	imageCreateInfo.extent = vk::Extent3D(texture9->mWidth, texture9->mHeight, texture9->mDepth);
	imageCreateInfo.mipLevels = texture9->mLevels;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
	imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
	imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferSrc;
	//imageCreateInfo.flags = vk::ImageCreateFlagBits::eVolumeCompatible;
	imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined; //VK_IMAGE_LAYOUT_PREINITIALIZED;
	imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;

	vk::Result result;
	auto& vulkanDevice = mRealDevice->mDevice;

	VmaAllocationCreateInfo imageAllocInfo = {};
	imageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	result = (vk::Result)vmaCreateImage(mRealDevice->mAllocator, (VkImageCreateInfo*)&imageCreateInfo, &imageAllocInfo, (VkImage*)&mImage, &mImageAllocation, &mImageAllocationInfo);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealTexture::RealTexture (CVolumeTexture9*) vmaCreateImage failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	vk::ImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.image = mImage;
	imageViewCreateInfo.viewType = vk::ImageViewType::e3D;
	imageViewCreateInfo.format = mRealFormat;
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	//imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.levelCount = texture9->mLevels;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	/*
	This block handles the luminance & x formats. They are converted to color formats but need a little mapping to make them work correctly.
	*/
	switch (texture9->mFormat)
	{
	case D3DFMT_A8:
		//TODO: Revisit A8 mapping.
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eR);
		break;
	case D3DFMT_L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_L16:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eOne);
		break;
	case D3DFMT_A8L8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG);
		break;
	case D3DFMT_X8R8G8B8:
	case D3DFMT_X8B8G8R8:
		imageViewCreateInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eOne);
		break;
	default:
		break;
	}

	result = vulkanDevice.createImageView(&imageViewCreateInfo, nullptr, &mImageView);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealTexture::RealTexture (CVolumeTexture9*) vkCreateImageView failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	mRealDevice->SetImageLayout(mImage, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
}

RealTexture::~RealTexture()
{
	if (mRealDevice != nullptr)
	{
		auto& device = mRealDevice->mDevice;
		device.destroyImageView(mImageView, nullptr);
		device.destroySampler(mSampler, nullptr);
		
		vmaDestroyImage(mRealDevice->mAllocator, (VkImage)mImage, mImageAllocation);
	}

}

