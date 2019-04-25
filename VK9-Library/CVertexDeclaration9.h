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

#include <vector>

class CDevice9;

vk::Format ConvertDeclType(D3DDECLTYPE input) noexcept;
uint32_t GetTextureCount(DWORD fvf) noexcept;

class CVertexDeclaration9 : public IDirect3DVertexDeclaration9
{
public:
	CVertexDeclaration9(CDevice9* device,const D3DVERTEXELEMENT9* pVertexElements);
	CVertexDeclaration9(CDevice9* device, DWORD fvf);
	~CVertexDeclaration9();

	//Reference Counting
	ULONG mReferenceCount = 1;
	ULONG mPrivateReferenceCount = 0;

	ULONG PrivateAddRef(void);
	ULONG PrivateRelease(void);

	//Creation Parameters
	std::vector<D3DVERTEXELEMENT9> mVertexElements;
	DWORD mFVF = 0;

	//Misc
	uint32_t mPositionSize = 3;
	BOOL mIsTransformed = 0;

	BOOL mHasPosition=0;
	BOOL mHasBlendWeight = 0;
	BOOL mHasBlendIndices = 0;
	BOOL mHasNormal = 0;
	BOOL mHasPSize = 0;
	int32_t mTextureCount = 0;
	BOOL mHasTangent = 0;
	BOOL mHasBinormal = 0;
	BOOL mHasTessfactor = 0;
	BOOL mHasPositionT = 0;
	BOOL mHasColor1=0;
	BOOL mHasColor2=0;
	BOOL mHasFog = 0;
	BOOL mHasDepth = 0;
	BOOL mHasSample = 0;

	std::vector<vk::VertexInputAttributeDescription> mVertexInputAttributeDescription;

private:
	CDevice9* mDevice;
public:
	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void  **ppv);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);	
	virtual ULONG STDMETHODCALLTYPE Release(void);

	//IDirect3DVertexDeclaration9
	virtual HRESULT STDMETHODCALLTYPE GetDeclaration(D3DVERTEXELEMENT9* pDecl, UINT* pNumElements);
	virtual HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice);
};
