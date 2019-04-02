#pragma once

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

#include <vulkan/vulkan.hpp>
#include <vulkan/vk_sdk_platform.h>
#include "d3d9.h"

#include "DeviceState.h"

class CDevice9;

class CStateBlock9 : public IDirect3DStateBlock9

{

public:
	CStateBlock9(CDevice9* device, D3DSTATEBLOCKTYPE Type);
	~CStateBlock9();

	//Reference Counting
	ULONG mReferenceCount = 1;
	ULONG mPrivateReferenceCount = 0;

	ULONG PrivateAddRef(void);
	ULONG PrivateRelease(void);

	//Device State
	DeviceState mDeviceState;
	D3DSTATEBLOCKTYPE mType = D3DSBT_FORCE_DWORD;

	//Set methods
	void LightEnable(unsigned long index, int enable);
	void SetClipPlane(unsigned long index, const float *plane);
	void SetCurrentTexturePalette(unsigned int paletteNumber);
	void SetFVF(DWORD FVF);
	void SetIndices(IDirect3DIndexBuffer9* indexData);
	void SetLight(unsigned long index, const D3DLIGHT9* light);
	void SetMaterial(const D3DMATERIAL9* material);
	void SetNPatchMode(float segments);
	void SetPixelShader(IDirect3DPixelShader9* shader);
	void SetPixelShaderConstantB(unsigned int startRegister, const int* constantData, unsigned int count);
	void SetPixelShaderConstantF(unsigned int startRegister, const float* constantData, unsigned int count);
	void SetPixelShaderConstantI(unsigned int startRegister, const int* constantData, unsigned int count);
	void SetRenderState(D3DRENDERSTATETYPE state, unsigned long value);
	void SetSamplerState(unsigned long index, D3DSAMPLERSTATETYPE state, unsigned long value);
	void SetScissorRect(const RECT* rect);
	void SetStreamSource(unsigned int stream, IDirect3DVertexBuffer9* data, unsigned int offset, unsigned int stride);
	void SetStreamSourceFreq(unsigned int streamNumber, unsigned int divider);
	void SetTexture(unsigned long index, IDirect3DBaseTexture9* texture);
	void SetTextureStageState(unsigned long stage, D3DTEXTURESTAGESTATETYPE type, unsigned long value);
	void SetTransform(D3DTRANSFORMSTATETYPE state, const D3DMATRIX* matrix);
	void SetViewport(const D3DVIEWPORT9* viewport);
	void SetVertexDeclaration(IDirect3DVertexDeclaration9* declaration);
	void SetVertexShader(IDirect3DVertexShader9* shader);
	void SetVertexShaderConstantB(unsigned int startRegister, const int* constantData, unsigned int count);
	void SetVertexShaderConstantF(unsigned int startRegister, const float* constantData, unsigned int count);
	void SetVertexShaderConstantI(unsigned int startRegister, const int* constantData, unsigned int count);

private:
	CDevice9* mDevice = nullptr;

	//Capture Individual states
	void CaptureRenderState(D3DRENDERSTATETYPE state);
	void CaptureSamplerState(unsigned long index, D3DSAMPLERSTATETYPE state);
	void CaptureTextureStageState(unsigned long stage, D3DTEXTURESTAGESTATETYPE type);
	void CaptureTransform(D3DTRANSFORMSTATETYPE state);

	//Capture Pixel states
	void CapturePixelRenderStates();
	void CapturePixelTextureStates();
	void CapturePixelSamplerStates();
	void CapturePixelShaderStates();

	//Capture Vertex states
	void CaptureVertexRenderStates();
	void CaptureVertexSamplerStates();
	void CaptureVertexTextureStates();
	void CaptureNPatchMode();
	void CaptureLightStates();
	void CaptureVertexShaderStates();
	void CaptureStreamSourceFrequencies();
	void CaptureVertexDeclaration();
	void CaptureFVF();

	//Capture Misc states
	void CaptureTextures();
	void CaptureTexturePalette();
	void CaptureVertexStreams();
	void CaptureIndexBuffer();
	void CaptureViewport();
	void CaptureScissorRectangle();
	void CaptureTransforms();
	void CaptureTextureTransforms();
	void CaptureClippingPlanes();
	void CaptureMaterial();

public:

	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void  **ppv);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

	//IDirect3DStateBlock9
	virtual HRESULT STDMETHODCALLTYPE Capture();
	virtual HRESULT STDMETHODCALLTYPE Apply();
	virtual HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice);

};
