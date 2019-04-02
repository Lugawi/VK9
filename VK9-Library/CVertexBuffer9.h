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

class CDevice9;

class CVertexBuffer9 : public IDirect3DVertexBuffer9
{
public:
	CVertexBuffer9(CDevice9* device,UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, HANDLE* pSharedHandle);
	~CVertexBuffer9();

	//Reference Counting
	ULONG mReferenceCount = 1;
	ULONG mPrivateReferenceCount = 0;

	ULONG PrivateAddRef(void);
	ULONG PrivateRelease(void);

	//Buffers (Staging and Vertex)
	std::vector<vk::UniqueBuffer> mStagingBuffers;
	std::vector<vk::UniqueDeviceMemory> mStagingBufferMemories;

	std::vector<vk::UniqueBuffer> mVertexBuffers;
	std::vector<vk::UniqueDeviceMemory> mVertexBufferMemories;

	size_t mLastFrameIndex = 0;
	size_t mIndex = 0;

	vk::Buffer mCurrentStagingBuffer;	
	vk::DeviceMemory mCurrentStagingBufferMemory;

	vk::Buffer mCurrentVertexBuffer;
	vk::DeviceMemory mCurrentVertexBufferMemory;

	//D3D9 State
	UINT mLength;
	DWORD mUsage;
	DWORD mFVF;
	D3DPOOL mPool;
	HANDLE* mSharedHandle;

	uint32_t mLockCount;
	bool mIsDirty;
	UINT mOffsetToLock = 0;
	UINT mSizeToLock = 0;

	//Helper Functions
	void AddStagingBuffer();
	void AddVertexBuffer();

private:
	CDevice9* mDevice;
public:
	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void  **ppv);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

	//IDirect3DResource9
	virtual HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice);
	virtual HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid);
	virtual DWORD STDMETHODCALLTYPE GetPriority();
	virtual HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData);
	virtual D3DRESOURCETYPE STDMETHODCALLTYPE GetType();
	virtual void STDMETHODCALLTYPE PreLoad();
	virtual DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew);
	virtual HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags);

	//IDirect3DVertexBuffer9
	virtual HRESULT STDMETHODCALLTYPE GetDesc(D3DVERTEXBUFFER_DESC* pDesc);
	virtual HRESULT STDMETHODCALLTYPE Lock(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags);
	virtual HRESULT STDMETHODCALLTYPE Unlock();
};
