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
 
#include "CIndexBuffer9.h"
#include "CDevice9.h"
#include "LogManager.h"
//#include "PrivateTypes.h"

CIndexBuffer9::CIndexBuffer9(CDevice9* device, UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, HANDLE* pSharedHandle)
	: mReferenceCount(1),
	mDevice(device),
	mLength(Length),
	mUsage(Usage),
	mFormat(Format),
	mPool(Pool),
	mSharedHandle(pSharedHandle),
	mIsDirty(true),
	mLockCount(0)
{
	AddStagingBuffer();
	AddIndexBuffer();

	mCurrentIndexBuffer = mIndexBuffers[mDevice->mFrameIndex][0].get();
	mCurrentIndexBufferMemory = mIndexBufferMemories[mDevice->mFrameIndex][0].get();
}

CIndexBuffer9::~CIndexBuffer9()
{

}

ULONG CIndexBuffer9::PrivateAddRef(void)
{
	return InterlockedIncrement(&mPrivateReferenceCount);
}

ULONG CIndexBuffer9::PrivateRelease(void)
{
	ULONG ref = InterlockedDecrement(&mPrivateReferenceCount);

	if (ref == 0 && mReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

void CIndexBuffer9::AddStagingBuffer()
{
	auto const bufferInfo = vk::BufferCreateInfo().setSize(mLength + 16).setUsage(vk::BufferUsageFlagBits::eTransferSrc);

	mStagingBuffer = mDevice->mDevice->createBufferUnique(bufferInfo);

	vk::MemoryRequirements mem_reqs;
	mDevice->mDevice->getBufferMemoryRequirements(mStagingBuffer.get(), &mem_reqs);

	auto mem_alloc = vk::MemoryAllocateInfo().setAllocationSize(mem_reqs.size).setMemoryTypeIndex(0);
	mDevice->FindMemoryTypeFromProperties(mem_reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, &mem_alloc.memoryTypeIndex);

	mStagingBufferMemory = mDevice->mDevice->allocateMemoryUnique(mem_alloc);

	mDevice->mDevice->bindBufferMemory(mStagingBuffer.get(), mStagingBufferMemory.get(), 0);
}

void CIndexBuffer9::AddIndexBuffer()
{
	auto const bufferInfo = vk::BufferCreateInfo().setSize(mLength + 16).setUsage(vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst);

	mIndexBuffers[mDevice->mFrameIndex].push_back(mDevice->mDevice->createBufferUnique(bufferInfo));

	vk::MemoryRequirements mem_reqs;
	mDevice->mDevice->getBufferMemoryRequirements(mIndexBuffers[mDevice->mFrameIndex][mIndexBuffers[mDevice->mFrameIndex].size() - 1].get(), &mem_reqs);

	auto mem_alloc = vk::MemoryAllocateInfo().setAllocationSize(mem_reqs.size).setMemoryTypeIndex(0);
	mDevice->FindMemoryTypeFromProperties(mem_reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &mem_alloc.memoryTypeIndex);

	mIndexBufferMemories[mDevice->mFrameIndex].push_back(mDevice->mDevice->allocateMemoryUnique(mem_alloc));

	mDevice->mDevice->bindBufferMemory(mIndexBuffers[mDevice->mFrameIndex][mIndexBuffers[mDevice->mFrameIndex].size() - 1].get(), mIndexBufferMemories[mDevice->mFrameIndex][mIndexBufferMemories[mDevice->mFrameIndex].size() - 1].get(), 0);
}

ULONG STDMETHODCALLTYPE CIndexBuffer9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::QueryInterface(REFIID riid,void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DIndexBuffer9))
	{
		(*ppv) = this;
		this->AddRef();
		return D3D_OK;
	}

	if (IsEqualGUID(riid, IID_IDirect3DResource9))
	{
		(*ppv) = this;
		this->AddRef();
		return D3D_OK;
	}

	if (IsEqualGUID(riid, IID_IUnknown))
	{
		(*ppv) = this;
		this->AddRef();
		return D3D_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CIndexBuffer9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0 && mPrivateReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::GetDevice(IDirect3DDevice9** ppDevice)
{ 
	mDevice->AddRef(); 
	(*ppDevice) = (IDirect3DDevice9*)mDevice; 
	return D3D_OK; 
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	Log(warning) << "CIndexBuffer9::FreePrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CIndexBuffer9::GetPriority()
{
	//TODO: Implement.

	Log(warning) << "CIndexBuffer9::GetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	Log(warning) << "CIndexBuffer9::GetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CIndexBuffer9::GetType()
{
	//return D3DRTYPE_SURFACE;
	//return D3DRTYPE_VOLUME;
	//return D3DRTYPE_TEXTURE;
	//return D3DRTYPE_VOLUMETEXTURE;
	//return D3DRTYPE_CUBETEXTURE;
	//return D3DRTYPE_VERTEXBUFFER;
	return D3DRTYPE_INDEXBUFFER;
}

void STDMETHODCALLTYPE CIndexBuffer9::PreLoad()
{
	//TODO: Implement.

	Log(warning) << "CIndexBuffer9::PreLoad is not implemented!" << std::endl;

	return; 
}

DWORD STDMETHODCALLTYPE CIndexBuffer9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	Log(warning) << "CIndexBuffer9::SetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	Log(warning) << "CIndexBuffer9::SetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::GetDesc(D3DINDEXBUFFER_DESC* pDesc)
{
	pDesc->Format = mFormat;
	pDesc->Type = D3DRTYPE_INDEXBUFFER;
	pDesc->Usage = mUsage;
	pDesc->Pool = mPool;
	pDesc->Size = mLength;

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::Lock(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
{
	InterlockedIncrement(&mLockCount);

	if (mPool == D3DPOOL_MANAGED)
	{
		if (!(Flags & D3DLOCK_READONLY))
		{ //If the lock allows write mark the buffer as dirty.
			mIsDirty = true;
		}
	}

	mOffsetToLock = OffsetToLock;
	mSizeToLock = SizeToLock;

	if (mLastFrameIndex != mDevice->mFrameIndex)
	{
		mIndex = 0;
		mLastFrameIndex = mDevice->mFrameIndex;
	}
	else
	{
		if ((Flags & D3DLOCK_NOOVERWRITE) != D3DLOCK_NOOVERWRITE && (Flags & D3DLOCK_READONLY) != D3DLOCK_READONLY)
		{
			//mLastIndex = mIndex;
			mIndex++;
		}
	}

	if (mIndex > ((int32_t)mIndexBuffers[mDevice->mFrameIndex].size()) - 1)
	{
		AddIndexBuffer();
	}

	mCurrentIndexBuffer = mIndexBuffers[mDevice->mFrameIndex][mIndex].get();
	mCurrentIndexBufferMemory = mIndexBufferMemories[mDevice->mFrameIndex][mIndex].get();

	(*ppbData) = mDevice->mDevice->mapMemory(mStagingBufferMemory.get(), OffsetToLock, VK_WHOLE_SIZE);

	return D3D_OK;
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::Unlock()
{
	mDevice->mDevice->unmapMemory(mStagingBufferMemory.get());

	mDevice->BeginRecordingUtilityCommands();
	{
		auto const region = vk::BufferCopy().setSize(mLength + 192 + 1024);
		mDevice->mCurrentUtilityCommandBuffer.copyBuffer(mStagingBuffer.get(), mCurrentIndexBuffer, 1, &region);
	}
	mDevice->StopRecordingUtilityCommands();

	//mDevice->mDevice->waitIdle();

	InterlockedDecrement(&mLockCount);

	return D3D_OK;
}
