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
 
#include "CVertexDeclaration9.h"
#include "CDevice9.h"
#include "LogManager.h"
//#include "PrivateTypes.h"

static uint32_t UsageOffsets[14] =
{
	0, //D3DDECLUSAGE_POSITION 0-1 (1 is for tweening)
	2, //D3DDECLUSAGE_BLENDWEIGHT 2-4
	5, //D3DDECLUSAGE_BLENDINDICES 5-7
	8, //D3DDECLUSAGE_NORMAL 8-9
	10, //D3DDECLUSAGE_PSIZE 10
	11, //D3DDECLUSAGE_TEXCOORD 11-26
	27, //D3DDECLUSAGE_TANGENT 27
	28, //D3DDECLUSAGE_BINORMAL 28
	29, //D3DDECLUSAGE_TESSFACTOR 29
	0, //D3DDECLUSAGE_POSITIONT 0 (same as POSITION)
	30, //D3DDECLUSAGE_COLOR 30-31 
	/*Re-use the last three TEXCOORD slots for Fog, Depth, and Sample because 32 is max.*/
	24, //D3DDECLUSAGE_FOG 
	25, //D3DDECLUSAGE_DEPTH
	26 //D3DDECLUSAGE_SAMPLE
};

vk::Format ConvertDeclType(D3DDECLTYPE input) noexcept
{
	vk::Format output = (vk::Format)VK_FORMAT_UNDEFINED;
	//TODO: finish mapping.
	switch (input)
	{
	case D3DDECLTYPE_FLOAT1: // 1D float expanded to (value, 0., 0., 1.)
		output = (vk::Format)VK_FORMAT_R32_SFLOAT;
		break;
	case D3DDECLTYPE_FLOAT2:  // 2D float expanded to (value, value, 0., 1.)
		output = (vk::Format)VK_FORMAT_R32G32_SFLOAT;
		break;
	case D3DDECLTYPE_FLOAT3: // 3D float expanded to (value, value, value, 1.)
		output = (vk::Format)VK_FORMAT_R32G32B32_SFLOAT;
		break;
	case D3DDECLTYPE_FLOAT4:  // 4D float
		output = (vk::Format)VK_FORMAT_R32G32B32A32_SFLOAT;
		break;
	case D3DDECLTYPE_D3DCOLOR:  // 4D packed unsigned bytes mapped to 0. to 1. range
		output = (vk::Format)VK_FORMAT_B8G8R8A8_UINT;
		break; // Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
	case D3DDECLTYPE_UBYTE4: // 4D unsigned byte
		output = (vk::Format)VK_FORMAT_R8G8B8A8_UINT;
		break;
	case D3DDECLTYPE_SHORT2:  // 2D signed short expanded to (value, value, 0., 1.)
		output = (vk::Format)VK_FORMAT_R16G16_SINT;
		break;
	case D3DDECLTYPE_SHORT4:  // 4D signed short
		output = (vk::Format)VK_FORMAT_R16G16B16A16_SINT;
		break;
	case D3DDECLTYPE_UBYTE4N:  // Each of 4 bytes is normalized by dividing to 255.0
		output = (vk::Format)VK_FORMAT_R8G8B8A8_UNORM;
		break;
	case D3DDECLTYPE_SHORT2N: // 2D signed short normalized (v[0]/32767.0,v[1]/32767.0,0,1)
		output = (vk::Format)VK_FORMAT_R16G16_SNORM;
		break;
	case D3DDECLTYPE_SHORT4N:  // 4D signed short normalized (v[0]/32767.0,v[1]/32767.0,v[2]/32767.0,v[3]/32767.0)
		output = (vk::Format)VK_FORMAT_R16G16B16A16_SNORM;
		break;
	case D3DDECLTYPE_USHORT2N:  // 2D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,0,1)
		output = (vk::Format)VK_FORMAT_R16G16_UNORM;
		break;
	case D3DDECLTYPE_USHORT4N:  // 4D unsigned short normalized (v[0]/65535.0,v[1]/65535.0,v[2]/65535.0,v[3]/65535.0)
		output = (vk::Format)VK_FORMAT_R16G16B16A16_UNORM;
		break;
	case D3DDECLTYPE_UDEC3:  // 3D unsigned 10 10 10 format expanded to (value, value, value, 1)
		//The only 10 10 10 formats I see have four components not 3.
		break;
	case D3DDECLTYPE_DEC3N:  // 3D signed 10 10 10 format normalized and expanded to (v[0]/511.0, v[1]/511.0, v[2]/511.0, 1)
		//The only 10 10 10 formats I see have four components not 3.
		break;
	case D3DDECLTYPE_FLOAT16_2:  // Two 16-bit floating point values, expanded to (value, value, 0, 1)
		output = (vk::Format)VK_FORMAT_R16G16_SFLOAT;
		break;
	case D3DDECLTYPE_FLOAT16_4:  // Four 16-bit floating point values
		output = (vk::Format)VK_FORMAT_R16G16B16A16_SFLOAT;
		break;
	case D3DDECLTYPE_UNUSED:  // When the type field in a decl is unused.
		output = (vk::Format)VK_FORMAT_UNDEFINED;
		break;
	default:
		break;
	}

	return output;
}

uint32_t GetTextureCount(DWORD fvf) noexcept
{
	//TODO: This should be able to be simplified by bitwise operators.

	if ((fvf & D3DFVF_TEX8) == D3DFVF_TEX8)
	{
		return 8;
	}

	if ((fvf & D3DFVF_TEX7) == D3DFVF_TEX7)
	{
		return 7;
	}

	if ((fvf & D3DFVF_TEX6) == D3DFVF_TEX6)
	{
		return 6;
	}

	if ((fvf & D3DFVF_TEX5) == D3DFVF_TEX5)
	{
		return 5;
	}

	if ((fvf & D3DFVF_TEX4) == D3DFVF_TEX4)
	{
		return 4;
	}

	if ((fvf & D3DFVF_TEX3) == D3DFVF_TEX3)
	{
		return 3;
	}

	if ((fvf & D3DFVF_TEX2) == D3DFVF_TEX2)
	{
		return 2;
	}

	if ((fvf & D3DFVF_TEX1) == D3DFVF_TEX1)
	{
		return 1;
	}

	return 0;
}

CVertexDeclaration9::CVertexDeclaration9(CDevice9* device, const D3DVERTEXELEMENT9* pVertexElements)
	: mDevice(device)
{
	int32_t i = 0;

	while (pVertexElements[i].Stream != 0xFF)
	{
		mVertexElements.push_back(pVertexElements[i]);

		mVertexInputAttributeDescription.push_back(vk::VertexInputAttributeDescription(
			UsageOffsets[pVertexElements[i].Usage] + pVertexElements[i].UsageIndex, 
			pVertexElements[i].Stream, 
			ConvertDeclType((D3DDECLTYPE)pVertexElements[i].Type), 
			pVertexElements[i].Offset));

		switch(pVertexElements[i].Usage)
		{
		case D3DDECLUSAGE_POSITION:
			mHasPosition = true;
			break;
		case D3DDECLUSAGE_BLENDWEIGHT:
			mHasBlendWeight = true;
			break;
		case D3DDECLUSAGE_BLENDINDICES:
			mHasBlendIndices = true;
			break;
		case D3DDECLUSAGE_NORMAL:
			mHasNormal = true;
			break;
		case D3DDECLUSAGE_PSIZE:
			mHasPSize = true;
			break;
		case D3DDECLUSAGE_TEXCOORD:
			mTextureCount++;
			break;
		case D3DDECLUSAGE_TANGENT:
			mHasTangent = true;
			break;
		case D3DDECLUSAGE_BINORMAL:
			mHasBinormal = true;
			break;
		case D3DDECLUSAGE_TESSFACTOR:
			mHasTessfactor = true;
			break;
		case D3DDECLUSAGE_POSITIONT:
			mHasPositionT = true;
			break;
		case D3DDECLUSAGE_COLOR:
			if (pVertexElements[i].UsageIndex)
			{
				mHasColor2 = true;
			}
			else
			{
				mHasColor1 = true;
			}		
			break;
		case D3DDECLUSAGE_FOG:
			mHasFog = true;
			break;
		case D3DDECLUSAGE_DEPTH:
			mHasDepth = true;
			break;
		case D3DDECLUSAGE_SAMPLE:
			mHasSample = true;
			break;
		default:
			Log(fatal) << "CVertexDeclaration9::CVertexDeclaration9 unknown vertex usage " << (uint32_t)pVertexElements[i].Usage << std::endl;
			break;
		}

		i++;
	}

	auto test = 0;
}

CVertexDeclaration9::CVertexDeclaration9(CDevice9* device, DWORD fvf)
	: mDevice(device),
	mFVF(fvf)
{
	if ((mFVF & D3DFVF_XYZRHW) == D3DFVF_XYZRHW)
	{
		mPositionSize = 4;
		mHasPosition = true;
		mIsTransformed = true;
	}
	else
	{
		if ((mFVF & D3DFVF_XYZW) == D3DFVF_XYZW)
		{
			mPositionSize = 4;
			mHasPosition = true;
		}
		else if ((mFVF & D3DFVF_XYZ) == D3DFVF_XYZ)
		{
			mPositionSize = 3;
			mHasPosition = true;
		}
		else if ((mFVF & D3DFVF_XYZB1) == D3DFVF_XYZB1)
		{
			mPositionSize = 3;
			mHasPosition = true;
			mHasColor1 = true;
		}
		else if ((mFVF & D3DFVF_XYZB2) == D3DFVF_XYZB2)
		{
			mPositionSize = 3;
			mHasPosition = true;
			mHasColor1 = true;
		}
		else if ((mFVF & D3DFVF_XYZB3) == D3DFVF_XYZB3)
		{
			mPositionSize = 3;
			mHasPosition = true;
			mHasColor1 = true;
		}
		else if ((mFVF & D3DFVF_XYZB4) == D3DFVF_XYZB4)
		{
			mPositionSize = 3;
			mHasPosition = true;
			mHasColor1 = true;
		}
		else if ((mFVF & D3DFVF_XYZB5) == D3DFVF_XYZB5)
		{
			mPositionSize = 3;
			mHasPosition = true;
			mHasColor1 = true;
		}

		if ((mFVF & D3DFVF_NORMAL) == D3DFVF_NORMAL)
		{
			mHasNormal = true;
		}
	}

	if ((mFVF & D3DFVF_PSIZE) == D3DFVF_PSIZE)
	{
		mHasPSize = true;
	}

	if ((mFVF & D3DFVF_DIFFUSE) == D3DFVF_DIFFUSE)
	{
		mHasColor1 = true;
	}

	if ((mFVF & D3DFVF_SPECULAR) == D3DFVF_SPECULAR)
	{
		mHasColor2 = true;
	}
	mTextureCount = GetTextureCount(mFVF);

	//Figure out attributes.
	uint32_t offset = 0;

	if (mHasPosition)
	{
		mVertexInputAttributeDescription.push_back(vk::VertexInputAttributeDescription(
			UsageOffsets[D3DDECLUSAGE_POSITION],
			0,
			vk::Format::eR32G32B32Sfloat,
			offset));

		offset += (sizeof(float) * mPositionSize);
	}

	if (mHasNormal)
	{
		mVertexInputAttributeDescription.push_back(vk::VertexInputAttributeDescription(
			UsageOffsets[D3DDECLUSAGE_NORMAL],
			0,
			vk::Format::eR32G32B32Sfloat,
			offset));

		offset += (sizeof(float) * 3);
	}

	if (mHasPSize)
	{
		mVertexInputAttributeDescription.push_back(vk::VertexInputAttributeDescription(
			UsageOffsets[D3DDECLUSAGE_PSIZE],
			0,
			vk::Format::eR32G32B32Sfloat,
			offset));

		offset += sizeof(float);
	}

	if (mHasColor1)
	{
		mVertexInputAttributeDescription.push_back(vk::VertexInputAttributeDescription(
			UsageOffsets[D3DDECLUSAGE_COLOR],
			0,
			vk::Format::eB8G8R8A8Uint,
			offset));

		offset += sizeof(uint32_t);
	}

	if (mHasColor2)
	{
		mVertexInputAttributeDescription.push_back(vk::VertexInputAttributeDescription(
			UsageOffsets[D3DDECLUSAGE_COLOR] + 1,
			0,
			vk::Format::eB8G8R8A8Uint,
			offset));

		offset += sizeof(uint32_t);
	}

	for (size_t j = 0; j < mTextureCount; j++)
	{
		mVertexInputAttributeDescription.push_back(vk::VertexInputAttributeDescription(
			UsageOffsets[D3DDECLUSAGE_TEXCOORD] + j,
			0,
			vk::Format::eR32G32Sfloat,
			offset));

		offset += (sizeof(float) * 2);
	}
}

CVertexDeclaration9::~CVertexDeclaration9()
{

}

ULONG CVertexDeclaration9::PrivateAddRef(void)
{
	return InterlockedIncrement(&mPrivateReferenceCount);
}

ULONG CVertexDeclaration9::PrivateRelease(void)
{
	ULONG ref = InterlockedDecrement(&mPrivateReferenceCount);

	if (ref == 0 && mReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

ULONG STDMETHODCALLTYPE CVertexDeclaration9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CVertexDeclaration9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DVertexDeclaration9))
	{
		(*ppv) = this;
		this->AddRef();
		return S_OK;
	}

	if (IsEqualGUID(riid, IID_IDirect3DResource9))
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

ULONG STDMETHODCALLTYPE CVertexDeclaration9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0 && mPrivateReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CVertexDeclaration9::GetDeclaration(D3DVERTEXELEMENT9* pDecl, UINT* pNumElements)
{
	(*pNumElements) = mVertexElements.size() + 1; //+1 for the terminator.

	if (pDecl!=nullptr) //If null only return the count so the caller can use it to construct an array of the correct size.
	{
		for (size_t i = 0; i < mVertexElements.size(); i++)
		{
			pDecl[i] = mVertexElements[i];
		}
		pDecl[mVertexElements.size()] = D3DDECL_END(); //not a mistake raw D3DVERTEXELEMENT9 arrays contain a terminator but I don't store it because all I really need is a count.
	}

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CVertexDeclaration9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return S_OK; 
}