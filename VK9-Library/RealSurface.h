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

#include <vulkan/vulkan.hpp>
#include <vulkan/vk_sdk_platform.h>

#include "RealDevice.h"

#ifndef REALSURFACE_H
#define REALSURFACE_H

class CSurface9;
class CVolume9;

struct RealSurface
{
	BOOL mIsFlushed = true;
	void* mData = nullptr;
	vk::Image* mParentImage;
	CSurface9* mSurface9 = nullptr;
	VmaAllocationInfo mImageAllocationInfo;

	vk::Image mImage;
	VmaAllocation mImageAllocation;

	vk::Image mStagingImage;
	VmaAllocation mStagingImageAllocation;

	std::vector< std::array<vk::Offset3D, 2> > mDirtyRects;

	vk::Extent3D mExtent;
	vk::Format mRealFormat = vk::Format::eR8G8B8A8Unorm;
	vk::ImageLayout mImageLayout = vk::ImageLayout::eGeneral;
	vk::SubresourceLayout mLayouts[1] = {};
	vk::ImageSubresource mSubresource;
	vk::ImageView mImageView;

	RealDevice* mRealDevice = nullptr; //null if not owner.
	RealSurface(RealDevice* realDevice, CSurface9* surface9, vk::Image* parentImage);
	~RealSurface();

	void Lock(D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags);
	void Unlock();
	void Flush();

};

#endif // REALSURFACE_H
