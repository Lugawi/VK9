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

#include "CVertexBuffer9.h"
#include "CDevice9.h"
#include "LogManager.h"
//#include "PrivateTypes.h"

CVertexBuffer9::CVertexBuffer9(CDevice9* device, UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, HANDLE* pSharedHandle)
	: mReferenceCount(1),
	mDevice(device),
	mLength(Length),
	mUsage(Usage),
	mFVF(FVF),
	mPool(Pool),
	mSharedHandle(pSharedHandle),
	mIsDirty(true),
	mLockCount(0)
{
	AddStagingBuffer();
	AddVertexBuffer();

	mCurrentStagingBuffer = mStagingBuffers[0].get();
	mCurrentStagingBufferMemory = mStagingBufferMemories[0].get();

	mCurrentVertexBuffer = mVertexBuffers[0].get();
	mCurrentVertexBufferMemory = mVertexBufferMemories[0].get();
}

CVertexBuffer9::~CVertexBuffer9()
{

}

ULONG CVertexBuffer9::PrivateAddRef(void)
{
	return InterlockedIncrement(&mPrivateReferenceCount);
}

ULONG CVertexBuffer9::PrivateRelease(void)
{
	ULONG ref = InterlockedDecrement(&mPrivateReferenceCount);

	if (ref == 0 && mReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

void CVertexBuffer9::AddStagingBuffer()
{
	auto const bufferInfo = vk::BufferCreateInfo().setSize(mLength + 192 + 1024).setUsage(vk::BufferUsageFlagBits::eTransferSrc);

	mStagingBuffers.push_back(mDevice->mDevice->createBufferUnique(bufferInfo));

	vk::MemoryRequirements mem_reqs;
	mDevice->mDevice->getBufferMemoryRequirements(mStagingBuffers[mStagingBuffers.size() - 1].get(), &mem_reqs);

	auto mem_alloc = vk::MemoryAllocateInfo().setAllocationSize(mem_reqs.size).setMemoryTypeIndex(0);
	mDevice->FindMemoryTypeFromProperties(mem_reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, &mem_alloc.memoryTypeIndex);

	mStagingBufferMemories.push_back(mDevice->mDevice->allocateMemoryUnique(mem_alloc));

	mDevice->mDevice->bindBufferMemory(mStagingBuffers[mStagingBuffers.size() - 1].get(), mStagingBufferMemories[mStagingBufferMemories.size() - 1].get(), 0);
}

void CVertexBuffer9::AddVertexBuffer()
{
	auto const bufferInfo = vk::BufferCreateInfo().setSize(mLength + 192 + 1024).setUsage(vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst);

	mVertexBuffers.push_back(mDevice->mDevice->createBufferUnique(bufferInfo));

	vk::MemoryRequirements mem_reqs;
	mDevice->mDevice->getBufferMemoryRequirements(mVertexBuffers[mVertexBuffers.size() - 1].get(), &mem_reqs);

	auto mem_alloc = vk::MemoryAllocateInfo().setAllocationSize(mem_reqs.size).setMemoryTypeIndex(0);
	mDevice->FindMemoryTypeFromProperties(mem_reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal, &mem_alloc.memoryTypeIndex);

	mVertexBufferMemories.push_back(mDevice->mDevice->allocateMemoryUnique(mem_alloc));

	mDevice->mDevice->bindBufferMemory(mVertexBuffers[mVertexBuffers.size() - 1].get(), mVertexBufferMemories[mVertexBufferMemories.size() - 1].get(), 0);
}

ULONG STDMETHODCALLTYPE CVertexBuffer9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::QueryInterface(REFIID riid, void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DVertexBuffer9))
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

ULONG STDMETHODCALLTYPE CVertexBuffer9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0 && mPrivateReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::GetDevice(IDirect3DDevice9** ppDevice)
{
	mDevice->AddRef();
	(*ppDevice) = (IDirect3DDevice9*)mDevice;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	Log(warning) << "CVertexBuffer9::FreePrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CVertexBuffer9::GetPriority()
{
	//TODO: Implement.

	Log(warning) << "CVertexBuffer9::GetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	Log(warning) << "CVertexBuffer9::GetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CVertexBuffer9::GetType()
{
	//return D3DRTYPE_SURFACE;
	//return D3DRTYPE_VOLUME;
	//return D3DRTYPE_TEXTURE;
	//return D3DRTYPE_VOLUMETEXTURE;
	//return D3DRTYPE_CUBETEXTURE;
	return D3DRTYPE_VERTEXBUFFER;
	//return D3DRTYPE_INDEXBUFFER;
}

void STDMETHODCALLTYPE CVertexBuffer9::PreLoad()
{
	//TODO: Implement.

	Log(warning) << "CVertexBuffer9::PreLoad is not implemented!" << std::endl;

	return;
}

DWORD STDMETHODCALLTYPE CVertexBuffer9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	Log(warning) << "CVertexBuffer9::SetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	Log(warning) << "CVertexBuffer9::SetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::GetDesc(D3DVERTEXBUFFER_DESC* pDesc)
{
	pDesc->Format = D3DFMT_VERTEXDATA;
	pDesc->Type = D3DRTYPE_VERTEXBUFFER;
	pDesc->Usage = mUsage;
	pDesc->Pool = mPool;
	pDesc->Size = mLength;
	pDesc->FVF = mFVF;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::Lock(UINT OffsetToLock, UINT SizeToLock, VOID** ppbData, DWORD Flags)
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
			mIndex++;
		}
	}

	if (mIndex > mVertexBuffers.size() - 1)
	{
		//AddStagingBuffer();
		AddVertexBuffer();
	}

	mCurrentStagingBuffer = mStagingBuffers[0].get(); //mIndex
	mCurrentStagingBufferMemory = mStagingBufferMemories[0].get(); //mIndex

	mCurrentVertexBuffer = mVertexBuffers[mIndex].get();
	mCurrentVertexBufferMemory = mVertexBufferMemories[mIndex].get();

	(*ppbData) = mDevice->mDevice->mapMemory(mCurrentStagingBufferMemory, OffsetToLock, VK_WHOLE_SIZE);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CVertexBuffer9::Unlock()
{
	mDevice->mDevice->unmapMemory(mCurrentStagingBufferMemory);

	mDevice->BeginRecordingUtilityCommands();
	{
		auto const region = vk::BufferCopy().setSize(mLength + 192 + 1024);
		mDevice->mCurrentUtilityCommandBuffer.copyBuffer(mCurrentStagingBuffer, mCurrentVertexBuffer, 1, &region);
	}
	mDevice->StopRecordingUtilityCommands();

	InterlockedDecrement(&mLockCount);

	return S_OK;
}