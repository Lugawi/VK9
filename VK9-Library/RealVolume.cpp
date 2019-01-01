// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
Copyright(c) 2019 Christopher Joseph Dean Schaefer

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

#include "RealVolume.h"
#include "CVolume9.h"

#include "Utilities.h"

RealVolume::RealVolume(RealDevice* realDevice, CVolume9* volume9, vk::Image* parentImage)
	: mRealDevice(realDevice),
	mVolume9(volume9),
	mParentImage(parentImage)
{
	mAttributeSize = 1; //TODO: figure out format
	mLength = mAttributeSize * volume9->mWidth * volume9->mHeight * volume9->mDepth;

	vk::BufferCreateInfo bufferCreateInfo;
	bufferCreateInfo.size = mLength;
	bufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY; //VMA_MEMORY_USAGE_CPU_TO_GPU;
	allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

	vmaCreateBuffer(mRealDevice->mAllocator, (VkBufferCreateInfo*)&bufferCreateInfo, &allocInfo, (VkBuffer*)&mBuffer, &mAllocation, &mAllocationInfo);

	//mSize = mLength / attributeSize;
}

RealVolume::~RealVolume()
{
	if (mRealDevice != nullptr)
	{
		auto& device = mRealDevice->mDevice;
		vmaDestroyBuffer(mRealDevice->mAllocator, (VkBuffer)mBuffer, mAllocation);
	}
}

void* RealVolume::Lock(size_t offset)
{
	if (mData == nullptr)
	{
		vmaMapMemory(mRealDevice->mAllocator, mAllocation, &mData);
	}

	//mIsFlushed = false;

	return ((char*)mData + offset);
}

void* RealVolume::LockBox(D3DLOCKED_BOX * pLockedVolume, CONST D3DBOX* pBox, DWORD Flags)
{
	size_t offset = 0;

	offset = mAttributeSize * pBox->Top * pBox->Left * pBox->Front;

	pLockedVolume->pBits = Lock(offset);
	pLockedVolume->RowPitch = mAttributeSize;
	pLockedVolume->SlicePitch = mAttributeSize;

	if ((Flags & D3DLOCK_READONLY) == D3DLOCK_READONLY)
	{
		//Nothing
	}
	else
	{
		mIsFlushed = false;
	}

	return pLockedVolume->pBits;
}

void RealVolume::Unlock()
{
	if (mData != nullptr)
	{
		vmaFlushAllocation(mRealDevice->mAllocator, mAllocation, 0, VK_WHOLE_SIZE);
		vmaUnmapMemory(mRealDevice->mAllocator, mAllocation);
		mData = nullptr;
	}
}

void RealVolume::Flush()
{
	if (mIsFlushed)
	{
		return;
	}

	if (mParentImage == nullptr)
	{
		return;
	}

	vk::CommandBuffer commandBuffer;
	vk::Result result;

	vk::CommandBufferAllocateInfo commandBufferInfo;
	commandBufferInfo.commandPool = mRealDevice->mCommandPool;
	commandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
	commandBufferInfo.commandBufferCount = 1;

	result = mRealDevice->mDevice.allocateCommandBuffers(&commandBufferInfo, &commandBuffer);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealVolume::Flush vkAllocateCommandBuffers failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	vk::CommandBufferInheritanceInfo commandBufferInheritanceInfo;
	commandBufferInheritanceInfo.renderPass = nullptr;
	commandBufferInheritanceInfo.subpass = 0;
	commandBufferInheritanceInfo.framebuffer = nullptr;
	commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
	//commandBufferInheritanceInfo.queryFlags = 0;
	//commandBufferInheritanceInfo.pipelineStatistics = 0;

	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

	result = commandBuffer.begin(&commandBufferBeginInfo);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealVolume::Flush vkBeginCommandBuffer failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	///////

	vk::BufferImageCopy bufferCopyRegion;
	bufferCopyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	bufferCopyRegion.imageSubresource.mipLevel = 0;
	bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
	bufferCopyRegion.imageSubresource.layerCount = 1;
	bufferCopyRegion.imageExtent.width = mVolume9->mWidth;
	bufferCopyRegion.imageExtent.height = mVolume9->mHeight;
	bufferCopyRegion.imageExtent.depth = mVolume9->mDepth;

	ReallySetImageLayout(commandBuffer, (*mParentImage), vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 1, mVolume9->mMipIndex, 1);

	commandBuffer.copyBufferToImage(mBuffer, (*mParentImage), vk::ImageLayout::eTransferDstOptimal, 1, &bufferCopyRegion);

	ReallySetImageLayout(commandBuffer, (*mParentImage), vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eGeneral, 1, mVolume9->mMipIndex, 1);

	///////

	commandBuffer.end();

	vk::CommandBuffer commandBuffers[] = { commandBuffer };
	vk::Fence nullFence;
	vk::SubmitInfo submitInfo;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffers;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;

	result = mRealDevice->mQueue.submit(1, &submitInfo, nullFence);
	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "RealVolume::Flush vkQueueSubmit failed with return code of " << GetResultString((VkResult)result);
		return;
	}

	mRealDevice->mQueue.waitIdle();

	mRealDevice->mDevice.freeCommandBuffers(mRealDevice->mCommandPool, 1, commandBuffers);

	mIsFlushed = true;
}