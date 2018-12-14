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

#include "RealVertexBuffer.h"

RealVertexBuffer::RealVertexBuffer(RealDevice* realDevice, size_t length, bool isDynamic)
	: mRealDevice(realDevice),
	mLength(length),
	mIsDynamic(isDynamic),
	mSize(0)
{
	vk::BufferCreateInfo bufferCreateInfo;
	bufferCreateInfo.size = length;
	
	VmaAllocationCreateInfo allocInfo = {};
	
	/*
	I use the stagingAllocation variable whichever buffer will be mapped. That way I can skip a branch in the lock and unlock.
	*/
	if (mIsDynamic)
	{
		bufferCreateInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		vmaCreateBuffer(mRealDevice->mAllocator, (VkBufferCreateInfo*)&bufferCreateInfo, &allocInfo, (VkBuffer*)&mBuffer, &mAllocation, &mAllocationInfo);

		bufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		vmaCreateBuffer(mRealDevice->mAllocator, (VkBufferCreateInfo*)&bufferCreateInfo, &allocInfo, (VkBuffer*)&mStagingBuffer, &mStagingAllocation, &mAllocationInfo);

		//d3d9 apps assume memory is cleared
		vmaMapMemory(mRealDevice->mAllocator, mStagingAllocation, &mData);
		memset(mData, 0, length);
		vmaUnmapMemory(mRealDevice->mAllocator, mStagingAllocation);
		mData = nullptr;
	}
	else
	{
		bufferCreateInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		vmaCreateBuffer(mRealDevice->mAllocator, (VkBufferCreateInfo*)&bufferCreateInfo, &allocInfo, (VkBuffer*)&mBuffer, &mStagingAllocation, &mAllocationInfo);

		//d3d9 apps assume memory is cleared
		vmaMapMemory(mRealDevice->mAllocator, mStagingAllocation, &mData);
		memset(mData, 0, length);
		vmaUnmapMemory(mRealDevice->mAllocator, mStagingAllocation);
		mData = nullptr;
	}
}

RealVertexBuffer::~RealVertexBuffer()
{
	if (mRealDevice != nullptr)
	{
		auto& device = mRealDevice->mDevice;
		if (mIsDynamic)
		{
			vmaDestroyBuffer(mRealDevice->mAllocator, (VkBuffer)mStagingBuffer, mStagingAllocation);
			vmaDestroyBuffer(mRealDevice->mAllocator, (VkBuffer)mBuffer, mAllocation);
		}
		else
		{
			vmaDestroyBuffer(mRealDevice->mAllocator, (VkBuffer)mBuffer, mStagingAllocation);
		}	
	}
}

void* RealVertexBuffer::Lock(size_t offset)
{
	if (mData == nullptr)
	{
		vmaMapMemory(mRealDevice->mAllocator, mStagingAllocation, &mData);
	}

	return ((char*)mData + offset);
}

void RealVertexBuffer::Unlock()
{
	if (mData != nullptr)
	{
		vmaFlushAllocation(mRealDevice->mAllocator, mStagingAllocation, 0, VK_WHOLE_SIZE);
		vmaUnmapMemory(mRealDevice->mAllocator, mStagingAllocation);
		mData = nullptr;
	}
}

