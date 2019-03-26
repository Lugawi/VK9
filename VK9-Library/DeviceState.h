#pragma once

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

#include <vulkan/vulkan.hpp>
#include <vulkan/vk_sdk_platform.h>
#include "d3d9.h"

#include<vector>

class CSurface9;
class CIndexBuffer9;
class CVertexBuffer9;
class CPixelShader9;
class CVertexShader9;
class CVertexDeclaration9;
class CSwapChain9;

struct DeviceState
{
	CSurface9* mDepthStencilSurface = nullptr;
	CIndexBuffer9* mIndexBuffer = nullptr;
	CPixelShader9* mPixelShader = nullptr;
	CVertexShader9* mVertexShader = nullptr;
	CVertexBuffer9* mVertexBuffer = nullptr;
	IDirect3DBaseTexture9* mTextures[16] = {};
	CVertexDeclaration9* mVertexDeclaration = nullptr;
	CSurface9* mRenderTargets[4] = {};
	UINT mMaxLatency = 0;
	INT mPriority = 0;
	UINT mAvailableTextureMemory = 0;
	std::vector<CSwapChain9*> mSwapChains;
};