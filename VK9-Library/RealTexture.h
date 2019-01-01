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

class CTexture9;
class CCubeTexture9;
class CVolumeTexture9;

#ifndef REALTEXTURE_H
#define REALTEXTURE_H

struct RealTexture
{
	void* mData = nullptr;
	int32_t mSize;

	vk::Extent3D mExtent;
	vk::Format mRealFormat = vk::Format::eUndefined;
	vk::Image mImage;
	VmaAllocation mImageAllocation;
	VmaAllocationInfo mImageAllocationInfo;
	vk::Sampler mSampler;
	vk::ImageView mImageView;
	uint32_t mLevels = 1;
	uint32_t mLayers = 1;

	CTexture9* mTexture9 = nullptr;
	CCubeTexture9* mCubeTexture9 = nullptr;
	CVolumeTexture9* mVolumeTexture9 = nullptr;

	RealDevice* mRealDevice = nullptr; //null if not owner.
	RealTexture(RealDevice* realDevice, CTexture9* texture9);
	RealTexture(RealDevice* realDevice, CCubeTexture9* texture9);
	RealTexture(RealDevice* realDevice, CVolumeTexture9* texture9);

	~RealTexture();
};

#endif // REALTEXTURE_H
