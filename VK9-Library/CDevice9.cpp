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

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#ifndef MAX_DESCRIPTOR
#define MAX_DESCRIPTOR 2048u
#endif // !MAX_DESCRIPTOR

#ifndef MAX_BUFFERUPDATE
#define MAX_BUFFERUPDATE 65536u
#endif // !MAX_BUFFERUPDATE

#define D3DCOLOR_A(dw) (((float)(((dw) >> 24) & 0xFF)) / 255.0f)
#define D3DCOLOR_R(dw) (((float)(((dw) >> 16) & 0xFF)) / 255.0f)
#define D3DCOLOR_G(dw) (((float)(((dw) >> 8) & 0xFF)) / 255.0f)
#define D3DCOLOR_B(dw) (((float)(((dw) >> 0) & 0xFF)) / 255.0f)

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
#include "CVolume9.h"
#include "LogManager.h"
#include "BitCast.h"
//#include "PrivateTypes.h"

const uint32_t XYZRHW_VERT[] =
#include "XYZRHW.vert.h"
;
const uint32_t XYZ_VERT[] =
#include "XYZ.vert.h"
;
const uint32_t XYZ_FRAG[] =
#include "XYZ.frag.h"
;

const uint32_t XYZRHW_DIFFUSE_VERT[] =
#include "XYZRHW_DIFFUSE.vert.h"
;
const uint32_t XYZ_DIFFUSE_VERT[] =
#include "XYZ_DIFFUSE.vert.h"
;
const uint32_t XYZ_DIFFUSE_FRAG[] =
#include "XYZ_DIFFUSE.frag.h"
;
const uint32_t XYZ_DIFFUSE_GEOM[] =
#include "XYZ_DIFFUSE.geom.h"
;

const uint32_t XYZRHW_TEX1_VERT[] =
#include "XYZRHW_TEX1.vert.h"
;
const uint32_t XYZ_TEX1_VERT[] =
#include "XYZ_TEX1.vert.h"
;
const uint32_t XYZ_TEX1_FRAG[] =
#include "XYZ_TEX1.frag.h"
;

const uint32_t XYZRHW_TEX2_VERT[] =
#include "XYZRHW_TEX2.vert.h"
;
const uint32_t XYZ_TEX2_VERT[] =
#include "XYZ_TEX2.vert.h"
;
const uint32_t XYZ_TEX2_FRAG[] =
#include "XYZ_TEX2.frag.h"
;

const uint32_t XYZRHW_DIFFUSE_TEX1_VERT[] =
#include "XYZRHW_DIFFUSE_TEX1.vert.h"
;
const uint32_t XYZ_DIFFUSE_TEX1_VERT[] =
#include "XYZ_DIFFUSE_TEX1.vert.h"
;
const uint32_t XYZ_DIFFUSE_TEX1_FRAG[] =
#include "XYZ_DIFFUSE_TEX1.frag.h"
;

const uint32_t XYZRHW_DIFFUSE_TEX2_VERT[] =
#include "XYZRHW_DIFFUSE_TEX2.vert.h"
;
const uint32_t XYZ_DIFFUSE_TEX2_VERT[] =
#include "XYZ_DIFFUSE_TEX2.vert.h"
;
const uint32_t XYZ_DIFFUSE_TEX2_FRAG[] =
#include "XYZ_DIFFUSE_TEX2.frag.h"
;

const uint32_t XYZ_NORMAL_VERT[] =
#include "XYZ_NORMAL.vert.h"
;
const uint32_t XYZ_NORMAL_FRAG[] =
#include "XYZ_NORMAL.frag.h"
;

const uint32_t XYZ_NORMAL_TEX1_VERT[] =
#include "XYZ_NORMAL_TEX1.vert.h"
;
const uint32_t XYZ_NORMAL_TEX1_FRAG[] =
#include "XYZ_NORMAL_TEX1.frag.h"
;

const uint32_t XYZ_NORMAL_TEX2_VERT[] =
#include "XYZ_NORMAL_TEX2.vert.h"
;
const uint32_t XYZ_NORMAL_TEX2_FRAG[] =
#include "XYZ_NORMAL_TEX2.frag.h"
;

const uint32_t XYZ_NORMAL_DIFFUSE_VERT[] =
#include "XYZ_NORMAL_DIFFUSE.vert.h"
;
const uint32_t XYZ_NORMAL_DIFFUSE_FRAG[] =
#include "XYZ_NORMAL_DIFFUSE.frag.h"
;

const uint32_t XYZ_NORMAL_DIFFUSE_TEX1_VERT[] =
#include "XYZ_NORMAL_DIFFUSE_TEX1.vert.h"
;
const uint32_t XYZ_NORMAL_DIFFUSE_TEX1_FRAG[] =
#include "XYZ_NORMAL_DIFFUSE_TEX1.frag.h"
;

const uint32_t XYZ_NORMAL_DIFFUSE_TEX2_VERT[] =
#include "XYZ_NORMAL_DIFFUSE_TEX2.vert.h"
;
const uint32_t XYZ_NORMAL_DIFFUSE_TEX2_FRAG[] =
#include "XYZ_NORMAL_DIFFUSE_TEX2.frag.h"
;

const uint32_t PIXEL_PASSTHROUGH_FRAG[] =
#include "PixelPassthrough.frag.h"
;

D3DMATRIX operator* (const D3DMATRIX& m1, const D3DMATRIX& m2)
{
	D3DMATRIX result;

	//TODO: Check to see msvc auto vectorization picks this up.
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
		}
	}

	return result;
}

int32_t ConvertPrimitiveCountToVertexCount(D3DPRIMITIVETYPE primtiveType, int32_t primtiveCount) noexcept
{
	int32_t output;

	switch (primtiveType)
	{
	case D3DPT_POINTLIST:
		output = primtiveCount;
		break;
	case D3DPT_LINELIST:
		output = primtiveCount * 2;
		break;
	case D3DPT_LINESTRIP:
		output = primtiveCount + 1;
		break;
	case D3DPT_TRIANGLELIST:
		output = primtiveCount * 3;
		break;
	case D3DPT_TRIANGLESTRIP:
		output = primtiveCount + 2;
		break;
	case D3DPT_TRIANGLEFAN:
		output = primtiveCount + 2;
		break;
	default:
		output = primtiveCount;
		break;
	}

	return output;
}

int32_t ConvertPrimitiveCountToBufferSize(D3DPRIMITIVETYPE primtiveType, int32_t primtiveCount, int32_t vertexStride) noexcept
{
	int32_t output;

	switch (primtiveType)
	{
	case D3DPT_POINTLIST:
		output = (primtiveCount)* vertexStride;
		break;
	case D3DPT_LINELIST:
		output = (primtiveCount * 2) * vertexStride;
		break;
	case D3DPT_LINESTRIP:
		output = (primtiveCount + 1) * vertexStride;
		break;
	case D3DPT_TRIANGLELIST:
		output = (primtiveCount * 3) * vertexStride;
		break;
	case D3DPT_TRIANGLESTRIP:
		output = (primtiveCount + 2) * vertexStride;
		break;
	case D3DPT_TRIANGLEFAN:
		output = (primtiveCount + 2) * vertexStride;
		break;
	default:
		output = primtiveCount * vertexStride;
		break;
	}

	return output;
}

vk::CullModeFlagBits GetCullMode(D3DCULL input) noexcept
{
	vk::CullModeFlagBits output;

	switch (input)
	{
	case D3DCULL_NONE:
		output = vk::CullModeFlagBits::eNone;
		break;
	case D3DCULL_CW:
		output = vk::CullModeFlagBits::eBack;
		break;
	case D3DCULL_CCW:
		output = vk::CullModeFlagBits::eBack;
		break;
	default:
		output = vk::CullModeFlagBits::eNone;
		break;
	}

	return output;
}

vk::FrontFace GetFrontFace(D3DCULL input) noexcept
{
	vk::FrontFace output;

	switch (input)
	{
	case D3DCULL_NONE:
		output = vk::FrontFace::eClockwise;
		break;
	case D3DCULL_CW:
		output = vk::FrontFace::eCounterClockwise;
		break;
	case D3DCULL_CCW:
		output = vk::FrontFace::eClockwise;
		break;
	default:
		output = vk::FrontFace::eClockwise;
		break;
	}

	return output;
}

vk::CompareOp ConvertCompareOperation(D3DCMPFUNC input) noexcept
{
	vk::CompareOp output;

	switch (input)
	{
	case D3DCMP_NEVER:
		output = (vk::CompareOp)VK_COMPARE_OP_NEVER;
		break;
	case D3DCMP_LESS:
		output = (vk::CompareOp)VK_COMPARE_OP_LESS;
		break;
	case D3DCMP_EQUAL:
		output = (vk::CompareOp)VK_COMPARE_OP_EQUAL;
		break;
	case D3DCMP_LESSEQUAL:
		output = (vk::CompareOp)VK_COMPARE_OP_LESS_OR_EQUAL;
		break;
	case D3DCMP_GREATER:
		output = (vk::CompareOp)VK_COMPARE_OP_GREATER;
		break;
	case D3DCMP_NOTEQUAL:
		output = (vk::CompareOp)VK_COMPARE_OP_NOT_EQUAL;
		break;
	case D3DCMP_GREATEREQUAL:
		output = (vk::CompareOp)VK_COMPARE_OP_GREATER_OR_EQUAL;
		break;
	case D3DCMP_ALWAYS:
		output = (vk::CompareOp)VK_COMPARE_OP_ALWAYS;
		break;
	default:
		output = (vk::CompareOp)VK_COMPARE_OP_MAX_ENUM;
		break;
	}

	return output;
}

vk::StencilOp ConvertStencilOperation(D3DSTENCILOP input) noexcept
{
	vk::StencilOp output;

	switch (input)
	{
	case D3DSTENCILOP_KEEP:
		output = (vk::StencilOp)VK_STENCIL_OP_KEEP;
		break;
	case D3DSTENCILOP_ZERO:
		output = (vk::StencilOp)VK_STENCIL_OP_ZERO;
		break;
	case D3DSTENCILOP_REPLACE:
		output = (vk::StencilOp)VK_STENCIL_OP_REPLACE;
		break;
	case D3DSTENCILOP_INCRSAT:
		output = (vk::StencilOp)VK_STENCIL_OP_INCREMENT_AND_CLAMP;
		break;
	case D3DSTENCILOP_DECRSAT:
		output = (vk::StencilOp)VK_STENCIL_OP_DECREMENT_AND_CLAMP;
		break;
	case D3DSTENCILOP_INVERT:
		output = (vk::StencilOp)VK_STENCIL_OP_INVERT;
		break;
	case D3DSTENCILOP_INCR:
		output = (vk::StencilOp)VK_STENCIL_OP_INCREMENT_AND_WRAP;
		break;
	case D3DSTENCILOP_DECR:
		output = (vk::StencilOp)VK_STENCIL_OP_DECREMENT_AND_WRAP;
		break;
	default:
		output = (vk::StencilOp)VK_STENCIL_OP_MAX_ENUM;
		break;
	}

	return output;
}

vk::PolygonMode ConvertFillMode(D3DFILLMODE input) noexcept
{
	vk::PolygonMode output;

	switch (input)
	{
	case D3DFILL_SOLID:
		output = (vk::PolygonMode)VK_POLYGON_MODE_FILL;
		break;
	case D3DFILL_POINT:
		output = (vk::PolygonMode)VK_POLYGON_MODE_POINT;
		break;
	case D3DFILL_WIREFRAME:
		output = (vk::PolygonMode)VK_POLYGON_MODE_LINE;
		break;
	default:
		output = (vk::PolygonMode)VK_POLYGON_MODE_FILL;
		break;
	}

	return output;
}

vk::PrimitiveTopology ConvertPrimitiveType(D3DPRIMITIVETYPE input) noexcept
{
	vk::PrimitiveTopology output;

	switch (input)
	{
	case D3DPT_POINTLIST:
		output = (vk::PrimitiveTopology)VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		break;
	case D3DPT_LINELIST:
		output = (vk::PrimitiveTopology)VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		break;
	case D3DPT_LINESTRIP:
		output = (vk::PrimitiveTopology)VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		break;
	case D3DPT_TRIANGLELIST:
		output = (vk::PrimitiveTopology)VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		break;
	case D3DPT_TRIANGLESTRIP:
		output = (vk::PrimitiveTopology)VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		break;
	case D3DPT_TRIANGLEFAN:
		output = (vk::PrimitiveTopology)VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		break;
	default:
		output = (vk::PrimitiveTopology)VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		break;
	}

	/* Types D3d9 doesn't support
		VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY = 6,
		VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY = 7,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY = 8,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY = 9,
		VK_PRIMITIVE_TOPOLOGY_PATCH_LIST = 10,
	*/

	return output;
}

vk::BlendFactor ConvertColorFactor(D3DBLEND input) noexcept
{
	vk::BlendFactor output;

	switch (input)
	{
	case D3DBLEND_ZERO:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_ZERO;
		break;
	case D3DBLEND_ONE:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_ONE;
		break;
	case D3DBLEND_SRCCOLOR:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_SRC_COLOR;
		break;
	case D3DBLEND_INVSRCCOLOR:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		break;
	case D3DBLEND_SRCALPHA:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_SRC_ALPHA;
		break;
	case D3DBLEND_INVSRCALPHA:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		break;
	case D3DBLEND_DESTALPHA:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_DST_ALPHA;
		break;
	case D3DBLEND_INVDESTALPHA:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		break;
	case D3DBLEND_DESTCOLOR:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_DST_COLOR;
		break;
	case D3DBLEND_INVDESTCOLOR:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		break;
	case D3DBLEND_SRCALPHASAT:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
		break;
	case D3DBLEND_BOTHSRCALPHA:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_CONSTANT_ALPHA;
		break;
	case D3DBLEND_BOTHINVSRCALPHA:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
		break;
	case D3DBLEND_BLENDFACTOR:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_CONSTANT_COLOR;
		break;
	case D3DBLEND_INVBLENDFACTOR:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
		break;
		//Revisit
#if !defined(D3D_DISABLE_9EX)
	case D3DBLEND_SRCCOLOR2:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_SRC1_COLOR;
		break;
	case D3DBLEND_INVSRCCOLOR2:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
		break;
#endif // !D3D_DISABLE_9EX
	default:
		output = (vk::BlendFactor)VK_BLEND_FACTOR_ONE;
		break;
	}

	return output;
}

vk::BlendOp ConvertColorOperation(D3DBLENDOP input) noexcept
{
	vk::BlendOp output;

	switch (input)
	{
	case D3DBLENDOP_ADD:
		output = (vk::BlendOp)VK_BLEND_OP_ADD;
		break;
	case D3DBLENDOP_SUBTRACT:
		output = (vk::BlendOp)VK_BLEND_OP_SUBTRACT;
		break;
	case D3DBLENDOP_REVSUBTRACT:
		output = (vk::BlendOp)VK_BLEND_OP_REVERSE_SUBTRACT;
		break;
	case D3DBLENDOP_MIN:
		output = (vk::BlendOp)VK_BLEND_OP_MIN;
		break;
	case D3DBLENDOP_MAX:
		output = (vk::BlendOp)VK_BLEND_OP_MAX;
		break;
	case D3DBLENDOP_FORCE_DWORD:
		output = (vk::BlendOp)VK_BLEND_OP_MAX_ENUM;
		break;
	default:
		output = (vk::BlendOp)VK_BLEND_OP_ADD;
		break;
	}

	return output;
}

vk::Filter ConvertFilter(D3DTEXTUREFILTERTYPE input) noexcept
{
	vk::Filter output;

	switch (input)
	{
	case D3DTEXF_NONE:	// filtering disabled (valid for mip filter only)
		output = (vk::Filter)VK_FILTER_NEAREST; //revisit
		break;
	case D3DTEXF_POINT:	// nearest
		output = (vk::Filter)VK_FILTER_NEAREST;
		break;
	case D3DTEXF_LINEAR:	// linear interpolation
		output = (vk::Filter)VK_FILTER_LINEAR;
		break;
	case D3DTEXF_ANISOTROPIC:	// anisotropic
		/*
		https://www.khronos.org/registry/vulkan/specs/1.0-extensions/html/vkspec.html
		If either magFilter or minFilter is VK_FILTER_CUBIC_IMG, anisotropyEnable must be VK_FALSE
		*/
		output = (vk::Filter)VK_FILTER_LINEAR;
		//output = VK_FILTER_CUBIC_IMG; //revisit
		break;
	case D3DTEXF_PYRAMIDALQUAD:	// 4-sample tent
		output = (vk::Filter)VK_FILTER_CUBIC_IMG; //revisit
		break;
	case D3DTEXF_GAUSSIANQUAD:	// 4-sample Gaussian
		output = (vk::Filter)VK_FILTER_CUBIC_IMG; //revisit
		break;
	default:
		output = (vk::Filter)VK_FILTER_NEAREST; //revisit
		break;
	}

	return output;
}

vk::SamplerAddressMode ConvertTextureAddress(D3DTEXTUREADDRESS input) noexcept
{
	vk::SamplerAddressMode output;

	switch (input)
	{
	case D3DTADDRESS_WRAP:
		output = (vk::SamplerAddressMode)VK_SAMPLER_ADDRESS_MODE_REPEAT;
		break;
	case D3DTADDRESS_MIRROR:
		output = (vk::SamplerAddressMode)VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		break;
	case D3DTADDRESS_CLAMP:
		output = (vk::SamplerAddressMode)VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		break;
	case D3DTADDRESS_BORDER:
		output = (vk::SamplerAddressMode)VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		break;
	case D3DTADDRESS_MIRRORONCE:
		output = (vk::SamplerAddressMode)VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
		break;
	default:
		output = (vk::SamplerAddressMode)VK_SAMPLER_ADDRESS_MODE_REPEAT;
		break;
	}

	return output;
}

vk::SamplerMipmapMode ConvertMipmapMode(D3DTEXTUREFILTERTYPE input) noexcept
{
	vk::SamplerMipmapMode output;

	switch (input)
	{
	case D3DTEXF_NONE:	// filtering disabled (valid for mip filter only)
		output = (vk::SamplerMipmapMode)VK_SAMPLER_MIPMAP_MODE_NEAREST; //revisit
		break;
	case D3DTEXF_POINT:	// nearest
		output = (vk::SamplerMipmapMode)VK_SAMPLER_MIPMAP_MODE_NEAREST;
		break;
	case D3DTEXF_LINEAR:	// linear interpolation
		output = (vk::SamplerMipmapMode)VK_SAMPLER_MIPMAP_MODE_LINEAR;
		break;
	case D3DTEXF_ANISOTROPIC:	// anisotropic
		output = (vk::SamplerMipmapMode)VK_SAMPLER_MIPMAP_MODE_LINEAR; //revisit
		break;
	case D3DTEXF_PYRAMIDALQUAD:	// 4-sample tent
		output = (vk::SamplerMipmapMode)VK_SAMPLER_MIPMAP_MODE_LINEAR; //revisit
		break;
	case D3DTEXF_GAUSSIANQUAD:	// 4-sample Gaussian
		output = (vk::SamplerMipmapMode)VK_SAMPLER_MIPMAP_MODE_LINEAR; //revisit
		break;
	default:
		output = (vk::SamplerMipmapMode)VK_SAMPLER_MIPMAP_MODE_LINEAR; //revisit
		break;
	}

	return output;
}

//std::array<std::array<float, 4>, 4> ConvertRowMajorToColumnMajor(const D3DMATRIX& matrix)
//{
//	std::array<std::array<float, 4>, 4> newMatrix;
//
//	for (size_t i = 0; i < 4; i++)
//	{
//		for (size_t j = 0; j < 4; j++)
//		{
//			newMatrix[i][j] = matrix.m[j][i];
//		}
//	}
//
//	return newMatrix;
//}

CDevice9::CDevice9(C9* c9, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode)
	:
	mC9(c9),
	mAdapter(Adapter),
	mDeviceType(DeviceType),
	mFocusWindow(hFocusWindow),
	mBehaviorFlags(BehaviorFlags),
	mInternalDeviceState(this, D3DSBT_ALL)
{
	Log(info) << "CDevice9::CDevice9" << std::endl;

	mC9->AddRef();

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

	if (hFocusWindow && !mPresentationParameters.hDeviceWindow)
	{
		mPresentationParameters.hDeviceWindow = hFocusWindow;
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

	mInternalDeviceState.mDeviceState.mScissorRect.right = mPresentationParameters.BackBufferWidth;
	mInternalDeviceState.mDeviceState.mScissorRect.bottom = mPresentationParameters.BackBufferHeight;

	//Setup Vulkan objects
	ResetVulkanDevice();

	//Set Default State

	/*
	The point here is to set the correct defaults and mark everything as dirty.
	The default value still need to be pushed into the buffers, pipelines, descriptors, etc.
	This block allows that to be handled by the normal logic that handles state changes.
	*/
	mInternalDeviceState.SetPixelShader(nullptr);
	mInternalDeviceState.SetVertexShader(nullptr);

	{
		D3DLIGHT9 light = {};
		for (size_t i = 0; i < 8; i++)
		{
			mInternalDeviceState.SetLight(i, &light);
		}
	}

	for (int i = 0; i < MAX_VERTEX_INPUTS; i++)
	{
		mInternalDeviceState.SetStreamSource(i, nullptr, 0, 0);
		mInternalDeviceState.SetStreamSourceFreq(i, 1);
	}

	mInternalDeviceState.SetIndices(nullptr);
	mInternalDeviceState.SetVertexDeclaration(nullptr);

	{
		D3DMATERIAL9 material;

		material.Diffuse.r = 1.0f;
		material.Diffuse.g = 1.0f;
		material.Diffuse.b = 1.0f;
		material.Diffuse.a = 0.0f;
		material.Ambient.r = 0.0f;
		material.Ambient.g = 0.0f;
		material.Ambient.b = 0.0f;
		material.Ambient.a = 0.0f;
		material.Emissive.r = 0.0f;
		material.Emissive.g = 0.0f;
		material.Emissive.b = 0.0f;
		material.Emissive.a = 0.0f;
		material.Specular.r = 0.0f;
		material.Specular.g = 0.0f;
		material.Specular.b = 0.0f;
		material.Specular.a = 0.0f;
		material.Power = 0.0f;

		mInternalDeviceState.SetMaterial(&material);
	}

	{
		D3DMATRIX identity = { 1, 0, 0, 0,
							   0, 1, 0, 0,
							   0, 0, 1, 0,
							   0, 0, 0, 1 };

		SetTransform(D3DTS_VIEW, &identity);
		SetTransform(D3DTS_PROJECTION, &identity);
		SetTransform(D3DTS_TEXTURE0, &identity);
		SetTransform(D3DTS_TEXTURE1, &identity);
		SetTransform(D3DTS_TEXTURE2, &identity);
		SetTransform(D3DTS_TEXTURE3, &identity);
		SetTransform(D3DTS_TEXTURE4, &identity);
		SetTransform(D3DTS_TEXTURE5, &identity);
		SetTransform(D3DTS_TEXTURE6, &identity);
		SetTransform(D3DTS_TEXTURE7, &identity);

		for (int i = 0; i < 12; i++)
		{
			SetTransform(D3DTS_WORLDMATRIX(i), &identity);
		}
	}

	for (int i = 0; i < MAX_PIXEL_SHADER_CONST; i++)
	{
		const float zero[4] = { 0, 0, 0, 0 };

		SetPixelShaderConstantF(i, zero, 1);
	}

	for (int i = 0; i < MAX_VERTEX_SHADER_CONST; i++)
	{
		const float zero[4] = { 0, 0, 0, 0 };

		SetVertexShaderConstantF(i, zero, 1);
	}

	for (int i = 0; i < 16; i++)
	{
		const int zero[4] = { 0, 0, 0, 0 };

		SetPixelShaderConstantI(i, zero, 1);
		SetVertexShaderConstantI(i, zero, 1);
		SetPixelShaderConstantB(i, &zero[0], 1);
		SetVertexShaderConstantB(i, &zero[0], 1);
	}

	SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	SetRenderState(D3DRS_LASTPIXEL, TRUE);
	SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	SetRenderState(D3DRS_ALPHAREF, 0);
	SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	SetRenderState(D3DRS_DITHERENABLE, FALSE);
	SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	SetRenderState(D3DRS_FOGENABLE, FALSE);
	SetRenderState(D3DRS_SPECULARENABLE, FALSE);
	SetRenderState(D3DRS_FOGCOLOR, 0);
	SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
	SetRenderState(D3DRS_FOGSTART, bit_cast(0.0f));
	SetRenderState(D3DRS_FOGEND, bit_cast(1.0f));
	SetRenderState(D3DRS_FOGDENSITY, bit_cast(1.0f));
	SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
	SetRenderState(D3DRS_STENCILENABLE, FALSE);
	SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	SetRenderState(D3DRS_STENCILREF, 0);
	SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
	SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
	SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
	SetRenderState(D3DRS_WRAP0, 0);
	SetRenderState(D3DRS_WRAP1, 0);
	SetRenderState(D3DRS_WRAP2, 0);
	SetRenderState(D3DRS_WRAP3, 0);
	SetRenderState(D3DRS_WRAP4, 0);
	SetRenderState(D3DRS_WRAP5, 0);
	SetRenderState(D3DRS_WRAP6, 0);
	SetRenderState(D3DRS_WRAP7, 0);
	SetRenderState(D3DRS_CLIPPING, TRUE);
	SetRenderState(D3DRS_LIGHTING, TRUE);
	SetRenderState(D3DRS_AMBIENT, 0);
	SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
	SetRenderState(D3DRS_COLORVERTEX, TRUE);
	SetRenderState(D3DRS_LOCALVIEWER, TRUE);
	SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
	SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);
	SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
	SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
	SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
	SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
	SetRenderState(D3DRS_POINTSIZE, bit_cast(64.0f));
	SetRenderState(D3DRS_POINTSIZE_MIN, bit_cast(1.0f));
	SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
	SetRenderState(D3DRS_POINTSCALE_A, bit_cast(1.0f));
	SetRenderState(D3DRS_POINTSCALE_B, bit_cast(0.0f));
	SetRenderState(D3DRS_POINTSCALE_C, bit_cast(0.0f));
	SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	SetRenderState(D3DRS_MULTISAMPLEMASK, 0xFFFFFFFF);
	SetRenderState(D3DRS_PATCHEDGESTYLE, D3DPATCHEDGE_DISCRETE);
	SetRenderState(D3DRS_DEBUGMONITORTOKEN, D3DDMT_ENABLE);
	SetRenderState(D3DRS_POINTSIZE_MAX, bit_cast(64.0f));
	SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
	SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);
	SetRenderState(D3DRS_TWEENFACTOR, bit_cast(0.0f));
	SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	SetRenderState(D3DRS_POSITIONDEGREE, D3DDEGREE_CUBIC);
	SetRenderState(D3DRS_NORMALDEGREE, D3DDEGREE_LINEAR);
	SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, bit_cast(0.0f));
	SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
	SetRenderState(D3DRS_MINTESSELLATIONLEVEL, bit_cast(1.0f));
	SetRenderState(D3DRS_MAXTESSELLATIONLEVEL, bit_cast(1.0f));
	SetRenderState(D3DRS_ADAPTIVETESS_X, bit_cast(0.0f));
	SetRenderState(D3DRS_ADAPTIVETESS_Y, bit_cast(0.0f));
	SetRenderState(D3DRS_ADAPTIVETESS_Z, bit_cast(1.0f));
	SetRenderState(D3DRS_ADAPTIVETESS_W, bit_cast(0.0f));
	SetRenderState(D3DRS_ENABLEADAPTIVETESSELLATION, FALSE);
	SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE);
	SetRenderState(D3DRS_CCW_STENCILFAIL, D3DSTENCILOP_KEEP);
	SetRenderState(D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP);
	SetRenderState(D3DRS_CCW_STENCILPASS, D3DSTENCILOP_KEEP);
	SetRenderState(D3DRS_CCW_STENCILFUNC, D3DCMP_ALWAYS);
	SetRenderState(D3DRS_COLORWRITEENABLE1, 0x0000000f);
	SetRenderState(D3DRS_COLORWRITEENABLE2, 0x0000000f);
	SetRenderState(D3DRS_COLORWRITEENABLE3, 0x0000000f);
	SetRenderState(D3DRS_BLENDFACTOR, 0xffffffff);
	SetRenderState(D3DRS_SRGBWRITEENABLE, 0);
	SetRenderState(D3DRS_DEPTHBIAS, bit_cast(0.0f));
	SetRenderState(D3DRS_WRAP8, 0);
	SetRenderState(D3DRS_WRAP9, 0);
	SetRenderState(D3DRS_WRAP10, 0);
	SetRenderState(D3DRS_WRAP11, 0);
	SetRenderState(D3DRS_WRAP12, 0);
	SetRenderState(D3DRS_WRAP13, 0);
	SetRenderState(D3DRS_WRAP14, 0);
	SetRenderState(D3DRS_WRAP15, 0);
	SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
	SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
	SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO);
	SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);

	SetRenderState((D3DRENDERSTATETYPE)(D3DRS_BLENDOPALPHA + 1), mPresentationParameters.BackBufferWidth); //screenWidth
	SetRenderState((D3DRENDERSTATETYPE)(D3DRS_BLENDOPALPHA + 2), mPresentationParameters.BackBufferHeight); //screenHeight
	SetRenderState((D3DRENDERSTATETYPE)(D3DRS_BLENDOPALPHA + 3), 1); //textureCount

	//Set texture and sampler state.

	SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	SetTextureStageState(0, D3DTSS_BUMPENVMAT00, 0); //bit_cast(0.0f)
	SetTextureStageState(0, D3DTSS_BUMPENVMAT01, 0); //bit_cast(0.0f)
	SetTextureStageState(0, D3DTSS_BUMPENVMAT10, 0); //bit_cast(0.0f)
	SetTextureStageState(0, D3DTSS_BUMPENVMAT11, 0); //bit_cast(0.0f)
	SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	SetTextureStageState(0, D3DTSS_BUMPENVLSCALE, 0); //bit_cast(0.0f)
	SetTextureStageState(0, D3DTSS_BUMPENVLOFFSET, 0); //bit_cast(0.0f)
	SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	SetTextureStageState(0, D3DTSS_COLORARG0, D3DTA_CURRENT);
	SetTextureStageState(0, D3DTSS_ALPHAARG0, D3DTA_CURRENT);
	SetTextureStageState(0, D3DTSS_RESULTARG, D3DTA_CURRENT);
	SetTextureStageState(0, D3DTSS_CONSTANT, 0);

	SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
	SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0x00000000);
	SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, 0);
	SetSamplerState(0, D3DSAMP_MAXMIPLEVEL, 0);
	SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 1);
	SetSamplerState(0, D3DSAMP_SRGBTEXTURE, 0);
	SetSamplerState(0, D3DSAMP_ELEMENTINDEX, 0);
	SetSamplerState(0, D3DSAMP_DMAPOFFSET, 0);

	for (size_t i = 1; i < 16; i++)
	{
		SetTextureStageState(i, D3DTSS_COLOROP, D3DTOP_DISABLE);
		SetTextureStageState(i, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		SetTextureStageState(i, D3DTSS_COLORARG2, D3DTA_CURRENT);
		SetTextureStageState(i, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		SetTextureStageState(i, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		SetTextureStageState(i, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
		SetTextureStageState(i, D3DTSS_BUMPENVMAT00, 0); //bit_cast(0.0f)
		SetTextureStageState(i, D3DTSS_BUMPENVMAT01, 0); //bit_cast(0.0f)
		SetTextureStageState(i, D3DTSS_BUMPENVMAT10, 0); //bit_cast(0.0f)
		SetTextureStageState(i, D3DTSS_BUMPENVMAT11, 0); //bit_cast(0.0f)
		SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, i);
		SetTextureStageState(i, D3DTSS_BUMPENVLSCALE, 0); //bit_cast(0.0f)
		SetTextureStageState(i, D3DTSS_BUMPENVLOFFSET, 0); //bit_cast(0.0f)
		SetTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		SetTextureStageState(i, D3DTSS_COLORARG0, D3DTA_CURRENT);
		SetTextureStageState(i, D3DTSS_ALPHAARG0, D3DTA_CURRENT);
		SetTextureStageState(i, D3DTSS_RESULTARG, D3DTA_CURRENT);
		SetTextureStageState(i, D3DTSS_CONSTANT, 0);

		SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		SetSamplerState(i, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		SetSamplerState(i, D3DSAMP_BORDERCOLOR, 0x00000000);
		SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, 0);
		SetSamplerState(i, D3DSAMP_MAXMIPLEVEL, 0);
		SetSamplerState(i, D3DSAMP_MAXANISOTROPY, 1);
		SetSamplerState(i, D3DSAMP_SRGBTEXTURE, 0);
		SetSamplerState(i, D3DSAMP_ELEMENTINDEX, 0);
		SetSamplerState(i, D3DSAMP_DMAPOFFSET, 0);
	}

}

CDevice9::~CDevice9()
{
	mDevice->waitIdle();

	for (auto& swapChain : mSwapChains)
	{
		swapChain->Release();
	}
	mSwapChains.clear();

	//if (mDepthStencilSurface)
	//{
	//	mDepthStencilSurface->Release();
	//	mDepthStencilSurface = nullptr;
	//}

	delete mDepthStencilSurface;

	mC9->Release();
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

void CDevice9::ResetVulkanDevice()
{
	//Create a device and command pool (unique device will auto destroy)
	{
		float queuePriority = 0.0f;
		const vk::DeviceQueueCreateInfo deviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), static_cast<uint32_t>(mC9->mGraphicsQueueFamilyIndex), 1, &queuePriority);
		std::vector<char const*> deviceExtensionNames;
		deviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		deviceExtensionNames.push_back("VK_KHR_maintenance1");
		mDevice = mC9->mPhysicalDevices[mC9->mPhysicalDeviceIndex].createDeviceUnique(vk::DeviceCreateInfo({}, 1, &deviceQueueCreateInfo, 0, nullptr, static_cast<uint32_t>(deviceExtensionNames.size()), deviceExtensionNames.data()));

		mCommandPool = mDevice->createCommandPoolUnique(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, deviceQueueCreateInfo.queueFamilyIndex));
	}

	{
		mDevice->getQueue(static_cast<uint32_t>(mC9->mGraphicsQueueFamilyIndex), 0, &mQueue);
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

		mUpVertexBuffer = mDevice->createBufferUnique(upVertexBufferInfo);

		vk::MemoryRequirements mem_reqs;
		mDevice->getBufferMemoryRequirements(mUpVertexBuffer.get(), &mem_reqs);

		auto mem_alloc = vk::MemoryAllocateInfo().setAllocationSize(mem_reqs.size).setMemoryTypeIndex(0);
		FindMemoryTypeFromProperties(mem_reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &mem_alloc.memoryTypeIndex);

		mUpVertexBufferMemory = mDevice->allocateMemoryUnique(mem_alloc);

		mDevice->bindBufferMemory(mUpVertexBuffer.get(), mUpVertexBufferMemory.get(), 0);
	}

	{
		auto const upIndexBufferInfo = vk::BufferCreateInfo().setSize(MAX_BUFFERUPDATE).setUsage(vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst);
		mUpIndexBuffer = mDevice->createBufferUnique(upIndexBufferInfo);

		vk::MemoryRequirements mem_reqs;
		mDevice->getBufferMemoryRequirements(mUpIndexBuffer.get(), &mem_reqs);

		auto mem_alloc = vk::MemoryAllocateInfo().setAllocationSize(mem_reqs.size).setMemoryTypeIndex(0);
		FindMemoryTypeFromProperties(mem_reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &mem_alloc.memoryTypeIndex);

		mUpIndexBufferMemory = mDevice->allocateMemoryUnique(mem_alloc);

		mDevice->bindBufferMemory(mUpIndexBuffer.get(), mUpIndexBufferMemory.get(), 0);
	}

	//Setup FF Buffers

	//RenderState
	{
		auto const renderStateBufferInfo = vk::BufferCreateInfo().setSize(sizeof(mInternalDeviceState.mDeviceState.mRenderState)).setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);
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
		auto const textureStageBufferInfo = vk::BufferCreateInfo().setSize(sizeof(mInternalDeviceState.mDeviceState.mTextureStageState)).setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);
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
		auto const lightBufferInfo = vk::BufferCreateInfo().setSize(sizeof(PaddedLight) * 8).setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);
		mLightBuffer = mDevice->createBufferUnique(lightBufferInfo);
		vk::MemoryRequirements lightMemoryRequirements;
		mDevice->getBufferMemoryRequirements(mLightBuffer.get(), &lightMemoryRequirements);
		auto lightMemoryAllocateInfo = vk::MemoryAllocateInfo().setAllocationSize(lightMemoryRequirements.size).setMemoryTypeIndex(0);
		FindMemoryTypeFromProperties(lightMemoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &lightMemoryAllocateInfo.memoryTypeIndex);
		mLightBufferMemory = mDevice->allocateMemoryUnique(lightMemoryAllocateInfo);
		mDevice->bindBufferMemory(mLightBuffer.get(), mLightBufferMemory.get(), 0);
	}

	//Light Enable
	{
		auto const lightEnableBufferInfo = vk::BufferCreateInfo().setSize(sizeof(mInternalDeviceState.mDeviceState.mLightEnableState)).setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);
		mLightEnableBuffer = mDevice->createBufferUnique(lightEnableBufferInfo);
		vk::MemoryRequirements lightEnableMemoryRequirements;
		mDevice->getBufferMemoryRequirements(mLightEnableBuffer.get(), &lightEnableMemoryRequirements);
		auto lightEnableMemoryAllocateInfo = vk::MemoryAllocateInfo().setAllocationSize(lightEnableMemoryRequirements.size).setMemoryTypeIndex(0);
		FindMemoryTypeFromProperties(lightEnableMemoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &lightEnableMemoryAllocateInfo.memoryTypeIndex);
		mLightEnableBufferMemory = mDevice->allocateMemoryUnique(lightEnableMemoryAllocateInfo);
		mDevice->bindBufferMemory(mLightEnableBuffer.get(), mLightEnableBufferMemory.get(), 0);
	}

	//Material
	{
		auto const materialBufferInfo = vk::BufferCreateInfo().setSize(sizeof(mInternalDeviceState.mDeviceState.mMaterial)).setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);
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
		auto const transformationBufferInfo = vk::BufferCreateInfo().setSize(sizeof(mInternalDeviceState.mDeviceState.mTransform)).setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);
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
		auto const vertexConstantBufferInfo = vk::BufferCreateInfo().setSize(sizeof(mInternalDeviceState.mDeviceState.mVertexShaderConstantI) + sizeof(mInternalDeviceState.mDeviceState.mVertexShaderConstantB) + sizeof(mInternalDeviceState.mDeviceState.mVertexShaderConstantF)).setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);
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
		auto const pixelConstantBufferInfo = vk::BufferCreateInfo().setSize(sizeof(mInternalDeviceState.mDeviceState.mPixelShaderConstantI) + sizeof(mInternalDeviceState.mDeviceState.mPixelShaderConstantB) + sizeof(mInternalDeviceState.mDeviceState.mPixelShaderConstantF)).setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);
		mPixelConstantBuffer = mDevice->createBufferUnique(pixelConstantBufferInfo);
		vk::MemoryRequirements pixelConstantMemoryRequirements;
		mDevice->getBufferMemoryRequirements(mPixelConstantBuffer.get(), &pixelConstantMemoryRequirements);
		auto pixelConstantMemoryAllocateInfo = vk::MemoryAllocateInfo().setAllocationSize(pixelConstantMemoryRequirements.size).setMemoryTypeIndex(0);
		FindMemoryTypeFromProperties(pixelConstantMemoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &pixelConstantMemoryAllocateInfo.memoryTypeIndex);
		mPixelConstantBufferMemory = mDevice->allocateMemoryUnique(pixelConstantMemoryAllocateInfo);
		mDevice->bindBufferMemory(mPixelConstantBuffer.get(), mPixelConstantBufferMemory.get(), 0);
	}

	//Handle B and I with constants maybe

	//Create Descriptor layout.
	const uint32_t textureCount = 16;
	{
		const vk::DescriptorSetLayoutBinding layoutBindings[9] =
		{
			vk::DescriptorSetLayoutBinding() /*Render State*/
				.setBinding(0)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount(1)
				.setStageFlags(vk::ShaderStageFlagBits::eAllGraphics)
				.setPImmutableSamplers(nullptr),
			vk::DescriptorSetLayoutBinding() /*Lights*/
				.setBinding(1)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount(1)
				.setStageFlags(vk::ShaderStageFlagBits::eVertex)
				.setPImmutableSamplers(nullptr),
			vk::DescriptorSetLayoutBinding() /*Light Enable*/
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
			vk::DescriptorSetLayoutBinding() /*Matrix/Transformation*/
				.setBinding(4)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount(1)
				.setStageFlags(vk::ShaderStageFlagBits::eVertex)
				.setPImmutableSamplers(nullptr),
			vk::DescriptorSetLayoutBinding() /*Texture Stages*/
				.setBinding(5)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount(1)
				.setStageFlags(vk::ShaderStageFlagBits::eFragment)
				.setPImmutableSamplers(nullptr),
			vk::DescriptorSetLayoutBinding() /*Image/Sampler*/
				.setBinding(6)
				.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
				.setDescriptorCount(textureCount)
				.setStageFlags(vk::ShaderStageFlagBits::eFragment)
				.setPImmutableSamplers(nullptr),
			vk::DescriptorSetLayoutBinding() /*Vertex Shader Const*/
				.setBinding(7)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount(1)
				.setStageFlags(vk::ShaderStageFlagBits::eVertex)
				.setPImmutableSamplers(nullptr),
			vk::DescriptorSetLayoutBinding() /*Pixel Shader Const*/
				.setBinding(8)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setDescriptorCount(1)
				.setStageFlags(vk::ShaderStageFlagBits::eFragment)
				.setPImmutableSamplers(nullptr),
		};
		auto const descriptorLayout = vk::DescriptorSetLayoutCreateInfo().setBindingCount(9).setPBindings(layoutBindings);
		mDescriptorLayout = mDevice->createDescriptorSetLayoutUnique(descriptorLayout);
	}

	//Setup descriptor write structures
	{
		//Render State
		mDescriptorBufferInfo[0].buffer = mRenderStateBuffer.get();
		mDescriptorBufferInfo[0].offset = 0;
		mDescriptorBufferInfo[0].range = sizeof(mInternalDeviceState.mDeviceState.mRenderState);

		mWriteDescriptorSet[0].dstBinding = 0;
		mWriteDescriptorSet[0].dstArrayElement = 0;
		mWriteDescriptorSet[0].descriptorType = vk::DescriptorType::eUniformBuffer;
		mWriteDescriptorSet[0].descriptorCount = 1;
		mWriteDescriptorSet[0].pBufferInfo = &mDescriptorBufferInfo[0];

		//Lights
		mDescriptorBufferInfo[1].buffer = mLightBuffer.get();
		mDescriptorBufferInfo[1].offset = 0;
		mDescriptorBufferInfo[1].range = sizeof(PaddedLight) * 8;

		mWriteDescriptorSet[1].dstBinding = 1;
		mWriteDescriptorSet[1].dstArrayElement = 0;
		mWriteDescriptorSet[1].descriptorType = vk::DescriptorType::eUniformBuffer;
		mWriteDescriptorSet[1].descriptorCount = 1;
		mWriteDescriptorSet[1].pBufferInfo = &mDescriptorBufferInfo[1];

		//Light Enable
		mDescriptorBufferInfo[2].buffer = mLightEnableBuffer.get();
		mDescriptorBufferInfo[2].offset = 0;
		mDescriptorBufferInfo[2].range = sizeof(mInternalDeviceState.mDeviceState.mLightEnableState);

		mWriteDescriptorSet[2].dstBinding = 2;
		mWriteDescriptorSet[2].dstArrayElement = 0;
		mWriteDescriptorSet[2].descriptorType = vk::DescriptorType::eUniformBuffer;
		mWriteDescriptorSet[2].descriptorCount = 1;
		mWriteDescriptorSet[2].pBufferInfo = &mDescriptorBufferInfo[2];

		//Material
		mDescriptorBufferInfo[3].buffer = mMaterialBuffer.get();
		mDescriptorBufferInfo[3].offset = 0;
		mDescriptorBufferInfo[3].range = sizeof(mInternalDeviceState.mDeviceState.mMaterial);

		mWriteDescriptorSet[3].dstBinding = 3;
		mWriteDescriptorSet[3].dstArrayElement = 0;
		mWriteDescriptorSet[3].descriptorType = vk::DescriptorType::eUniformBuffer;
		mWriteDescriptorSet[3].descriptorCount = 1;
		mWriteDescriptorSet[3].pBufferInfo = &mDescriptorBufferInfo[3];

		//Transformation
		mDescriptorBufferInfo[4].buffer = mTransformationBuffer.get();
		mDescriptorBufferInfo[4].offset = 0;
		mDescriptorBufferInfo[4].range = sizeof(mInternalDeviceState.mDeviceState.mTransform);

		mWriteDescriptorSet[4].dstBinding = 4;
		mWriteDescriptorSet[4].dstArrayElement = 0;
		mWriteDescriptorSet[4].descriptorType = vk::DescriptorType::eUniformBuffer;
		mWriteDescriptorSet[4].descriptorCount = 1;
		mWriteDescriptorSet[4].pBufferInfo = &mDescriptorBufferInfo[4];

		//Texture Stages
		mDescriptorBufferInfo[5].buffer = mTextureStageBuffer.get();
		mDescriptorBufferInfo[5].offset = 0;
		mDescriptorBufferInfo[5].range = sizeof(mInternalDeviceState.mDeviceState.mTextureStageState);

		mWriteDescriptorSet[5].dstBinding = 5;
		mWriteDescriptorSet[5].dstArrayElement = 0;
		mWriteDescriptorSet[5].descriptorType = vk::DescriptorType::eUniformBuffer;
		mWriteDescriptorSet[5].descriptorCount = 1;
		mWriteDescriptorSet[5].pBufferInfo = &mDescriptorBufferInfo[5];

		//Image/Sampler
		//for (size_t i = 0; i < textureCount; i++)
		//{
		//	//mDescriptorImageInfo[i].sampler = mSampler;
		//	//mDescriptorImageInfo[i].imageView = mImageView;
		//	mDescriptorImageInfo[i].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		//}

		mWriteDescriptorSet[6].dstBinding = 6;
		mWriteDescriptorSet[6].dstArrayElement = 0;
		mWriteDescriptorSet[6].descriptorType = vk::DescriptorType::eCombinedImageSampler;
		mWriteDescriptorSet[6].descriptorCount = textureCount;
		mWriteDescriptorSet[6].pImageInfo = mDescriptorImageInfo;

		//Vertex Shader Const
		mDescriptorBufferInfo[7].buffer = mVertexConstantBuffer.get();
		mDescriptorBufferInfo[7].offset = 0;
		mDescriptorBufferInfo[7].range = sizeof(mInternalDeviceState.mDeviceState.mVertexShaderConstantI) + sizeof(mInternalDeviceState.mDeviceState.mVertexShaderConstantB) + sizeof(mInternalDeviceState.mDeviceState.mVertexShaderConstantF);

		mWriteDescriptorSet[7].dstBinding = 7;
		mWriteDescriptorSet[7].dstArrayElement = 0;
		mWriteDescriptorSet[7].descriptorType = vk::DescriptorType::eUniformBuffer;
		mWriteDescriptorSet[7].descriptorCount = 1;
		mWriteDescriptorSet[7].pBufferInfo = &mDescriptorBufferInfo[7];

		//Pixel Shader Const
		mDescriptorBufferInfo[8].buffer = mPixelConstantBuffer.get();
		mDescriptorBufferInfo[8].offset = 0;
		mDescriptorBufferInfo[8].range = sizeof(mInternalDeviceState.mDeviceState.mPixelShaderConstantI) + sizeof(mInternalDeviceState.mDeviceState.mPixelShaderConstantB) + sizeof(mInternalDeviceState.mDeviceState.mPixelShaderConstantF);

		mWriteDescriptorSet[8].dstBinding = 8;
		mWriteDescriptorSet[8].dstArrayElement = 0;
		mWriteDescriptorSet[8].descriptorType = vk::DescriptorType::eUniformBuffer;
		mWriteDescriptorSet[8].descriptorCount = 1;
		mWriteDescriptorSet[8].pBufferInfo = &mDescriptorBufferInfo[8];
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

		//TODO: look into optimizations using push constants and specialization constants.

		auto const pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo()
			.setSetLayoutCount(1)
			.setPPushConstantRanges(ranges.data())
			.setPushConstantRangeCount(1)
			.setPSetLayouts(&mDescriptorLayout.get());
		mPipelineLayout = mDevice->createPipelineLayoutUnique(pipelineLayoutCreateInfo);
	}

	//Setup Pipeline Cache
	{
		vk::PipelineCacheCreateInfo const pipelineCacheInfo;
		mPipelineCache = mDevice->createPipelineCacheUnique(pipelineCacheInfo);
	}

	//Load fixed function shaders.
	mVertShaderModule_XYZRHW = LoadShaderFromConst(XYZRHW_VERT);
	mVertShaderModule_XYZ = LoadShaderFromConst(XYZ_VERT);
	mFragShaderModule_XYZ = LoadShaderFromConst(XYZ_FRAG);

	mVertShaderModule_XYZRHW_DIFFUSE = LoadShaderFromConst(XYZRHW_DIFFUSE_VERT);
	mVertShaderModule_XYZ_DIFFUSE = LoadShaderFromConst(XYZ_DIFFUSE_VERT);
	mFragShaderModule_XYZ_DIFFUSE = LoadShaderFromConst(XYZ_DIFFUSE_FRAG);
	mGeomShaderModule_XYZ_DIFFUSE = LoadShaderFromConst(XYZ_DIFFUSE_GEOM);

	mVertShaderModule_XYZRHW_TEX1 = LoadShaderFromConst(XYZRHW_TEX1_VERT);
	mVertShaderModule_XYZ_TEX1 = LoadShaderFromConst(XYZ_TEX1_VERT);
	mFragShaderModule_XYZ_TEX1 = LoadShaderFromConst(XYZ_TEX1_FRAG);

	mVertShaderModule_XYZRHW_TEX2 = LoadShaderFromConst(XYZRHW_TEX2_VERT);
	mVertShaderModule_XYZ_TEX2 = LoadShaderFromConst(XYZ_TEX2_VERT);
	mFragShaderModule_XYZ_TEX2 = LoadShaderFromConst(XYZ_TEX2_FRAG);

	mVertShaderModule_XYZRHW_DIFFUSE_TEX1 = LoadShaderFromConst(XYZRHW_DIFFUSE_TEX1_VERT);
	mVertShaderModule_XYZ_DIFFUSE_TEX1 = LoadShaderFromConst(XYZ_DIFFUSE_TEX1_VERT);
	mFragShaderModule_XYZ_DIFFUSE_TEX1 = LoadShaderFromConst(XYZ_DIFFUSE_TEX1_FRAG);

	mVertShaderModule_XYZRHW_DIFFUSE_TEX2 = LoadShaderFromConst(XYZRHW_DIFFUSE_TEX2_VERT);
	mVertShaderModule_XYZ_DIFFUSE_TEX2 = LoadShaderFromConst(XYZ_DIFFUSE_TEX2_VERT);
	mFragShaderModule_XYZ_DIFFUSE_TEX2 = LoadShaderFromConst(XYZ_DIFFUSE_TEX2_FRAG);

	mVertShaderModule_XYZ_NORMAL = LoadShaderFromConst(XYZ_NORMAL_VERT);
	mFragShaderModule_XYZ_NORMAL = LoadShaderFromConst(XYZ_NORMAL_FRAG);

	mVertShaderModule_XYZ_NORMAL_TEX1 = LoadShaderFromConst(XYZ_NORMAL_TEX1_VERT);
	mFragShaderModule_XYZ_NORMAL_TEX1 = LoadShaderFromConst(XYZ_NORMAL_TEX1_FRAG);

	mVertShaderModule_XYZ_NORMAL_TEX2 = LoadShaderFromConst(XYZ_NORMAL_TEX2_VERT);
	mFragShaderModule_XYZ_NORMAL_TEX2 = LoadShaderFromConst(XYZ_NORMAL_TEX2_FRAG);

	mVertShaderModule_XYZ_NORMAL_DIFFUSE = LoadShaderFromConst(XYZ_NORMAL_DIFFUSE_VERT);
	mFragShaderModule_XYZ_NORMAL_DIFFUSE = LoadShaderFromConst(XYZ_NORMAL_DIFFUSE_FRAG);

	mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX1 = LoadShaderFromConst(XYZ_NORMAL_DIFFUSE_TEX1_VERT);
	mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX1 = LoadShaderFromConst(XYZ_NORMAL_DIFFUSE_TEX1_FRAG);

	mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = LoadShaderFromConst(XYZ_NORMAL_DIFFUSE_TEX2_VERT);
	mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX2 = LoadShaderFromConst(XYZ_NORMAL_DIFFUSE_TEX2_FRAG);

	mFragShaderModule_Passthrough = LoadShaderFromConst(PIXEL_PASSTHROUGH_FRAG);

	//Create Command Buffers, Fences, and semaphores
	vk::SemaphoreCreateInfo semaphoreCreateInfo;
	vk::FenceCreateInfo fenceCreateInfo(vk::FenceCreateFlagBits::eSignaled);

	mDrawCommandBuffers = mDevice->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo(mCommandPool.get(), vk::CommandBufferLevel::ePrimary, 3));
	for (size_t i = 0; i < mDrawCommandBuffers.size(); i++)
	{
		mDrawFences.push_back(mDevice->createFenceUnique(fenceCreateInfo));
		mImageAvailableSemaphores.push_back(mDevice->createSemaphoreUnique(semaphoreCreateInfo));
		mRenderFinishedSemaphores.push_back(mDevice->createSemaphoreUnique(semaphoreCreateInfo));
	}

	mUtilityCommandBuffers = mDevice->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo(mCommandPool.get(), vk::CommandBufferLevel::ePrimary, 7));
	for (size_t i = 0; i < mUtilityCommandBuffers.size(); i++)
	{
		mUtilityFences.push_back(mDevice->createFenceUnique(fenceCreateInfo));
	}

	/*
		mSwapChains[0]->mBackBuffer->ResetViewAndStagingBuffer();
		mSwapChains[0]->mFrontBuffer->ResetViewAndStagingBuffer();
	*/

	//Add implicit swap chain.
	mSwapChains.clear();
	CSwapChain9* ptr = nullptr;
	CreateAdditionalSwapChain(&mPresentationParameters, (IDirect3DSwapChain9**)&ptr);
	mSwapChains.push_back(ptr);

	//Add implicit render target
	SetRenderTarget(0, mSwapChains[0]->mBackBuffer);

	//Add implicit stencil buffer surface.
	CSurface9* depth = new CSurface9(this, (CTexture9*)nullptr, mPresentationParameters.BackBufferWidth, mPresentationParameters.BackBufferHeight, D3DUSAGE_DEPTHSTENCIL, 1, mPresentationParameters.AutoDepthStencilFormat, mPresentationParameters.MultiSampleType, mPresentationParameters.MultiSampleQuality, false, false, D3DPOOL_DEFAULT, nullptr);
	SetDepthStencilSurface(depth);
	depth->Release();

	mBlankTexture = std::make_unique<CTexture9>(this, 1, 1, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, nullptr);

	vk::ClearColorValue clearColorValue;
	for (size_t i = 0; i < 4; i++)
	{
		clearColorValue.float32[i] = 1.0f;
	}
	mBlankTexture->Clear(clearColorValue);
}

void CDevice9::BeginRecordingCommands()
{
	if (mIsRecording)
	{
		return;
	}

	mFrameIndex = (mFrameIndex++) % mDrawCommandBuffers.size();
	mDescriptorSetIndex = 0;

	mDevice->waitForFences(1, &mDrawFences[mFrameIndex].get(), VK_TRUE, UINT64_MAX);
	mDevice->resetFences(1, &mDrawFences[mFrameIndex].get());

	mPipelines[mFrameIndex].clear(); //I need to profile this to see what the cost of deleting all of these each frame is.
	mLastPrimitiveType = D3DPT_FORCE_DWORD; //Force pipeline rebuild on first draw.

	mCurrentDrawCommandBuffer = mDrawCommandBuffers[mFrameIndex].get();

	vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	mCurrentDrawCommandBuffer.begin(&beginInfo);

	//Set Scissor because we don't know if the user will set a new one this frame.
	const vk::Rect2D scissor(vk::Offset2D(mInternalDeviceState.mDeviceState.mScissorRect.left, mInternalDeviceState.mDeviceState.mScissorRect.top), vk::Extent2D(mInternalDeviceState.mDeviceState.mScissorRect.right, mInternalDeviceState.mDeviceState.mScissorRect.bottom));
	mCurrentDrawCommandBuffer.setScissor(0, 1, &scissor);

	//Set the view because we don't know if the user will set a new one this frame.
	const auto viewport = vk::Viewport()
		.setX(static_cast<float>(mInternalDeviceState.mDeviceState.mViewport.X) - 0.5f)
		.setY((static_cast<float>(mInternalDeviceState.mDeviceState.mViewport.Height) - 0.5f) - (static_cast<float>(mInternalDeviceState.mDeviceState.mViewport.Y)))
		.setWidth(static_cast<float>(mInternalDeviceState.mDeviceState.mViewport.Width) - 0.5f)
		.setHeight(-(static_cast<float>(mInternalDeviceState.mDeviceState.mViewport.Height) - 0.5f))
		.setMinDepth(static_cast<float>(mInternalDeviceState.mDeviceState.mViewport.MinZ))
		.setMaxDepth(static_cast<float>(mInternalDeviceState.mDeviceState.mViewport.MaxZ));
	mCurrentDrawCommandBuffer.setViewport(0, 1, &viewport);

	//Set the depthBias because we don't know if they user will set a new one this frame.
	if (mInternalDeviceState.mDeviceState.mRenderState[D3DRS_ZENABLE] != D3DZB_FALSE) //&& type > 3
	{
		mCurrentDrawCommandBuffer.setDepthBias(
			bit_cast(mInternalDeviceState.mDeviceState.mRenderState[D3DRS_DEPTHBIAS]),
			0.0f,
			bit_cast(mInternalDeviceState.mDeviceState.mRenderState[D3DRS_SLOPESCALEDEPTHBIAS]));
	}
	else
	{
		mCurrentDrawCommandBuffer.setDepthBias(
			0.0f,
			0.0f,
			0.0f);
	}

	//Bind the descriptor because we don't know if the user will set a new one this frame.
	if (mLastDescriptorSet != vk::DescriptorSet())
	{
		mCurrentDrawCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout.get(), 0, 1, &mLastDescriptorSet, 0, nullptr);
	}

	mIsRecording = true;

	mInternalDeviceState.mDeviceState.mCapturedAnyStreamSource = true; //Mark vertex streams as dirty so next draw will reset them.
	mInternalDeviceState.mDeviceState.mCapturedIndexBuffer = true; //Mark index as dirty so next draw will reset it.
}

void CDevice9::StopRecordingCommands()
{
	if (!mIsRecording)
	{
		return;
	}

	mCurrentDrawCommandBuffer.end();

	vk::PipelineStageFlags pipelineFlag[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	vk::SubmitInfo submitInfo;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &mImageAvailableSemaphores[mFrameIndex].get();
	submitInfo.pWaitDstStageMask = pipelineFlag;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &mCurrentDrawCommandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &mRenderFinishedSemaphores[mFrameIndex].get();
	mQueue.submit(1, &submitInfo, mDrawFences[mFrameIndex].get());

	mIsRecording = false;
}

void CDevice9::BeginRecordingUtilityCommands()
{
	mUtilityRecordingCount++;

	if (mUtilityRecordingCount > 1)
	{
		return;
	}

	mUtilityIndex = (mUtilityIndex++) % mUtilityCommandBuffers.size();

	mDevice->waitForFences(1, &mUtilityFences[mUtilityIndex].get(), VK_TRUE, UINT64_MAX);
	mDevice->resetFences(1, &mUtilityFences[mUtilityIndex].get());

	mCurrentUtilityCommandBuffer = mUtilityCommandBuffers[mUtilityIndex].get();

	vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	mCurrentUtilityCommandBuffer.begin(&beginInfo);
}

void CDevice9::StopRecordingUtilityCommands()
{
	mUtilityRecordingCount--;

	if (mUtilityRecordingCount > 0)
	{
		return;
	}

	mCurrentUtilityCommandBuffer.end();

	vk::PipelineStageFlags pipelineFlag[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput }; //eAllCommands
	vk::SubmitInfo submitInfo;
	submitInfo.pWaitDstStageMask = pipelineFlag;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &mCurrentUtilityCommandBuffer;
	mQueue.submit(1, &submitInfo, mUtilityFences[mUtilityIndex].get());

	mUtilityRecordingCount = 0;
}

void CDevice9::BeginDraw(D3DPRIMITIVETYPE primitiveType)
{
	if (mIsDrawing)
	{
		return;
	}

	//Check to see if the stream sources have been changed and if so bind the current buffers.
	if (mInternalDeviceState.mDeviceState.mCapturedAnyStreamSource)
	{
		const VkDeviceSize offsetInBytes = 0;
		std::vector<vk::Buffer> vertexBuffers;
		vertexBuffers.reserve(MAX_VERTEX_INPUTS);

		for (auto& streamSource : mInternalDeviceState.mDeviceState.mStreamSource)
		{
			if (streamSource.vertexBuffer)
			{
				vertexBuffers.push_back(streamSource.vertexBuffer->mCurrentVertexBuffer);
			}
		}

		mCurrentDrawCommandBuffer.bindVertexBuffers(0, vertexBuffers.size(), vertexBuffers.data(), &offsetInBytes);

		mInternalDeviceState.mDeviceState.mCapturedAnyStreamSource = false;
	}

	//Check to see if the index buffer has been changed and if so bind the current buffer if not null.
	if (mInternalDeviceState.mDeviceState.mCapturedIndexBuffer)
	{
		if (mInternalDeviceState.mDeviceState.mIndexBuffer)
		{
			switch (mInternalDeviceState.mDeviceState.mIndexBuffer->mFormat)
			{
			case D3DFMT_INDEX16:
				mCurrentDrawCommandBuffer.bindIndexBuffer(mInternalDeviceState.mDeviceState.mIndexBuffer->mCurrentIndexBuffer, 0, vk::IndexType::eUint16);
				break;
			case D3DFMT_INDEX32:
				mCurrentDrawCommandBuffer.bindIndexBuffer(mInternalDeviceState.mDeviceState.mIndexBuffer->mCurrentIndexBuffer, 0, vk::IndexType::eUint32);
				break;
			default:
				Log(warning) << "CDevice9::BeginDraw unknown index format! - " << mInternalDeviceState.mDeviceState.mIndexBuffer->mFormat << std::endl;
				break;
			}
		}

		mInternalDeviceState.mDeviceState.mCapturedIndexBuffer = false;
	}

	//Check to see if the pipeline is stale. If so create a new one and bind it.
	auto& deviceState = mInternalDeviceState.mDeviceState;
	if ((mLastPrimitiveType != primitiveType)
		|| deviceState.mCapturedVertexShader
		|| deviceState.mCapturedPixelShader
		|| deviceState.mCapturedAnyStreamFrequency
		|| deviceState.mCapturedAnyStreamSource
		|| deviceState.mCapturedFVF
		|| deviceState.mCapturedVertexDeclaration
		|| deviceState.mCapturedRenderState[D3DRS_CULLMODE]
		|| deviceState.mCapturedRenderState[D3DRS_ZENABLE]
		|| deviceState.mCapturedRenderState[D3DRS_ZWRITEENABLE]
		|| deviceState.mCapturedRenderState[D3DRS_ZFUNC]
		|| deviceState.mCapturedRenderState[D3DRS_STENCILREF]
		|| deviceState.mCapturedRenderState[D3DRS_STENCILMASK]
		|| deviceState.mCapturedRenderState[D3DRS_STENCILWRITEMASK]
		|| deviceState.mCapturedRenderState[D3DRS_CCW_STENCILFAIL]
		|| deviceState.mCapturedRenderState[D3DRS_STENCILFAIL]
		|| deviceState.mCapturedRenderState[D3DRS_CCW_STENCILPASS]
		|| deviceState.mCapturedRenderState[D3DRS_STENCILPASS]
		|| deviceState.mCapturedRenderState[D3DRS_CCW_STENCILFUNC]
		|| deviceState.mCapturedRenderState[D3DRS_STENCILFUNC]
		|| deviceState.mCapturedRenderState[D3DRS_FILLMODE]
		|| deviceState.mCapturedRenderState[D3DRS_COLORWRITEENABLE]
		|| deviceState.mCapturedRenderState[D3DRS_ALPHABLENDENABLE]
		|| deviceState.mCapturedRenderState[D3DRS_BLENDOP]
		|| deviceState.mCapturedRenderState[D3DRS_SRCBLEND]
		|| deviceState.mCapturedRenderState[D3DRS_DESTBLEND]
		|| deviceState.mCapturedRenderState[D3DRS_BLENDOPALPHA]
		|| deviceState.mCapturedRenderState[D3DRS_SRCBLENDALPHA]
		|| deviceState.mCapturedRenderState[D3DRS_DESTBLENDALPHA]
		|| deviceState.mCapturedRenderState[D3DRS_POINTSPRITEENABLE]
		)
	{
		auto& vertexDeclaration = (*mInternalDeviceState.mDeviceState.mVertexDeclaration); //If this is null we can't do anything anyway.

		std::vector<vk::PipelineShaderStageCreateInfo> shaderStageInfo;
		shaderStageInfo.reserve(3);

		if (mInternalDeviceState.mDeviceState.mVertexShader != nullptr)
		{
			shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mInternalDeviceState.mDeviceState.mVertexShader->mShader.get()).setPName("main"));


			if (mInternalDeviceState.mDeviceState.mPixelShader != nullptr)
			{
				shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eFragment).setModule(mInternalDeviceState.mDeviceState.mPixelShader->mShader.get()).setPName("main"));
			}
			else
			{
				shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eFragment).setModule(mFragShaderModule_Passthrough.get()).setPName("main"));
			}
		}
		else
		{
			if (vertexDeclaration.mHasPosition && !vertexDeclaration.mHasNormal && !vertexDeclaration.mHasPSize && !vertexDeclaration.mHasColor1 && !vertexDeclaration.mHasColor2)
			{
				switch (vertexDeclaration.mTextureCount)
				{
				case 0:
					if (vertexDeclaration.mIsTransformed)
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZRHW.get()).setPName("main"));
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZ.get()).setPName("main"));
					}

					if (deviceState.mRenderState[D3DRS_POINTSPRITEENABLE])
					{
						Log(fatal) << "CDevice9::BeginDraw point sprite not supported with hasPosition && !hasColor && !hasNormal && " << vertexDeclaration.mTextureCount << std::endl;
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eFragment).setModule(mFragShaderModule_XYZ.get()).setPName("main"));
					}
					break;
				case 1:
					if (vertexDeclaration.mIsTransformed)
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZRHW_TEX1.get()).setPName("main"));
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZ_TEX1.get()).setPName("main"));
					}

					if (deviceState.mRenderState[D3DRS_POINTSPRITEENABLE])
					{
						Log(fatal) << "CDevice9::BeginDraw point sprite not supported with hasPosition && !hasColor && !hasNormal && " << vertexDeclaration.mTextureCount << std::endl;
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eFragment).setModule(mFragShaderModule_XYZ_TEX1.get()).setPName("main"));
					}
					break;
				case 2:
					if (vertexDeclaration.mIsTransformed)
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZRHW_TEX2.get()).setPName("main"));
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZ_TEX2.get()).setPName("main"));
					}

					if (deviceState.mRenderState[D3DRS_POINTSPRITEENABLE])
					{
						Log(fatal) << "CDevice9::BeginDraw point sprite not supported with hasPosition && !hasColor && !hasNormal && " << vertexDeclaration.mTextureCount << std::endl;
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eFragment).setModule(mFragShaderModule_XYZ_TEX2.get()).setPName("main"));
					}
					break;
				default:
					Log(fatal) << "CDevice9::BeginDraw unsupported texture count " << vertexDeclaration.mTextureCount << std::endl;
					break;
				}
			}
			else if (vertexDeclaration.mHasPosition && !vertexDeclaration.mHasNormal && !vertexDeclaration.mHasPSize && vertexDeclaration.mHasColor1 && !vertexDeclaration.mHasColor2)
			{
				switch (vertexDeclaration.mTextureCount)
				{
				case 0:
					if (vertexDeclaration.mIsTransformed)
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZRHW_DIFFUSE.get()).setPName("main"));
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZ_DIFFUSE.get()).setPName("main"));
					}

					if (deviceState.mRenderState[D3DRS_POINTSPRITEENABLE])
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eGeometry).setModule(mGeomShaderModule_XYZ_DIFFUSE.get()).setPName("main"));
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eFragment).setModule(mFragShaderModule_XYZ_DIFFUSE_TEX1.get()).setPName("main"));
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eFragment).setModule(mFragShaderModule_XYZ_DIFFUSE.get()).setPName("main"));
					}

					break;
				case 1:
					if (vertexDeclaration.mIsTransformed)
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZRHW_DIFFUSE_TEX1.get()).setPName("main"));
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZ_DIFFUSE_TEX1.get()).setPName("main"));
					}

					if (deviceState.mRenderState[D3DRS_POINTSPRITEENABLE])
					{
						Log(fatal) << "CDevice9::BeginDraw point sprite not supported with hasPosition && hasColor && !hasNormal && " << vertexDeclaration.mTextureCount << std::endl;
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eFragment).setModule(mFragShaderModule_XYZ_DIFFUSE_TEX1.get()).setPName("main"));
					}
					break;
				case 2:
					if (vertexDeclaration.mIsTransformed)
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZRHW_DIFFUSE_TEX2.get()).setPName("main"));
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZ_DIFFUSE_TEX2.get()).setPName("main"));
					}

					if (deviceState.mRenderState[D3DRS_POINTSPRITEENABLE])
					{
						Log(fatal) << "CDevice9::BeginDraw point sprite not supported with hasPosition && hasColor && !hasNormal && " << vertexDeclaration.mTextureCount << std::endl;
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eFragment).setModule(mFragShaderModule_XYZ_DIFFUSE_TEX2.get()).setPName("main"));
					}
					break;
				default:
					Log(fatal) << "CDevice9::BeginDraw unsupported texture count " << vertexDeclaration.mTextureCount << std::endl;
					break;
				}
			}
			else if (vertexDeclaration.mHasPosition && vertexDeclaration.mHasNormal && !vertexDeclaration.mHasPSize && vertexDeclaration.mHasColor1 && !vertexDeclaration.mHasColor2)
			{
				switch (vertexDeclaration.mTextureCount)
				{
				case 2:
					shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX2.get()).setPName("main"));

					if (deviceState.mRenderState[D3DRS_POINTSPRITEENABLE])
					{
						Log(fatal) << "CDevice9::BeginDraw point sprite not supported with hasPosition && hasColor && hasNormal && " << vertexDeclaration.mTextureCount << std::endl;
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eFragment).setModule(mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX2.get()).setPName("main"));
					}
					break;
				case 1:
					shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZ_NORMAL_DIFFUSE_TEX1.get()).setPName("main"));
					if (deviceState.mRenderState[D3DRS_POINTSPRITEENABLE])
					{
						Log(fatal) << "CDevice9::BeginDraw point sprite not supported with hasPosition && hasColor && hasNormal && " << vertexDeclaration.mTextureCount << std::endl;
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eFragment).setModule(mFragShaderModule_XYZ_NORMAL_DIFFUSE_TEX1.get()).setPName("main"));
					}
					break;
				case 0:
					shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZ_NORMAL_DIFFUSE.get()).setPName("main"));
					if (deviceState.mRenderState[D3DRS_POINTSPRITEENABLE])
					{
						Log(fatal) << "CDevice9::BeginDraw point sprite not supported with hasPosition && hasColor && hasNormal && " << vertexDeclaration.mTextureCount << std::endl;
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eFragment).setModule(mFragShaderModule_XYZ_NORMAL_DIFFUSE.get()).setPName("main"));
					}
					break;
				default:
					Log(fatal) << "CDevice9::BeginDraw unsupported texture count " << vertexDeclaration.mTextureCount << std::endl;
					break;
				}
			}
			else if (vertexDeclaration.mHasPosition  && vertexDeclaration.mHasNormal && !vertexDeclaration.mHasPSize && !vertexDeclaration.mHasColor1 && !vertexDeclaration.mHasColor2)
			{
				switch (vertexDeclaration.mTextureCount)
				{
				case 0:
					shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZ_NORMAL.get()).setPName("main"));
					if (deviceState.mRenderState[D3DRS_POINTSPRITEENABLE])
					{
						Log(fatal) << "CDevice9::BeginDraw point sprite not supported with hasPosition && !hasColor && hasNormal && " << vertexDeclaration.mTextureCount << std::endl;
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eFragment).setModule(mFragShaderModule_XYZ_NORMAL.get()).setPName("main"));
					}
					break;
				case 1:
					shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZ_NORMAL_TEX1.get()).setPName("main"));
					if (deviceState.mRenderState[D3DRS_POINTSPRITEENABLE])
					{
						Log(fatal) << "CDevice9::BeginDraw point sprite not supported with hasPosition && !hasColor && hasNormal && " << vertexDeclaration.mTextureCount << std::endl;
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eFragment).setModule(mFragShaderModule_XYZ_NORMAL_TEX1.get()).setPName("main"));
					}
					break;
				case 2:
					shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eVertex).setModule(mVertShaderModule_XYZ_NORMAL_TEX2.get()).setPName("main"));
					if (deviceState.mRenderState[D3DRS_POINTSPRITEENABLE])
					{
						Log(fatal) << "CDevice9::BeginDraw point sprite not supported with hasPosition && !hasColor && hasNormal && " << vertexDeclaration.mTextureCount << std::endl;
					}
					else
					{
						shaderStageInfo.push_back(vk::PipelineShaderStageCreateInfo().setStage(vk::ShaderStageFlagBits::eFragment).setModule(mFragShaderModule_XYZ_NORMAL_TEX2.get()).setPName("main"));
					}
					break;
				default:
					Log(fatal) << "CDevice9::BeginDraw unsupported texture count " << vertexDeclaration.mTextureCount << std::endl;
					break;
				}
			}
			else
			{
				Log(fatal) << "CDevice9::BeginDraw unsupported layout." << std::endl;
				Log(fatal) << "CDevice9::BeginDraw hasPosition = " << vertexDeclaration.mHasPosition << std::endl;
				Log(fatal) << "CDevice9::BeginDraw hasNormal = " << vertexDeclaration.mHasNormal << std::endl;
				Log(fatal) << "CDevice9::BeginDraw hasPSize = " << vertexDeclaration.mHasPSize << std::endl;
				Log(fatal) << "CDevice9::BeginDraw hasColor1 = " << vertexDeclaration.mHasColor1 << std::endl;
				Log(fatal) << "CDevice9::BeginDraw hasColor2 = " << vertexDeclaration.mHasColor2 << std::endl;
				Log(fatal) << "CDevice9::BeginDraw textureCount = " << vertexDeclaration.mTextureCount << std::endl;
			}
		}

		std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescription;
		for (size_t i = 0; i < MAX_VERTEX_INPUTS; i++)
		{
			auto& streamSource = deviceState.mStreamSource[i];
			if (streamSource.vertexBuffer)
			{
				auto inputRate = (deviceState.mStreamSourceFrequency[i] == D3DSTREAMSOURCE_INSTANCEDATA) ? vk::VertexInputRate::eInstance : vk::VertexInputRate::eVertex;
				vertexInputBindingDescription.push_back(vk::VertexInputBindingDescription(i, streamSource.stride, inputRate));
			}
		}

		auto const vertexInputInfo = vk::PipelineVertexInputStateCreateInfo()
			.setPVertexAttributeDescriptions(vertexDeclaration.mVertexInputAttributeDescription.data())
			.setVertexAttributeDescriptionCount(vertexDeclaration.mVertexInputAttributeDescription.size())
			.setPVertexBindingDescriptions(vertexInputBindingDescription.data())
			.setVertexBindingDescriptionCount(vertexInputBindingDescription.size());

		auto const inputAssemblyInfo = vk::PipelineInputAssemblyStateCreateInfo().setTopology(ConvertPrimitiveType(primitiveType));
		auto const viewportInfo = vk::PipelineViewportStateCreateInfo().setViewportCount(1).setScissorCount(1);
		auto const rasterizationInfo = vk::PipelineRasterizationStateCreateInfo()
			.setDepthClampEnable(VK_FALSE)
			.setRasterizerDiscardEnable(VK_FALSE)
			.setPolygonMode(ConvertFillMode((D3DFILLMODE)deviceState.mRenderState[D3DRS_FILLMODE]))
			.setCullMode(GetCullMode((D3DCULL)deviceState.mRenderState[D3DRS_CULLMODE]))
			.setFrontFace(GetFrontFace((D3DCULL)deviceState.mRenderState[D3DRS_CULLMODE]))
			.setDepthBiasEnable(VK_TRUE)
			.setLineWidth(1.0f);

		auto const multisampleInfo = vk::PipelineMultisampleStateCreateInfo();

		auto const frontStencilOp = vk::StencilOpState()
			.setReference(deviceState.mRenderState[D3DRS_STENCILREF])
			.setCompareMask(deviceState.mRenderState[D3DRS_STENCILMASK])
			.setWriteMask(deviceState.mRenderState[D3DRS_STENCILWRITEMASK])
			.setFailOp(deviceState.mRenderState[D3DRS_CULLMODE] != D3DCULL_CCW ? ConvertStencilOperation((D3DSTENCILOP)deviceState.mRenderState[D3DRS_CCW_STENCILFAIL]) : ConvertStencilOperation((D3DSTENCILOP)deviceState.mRenderState[D3DRS_STENCILFAIL]))
			.setPassOp(deviceState.mRenderState[D3DRS_CULLMODE] != D3DCULL_CCW ? ConvertStencilOperation((D3DSTENCILOP)deviceState.mRenderState[D3DRS_CCW_STENCILPASS]) : ConvertStencilOperation((D3DSTENCILOP)deviceState.mRenderState[D3DRS_STENCILPASS]))
			.setCompareOp(deviceState.mRenderState[D3DRS_CULLMODE] != D3DCULL_CCW ? ConvertCompareOperation((D3DCMPFUNC)deviceState.mRenderState[D3DRS_CCW_STENCILFUNC]) : ConvertCompareOperation((D3DCMPFUNC)deviceState.mRenderState[D3DRS_STENCILFUNC]));
		auto const backStencilOp = vk::StencilOpState()
			.setReference(deviceState.mRenderState[D3DRS_STENCILREF])
			.setCompareMask(deviceState.mRenderState[D3DRS_STENCILMASK])
			.setWriteMask(deviceState.mRenderState[D3DRS_STENCILWRITEMASK])
			.setFailOp(deviceState.mRenderState[D3DRS_CULLMODE] == D3DCULL_CCW ? ConvertStencilOperation((D3DSTENCILOP)deviceState.mRenderState[D3DRS_CCW_STENCILFAIL]) : ConvertStencilOperation((D3DSTENCILOP)deviceState.mRenderState[D3DRS_STENCILFAIL]))
			.setPassOp(deviceState.mRenderState[D3DRS_CULLMODE] == D3DCULL_CCW ? ConvertStencilOperation((D3DSTENCILOP)deviceState.mRenderState[D3DRS_CCW_STENCILPASS]) : ConvertStencilOperation((D3DSTENCILOP)deviceState.mRenderState[D3DRS_STENCILPASS]))
			.setCompareOp(deviceState.mRenderState[D3DRS_CULLMODE] == D3DCULL_CCW ? ConvertCompareOperation((D3DCMPFUNC)deviceState.mRenderState[D3DRS_CCW_STENCILFUNC]) : ConvertCompareOperation((D3DCMPFUNC)deviceState.mRenderState[D3DRS_STENCILFUNC]));
		auto const depthStencilInfo = vk::PipelineDepthStencilStateCreateInfo()
			.setDepthTestEnable(deviceState.mRenderState[D3DRS_ZENABLE])
			.setDepthWriteEnable(deviceState.mRenderState[D3DRS_ZWRITEENABLE])
			.setDepthCompareOp(ConvertCompareOperation((D3DCMPFUNC)deviceState.mRenderState[D3DRS_ZFUNC]))
			.setDepthBoundsTestEnable(VK_FALSE)
			.setStencilTestEnable(deviceState.mRenderState[D3DRS_STENCILENABLE])
			.setFront(frontStencilOp)
			.setBack(backStencilOp);

		vk::PipelineColorBlendAttachmentState const colorBlendAttachments[1] =
		{
			vk::PipelineColorBlendAttachmentState()
			.setColorWriteMask((vk::ColorComponentFlagBits)deviceState.mRenderState[D3DRS_COLORWRITEENABLE])
			.setBlendEnable(deviceState.mRenderState[D3DRS_ALPHABLENDENABLE])
			.setColorBlendOp(ConvertColorOperation((D3DBLENDOP)deviceState.mRenderState[D3DRS_BLENDOP]))
			.setSrcColorBlendFactor(ConvertColorFactor((D3DBLEND)deviceState.mRenderState[D3DRS_SRCBLEND]))
			.setDstColorBlendFactor(ConvertColorFactor((D3DBLEND)deviceState.mRenderState[D3DRS_DESTBLEND]))
			.setAlphaBlendOp(ConvertColorOperation((D3DBLENDOP)deviceState.mRenderState[D3DRS_BLENDOPALPHA]))
			.setSrcAlphaBlendFactor(ConvertColorFactor((D3DBLEND)deviceState.mRenderState[D3DRS_SRCBLENDALPHA]))
			.setDstAlphaBlendFactor(ConvertColorFactor((D3DBLEND)deviceState.mRenderState[D3DRS_DESTBLENDALPHA]))
		};
		auto const colorBlendInfo = vk::PipelineColorBlendStateCreateInfo().setAttachmentCount(1).setPAttachments(colorBlendAttachments);

		vk::DynamicState const dynamicStates[3] = { vk::DynamicState::eViewport, vk::DynamicState::eScissor, vk::DynamicState::eDepthBias };
		auto const dynamicStateInfo = vk::PipelineDynamicStateCreateInfo().setPDynamicStates(dynamicStates).setDynamicStateCount(3);
		auto const pipeline = vk::GraphicsPipelineCreateInfo()
			.setStageCount(shaderStageInfo.size())
			.setPStages(shaderStageInfo.data())
			.setPVertexInputState(&vertexInputInfo)
			.setPInputAssemblyState(&inputAssemblyInfo)
			.setPViewportState(&viewportInfo)
			.setPRasterizationState(&rasterizationInfo)
			.setPMultisampleState(&multisampleInfo)
			.setPDepthStencilState(&depthStencilInfo)
			.setPColorBlendState(&colorBlendInfo)
			.setPDynamicState(&dynamicStateInfo)
			.setLayout(mPipelineLayout.get())
			.setRenderPass(mCurrentRenderContainer->mRenderPass.get());

		mPipelines[mFrameIndex].push_back(mDevice->createGraphicsPipelineUnique(mPipelineCache.get(), pipeline));
		mCurrentDrawCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipelines[mFrameIndex][mPipelines[mFrameIndex].size() - 1].get());

		deviceState.mCapturedVertexShader = false;
		deviceState.mCapturedPixelShader = false;
		deviceState.mCapturedAnyStreamFrequency = false;
		deviceState.mCapturedAnyStreamSource = false;
		deviceState.mCapturedFVF = false;
		deviceState.mCapturedVertexDeclaration = false;
		deviceState.mCapturedRenderState[D3DRS_CULLMODE] = false;
		deviceState.mCapturedRenderState[D3DRS_ZENABLE] = false;
		deviceState.mCapturedRenderState[D3DRS_ZWRITEENABLE] = false;
		deviceState.mCapturedRenderState[D3DRS_ZFUNC] = false;
		deviceState.mCapturedRenderState[D3DRS_STENCILREF] = false;
		deviceState.mCapturedRenderState[D3DRS_STENCILMASK] = false;
		deviceState.mCapturedRenderState[D3DRS_STENCILWRITEMASK] = false;
		deviceState.mCapturedRenderState[D3DRS_CCW_STENCILFAIL] = false;
		deviceState.mCapturedRenderState[D3DRS_STENCILFAIL] = false;
		deviceState.mCapturedRenderState[D3DRS_CCW_STENCILPASS] = false;
		deviceState.mCapturedRenderState[D3DRS_STENCILPASS] = false;
		deviceState.mCapturedRenderState[D3DRS_CCW_STENCILFUNC] = false;
		deviceState.mCapturedRenderState[D3DRS_STENCILFUNC] = false;
		deviceState.mCapturedRenderState[D3DRS_FILLMODE] = false;
		deviceState.mCapturedRenderState[D3DRS_COLORWRITEENABLE] = false;
		deviceState.mCapturedRenderState[D3DRS_ALPHABLENDENABLE] = false;
		deviceState.mCapturedRenderState[D3DRS_BLENDOP] = false;
		deviceState.mCapturedRenderState[D3DRS_SRCBLEND] = false;
		deviceState.mCapturedRenderState[D3DRS_DESTBLEND] = false;
		deviceState.mCapturedRenderState[D3DRS_BLENDOPALPHA] = false;
		deviceState.mCapturedRenderState[D3DRS_SRCBLENDALPHA] = false;
		deviceState.mCapturedRenderState[D3DRS_DESTBLENDALPHA] = false;
		deviceState.mCapturedRenderState[D3DRS_POINTSPRITEENABLE] = false;

		mLastPrimitiveType = primitiveType;
	}

	//Check to see if the texture stuff has changed and if so update the descriptor set.
	if (deviceState.mCapturedAnyTexture || deviceState.mCapturedAnySamplerState) //1==1 || 
	{
		//If we're out of descriptor sets to write into then allocate a new one.
		if (mDescriptorSetIndex >= mDescriptorSets[mFrameIndex].size())
		{
			vk::DescriptorSet descriptorSet;
			vk::DescriptorSetAllocateInfo descriptorSetInfo(mDescriptorPool.get(), 1, &mDescriptorLayout.get());
			vk::Result result = mDevice->allocateDescriptorSets(&descriptorSetInfo, &descriptorSet);
			if (result != vk::Result::eSuccess)
			{
				Log(fatal) << "CDevice9::BeginDraw vkAllocateDescriptorSets failed with return code of " << result << std::endl;
				//return;
			}
			mDescriptorSets[mFrameIndex].push_back(descriptorSet);
		}

		for (size_t i = 0; i < 16; i++)
		{
			mDescriptorImageInfo[i].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			mDescriptorImageInfo[i].sampler = vk::Sampler();

			//Look for an existing sampler.
			for (auto& samplerContainer : mSamplerContainers)
			{
				if (samplerContainer->mSamplerState == deviceState.mSamplerState[i])
				{
					mDescriptorImageInfo[i].sampler = samplerContainer->mSampler.get();
				}
			}

			//If no matching sampler was found then make one.
			if (mDescriptorImageInfo[i].sampler == vk::Sampler())
			{
				mSamplerContainers.push_back(std::make_unique<SamplerContainer>(mDevice.get(), deviceState.mSamplerState[i]));
				mDescriptorImageInfo[i].sampler = mSamplerContainers[mSamplerContainers.size() - 1]->mSampler.get();
			}

			//Bind the current set texture or if null bind a dummy texture.
			if (deviceState.mTexture[i])
			{
				switch (deviceState.mTexture[i]->GetType())
				{
				case D3DRTYPE_TEXTURE:
					mDescriptorImageInfo[i].imageView = reinterpret_cast <CTexture9*>(deviceState.mTexture[i])->mImageView.get();
					break;
				case D3DRTYPE_VOLUMETEXTURE:
					//mDescriptorImageInfo[i].imageView = reinterpret_cast <CVolumeTexture9*>(deviceState.mTexture[i])->mImageView.get();
					break;
				case D3DRTYPE_CUBETEXTURE:
					mDescriptorImageInfo[i].imageView = reinterpret_cast <CCubeTexture9*>(deviceState.mTexture[i])->mImageView.get();
					break;
				}
			}
			else
			{
				mDescriptorImageInfo[i].imageView = mBlankTexture->mImageView.get();
			}
		}

		mLastDescriptorSet = mDescriptorSets[mFrameIndex][mDescriptorSetIndex];

		mWriteDescriptorSet[0].dstSet = mLastDescriptorSet;
		mWriteDescriptorSet[1].dstSet = mLastDescriptorSet;
		mWriteDescriptorSet[2].dstSet = mLastDescriptorSet;
		mWriteDescriptorSet[3].dstSet = mLastDescriptorSet;
		mWriteDescriptorSet[4].dstSet = mLastDescriptorSet;
		mWriteDescriptorSet[5].dstSet = mLastDescriptorSet;
		mWriteDescriptorSet[6].dstSet = mLastDescriptorSet;
		mWriteDescriptorSet[6].pImageInfo = mDescriptorImageInfo;
		mWriteDescriptorSet[7].dstSet = mLastDescriptorSet;
		mWriteDescriptorSet[8].dstSet = mLastDescriptorSet;

		mDevice->updateDescriptorSets(9, &mWriteDescriptorSet[0], 0, nullptr);
		mCurrentDrawCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mPipelineLayout.get(), 0, 1, &mLastDescriptorSet, 0, nullptr);

		deviceState.mCapturedAnySamplerState = false;
		deviceState.mCapturedAnyTexture = false;

		mDescriptorSetIndex++;
	}

	vk::RenderPassBeginInfo renderPassBeginInfo;
	renderPassBeginInfo.renderPass = mCurrentRenderContainer->mRenderPass.get();
	renderPassBeginInfo.framebuffer = mCurrentRenderContainer->mFrameBuffers[mFrameIndex].get();
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = mPresentationParameters.BackBufferWidth;
	renderPassBeginInfo.renderArea.extent.height = mPresentationParameters.BackBufferHeight;
	//renderPassBeginInfo.clearValueCount = 2;
	//renderPassBeginInfo.pClearValues = clearValues;
	mCurrentDrawCommandBuffer.beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);

	mIsDrawing = true;
}

void CDevice9::StopDraw()
{
	if (!mIsDrawing)
	{
		return;
	}

	mCurrentDrawCommandBuffer.endRenderPass();

	mIsDrawing = false;
}

void CDevice9::RebuildRenderPass()
{
	mCurrentRenderContainer = nullptr;
	for (auto& renderContainer : mRenderContainers)
	{
		if (renderContainer->mRenderTargets == mRenderTargets && renderContainer->mDepthStencilSurface == mDepthStencilSurface)
		{
			mCurrentRenderContainer = renderContainer.get();
		}
	}

	if (!mCurrentRenderContainer)
	{
		mRenderContainers.push_back(std::make_unique<RenderContainer>(mDevice.get(), mDepthStencilSurface, mRenderTargets));
		mCurrentRenderContainer = mRenderContainers[mRenderContainers.size() - 1].get();
	}
}

HRESULT STDMETHODCALLTYPE CDevice9::Clear(DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	BeginRecordingCommands();

	/*
	https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkRenderPassBeginInfo.html
	The array is indexed by attachment number. Only elements corresponding to cleared attachments are used. Other elements of pClearValues are ignored.
	*/
	std::vector<vk::ClearValue> clearValues;
	const std::array<float, 4> colorValues = { D3DCOLOR_R(Color), D3DCOLOR_G(Color), D3DCOLOR_B(Color), D3DCOLOR_A(Color) };
	for (auto& renderTarget : mRenderTargets)
	{
		if (renderTarget)
		{
			clearValues.push_back(vk::ClearColorValue(colorValues));
		}
	}
	clearValues.push_back(vk::ClearDepthStencilValue(Z, Stencil));

	if (((Flags & D3DCLEAR_TARGET) == D3DCLEAR_TARGET) && (((Flags & D3DCLEAR_STENCIL) == D3DCLEAR_STENCIL) || ((Flags & D3DCLEAR_ZBUFFER) == D3DCLEAR_ZBUFFER)))
	{
		vk::RenderPassBeginInfo renderPassBeginInfo;
		renderPassBeginInfo.renderPass = mCurrentRenderContainer->mClearBothRenderPass.get();
		renderPassBeginInfo.framebuffer = mCurrentRenderContainer->mClearBothFrameBuffers[mFrameIndex].get();
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = mPresentationParameters.BackBufferWidth;
		renderPassBeginInfo.renderArea.extent.height = mPresentationParameters.BackBufferHeight;
		renderPassBeginInfo.clearValueCount = clearValues.size();
		renderPassBeginInfo.pClearValues = clearValues.data();
		mCurrentDrawCommandBuffer.beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);
		mCurrentDrawCommandBuffer.endRenderPass();
	}
	else if ((Flags & D3DCLEAR_TARGET) == D3DCLEAR_TARGET)
	{
		std::array<float, 4> colorValues = { D3DCOLOR_R(Color), D3DCOLOR_G(Color), D3DCOLOR_B(Color), D3DCOLOR_A(Color) };
		clearValues.push_back(vk::ClearColorValue(colorValues));

		vk::RenderPassBeginInfo renderPassBeginInfo;
		renderPassBeginInfo.renderPass = mCurrentRenderContainer->mClearColorRenderPass.get();
		renderPassBeginInfo.framebuffer = mCurrentRenderContainer->mClearColorFrameBuffers[mFrameIndex].get();
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = mPresentationParameters.BackBufferWidth;
		renderPassBeginInfo.renderArea.extent.height = mPresentationParameters.BackBufferHeight;
		renderPassBeginInfo.clearValueCount = clearValues.size();
		renderPassBeginInfo.pClearValues = clearValues.data();
		mCurrentDrawCommandBuffer.beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);
		mCurrentDrawCommandBuffer.endRenderPass();
	}
	else if (((Flags & D3DCLEAR_STENCIL) == D3DCLEAR_STENCIL) || ((Flags & D3DCLEAR_ZBUFFER) == D3DCLEAR_ZBUFFER))
	{
		clearValues.push_back(vk::ClearDepthStencilValue(Z, Stencil));

		vk::RenderPassBeginInfo renderPassBeginInfo;
		renderPassBeginInfo.renderPass = mCurrentRenderContainer->mClearDepthRenderPass.get();
		renderPassBeginInfo.framebuffer = mCurrentRenderContainer->mClearDepthFrameBuffers[mFrameIndex].get();
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = mPresentationParameters.BackBufferWidth;
		renderPassBeginInfo.renderArea.extent.height = mPresentationParameters.BackBufferHeight;
		renderPassBeginInfo.clearValueCount = clearValues.size();
		renderPassBeginInfo.pClearValues = clearValues.data();
		mCurrentDrawCommandBuffer.beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);
		mCurrentDrawCommandBuffer.endRenderPass();
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::BeginScene() //
{
	BeginRecordingCommands();

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
	return mSwapChains[0]->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, 0/*dwFlags*/);
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
	mRecordedDeviceState = new CStateBlock9(this, (D3DSTATEBLOCKTYPE)0);

	mRecordedDeviceState->PrivateAddRef();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::ColorFill(IDirect3DSurface9 *pSurface, const RECT *pRect, D3DCOLOR color)
{
	BeginRecordingCommands();

	//TODO: Implement.

	Log(warning) << "CDevice9::ColorFill is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS *pPresentationParameters, IDirect3DSwapChain9 **ppSwapChain)
{
	auto ptr = new CSwapChain9(this, pPresentationParameters);

	(*ppSwapChain) = ptr;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9 **ppCubeTexture, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CCubeTexture9* obj = new CCubeTexture9(this, EdgeLength, Levels, Usage, Format, Pool, pSharedHandle);

	(*ppCubeTexture) = (IDirect3DCubeTexture9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CSurface9* obj = new CSurface9(this, (CTexture9*)nullptr, Width, Height, D3DUSAGE_DEPTHSTENCIL, 1 /*Levels*/, Format, MultiSample, MultisampleQuality, Discard, false /*Lockable*/, D3DPOOL_DEFAULT, pSharedHandle);

	(*ppSurface) = (IDirect3DSurface9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9 **ppIndexBuffer, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CIndexBuffer9* obj = new CIndexBuffer9(this, Length, Usage, Format, Pool, pSharedHandle);

	(*ppIndexBuffer) = (IDirect3DIndexBuffer9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CSurface9* ptr = new CSurface9(this, (CTexture9*)nullptr, Width, Height, D3DUSAGE_RENDERTARGET, 1, Format, D3DMULTISAMPLE_NONE, 0, false, false, Pool, pSharedHandle);

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
		CQuery9* obj = new CQuery9(this, Type);

		(*ppQuery) = (IDirect3DQuery9*)obj;

		return S_OK;
	}
	else
	{
		return D3DERR_NOTAVAILABLE;
	}
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CSurface9* ptr = new CSurface9(this, (CTexture9*)nullptr, Width, Height, D3DUSAGE_RENDERTARGET, 1, Format, MultiSample, MultisampleQuality, false, Lockable, D3DPOOL_DEFAULT, pSharedHandle);

	(*ppSurface) = (IDirect3DSurface9*)ptr;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9 **ppSB)
{
	HRESULT result = S_OK;

	CStateBlock9* obj = new CStateBlock9(this, Type);

	(*ppSB) = (IDirect3DStateBlock9*)obj;

	obj->AddRef();

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9 **ppTexture, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CTexture9* obj = new CTexture9(this, Width, Height, Levels, Usage, Format, Pool, pSharedHandle);

	(*ppTexture) = (IDirect3DTexture9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9 **ppVertexBuffer, HANDLE *pSharedHandle)
{
	HRESULT result = S_OK;

	CVertexBuffer9* obj = new CVertexBuffer9(this, Length, Usage, FVF, Pool, pSharedHandle);

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
	CVolumeTexture9* obj = new CVolumeTexture9(this, Width, Height, Depth, Levels, Usage, Format, Pool, pSharedHandle);

	(*ppVolumeTexture) = (IDirect3DVolumeTexture9*)obj;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DeletePatch(UINT Handle)
{
	//TODO: Implement.

	Log(warning) << "CDevice9::DeletePatch is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount)
{
	BeginRecordingCommands();

	BeginDraw(Type);
	{
		mCurrentDrawCommandBuffer.drawIndexed(ConvertPrimitiveCountToVertexCount(Type, PrimitiveCount), 1, StartIndex, BaseVertexIndex, 0);
	}
	StopDraw();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, const void *pIndexData, D3DFORMAT IndexDataFormat, const void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	BeginRecordingCommands();

	mCurrentDrawCommandBuffer.updateBuffer(mUpVertexBuffer.get(), 0, ConvertPrimitiveCountToBufferSize(PrimitiveType, PrimitiveCount, VertexStreamZeroStride), pVertexStreamZeroData);
	mCurrentDrawCommandBuffer.updateBuffer(mUpIndexBuffer.get(), 0, ConvertPrimitiveCountToBufferSize(PrimitiveType, PrimitiveCount, (IndexDataFormat == D3DFMT_INDEX16) ? 2 : 4), pIndexData);

	BeginDraw(PrimitiveType);
	{
		mInternalDeviceState.mDeviceState.mCapturedAnyStreamSource = true; //Mark vertex streams as dirty so next draw will reset them.
		mInternalDeviceState.mDeviceState.mCapturedIndexBuffer = true;

		//TODO: check to see if I need a new pipeline. (I probably do because I'm getting a new stride)

		const VkDeviceSize offsetInBytes = 0;
		mCurrentDrawCommandBuffer.bindVertexBuffers(0, 1, &mUpVertexBuffer.get(), &offsetInBytes);

		switch (IndexDataFormat)
		{
		case D3DFMT_INDEX16:
			mCurrentDrawCommandBuffer.bindIndexBuffer(mUpIndexBuffer.get(), 0, vk::IndexType::eUint16);
			break;
		case D3DFMT_INDEX32:
			mCurrentDrawCommandBuffer.bindIndexBuffer(mUpIndexBuffer.get(), 0, vk::IndexType::eUint32);
			break;
		default:
			Log(warning) << "CDevice9::DrawIndexedPrimitiveUP unknown index format! - " << IndexDataFormat << std::endl;
			break;
		}

		mCurrentDrawCommandBuffer.drawIndexed(ConvertPrimitiveCountToVertexCount(PrimitiveType, PrimitiveCount), 1, 0, 0, 0);
	}
	StopDraw();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	BeginRecordingCommands();

	BeginDraw(PrimitiveType);
	{
		mCurrentDrawCommandBuffer.draw(ConvertPrimitiveCountToVertexCount(PrimitiveType, PrimitiveCount), 1, StartVertex, 0);
	}
	StopDraw();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	BeginRecordingCommands();

	mCurrentDrawCommandBuffer.updateBuffer(mUpVertexBuffer.get(), 0, ConvertPrimitiveCountToBufferSize(PrimitiveType, PrimitiveCount, VertexStreamZeroStride), pVertexStreamZeroData);

	BeginDraw(PrimitiveType);
	{
		mInternalDeviceState.mDeviceState.mCapturedAnyStreamSource = true; //Mark vertex streams as dirty so next draw will reset them.

		//TODO: check to see if I need a new pipeline. (I probably do because I'm getting a new stride)

		const VkDeviceSize offsetInBytes = 0;
		mCurrentDrawCommandBuffer.bindVertexBuffers(0, 1, &mUpVertexBuffer.get(), &offsetInBytes);

		mCurrentDrawCommandBuffer.draw(ConvertPrimitiveCountToVertexCount(PrimitiveType, PrimitiveCount), 1, 0, 0);
	}
	StopDraw();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawRectPatch(UINT Handle, const float *pNumSegs, const D3DRECTPATCH_INFO *pRectPatchInfo)
{
	BeginRecordingCommands();

	//BeginDraw(PrimitiveType);
	{
		//TODO: Implement.
		Log(warning) << "CDevice9::DrawRectPatch is not implemented!" << std::endl;
	}
	//StopDraw();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::DrawTriPatch(UINT Handle, const float *pNumSegs, const D3DTRIPATCH_INFO *pTriPatchInfo)
{
	BeginRecordingCommands();

	//BeginDraw(PrimitiveType);
	{
		//TODO: Implement.
		Log(warning) << "CDevice9::DrawTriPatch is not implemented!" << std::endl;
	}
	//StopDraw();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::EndStateBlock(IDirect3DStateBlock9** ppSB)
{
	(*ppSB) = mRecordedDeviceState;

	mRecordedDeviceState->AddRef();
	mRecordedDeviceState->PrivateRelease();
	mRecordedDeviceState = nullptr;

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
	Log(warning) << "CDevice9::GetAvailableTextureMem is not implemented!" << std::endl;

	return mAvailableTextureMemory;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetBackBuffer(UINT  iSwapChain, UINT BackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9 **ppBackBuffer)
{
	return mSwapChains[iSwapChain]->GetBackBuffer(BackBuffer, Type, ppBackBuffer);
}

HRESULT STDMETHODCALLTYPE CDevice9::GetClipPlane(DWORD Index, float* pPlane)
{
	memcpy(pPlane, &mInternalDeviceState.mDeviceState.mClipPlane[Index], 4 * sizeof(float));

	return S_OK;
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
	(*pPaletteNumber) = mInternalDeviceState.mDeviceState.mPaletteNumber;

	return S_OK;
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
	mC9->GetDeviceCaps(mAdapter, mDeviceType, pCaps);

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
	Log(warning) << "CDevice9::GetDisplayMode is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetFrontBufferData(UINT  iSwapChain, IDirect3DSurface9 *pDestSurface)
{
	return mSwapChains[iSwapChain]->GetFrontBufferData(pDestSurface);
}

HRESULT STDMETHODCALLTYPE CDevice9::GetFVF(DWORD *pFVF)
{
	(*pFVF) = mInternalDeviceState.mDeviceState.mFVF;

	return S_OK;
}

void STDMETHODCALLTYPE CDevice9::GetGammaRamp(UINT  iSwapChain, D3DGAMMARAMP *pRamp)
{
	auto& swapChain = mSwapChains[iSwapChain];

	HDC windowHdc = GetDC(swapChain->mPresentationParameters.hDeviceWindow);

	if (!GetDeviceGammaRamp(windowHdc, (LPVOID)pRamp))
	{
		(*pRamp) = {};
	}
}

HRESULT STDMETHODCALLTYPE CDevice9::GetIndices(IDirect3DIndexBuffer9 **ppIndexData) //,UINT *pBaseVertexIndex ?
{
	(*ppIndexData) = mInternalDeviceState.mDeviceState.mIndexBuffer;

	if ((*ppIndexData) != nullptr)
	{
		(*ppIndexData)->AddRef();
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetLight(DWORD Index, D3DLIGHT9 *pLight)
{
	(*pLight) = mInternalDeviceState.mDeviceState.mLight[Index];

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetLightEnable(DWORD Index, BOOL *pEnable)
{
	(*pEnable) = mInternalDeviceState.mDeviceState.mLightEnableState[Index];

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetMaterial(D3DMATERIAL9 *pMaterial)
{
	(*pMaterial) = mInternalDeviceState.mDeviceState.mMaterial;

	return S_OK;
}

FLOAT STDMETHODCALLTYPE CDevice9::GetNPatchMode()
{
	return mInternalDeviceState.mDeviceState.mNPatchMode;
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
	(*ppShader) = mInternalDeviceState.mDeviceState.mPixelShader;

	if ((*ppShader) != nullptr)
	{
		(*ppShader)->AddRef();
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantB(UINT StartRegister, BOOL *pConstantData, UINT BoolCount)
{
	memcpy(pConstantData, &mInternalDeviceState.mDeviceState.mPixelShaderConstantB[StartRegister], BoolCount * sizeof(int));

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantF(UINT StartRegister, float *pConstantData, UINT Vector4fCount)
{
	memcpy(pConstantData, &mInternalDeviceState.mDeviceState.mPixelShaderConstantF[StartRegister], Vector4fCount * sizeof(float[4]));

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetPixelShaderConstantI(UINT StartRegister, int *pConstantData, UINT Vector4iCount)
{
	memcpy(pConstantData, &mInternalDeviceState.mDeviceState.mPixelShaderConstantI[StartRegister], Vector4iCount * sizeof(int[4]));

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRasterStatus(UINT  iSwapChain, D3DRASTER_STATUS *pRasterStatus)
{
	return mSwapChains[iSwapChain]->GetRasterStatus(pRasterStatus);
}

HRESULT STDMETHODCALLTYPE CDevice9::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue)
{
	(*pValue) = mInternalDeviceState.mDeviceState.mRenderState[State];

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
	DWORD index = Sampler;

	if (index < 16)
	{
		(*pValue) = mInternalDeviceState.mDeviceState.mSamplerState[index][Type];
	}
	else if (index >= D3DVERTEXTEXTURESAMPLER0)
	{
		index = 16 + (index - D3DVERTEXTEXTURESAMPLER0);
		(*pValue) = mInternalDeviceState.mDeviceState.mSamplerState[index][Type];
	}
	//else
	//{
	//	(*pValue) = 0;
	//}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetScissorRect(RECT* pRect)
{
	(*pRect) = mInternalDeviceState.mDeviceState.mScissorRect;

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
	auto& entry = mInternalDeviceState.mDeviceState.mStreamSource[StreamNumber];

	(*ppStreamData) = entry.vertexBuffer;
	(*pOffsetInBytes) = entry.offset;
	(*pStride) = entry.stride;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetStreamSourceFreq(UINT StreamNumber, UINT *pDivider)
{
	(*pDivider) = mInternalDeviceState.mDeviceState.mStreamSourceFrequency[StreamNumber];

	return S_OK;
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
	(*ppTexture) = mInternalDeviceState.mDeviceState.mTexture[Stage];

	if ((*ppTexture) != nullptr)
	{
		(*ppTexture)->AddRef();
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
{
	(*pValue) = mInternalDeviceState.mDeviceState.mTextureStageState[Stage][Type];

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
	(*pMatrix) = mInternalDeviceState.mDeviceState.mTransform[State];

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl)
{
	(*ppDecl) = mInternalDeviceState.mDeviceState.mVertexDeclaration;

	if ((*ppDecl) != nullptr)
	{
		(*ppDecl)->AddRef();
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShader(IDirect3DVertexShader9** ppShader)
{
	(*ppShader) = mInternalDeviceState.mDeviceState.mVertexShader;

	if ((*ppShader) != nullptr)
	{
		(*ppShader)->AddRef();
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
	memcpy(pConstantData, &mInternalDeviceState.mDeviceState.mVertexShaderConstantB[StartRegister], BoolCount * sizeof(int));

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
	memcpy(pConstantData, &mInternalDeviceState.mDeviceState.mVertexShaderConstantF[StartRegister], Vector4fCount * sizeof(float[4]));

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
	memcpy(pConstantData, &mInternalDeviceState.mDeviceState.mVertexShaderConstantI[StartRegister], Vector4iCount * sizeof(int[4]));

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetViewport(D3DVIEWPORT9* pViewport)
{
	(*pViewport) = mInternalDeviceState.mDeviceState.mViewport;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::LightEnable(DWORD LightIndex, BOOL bEnable)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->LightEnable(LightIndex, bEnable);
	}
	else
	{
		BeginRecordingCommands();

		mCurrentDrawCommandBuffer.updateBuffer(mLightEnableBuffer.get(), 0, sizeof(BOOL), &bEnable);

		mInternalDeviceState.LightEnable(LightIndex, bEnable);
	}

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
	if (pPresentationParameters != nullptr)
	{
		if (pPresentationParameters->BackBufferFormat == D3DFMT_UNKNOWN)
		{
			pPresentationParameters->BackBufferFormat = D3DFMT_X8R8G8B8;
		}
		memcpy(&mPresentationParameters, pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS));
	}

	StopRecordingCommands();
	mDevice->waitIdle();
	ResetVulkanDevice();

	return S_OK;
}

HRESULT CDevice9::SetClipPlane(DWORD Index, const float *pPlane)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetClipPlane(Index, pPlane);
	}
	else
	{
		//BeginRecordingCommands();

		mInternalDeviceState.SetClipPlane(Index, pPlane);
	}

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
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetCurrentTexturePalette(PaletteNumber);
	}
	else
	{
		//BeginRecordingCommands();

		mInternalDeviceState.SetCurrentTexturePalette(PaletteNumber);
	}

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

	RebuildRenderPass();

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
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetFVF(FVF);
	}
	else
	{
		//BeginRecordingCommands();

		mInternalDeviceState.SetFVF(FVF);

		//Convert the FVF into a vertex declaration.
		mInternalDeviceState.SetVertexDeclaration(nullptr);
		for (auto& vertexDeclaration : mVertexDeclarations)
		{
			if (vertexDeclaration && vertexDeclaration->mFVF == FVF)
			{
				mInternalDeviceState.SetVertexDeclaration(vertexDeclaration.get());
			}
		}
		if (!mInternalDeviceState.mDeviceState.mVertexDeclaration)
		{
			mVertexDeclarations.push_back(std::make_unique<CVertexDeclaration9>(this, FVF));
			mInternalDeviceState.SetVertexDeclaration(mVertexDeclarations[mVertexDeclarations.size() - 1].get());
		}
	}

	return S_OK;
}

void STDMETHODCALLTYPE CDevice9::SetGammaRamp(UINT  iSwapChain, DWORD Flags, const D3DGAMMARAMP *pRamp)
{
	auto& swapChain = mSwapChains[iSwapChain];

	HDC windowHdc = GetDC(swapChain->mPresentationParameters.hDeviceWindow);
	SetDeviceGammaRamp(windowHdc, (LPVOID)pRamp);

	if (Flags != D3DSGR_NO_CALIBRATION)
	{
		//TODO: Implement.
		Log(warning) << "CDevice9::SetGammaRamp D3DSGR_CALIBRATE is not implemented!" << std::endl;
	}
}

HRESULT STDMETHODCALLTYPE CDevice9::SetIndices(IDirect3DIndexBuffer9* pIndexData)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetIndices(pIndexData);
	}
	else
	{
		//BeginRecordingCommands();

		mInternalDeviceState.SetIndices(pIndexData);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetLight(DWORD Index, const D3DLIGHT9* pLight)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetLight(Index, pLight);
	}
	else
	{
		BeginRecordingCommands();

		PaddedLight light = (*pLight);
		mCurrentDrawCommandBuffer.updateBuffer(mLightBuffer.get(), Index * sizeof(PaddedLight), sizeof(PaddedLight), &light);

		mInternalDeviceState.SetLight(Index, pLight);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetMaterial(const D3DMATERIAL9* pMaterial)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetMaterial(pMaterial);
	}
	else
	{
		BeginRecordingCommands();

		mCurrentDrawCommandBuffer.updateBuffer(mMaterialBuffer.get(), 0, sizeof(D3DMATERIAL9), pMaterial);

		mInternalDeviceState.SetMaterial(pMaterial);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetNPatchMode(float nSegments)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetNPatchMode(nSegments);
	}
	else
	{
		//BeginRecordingCommands();

		mInternalDeviceState.SetNPatchMode(nSegments);
	}

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
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetPixelShader(pShader);
	}
	else
	{
		//BeginRecordingCommands();

		mInternalDeviceState.SetPixelShader(pShader);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
	}
	else
	{
		BeginRecordingCommands();

		mCurrentDrawCommandBuffer.updateBuffer(mPixelConstantBuffer.get(), sizeof(mInternalDeviceState.mDeviceState.mPixelShaderConstantI) + StartRegister * sizeof(int), BoolCount * sizeof(int), pConstantData);

		mInternalDeviceState.SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantF(UINT StartRegister, const float *pConstantData, UINT Vector4fCount)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
	}
	else
	{
		BeginRecordingCommands();

		mCurrentDrawCommandBuffer.updateBuffer(mPixelConstantBuffer.get(), sizeof(mInternalDeviceState.mDeviceState.mPixelShaderConstantI) + sizeof(mInternalDeviceState.mDeviceState.mPixelShaderConstantB) + StartRegister * sizeof(float[4]), Vector4fCount * sizeof(float[4]), pConstantData);

		mInternalDeviceState.SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetPixelShaderConstantI(UINT StartRegister, const int *pConstantData, UINT Vector4iCount)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
	}
	else
	{
		BeginRecordingCommands();

		mCurrentDrawCommandBuffer.updateBuffer(mPixelConstantBuffer.get(), StartRegister * sizeof(int[4]), Vector4iCount * sizeof(int[4]), pConstantData);

		mInternalDeviceState.SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetRenderState(State, Value);
	}
	else
	{
		BeginRecordingCommands();

		mCurrentDrawCommandBuffer.updateBuffer(mRenderStateBuffer.get(), State * sizeof(DWORD), sizeof(DWORD), &Value);

		mInternalDeviceState.SetRenderState(State, Value);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
	if (mRenderTargets[RenderTargetIndex] != nullptr)
	{
		mRenderTargets[RenderTargetIndex]->PrivateRelease();
	}
	CSurface9* surface = (CSurface9*)pRenderTarget;
	mRenderTargets[RenderTargetIndex] = surface;
	if (mRenderTargets[RenderTargetIndex] != nullptr)
	{
		mRenderTargets[RenderTargetIndex]->PrivateAddRef();
	}

	//Set the viewport to the size of the new render target.
	D3DVIEWPORT9 viewport = {};
	viewport.Width = surface->mWidth;
	viewport.Height = surface->mHeight;
	viewport.MaxZ = 1;
	SetViewport(&viewport);

	RebuildRenderPass();

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetSamplerState(Sampler, Type, Value);
	}
	else
	{
		//BeginRecordingCommands();

		mInternalDeviceState.SetSamplerState(Sampler, Type, Value);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetScissorRect(const RECT* pRect)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetScissorRect(pRect);
	}
	else
	{
		BeginRecordingCommands();

		const vk::Rect2D scissor(vk::Offset2D(pRect->left, pRect->top), vk::Extent2D(pRect->right, pRect->bottom));
		mCurrentDrawCommandBuffer.setScissor(0, 1, &scissor);

		mInternalDeviceState.SetScissorRect(pRect);
	}

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
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
	}
	else
	{
		BeginRecordingCommands();

		mInternalDeviceState.SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetStreamSourceFreq(UINT StreamNumber, UINT FrequencyParameter)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetStreamSourceFreq(StreamNumber, FrequencyParameter);
	}
	else
	{
		//BeginRecordingCommands();

		mInternalDeviceState.SetStreamSourceFreq(StreamNumber, FrequencyParameter);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTexture(DWORD Sampler, IDirect3DBaseTexture9* pTexture)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetTexture(Sampler, pTexture);
	}
	else
	{
		//BeginRecordingCommands();

		mInternalDeviceState.SetTexture(Sampler, pTexture);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetTextureStageState(Stage, Type, Value);
	}
	else
	{
		BeginRecordingCommands();

		mCurrentDrawCommandBuffer.updateBuffer(mTextureStageBuffer.get(), (Stage * (D3DTSS_CONSTANT + 1 + 7)) + (Type * sizeof(DWORD)), sizeof(DWORD), &Value);

		mInternalDeviceState.SetTextureStageState(Stage, Type, Value);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetTransform(State, pMatrix);
	}
	else
	{
		BeginRecordingCommands();

		if (pMatrix)
		{
			const auto columnMajorMatrix = (*pMatrix);

			mCurrentDrawCommandBuffer.updateBuffer(mTransformationBuffer.get(), State * sizeof(D3DMATRIX), sizeof(D3DMATRIX), &columnMajorMatrix);
			mInternalDeviceState.SetTransform(State, pMatrix);
		}
		else
		{
			//For d3d9 NULL really means identity so send that instead.
			const D3DMATRIX identity = { 1, 0, 0, 0,
										 0, 1, 0, 0,
										 0, 0, 1, 0,
										 0, 0, 0, 1 };

			mCurrentDrawCommandBuffer.updateBuffer(mTransformationBuffer.get(), State * sizeof(D3DMATRIX), sizeof(D3DMATRIX), &identity);
			mInternalDeviceState.SetTransform(State, &identity);
		}

		//Update mvp matrix
		//TODO: find out if 0 and 1 are really un-used or just undocumented.
		//TODO: limit matrix multiply to when needs to happen. (Unless branch cost more than savings.)
		const auto mvp =
			mInternalDeviceState.mDeviceState.mTransform[D3DTS_WORLD] *
			mInternalDeviceState.mDeviceState.mTransform[D3DTS_VIEW] *
			mInternalDeviceState.mDeviceState.mTransform[D3DTS_PROJECTION];
		mCurrentDrawCommandBuffer.updateBuffer(mTransformationBuffer.get(), 0, sizeof(D3DMATRIX), &mvp);

		const auto mv =
			mInternalDeviceState.mDeviceState.mTransform[D3DTS_WORLD] *
			mInternalDeviceState.mDeviceState.mTransform[D3DTS_VIEW];
		mCurrentDrawCommandBuffer.updateBuffer(mTransformationBuffer.get(), sizeof(D3DMATRIX), sizeof(D3DMATRIX), &mv);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetVertexDeclaration(pDecl);
	}
	else
	{
		//BeginRecordingCommands();

		mInternalDeviceState.SetVertexDeclaration(pDecl);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShader(IDirect3DVertexShader9* pShader)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetVertexShader(pShader);
	}
	else
	{
		//BeginRecordingCommands();

		mInternalDeviceState.SetVertexShader(pShader);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
	}
	else
	{
		BeginRecordingCommands();

		mCurrentDrawCommandBuffer.updateBuffer(mVertexConstantBuffer.get(), sizeof(mInternalDeviceState.mDeviceState.mVertexShaderConstantI) + StartRegister * sizeof(int), BoolCount * sizeof(int), pConstantData);

		mInternalDeviceState.SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
	}
	else
	{
		BeginRecordingCommands();

		mCurrentDrawCommandBuffer.updateBuffer(mVertexConstantBuffer.get(), sizeof(mInternalDeviceState.mDeviceState.mVertexShaderConstantI) + sizeof(mInternalDeviceState.mDeviceState.mVertexShaderConstantB) + StartRegister * sizeof(float[4]), Vector4fCount * sizeof(float[4]), pConstantData);

		mInternalDeviceState.SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetVertexShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
	}
	else
	{
		BeginRecordingCommands();

		mCurrentDrawCommandBuffer.updateBuffer(mVertexConstantBuffer.get(), StartRegister * sizeof(int[4]), Vector4iCount * sizeof(int[4]), pConstantData);

		mInternalDeviceState.SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::SetViewport(const D3DVIEWPORT9* pViewport)
{
	if (mRecordedDeviceState)
	{
		mRecordedDeviceState->SetViewport(pViewport);
	}
	else
	{
		mInternalDeviceState.SetViewport(pViewport);

		if (mIsRecording)
		{
			/*
			We need to shift by 0.5 because D3D9 is offset by half a pixel.
			It's the difference between measuring from the center versus measuring from the edge.
			https://docs.microsoft.com/en-us/windows/desktop/direct3d9/directly-mapping-texels-to-pixels

			We also need to use the -height and move y (VK_KHR_maintenance1) to handle the handedness of d3d9.
			*/
			const auto viewport = vk::Viewport()
				.setX(static_cast<float>(mInternalDeviceState.mDeviceState.mViewport.X) - 0.5f)
				.setY((static_cast<float>(mInternalDeviceState.mDeviceState.mViewport.Height) - 0.5f) - (static_cast<float>(mInternalDeviceState.mDeviceState.mViewport.Y)))
				.setWidth(static_cast<float>(mInternalDeviceState.mDeviceState.mViewport.Width) - 0.5f)
				.setHeight(-(static_cast<float>(mInternalDeviceState.mDeviceState.mViewport.Height) - 0.5f))
				.setMinDepth(static_cast<float>(mInternalDeviceState.mDeviceState.mViewport.MinZ))
				.setMaxDepth(static_cast<float>(mInternalDeviceState.mDeviceState.mViewport.MaxZ));
			mCurrentDrawCommandBuffer.setViewport(0, 1, &viewport);
		}
		else
		{
			BeginRecordingCommands();
		}
	}

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
	Log(warning) << "CDevice9::StretchRect is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::TestCooperativeLevel()
{
	Log(warning) << "CDevice9::TestCooperativeLevel is not implemented!" << std::endl;

	//if (mCommandStreamManager->mResult == vk::Result::eErrorDeviceLost) { return D3DERR_DEVICELOST; }

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::UpdateSurface(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, const POINT* pDestinationPoint)
{
	Log(warning) << "CDevice9::UpdateSurface is not implemented!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
	Log(warning) << "CDevice9::UpdateTexture is not implemented!" << std::endl;

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
	return mSwapChains[0]->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
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

	CSurface9* ptr = new CSurface9(this, (CTexture9*)nullptr, Width, Height, Usage, 1, Format, MultiSample, MultisampleQuality, false, Lockable, D3DPOOL_DEFAULT, pSharedHandle);

	(*ppSurface) = (IDirect3DSurface9*)ptr;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateOffscreenPlainSurfaceEx(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle, DWORD Usage)
{
	HRESULT result = S_OK;

	CSurface9* ptr = new CSurface9(this, (CTexture9*)nullptr, Width, Height, Usage, 1, Format, D3DMULTISAMPLE_NONE, 0, false, false, Pool, pSharedHandle);

	(*ppSurface) = (IDirect3DSurface9*)ptr;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::CreateDepthStencilSurfaceEx(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle, DWORD Usage)
{
	HRESULT result = S_OK;

	CSurface9* obj = new CSurface9(this, (CTexture9*)nullptr, Width, Height, Usage, 1, Format, MultiSample, MultisampleQuality, Discard, false, D3DPOOL_DEFAULT, pSharedHandle);

	(*ppSurface) = (IDirect3DSurface9*)obj;

	return result;
}

HRESULT STDMETHODCALLTYPE CDevice9::ResetEx(D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode)
{
	if (pPresentationParameters != nullptr)
	{
		if (pPresentationParameters->BackBufferFormat == D3DFMT_UNKNOWN)
		{
			pPresentationParameters->BackBufferFormat = D3DFMT_X8R8G8B8;
		}
		memcpy(&mPresentationParameters, pPresentationParameters, sizeof(D3DPRESENT_PARAMETERS));
	}

	StopRecordingCommands();
	mDevice->waitIdle();
	ResetVulkanDevice();

	Log(warning) << "CDevice9::ResetEx does not handle D3DDISPLAYMODEEX!" << std::endl;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDevice9::GetDisplayModeEx(UINT iSwapChain, D3DDISPLAYMODEEX *pMode, D3DDISPLAYROTATION *pRotation)
{
	Log(warning) << "CDevice9::GetDisplayModeEx is not implemented!" << std::endl;

	(*pRotation) = D3DDISPLAYROTATION_IDENTITY;

	return S_OK;
}

RenderContainer::RenderContainer(vk::Device& device, CSurface9* depthStencilSurface, std::array<CSurface9*, 4>& renderTargets)
	: mDepthStencilSurface(depthStencilSurface),
	mRenderTargets(renderTargets)
{
	std::vector<vk::AttachmentDescription> clearColorAttachments;
	std::vector<vk::AttachmentDescription> clearDepthAttachments;
	std::vector<vk::AttachmentDescription> clearBothAttachments;
	std::vector<vk::AttachmentDescription> attachments;
	std::vector<vk::AttachmentReference> colorReference;
	std::vector<vk::ImageView> frameAttachments;

	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t index = 0;
	for (auto& renderTarget : renderTargets)
	{
		if (renderTarget)
		{
			width = renderTarget->mWidth;
			height = renderTarget->mHeight;

			frameAttachments.push_back(renderTarget->mImageView.get());
			attachments.push_back(vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), ConvertFormat(renderTarget->mFormat), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eColorAttachmentOptimal));

			clearColorAttachments.push_back(vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), ConvertFormat(renderTarget->mFormat), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eColorAttachmentOptimal));
			clearDepthAttachments.push_back(vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), ConvertFormat(renderTarget->mFormat), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eColorAttachmentOptimal));
			clearBothAttachments.push_back(vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), ConvertFormat(renderTarget->mFormat), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eColorAttachmentOptimal));

			colorReference.push_back(vk::AttachmentReference(index, vk::ImageLayout::eColorAttachmentOptimal));

			index += 1;
		}
	}

	if (depthStencilSurface)
	{
		frameAttachments.push_back(depthStencilSurface->mImageView.get());
		attachments.push_back(vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), ConvertFormat(depthStencilSurface->mFormat), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore, vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::ImageLayout::eDepthStencilAttachmentOptimal));

		clearColorAttachments.push_back(vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), ConvertFormat(depthStencilSurface->mFormat), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore, vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::ImageLayout::eDepthStencilAttachmentOptimal));
		clearDepthAttachments.push_back(vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), ConvertFormat(depthStencilSurface->mFormat), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::ImageLayout::eDepthStencilAttachmentOptimal));
		clearBothAttachments.push_back(vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), ConvertFormat(depthStencilSurface->mFormat), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::ImageLayout::eDepthStencilAttachmentOptimal));

		vk::AttachmentReference depthReference(index, vk::ImageLayout::eDepthStencilAttachmentOptimal);

		{
			vk::SubpassDescription subpass(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, 0, nullptr, colorReference.size(), colorReference.data(), nullptr, &depthReference);
			vk::SubpassDependency dependency;
			dependency.srcStageMask = vk::PipelineStageFlagBits::eAllGraphics;
			dependency.dstStageMask = vk::PipelineStageFlagBits::eAllGraphics;
			dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

			mRenderPass = device.createRenderPassUnique(vk::RenderPassCreateInfo(vk::RenderPassCreateFlags(), attachments.size(), attachments.data(), 1, &subpass, 1, &dependency));
		}

		{
			vk::SubpassDescription subpass(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, 0, nullptr, colorReference.size(), colorReference.data(), nullptr, &depthReference);
			vk::SubpassDependency dependency;
			dependency.srcStageMask = vk::PipelineStageFlagBits::eAllGraphics;
			dependency.dstStageMask = vk::PipelineStageFlagBits::eAllGraphics;
			dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

			mClearColorRenderPass = device.createRenderPassUnique(vk::RenderPassCreateInfo(vk::RenderPassCreateFlags(), clearColorAttachments.size(), clearColorAttachments.data(), 1, &subpass, 1, &dependency));
		}

		{
			vk::SubpassDescription subpass(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, 0, nullptr, colorReference.size(), colorReference.data(), nullptr, &depthReference);
			vk::SubpassDependency dependency;
			dependency.srcStageMask = vk::PipelineStageFlagBits::eAllGraphics;
			dependency.dstStageMask = vk::PipelineStageFlagBits::eAllGraphics;
			dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

			mClearDepthRenderPass = device.createRenderPassUnique(vk::RenderPassCreateInfo(vk::RenderPassCreateFlags(), clearDepthAttachments.size(), clearDepthAttachments.data(), 1, &subpass, 1, &dependency));
		}

		{
			vk::SubpassDescription subpass(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, 0, nullptr, colorReference.size(), colorReference.data(), nullptr, &depthReference);
			vk::SubpassDependency dependency;
			dependency.srcStageMask = vk::PipelineStageFlagBits::eAllGraphics;
			dependency.dstStageMask = vk::PipelineStageFlagBits::eAllGraphics;
			dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

			mClearBothRenderPass = device.createRenderPassUnique(vk::RenderPassCreateInfo(vk::RenderPassCreateFlags(), clearBothAttachments.size(), clearBothAttachments.data(), 1, &subpass, 1, &dependency));
		}
	}
	else
	{
		{
			vk::SubpassDescription subpass(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, 0, nullptr, colorReference.size(), colorReference.data(), nullptr, nullptr);
			vk::SubpassDependency dependency;
			dependency.srcStageMask = vk::PipelineStageFlagBits::eAllGraphics;
			dependency.dstStageMask = vk::PipelineStageFlagBits::eAllGraphics;
			dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

			mRenderPass = device.createRenderPassUnique(vk::RenderPassCreateInfo(vk::RenderPassCreateFlags(), attachments.size(), attachments.data(), 1, &subpass, 1, &dependency));
		}

		{
			vk::SubpassDescription subpass(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, 0, nullptr, colorReference.size(), colorReference.data(), nullptr, nullptr);
			vk::SubpassDependency dependency;
			dependency.srcStageMask = vk::PipelineStageFlagBits::eAllGraphics;
			dependency.dstStageMask = vk::PipelineStageFlagBits::eAllGraphics;
			dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

			mClearColorRenderPass = device.createRenderPassUnique(vk::RenderPassCreateInfo(vk::RenderPassCreateFlags(), clearColorAttachments.size(), clearColorAttachments.data(), 1, &subpass, 1, &dependency));
		}

		//Fake depth clear and both clear because apps may still request a clear containing depth even when there is no depth surface.
		{
			vk::SubpassDescription subpass(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, 0, nullptr, colorReference.size(), colorReference.data(), nullptr, nullptr);
			vk::SubpassDependency dependency;
			dependency.srcStageMask = vk::PipelineStageFlagBits::eAllGraphics;
			dependency.dstStageMask = vk::PipelineStageFlagBits::eAllGraphics;
			dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

			mClearDepthRenderPass = device.createRenderPassUnique(vk::RenderPassCreateInfo(vk::RenderPassCreateFlags(), attachments.size(), attachments.data(), 1, &subpass, 1, &dependency));
		}

		{
			vk::SubpassDescription subpass(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, 0, nullptr, colorReference.size(), colorReference.data(), nullptr, nullptr);
			vk::SubpassDependency dependency;
			dependency.srcStageMask = vk::PipelineStageFlagBits::eAllGraphics;
			dependency.dstStageMask = vk::PipelineStageFlagBits::eAllGraphics;
			dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

			mClearBothRenderPass = device.createRenderPassUnique(vk::RenderPassCreateInfo(vk::RenderPassCreateFlags(), clearColorAttachments.size(), clearColorAttachments.data(), 1, &subpass, 1, &dependency));
		}
	}

	vk::FramebufferCreateInfo framebufferCreateInfo;
	framebufferCreateInfo.renderPass = mRenderPass.get();
	framebufferCreateInfo.attachmentCount = frameAttachments.size();
	framebufferCreateInfo.pAttachments = frameAttachments.data();
	framebufferCreateInfo.width = width;
	framebufferCreateInfo.height = height;
	framebufferCreateInfo.layers = 1;

	for (size_t i = 0; i < 3; i++)
	{
		mFrameBuffers.push_back(device.createFramebufferUnique(framebufferCreateInfo, nullptr));
	}

	framebufferCreateInfo.renderPass = mClearColorRenderPass.get();
	for (size_t i = 0; i < 3; i++)
	{
		mClearColorFrameBuffers.push_back(device.createFramebufferUnique(framebufferCreateInfo, nullptr));
	}

	framebufferCreateInfo.renderPass = mClearDepthRenderPass.get();
	for (size_t i = 0; i < 3; i++)
	{
		mClearDepthFrameBuffers.push_back(device.createFramebufferUnique(framebufferCreateInfo, nullptr));
	}

	framebufferCreateInfo.renderPass = mClearBothRenderPass.get();
	for (size_t i = 0; i < 3; i++)
	{
		mClearBothFrameBuffers.push_back(device.createFramebufferUnique(framebufferCreateInfo, nullptr));
	}
}

RenderContainer::~RenderContainer()
{

}

SamplerContainer::SamplerContainer(vk::Device& device, std::array<DWORD, D3DSAMP_DMAPOFFSET + 1>& samplerState)
	: mSamplerState(samplerState)
{
	const vk::SamplerCreateInfo samplerCreateInfo = vk::SamplerCreateInfo()
		.setMagFilter(ConvertFilter((D3DTEXTUREFILTERTYPE)mSamplerState[D3DSAMP_MAGFILTER]))
		.setMinFilter(ConvertFilter((D3DTEXTUREFILTERTYPE)mSamplerState[D3DSAMP_MINFILTER]))
		.setAddressModeU(ConvertTextureAddress((D3DTEXTUREADDRESS)mSamplerState[D3DSAMP_ADDRESSU]))
		.setAddressModeV(ConvertTextureAddress((D3DTEXTUREADDRESS)mSamplerState[D3DSAMP_ADDRESSV]))
		.setAddressModeW(ConvertTextureAddress((D3DTEXTUREADDRESS)mSamplerState[D3DSAMP_ADDRESSW]))
		.setMipmapMode(ConvertMipmapMode((D3DTEXTUREFILTERTYPE)mSamplerState[D3DSAMP_MIPFILTER]))
		.setMipLodBias(bit_cast(mSamplerState[D3DSAMP_MIPMAPLODBIAS]))
		.setBorderColor(vk::BorderColor::eFloatOpaqueWhite)
		.setUnnormalizedCoordinates(VK_FALSE)
		.setCompareOp(vk::CompareOp::eNever)
		.setMinLod(0.0f)
		.setMaxLod((mSamplerState[D3DSAMP_MIPFILTER] == D3DTEXF_NONE) ? 0.0f : bit_cast(mSamplerState[D3DSAMP_MAXMIPLEVEL]));

	//TODO: handle anisotropy

	mSampler = device.createSamplerUnique(samplerCreateInfo);
}

SamplerContainer::~SamplerContainer()
{

}