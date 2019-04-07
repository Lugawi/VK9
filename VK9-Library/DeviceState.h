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

 //On my test hardware this was 32.
#define MAX_VERTEX_INPUTS 32
#define MAX_PIXEL_SHADER_CONST 256
#define MAX_VERTEX_SHADER_CONST 256

class CSurface9;
class CIndexBuffer9;
class CVertexBuffer9;
class CPixelShader9;
class CVertexShader9;
class CVertexDeclaration9;
class CSwapChain9;

struct PaddedLight
{
	D3DLIGHTTYPE    Type;            /* Type of light source */
	float           Range;            /* Cutoff range */
	float           Falloff;          /* Falloff */
	float           Attenuation0;     /* Constant attenuation */
	float           Attenuation1;     /* Linear attenuation */
	float           Attenuation2;     /* Quadratic attenuation */
	float           Theta;            /* Inner angle of spotlight cone */
	float           Phi;              /* Outer angle of spotlight cone */
	D3DCOLORVALUE   Diffuse;         /* Diffuse color of light */
	D3DCOLORVALUE   Specular;        /* Specular color of light */
	D3DCOLORVALUE   Ambient;         /* Ambient color of light */
	D3DVECTOR       Position;         /* Position in world space */
	int filler1;
	D3DVECTOR       Direction;        /* Direction in world space */
	int filler2;

	PaddedLight() {}

	PaddedLight(const D3DLIGHT9& light)
	{
		
		this->Type = light.Type;
		this->Diffuse = light.Diffuse;
		this->Specular = light.Specular;
		this->Ambient = light.Ambient;
		this->Position = light.Position;
		this->Direction = light.Direction;
		this->Range = light.Range;
		this->Falloff = light.Falloff;
		this->Attenuation0 = light.Attenuation0;
		this->Attenuation1 = light.Attenuation1;
		this->Attenuation2 = light.Attenuation2;
		this->Theta = light.Theta;
		this->Phi = light.Phi;	
	}

};

struct DeviceState
{
	bool mCapturedVertexDeclaration = false;
	CVertexDeclaration9* mVertexDeclaration = nullptr;

	bool mCapturedFVF = false;
	DWORD mFVF = 0;

	bool mCapturedIndexBuffer = false;
	CIndexBuffer9* mIndexBuffer = nullptr;

	// + 3 is for extra state I'm sticking on the end.
	bool mCapturedRenderState[D3DRS_BLENDOPALPHA + 1 + 3] = {};
	unsigned long mRenderState[D3DRS_BLENDOPALPHA + 1 + 3] = {};

	bool mCapturedNPatchMode = false;
	float mNPatchMode = 0.0f;

	//A device can have only 8 textures.
	//https://docs.microsoft.com/en-us/windows/desktop/api/d3d9helper/nf-d3d9helper-idirect3ddevice9-settexturestagestate

	bool mCapturedAnyTextureStageState = false;
	bool mCapturedTextureStageState[8][D3DTSS_CONSTANT + 1];
	unsigned long mTextureStageState[8][D3DTSS_CONSTANT + 1];

	bool mCapturedAnySamplerState = false;
	bool mCapturedSamplerState[16 + 4][D3DSAMP_DMAPOFFSET + 1];
	std::array< std::array<DWORD, D3DSAMP_DMAPOFFSET + 1>, 16 + 4> mSamplerState = {};

	bool mCapturedAnyStreamSource = false;
	bool mCapturedStreamSource[MAX_VERTEX_INPUTS] = {};
	struct StreamSource
	{
		CVertexBuffer9* vertexBuffer;
		unsigned int offset;
		unsigned int stride;
	};
	StreamSource mStreamSource[MAX_VERTEX_INPUTS] = {};

	bool mCapturedAnyStreamFrequency = false;
	bool mCapturedStreamSourceFrequency[MAX_VERTEX_INPUTS] = {};
	unsigned int mStreamSourceFrequency[MAX_VERTEX_INPUTS] = {};

	bool mCapturedAnyTexture = false;
	bool mCapturedTexture[16 + 4] = {};
	IDirect3DBaseTexture9* mTexture[16 + 4] = {};

	bool mCapturedTransform[512] = {};
	D3DMATRIX mTransform[512] = {};

	bool mCapturedMaterial = false;
	D3DMATERIAL9 mMaterial = {};

	bool mCapturedLight[8] = {};
	D3DLIGHT9 mLight[8] = {};

	bool mCapturedLightEnable[8] = {};
	int mLightEnableState[8] = {};

	bool mCapturedPixelShader = false;
	CPixelShader9* mPixelShader = nullptr;

	bool mCapturedVertexShader = false;
	CVertexShader9* mVertexShader = nullptr;

	bool mCapturedViewport;
	D3DVIEWPORT9 mViewport;

	float mPixelShaderConstantF[MAX_PIXEL_SHADER_CONST][4] = {};
	int mPixelShaderConstantI[16][4] = {};
	int mPixelShaderConstantB[16] = {};

	float mVertexShaderConstantF[MAX_VERTEX_SHADER_CONST][4] = {};
	int mVertexShaderConstantI[16][4] = {};
	int mVertexShaderConstantB[16] = {};

	bool mCapturedClipPlane[6] = {};
	float mClipPlane[6][4] = {};

	bool mCapturedScissorRect = false;
	RECT mScissorRect = {};

	bool mCapturedPaletteNumber;
	unsigned int mPaletteNumber;
};