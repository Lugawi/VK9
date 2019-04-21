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

	mCurrentStagingBuffer = mStagingBuffers[0].get();
	mCurrentStagingBufferMemory = mStagingBufferMemories[0].get();

	mCurrentIndexBuffer = mIndexBuffers[0].get();
	mCurrentIndexBufferMemory = mIndexBufferMemories[0].get();
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

	mStagingBuffers.push_back(mDevice->mDevice->createBufferUnique(bufferInfo));

	vk::MemoryRequirements mem_reqs;
	mDevice->mDevice->getBufferMemoryRequirements(mStagingBuffers[mStagingBuffers.size() - 1].get(), &mem_reqs);

	auto mem_alloc = vk::MemoryAllocateInfo().setAllocationSize(mem_reqs.size).setMemoryTypeIndex(0);
	mDevice->FindMemoryTypeFromProperties(mem_reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, &mem_alloc.memoryTypeIndex);

	mStagingBufferMemories.push_back(mDevice->mDevice->allocateMemoryUnique(mem_alloc));

	mDevice->mDevice->bindBufferMemory(mStagingBuffers[mStagingBuffers.size() - 1].get(), mStagingBufferMemories[mStagingBufferMemories.size() - 1].get(), 0);
}

void CIndexBuffer9::AddIndexBuffer()
{
	auto const bufferInfo = vk::BufferCreateInfo().setSize(mLength + 16).setUsage(vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst);

	mIndexBuffers.push_back(mDevice->mDevice->createBufferUnique(bufferInfo));

	vk::MemoryRequirements mem_reqs;
	mDevice->mDevice->getBufferMemoryRequirements(mIndexBuffers[mIndexBuffers.size() - 1].get(), &mem_reqs);

	auto mem_alloc = vk::MemoryAllocateInfo().setAllocationSize(mem_reqs.size).setMemoryTypeIndex(0);
	mDevice->FindMemoryTypeFromProperties(mem_reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &mem_alloc.memoryTypeIndex);

	mIndexBufferMemories.push_back(mDevice->mDevice->allocateMemoryUnique(mem_alloc));

	mDevice->mDevice->bindBufferMemory(mIndexBuffers[mIndexBuffers.size() - 1].get(), mIndexBufferMemories[mIndexBufferMemories.size() - 1].get(), 0);
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
	return S_OK; 
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

	return S_OK;
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

	if (mIndex > mIndexBuffers.size() - 1)
	{
		//AddStagingBuffer();
		AddIndexBuffer();
	}

	mCurrentStagingBuffer = mStagingBuffers[0].get(); //mIndex
	mCurrentStagingBufferMemory = mStagingBufferMemories[0].get(); //mIndex

	mCurrentIndexBuffer = mIndexBuffers[mIndex].get();
	mCurrentIndexBufferMemory = mIndexBufferMemories[mIndex].get();

	(*ppbData) = mDevice->mDevice->mapMemory(mCurrentStagingBufferMemory, OffsetToLock, VK_WHOLE_SIZE);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CIndexBuffer9::Unlock()
{
	mDevice->mDevice->unmapMemory(mCurrentStagingBufferMemory);

	mDevice->BeginRecordingUtilityCommands();
	{
		auto const region = vk::BufferCopy().setSize(mLength + 192 + 1024);
		mDevice->mCurrentUtilityCommandBuffer.copyBuffer(mCurrentStagingBuffer, mCurrentIndexBuffer, 1, &region);
	}
	mDevice->StopRecordingUtilityCommands();

	InterlockedDecrement(&mLockCount);

	return S_OK;
}
