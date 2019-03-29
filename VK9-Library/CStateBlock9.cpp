// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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

#include "CStateBlock9.h"
#include "LogManager.h"

#include "CDevice9.h"
#include "CSurface9.h"
#include "CIndexBuffer9.h"
#include "CVertexBuffer9.h"
#include "CPixelShader9.h"
#include "CVertexShader9.h"
#include "CVertexDeclaration9.h"
#include "CSwapChain9.h"

#include "CTexture9.h"
#include "CVolumeTexture9.h"
#include "CCubeTexture9.h"

CStateBlock9::CStateBlock9(CDevice9* device, D3DSTATEBLOCKTYPE Type)
	: mDevice(device),
	mType(Type)
{
	if (mType == D3DSBT_PIXELSTATE || mType == D3DSBT_ALL)
	{
		CapturePixelRenderStates();
		CapturePixelTextureStates();
		CapturePixelSamplerStates();
		CapturePixelShaderStates();
	}

	if (mType == D3DSBT_VERTEXSTATE || mType == D3DSBT_ALL)
	{
		CaptureVertexRenderStates();
		CaptureVertexSamplerStates();
		CaptureVertexTextureStates();
		CaptureNPatchMode();
		CaptureLightStates();
		CaptureVertexShaderStates();
		CaptureStreamSourceFrequencies();
		CaptureFVF();
		CaptureVertexDeclaration();
	}

	if (mType == D3DSBT_ALL)   // Capture remaining states
	{
		CaptureTextures();
		CaptureTexturePalette();
		CaptureVertexStreams();
		CaptureIndexBuffer();
		CaptureViewport();
		CaptureScissorRectangle();
		CaptureTransforms();
		CaptureTextureTransforms();
		CaptureClippingPlanes();
		CaptureMaterial();
	}
}

CStateBlock9::~CStateBlock9()
{
	//Free private references

	if (mDeviceState.mVertexDeclaration)
	{
		mDeviceState.mVertexDeclaration->PrivateRelease();
	}

	if (mDeviceState.mIndexBuffer)
	{
		mDeviceState.mIndexBuffer->PrivateRelease();
	}

	for (int stream = 0; stream < MAX_VERTEX_INPUTS; stream++)
	{
		if (mDeviceState.mStreamSource[stream].vertexBuffer)
		{
			mDeviceState.mStreamSource[stream].vertexBuffer->PrivateRelease();
		}
	}

	for (int sampler = 0; sampler < 16 + 4; sampler++)
	{
		if (mDeviceState.mTexture[sampler])
		{
			switch (mDeviceState.mTexture[sampler]->GetType())
			{
			case D3DRTYPE_TEXTURE:
				reinterpret_cast <CTexture9*>(mDeviceState.mTexture[sampler])->PrivateRelease();
				break;
			case D3DRTYPE_VOLUMETEXTURE:
				reinterpret_cast <CVolumeTexture9*>(mDeviceState.mTexture[sampler])->PrivateRelease();
				break;
			case D3DRTYPE_CUBETEXTURE:
				reinterpret_cast <CCubeTexture9*>(mDeviceState.mTexture[sampler])->PrivateRelease();
				break;
			}		
		}
	}

	if (mDeviceState.mPixelShader)
	{
		mDeviceState.mPixelShader->PrivateRelease();
	}

	if (mDeviceState.mVertexShader)
	{
		mDeviceState.mVertexShader->PrivateRelease();
	}
}

ULONG CStateBlock9::PrivateAddRef(void)
{
	return InterlockedIncrement(&mPrivateReferenceCount);
}

ULONG CStateBlock9::PrivateRelease(void)
{
	ULONG ref = InterlockedDecrement(&mPrivateReferenceCount);

	if (ref == 0 && mReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

void CStateBlock9::LightEnable(unsigned long index, int enable)
{
	if (index < 8)
	{
		mDeviceState.mCapturedLightEnable[index] = true;
		mDeviceState.mLightEnableState[index] = enable;
	}
}

void CStateBlock9::SetClipPlane(unsigned long index, const float* plane)
{
	mDeviceState.mCapturedClipPlane[index] = true;
	mDeviceState.mClipPlane[index][0] = plane[0];
	mDeviceState.mClipPlane[index][1] = plane[1];
	mDeviceState.mClipPlane[index][2] = plane[2];
	mDeviceState.mClipPlane[index][3] = plane[3];
}

void CStateBlock9::SetCurrentTexturePalette(unsigned int paletteNumber)
{
	mDeviceState.mCapturedPaletteNumber = true;
	mDeviceState.mPaletteNumber = paletteNumber;
}

void CStateBlock9::SetFVF(unsigned long FVF)
{
	mDeviceState.mCapturedFVF = true;
	mDeviceState.mFVF = FVF;
}

void CStateBlock9::SetIndices(IDirect3DIndexBuffer9* indexBuffer)
{
	if (indexBuffer)
	{
		reinterpret_cast <CIndexBuffer9*>(indexBuffer)->PrivateAddRef();
	}
	if (mDeviceState.mIndexBuffer)
	{
		mDeviceState.mIndexBuffer->PrivateRelease();
	}

	mDeviceState.mCapturedIndexBuffer = true;
	mDeviceState.mIndexBuffer = reinterpret_cast <CIndexBuffer9*>(indexBuffer);
}

void CStateBlock9::SetLight(unsigned long index, const D3DLIGHT9* light)
{
	if (index < 8)
	{
		mDeviceState.mCapturedLight[index] = true;
		mDeviceState.mLight[index] = *light;
	}
}

void CStateBlock9::SetMaterial(const D3DMATERIAL9* material)
{
	mDeviceState.mCapturedMaterial = true;
	mDeviceState.mMaterial = *material;
}

void CStateBlock9::SetNPatchMode(float segments)
{
	mDeviceState.mCapturedNPatchMode = true;
	mDeviceState.mNPatchMode = segments;
}

void CStateBlock9::SetPixelShader(IDirect3DPixelShader9* pixelShader)
{
	if (pixelShader)
	{
		reinterpret_cast <CPixelShader9*>(pixelShader)->PrivateAddRef();
	}
	if (mDeviceState.mPixelShader)
	{
		mDeviceState.mPixelShader->PrivateRelease();
	}

	mDeviceState.mCapturedPixelShader = true;
	mDeviceState.mPixelShader = reinterpret_cast <CPixelShader9*>(pixelShader);
}

void CStateBlock9::SetPixelShaderConstantB(unsigned int startRegister, const int* constantData, unsigned int count)
{
	memcpy(&mDeviceState.mPixelShaderConstantB[startRegister], constantData, count * sizeof(int));
}

void CStateBlock9::SetPixelShaderConstantF(unsigned int startRegister, const float* constantData, unsigned int count)
{
	memcpy(&mDeviceState.mPixelShaderConstantF[startRegister], constantData, count * sizeof(float[4]));
}

void CStateBlock9::SetPixelShaderConstantI(unsigned int startRegister, const int* constantData, unsigned int count)
{
	memcpy(&mDeviceState.mPixelShaderConstantI[startRegister], constantData, count * sizeof(int[4]));
}

void CStateBlock9::SetRenderState(D3DRENDERSTATETYPE state, unsigned long value)
{
	mDeviceState.mCapturedRenderState[state] = true;
	mDeviceState.mRenderState[state] = value;
}

void CStateBlock9::SetSamplerState(unsigned long index, D3DSAMPLERSTATETYPE state, unsigned long value)
{
	unsigned int sampler = index < 16 ? index : (index - D3DVERTEXTEXTURESAMPLER0) + 16;

	if (sampler >= 16 + 4)
	{
		return;
	}

	mDeviceState.mCapturedSamplerState[sampler][state] = true;
	mDeviceState.mSamplerState[sampler][state] = value;
}

void CStateBlock9::SetScissorRect(const RECT* rect)
{
	mDeviceState.mCapturedScissorRect = true;
	mDeviceState.mScissorRect = *rect;
}

void CStateBlock9::SetStreamSource(unsigned int stream, IDirect3DVertexBuffer9* vertexBuffer, unsigned int offset, unsigned int stride)
{
	if (vertexBuffer)
	{
		reinterpret_cast <CVertexBuffer9*>(vertexBuffer)->PrivateAddRef();
	}
	if (mDeviceState.mStreamSource[stream].vertexBuffer)
	{
		mDeviceState.mStreamSource[stream].vertexBuffer->PrivateRelease();
	}

	mDeviceState.mCapturedStreamSource[stream] = true;
	mDeviceState.mStreamSource[stream].vertexBuffer = reinterpret_cast <CVertexBuffer9*>(vertexBuffer);
	mDeviceState.mStreamSource[stream].offset = offset;
	mDeviceState.mStreamSource[stream].stride = stride;
}

void CStateBlock9::SetStreamSourceFreq(unsigned int streamNumber, unsigned int divider)
{
	mDeviceState.mCapturedStreamSourceFrequency[streamNumber] = true;
	mDeviceState.mStreamSourceFrequency[streamNumber] = divider;
}

void CStateBlock9::SetTexture(unsigned long index, IDirect3DBaseTexture9* texture)
{
	unsigned int sampler = index < 16 ? index : (index - D3DVERTEXTEXTURESAMPLER0) + 16;

	if (sampler >= 16 + 4)
	{
		return;
	}

	if (texture)
	{
		switch (texture->GetType())
		{
		case D3DRTYPE_TEXTURE:
			reinterpret_cast <CTexture9*>(texture)->PrivateAddRef();
			break;
		case D3DRTYPE_VOLUMETEXTURE:
			reinterpret_cast <CVolumeTexture9*>(texture)->PrivateAddRef();
			break;
		case D3DRTYPE_CUBETEXTURE:
			reinterpret_cast <CCubeTexture9*>(texture)->PrivateAddRef();
			break;
		}
	}
	if (mDeviceState.mTexture[sampler])
	{
		switch (mDeviceState.mTexture[sampler]->GetType())
		{
		case D3DRTYPE_TEXTURE:
			reinterpret_cast <CTexture9*>(mDeviceState.mTexture[sampler])->PrivateRelease();
			break;
		case D3DRTYPE_VOLUMETEXTURE:
			reinterpret_cast <CVolumeTexture9*>(mDeviceState.mTexture[sampler])->PrivateRelease();
			break;
		case D3DRTYPE_CUBETEXTURE:
			reinterpret_cast <CCubeTexture9*>(mDeviceState.mTexture[sampler])->PrivateRelease();
			break;
		}
	}

	mDeviceState.mCapturedTexture[sampler] = true;
	mDeviceState.mTexture[sampler] = texture;
}

void CStateBlock9::SetTextureStageState(unsigned long stage, D3DTEXTURESTAGESTATETYPE type, unsigned long value)
{
	mDeviceState.mCapturedTextureStageState[stage][type] = true;
	mDeviceState.mTextureStageState[stage][type] = value;
}

void CStateBlock9::SetTransform(D3DTRANSFORMSTATETYPE state, const D3DMATRIX* matrix)
{
	mDeviceState.mCapturedTransform[state] = true;
	mDeviceState.mTransform[state] = *matrix;
}

void CStateBlock9::SetViewport(const D3DVIEWPORT9* viewport)
{
	mDeviceState.mCapturedViewport = true;
	mDeviceState.mViewport = *viewport;
}

void CStateBlock9::SetVertexDeclaration(IDirect3DVertexDeclaration9* vertexDeclaration)
{
	if (vertexDeclaration)
	{
		reinterpret_cast <CVertexDeclaration9*>(vertexDeclaration)->PrivateAddRef();
	}
	if (mDeviceState.mVertexDeclaration)
	{
		mDeviceState.mVertexDeclaration->PrivateRelease();
	}

	mDeviceState.mCapturedVertexDeclaration = true;
	mDeviceState.mVertexDeclaration = reinterpret_cast <CVertexDeclaration9*>(vertexDeclaration);
}

void CStateBlock9::SetVertexShader(IDirect3DVertexShader9* vertexShader)
{
	if (vertexShader)
	{
		reinterpret_cast <CVertexShader9*>(vertexShader)->PrivateAddRef();
	}
	if (mDeviceState.mVertexShader)
	{
		mDeviceState.mVertexShader->PrivateRelease();
	}

	mDeviceState.mCapturedVertexShader = true;
	mDeviceState.mVertexShader = reinterpret_cast <CVertexShader9*>(vertexShader);
}

void CStateBlock9::SetVertexShaderConstantB(unsigned int startRegister, const int* constantData, unsigned int count)
{
	memcpy(&mDeviceState.mVertexShaderConstantB[startRegister], constantData, count * sizeof(int));
}

void CStateBlock9::SetVertexShaderConstantF(unsigned int startRegister, const float* constantData, unsigned int count)
{
	memcpy(&mDeviceState.mVertexShaderConstantF[startRegister], constantData, count * sizeof(float[4]));
}

void CStateBlock9::SetVertexShaderConstantI(unsigned int startRegister, const int* constantData, unsigned int count)
{
	memcpy(&mDeviceState.mVertexShaderConstantI[startRegister], constantData, count * sizeof(int[4]));
}

void CStateBlock9::CaptureRenderState(D3DRENDERSTATETYPE state)
{
	mDevice->GetRenderState(state, &mDeviceState.mRenderState[state]);
	mDeviceState.mCapturedRenderState[state] = true;
}

void CStateBlock9::CaptureSamplerState(unsigned long index, D3DSAMPLERSTATETYPE state)
{
	if (index < 16)
	{
		mDevice->GetSamplerState(index, state, &mDeviceState.mSamplerState[index][state]);
		mDeviceState.mCapturedSamplerState[index][state] = true;
	}
	else if (index >= D3DVERTEXTEXTURESAMPLER0)
	{
		unsigned int sampler = 16 + (index - D3DVERTEXTEXTURESAMPLER0);

		mDevice->GetSamplerState(index, state, &mDeviceState.mSamplerState[sampler][state]);
		mDeviceState.mCapturedSamplerState[sampler][state] = true;
	}
}

void CStateBlock9::CaptureTextureStageState(unsigned long stage, D3DTEXTURESTAGESTATETYPE type)
{
	mDevice->GetTextureStageState(stage, type, &mDeviceState.mTextureStageState[stage][type]);
	mDeviceState.mCapturedTextureStageState[stage][type] = true;
}

void CStateBlock9::CaptureTransform(D3DTRANSFORMSTATETYPE state)
{
	mDevice->GetTransform(state, &mDeviceState.mTransform[state]);
	mDeviceState.mCapturedTransform[state] = true;
}

void CStateBlock9::CapturePixelRenderStates()
{
	CaptureRenderState(D3DRS_ZENABLE);
	CaptureRenderState(D3DRS_FILLMODE);
	CaptureRenderState(D3DRS_SHADEMODE);
	CaptureRenderState(D3DRS_ZWRITEENABLE);
	CaptureRenderState(D3DRS_ALPHATESTENABLE);
	CaptureRenderState(D3DRS_LASTPIXEL);
	CaptureRenderState(D3DRS_SRCBLEND);
	CaptureRenderState(D3DRS_DESTBLEND);
	CaptureRenderState(D3DRS_ZFUNC);
	CaptureRenderState(D3DRS_ALPHAREF);
	CaptureRenderState(D3DRS_ALPHAFUNC);
	CaptureRenderState(D3DRS_DITHERENABLE);
	CaptureRenderState(D3DRS_FOGSTART);
	CaptureRenderState(D3DRS_FOGEND);
	CaptureRenderState(D3DRS_FOGDENSITY);
	CaptureRenderState(D3DRS_ALPHABLENDENABLE);
	CaptureRenderState(D3DRS_DEPTHBIAS);
	CaptureRenderState(D3DRS_STENCILENABLE);
	CaptureRenderState(D3DRS_STENCILFAIL);
	CaptureRenderState(D3DRS_STENCILZFAIL);
	CaptureRenderState(D3DRS_STENCILPASS);
	CaptureRenderState(D3DRS_STENCILFUNC);
	CaptureRenderState(D3DRS_STENCILREF);
	CaptureRenderState(D3DRS_STENCILMASK);
	CaptureRenderState(D3DRS_STENCILWRITEMASK);
	CaptureRenderState(D3DRS_TEXTUREFACTOR);
	CaptureRenderState(D3DRS_WRAP0);
	CaptureRenderState(D3DRS_WRAP1);
	CaptureRenderState(D3DRS_WRAP2);
	CaptureRenderState(D3DRS_WRAP3);
	CaptureRenderState(D3DRS_WRAP4);
	CaptureRenderState(D3DRS_WRAP5);
	CaptureRenderState(D3DRS_WRAP6);
	CaptureRenderState(D3DRS_WRAP7);
	CaptureRenderState(D3DRS_WRAP8);
	CaptureRenderState(D3DRS_WRAP9);
	CaptureRenderState(D3DRS_WRAP10);
	CaptureRenderState(D3DRS_WRAP11);
	CaptureRenderState(D3DRS_WRAP12);
	CaptureRenderState(D3DRS_WRAP13);
	CaptureRenderState(D3DRS_WRAP14);
	CaptureRenderState(D3DRS_WRAP15);
	CaptureRenderState(D3DRS_COLORWRITEENABLE);
	CaptureRenderState(D3DRS_BLENDOP);
	CaptureRenderState(D3DRS_SCISSORTESTENABLE);
	CaptureRenderState(D3DRS_SLOPESCALEDEPTHBIAS);
	CaptureRenderState(D3DRS_ANTIALIASEDLINEENABLE);
	CaptureRenderState(D3DRS_TWOSIDEDSTENCILMODE);
	CaptureRenderState(D3DRS_CCW_STENCILFAIL);
	CaptureRenderState(D3DRS_CCW_STENCILZFAIL);
	CaptureRenderState(D3DRS_CCW_STENCILPASS);
	CaptureRenderState(D3DRS_CCW_STENCILFUNC);
	CaptureRenderState(D3DRS_COLORWRITEENABLE1);
	CaptureRenderState(D3DRS_COLORWRITEENABLE2);
	CaptureRenderState(D3DRS_COLORWRITEENABLE3);
	CaptureRenderState(D3DRS_BLENDFACTOR);
	CaptureRenderState(D3DRS_SRGBWRITEENABLE);
	CaptureRenderState(D3DRS_SEPARATEALPHABLENDENABLE);
	CaptureRenderState(D3DRS_SRCBLENDALPHA);
	CaptureRenderState(D3DRS_DESTBLENDALPHA);
	CaptureRenderState(D3DRS_BLENDOPALPHA);
}

void CStateBlock9::CapturePixelTextureStates()
{
	for (int stage = 0; stage < 8; stage++)
	{
		CaptureTextureStageState(stage, D3DTSS_COLOROP);
		CaptureTextureStageState(stage, D3DTSS_COLORARG1);
		CaptureTextureStageState(stage, D3DTSS_COLORARG2);
		CaptureTextureStageState(stage, D3DTSS_ALPHAOP);
		CaptureTextureStageState(stage, D3DTSS_ALPHAARG1);
		CaptureTextureStageState(stage, D3DTSS_ALPHAARG2);
		CaptureTextureStageState(stage, D3DTSS_BUMPENVMAT00);
		CaptureTextureStageState(stage, D3DTSS_BUMPENVMAT01);
		CaptureTextureStageState(stage, D3DTSS_BUMPENVMAT10);
		CaptureTextureStageState(stage, D3DTSS_BUMPENVMAT11);
		CaptureTextureStageState(stage, D3DTSS_TEXCOORDINDEX);
		CaptureTextureStageState(stage, D3DTSS_BUMPENVLSCALE);
		CaptureTextureStageState(stage, D3DTSS_BUMPENVLOFFSET);
		CaptureTextureStageState(stage, D3DTSS_TEXTURETRANSFORMFLAGS);
		CaptureTextureStageState(stage, D3DTSS_COLORARG0);
		CaptureTextureStageState(stage, D3DTSS_ALPHAARG0);
		CaptureTextureStageState(stage, D3DTSS_RESULTARG);
	}
}

void CStateBlock9::CapturePixelSamplerStates()
{
	for (int sampler = 0; sampler <= D3DVERTEXTEXTURESAMPLER3; sampler++)
	{
		CaptureSamplerState(sampler, D3DSAMP_ADDRESSU);
		CaptureSamplerState(sampler, D3DSAMP_ADDRESSV);
		CaptureSamplerState(sampler, D3DSAMP_ADDRESSW);
		CaptureSamplerState(sampler, D3DSAMP_BORDERCOLOR);
		CaptureSamplerState(sampler, D3DSAMP_MAGFILTER);
		CaptureSamplerState(sampler, D3DSAMP_MINFILTER);
		CaptureSamplerState(sampler, D3DSAMP_MIPFILTER);
		CaptureSamplerState(sampler, D3DSAMP_MIPMAPLODBIAS);
		CaptureSamplerState(sampler, D3DSAMP_MAXMIPLEVEL);
		CaptureSamplerState(sampler, D3DSAMP_MAXANISOTROPY);
		CaptureSamplerState(sampler, D3DSAMP_SRGBTEXTURE);
		CaptureSamplerState(sampler, D3DSAMP_ELEMENTINDEX);
	}
}

void CStateBlock9::CapturePixelShaderStates()
{
	mDeviceState.mCapturedPixelShader = true;
	mDevice->GetPixelShader(reinterpret_cast<IDirect3DPixelShader9**>(&mDeviceState.mPixelShader));

	//if (mPixelShader)
	//{
	//	mPixelShader->PrivateAddRef();
	//}

	mDevice->GetPixelShaderConstantF(0, mDeviceState.mPixelShaderConstantF[0], 32);
	mDevice->GetPixelShaderConstantI(0, mDeviceState.mPixelShaderConstantI[0], 16);
	mDevice->GetPixelShaderConstantB(0, mDeviceState.mPixelShaderConstantB, 16);
}

void CStateBlock9::CaptureVertexRenderStates()
{
	CaptureRenderState(D3DRS_CULLMODE);
	CaptureRenderState(D3DRS_FOGENABLE);
	CaptureRenderState(D3DRS_FOGCOLOR);
	CaptureRenderState(D3DRS_FOGTABLEMODE);
	CaptureRenderState(D3DRS_FOGSTART);
	CaptureRenderState(D3DRS_FOGEND);
	CaptureRenderState(D3DRS_FOGDENSITY);
	CaptureRenderState(D3DRS_RANGEFOGENABLE);
	CaptureRenderState(D3DRS_AMBIENT);
	CaptureRenderState(D3DRS_COLORVERTEX);
	CaptureRenderState(D3DRS_FOGVERTEXMODE);
	CaptureRenderState(D3DRS_CLIPPING);
	CaptureRenderState(D3DRS_LIGHTING);
	CaptureRenderState(D3DRS_LOCALVIEWER);
	CaptureRenderState(D3DRS_EMISSIVEMATERIALSOURCE);
	CaptureRenderState(D3DRS_AMBIENTMATERIALSOURCE);
	CaptureRenderState(D3DRS_DIFFUSEMATERIALSOURCE);
	CaptureRenderState(D3DRS_SPECULARMATERIALSOURCE);
	CaptureRenderState(D3DRS_VERTEXBLEND);
	CaptureRenderState(D3DRS_CLIPPLANEENABLE);
	CaptureRenderState(D3DRS_POINTSIZE);
	CaptureRenderState(D3DRS_POINTSIZE_MIN);
	CaptureRenderState(D3DRS_POINTSPRITEENABLE);
	CaptureRenderState(D3DRS_POINTSCALEENABLE);
	CaptureRenderState(D3DRS_POINTSCALE_A);
	CaptureRenderState(D3DRS_POINTSCALE_B);
	CaptureRenderState(D3DRS_POINTSCALE_C);
	CaptureRenderState(D3DRS_MULTISAMPLEANTIALIAS);
	CaptureRenderState(D3DRS_MULTISAMPLEMASK);
	CaptureRenderState(D3DRS_PATCHEDGESTYLE);
	CaptureRenderState(D3DRS_POINTSIZE_MAX);
	CaptureRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE);
	CaptureRenderState(D3DRS_TWEENFACTOR);
	CaptureRenderState(D3DRS_POSITIONDEGREE);
	CaptureRenderState(D3DRS_NORMALDEGREE);
	CaptureRenderState(D3DRS_MINTESSELLATIONLEVEL);
	CaptureRenderState(D3DRS_MAXTESSELLATIONLEVEL);
	CaptureRenderState(D3DRS_ADAPTIVETESS_X);
	CaptureRenderState(D3DRS_ADAPTIVETESS_Y);
	CaptureRenderState(D3DRS_ADAPTIVETESS_Z);
	CaptureRenderState(D3DRS_ADAPTIVETESS_W);
	CaptureRenderState(D3DRS_ENABLEADAPTIVETESSELLATION);
	CaptureRenderState(D3DRS_NORMALIZENORMALS);
	CaptureRenderState(D3DRS_SPECULARENABLE);
	CaptureRenderState(D3DRS_SHADEMODE);
}

void CStateBlock9::CaptureVertexSamplerStates()
{
	for (int sampler = 0; sampler <= D3DVERTEXTEXTURESAMPLER3; sampler++)
	{
		CaptureSamplerState(sampler, D3DSAMP_DMAPOFFSET);
	}
}

void CStateBlock9::CaptureVertexTextureStates()
{
	for (int stage = 0; stage < 8; stage++)
	{
		CaptureTextureStageState(stage, D3DTSS_TEXCOORDINDEX);
		CaptureTextureStageState(stage, D3DTSS_TEXTURETRANSFORMFLAGS);
	}
}

void CStateBlock9::CaptureNPatchMode()
{
	mDeviceState.mNPatchMode = mDevice->GetNPatchMode();
	mDeviceState.mCapturedNPatchMode = true;
}

void CStateBlock9::CaptureLightStates()
{
	for (int index = 0; index < 8; index++)
	{
		long result = mDevice->GetLight(index, &mDeviceState.mLight[index]);
		mDeviceState.mCapturedLight[index] = SUCCEEDED(result);
	}

	for (int index = 0; index < 8; index++)
	{
		mDeviceState.mLightEnableState[index] = false;
		long result = mDevice->GetLightEnable(index, &mDeviceState.mLightEnableState[index]);
		mDeviceState.mCapturedLightEnable[index] = SUCCEEDED(result);
	}
}

void CStateBlock9::CaptureVertexShaderStates()
{
	mDeviceState.mCapturedVertexShader = true;
	mDevice->GetVertexShader(reinterpret_cast<IDirect3DVertexShader9**>(&mDeviceState.mVertexShader));

	//if (mVertexShader)
	//{
	//	vertexShader->PrivateAddRef();
	//}

	mDevice->GetVertexShaderConstantF(0, mDeviceState.mVertexShaderConstantF[0], MAX_VERTEX_SHADER_CONST);
	mDevice->GetVertexShaderConstantI(0, mDeviceState.mVertexShaderConstantI[0], 16);
	mDevice->GetVertexShaderConstantB(0, mDeviceState.mVertexShaderConstantB, 16);
}

void CStateBlock9::CaptureStreamSourceFrequencies()
{
	for (int stream = 0; stream < MAX_VERTEX_INPUTS; stream++)
	{
		mDeviceState.mCapturedStreamSourceFrequency[stream] = true;
		mDevice->GetStreamSourceFreq(stream, &mDeviceState.mStreamSourceFrequency[stream]);
	}
}

void CStateBlock9::CaptureFVF()
{
	mDevice->GetFVF(&mDeviceState.mFVF);
	mDeviceState.mCapturedFVF = true;
}

void CStateBlock9::CaptureVertexDeclaration()
{
	mDeviceState.mCapturedVertexDeclaration = true;
	mDevice->GetVertexDeclaration(reinterpret_cast<IDirect3DVertexDeclaration9**>(&mDeviceState.mVertexDeclaration));

	//if (mVertexDeclaration)
	//{
	//	vertexDeclaration->PrivateAddRef();
	//}
}

void CStateBlock9::CaptureTextures()
{
	for (int sampler = 0; sampler < 16 + 4; sampler++)
	{
		mDeviceState.mCapturedTexture[sampler] = true;
		int index = sampler < 16 ? sampler : D3DVERTEXTEXTURESAMPLER0 + (sampler - 16);
		mDevice->GetTexture(index, reinterpret_cast<IDirect3DBaseTexture9**>(&mDeviceState.mTexture[sampler]));

		//if (mTexture[sampler])
		//{
		//	mTexture[sampler]->PrivateAddRef();
		//}
	}
}

void CStateBlock9::CaptureTexturePalette()
{
	mDeviceState.mCapturedPaletteNumber = true;
	mDevice->GetCurrentTexturePalette(&mDeviceState.mPaletteNumber);
}

void CStateBlock9::CaptureVertexStreams()
{
	for (int stream = 0; stream < MAX_VERTEX_INPUTS; stream++)
	{
		mDeviceState.mCapturedStreamSource[stream] = true;
		mDevice->GetStreamSource(stream, reinterpret_cast<IDirect3DVertexBuffer9**>(&mDeviceState.mStreamSource[stream].vertexBuffer), &mDeviceState.mStreamSource[stream].offset, &mDeviceState.mStreamSource[stream].stride);

		//if (mStreamSource[stream].vertexBuffer)
		//{
		//	mStreamSource[stream].vertexBuffer->PrivateAddRef();
		//}
	}
}

void CStateBlock9::CaptureIndexBuffer()
{
	mDeviceState.mCapturedIndexBuffer = true;
	mDevice->GetIndices(reinterpret_cast<IDirect3DIndexBuffer9**>(&mDeviceState.mIndexBuffer));

	//if (mIndexBuffer)
	//{
	//	mIndexBuffer->PrivateAddRef();
	//}
}

void CStateBlock9::CaptureViewport()
{
	mDevice->GetViewport(&mDeviceState.mViewport);
	mDeviceState.mCapturedViewport = true;
}

void CStateBlock9::CaptureScissorRectangle()
{
	mDevice->GetScissorRect(&mDeviceState.mScissorRect);
	mDeviceState.mCapturedScissorRect = true;
}

void CStateBlock9::CaptureTransforms()
{
	CaptureTransform(D3DTS_VIEW);
	CaptureTransform(D3DTS_PROJECTION);
	CaptureTransform(D3DTS_WORLD);
}

void CStateBlock9::CaptureTextureTransforms()
{
	CaptureTransform(D3DTS_TEXTURE0);
	CaptureTransform(D3DTS_TEXTURE1);
	CaptureTransform(D3DTS_TEXTURE2);
	CaptureTransform(D3DTS_TEXTURE3);
	CaptureTransform(D3DTS_TEXTURE4);
	CaptureTransform(D3DTS_TEXTURE5);
	CaptureTransform(D3DTS_TEXTURE6);
	CaptureTransform(D3DTS_TEXTURE7);
}

void CStateBlock9::CaptureClippingPlanes()
{
	for (int index = 0; index < 6; index++)
	{
		mDevice->GetClipPlane(index, (float*)&mDeviceState.mClipPlane[index]);
		mDeviceState.mCapturedClipPlane[index] = true;
	}
}

void CStateBlock9::CaptureMaterial()
{
	mDevice->GetMaterial(&mDeviceState.mMaterial);
	mDeviceState.mCapturedMaterial = true;
}

ULONG STDMETHODCALLTYPE CStateBlock9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CStateBlock9::QueryInterface(REFIID riid, void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DStateBlock9))
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

ULONG STDMETHODCALLTYPE CStateBlock9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0 && mPrivateReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CStateBlock9::Capture()
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CStateBlock9::Apply()
{


	return S_OK;
}

HRESULT STDMETHODCALLTYPE CStateBlock9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return S_OK; 
}