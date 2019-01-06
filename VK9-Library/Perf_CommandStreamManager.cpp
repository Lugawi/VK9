// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
Copyright(c) 2018 Christopher Joseph Dean Schaefer

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

#include "Perf_CommandStreamManager.h"
#include "Perf_ProcessQueue.h"

#include "Utilities.h"

#include <iostream>
#include <fstream>

#include <winuser.h>

CommandStreamManager::CommandStreamManager()
	: mWorkerThread(ProcessQueue, this), mRenderManager(mConfiguration)
{
	//Setup Defaults;
#ifdef _DEBUG
	mConfiguration["LogFile"] = "VK9.log";
	mConfiguration["LogLevel"] = "0";
	mConfiguration["EnableDebugLayers"] = "1";
#else
	mConfiguration["LogFile"] = "VK9.log";
	mConfiguration["LogLevel"] = "3";
	mConfiguration["EnableDebugLayers"] = "0";
#endif

	//Load Configuration
	LoadConfiguration("VK9.conf");

	//Setup Logging.
	LogManager::Create(mConfiguration["LogFile"], (SeverityLevel)std::stoi(mConfiguration["LogLevel"]));


	Log(info) << "CommandStreamManager::CommandStreamManager" << std::endl;
}

CommandStreamManager::~CommandStreamManager()
{
	IsRunning = 0;
	mWorkerThread.join();
	Log(info) << "CommandStreamManager::~CommandStreamManager" << std::endl;
	LogManager::Destroy();
}

size_t CommandStreamManager::RequestWork(WorkItem* workItem)
{
	if (workItem->Caller != nullptr)
	{
		workItem->Caller->AddRef();
	}
	size_t count = 0;
	while (!mWorkItems.Push(workItem, count)) {}

	size_t key = 0;

	//fetching key should be atomic because it's an atomic size_t.
	switch (workItem->WorkItemType)
	{
	case WorkItemType::Instance_Create:
		key = mRenderManager.mStateManager.mInstanceKey++;
		break;
	case WorkItemType::Device_Create:
		key = mRenderManager.mStateManager.mDeviceKey++;
		break;
	case WorkItemType::VertexBuffer_Create:
		key = mRenderManager.mStateManager.mVertexBufferKey++;
		break;
	case WorkItemType::IndexBuffer_Create:
		key = mRenderManager.mStateManager.mIndexBufferKey++;
		break;
	case WorkItemType::Texture_Create:
		key = mRenderManager.mStateManager.mTextureKey++;
		break;
	case WorkItemType::CubeTexture_Create:
		key = mRenderManager.mStateManager.mTextureKey++;
		break;
	case WorkItemType::VolumeTexture_Create:
		key = mRenderManager.mStateManager.mTextureKey++;
		break;
	case WorkItemType::Surface_Create:
		key = mRenderManager.mStateManager.mSurfaceKey++;
		break;
	case WorkItemType::Volume_Create:
		key = mRenderManager.mStateManager.mVolumeKey++;
		break;
	case WorkItemType::Shader_Create:
		key = mRenderManager.mStateManager.mShaderConverterKey++;
		break;
	case WorkItemType::Query_Create:
		key = mRenderManager.mStateManager.mQueryKey++;
		break;
	default:
		break;
	}

	return key;
}

size_t CommandStreamManager::RequestWorkAndWait(WorkItem* workItem)
{
	workItem->WillWait = true;

	ResetEvent(workItem->WaitHandle);

	size_t result = this->RequestWork(workItem);	
	
	if (WaitForSingleObject(workItem->WaitHandle, INFINITE) == WAIT_TIMEOUT)
	{
		Log(warning) << "CommandStreamManager::RequestWorkAndWait semaphore timeout!" << std::endl;
	}

	return result;
}

WorkItem* CommandStreamManager::GetWorkItem(IUnknown* caller)
{
	WorkItem* returnValue = nullptr;

	size_t count = 0;
	if (!mUnusedWorkItems.Pop(returnValue, count))
	{
		returnValue = new WorkItem();
	}
	else
	{	
		returnValue->WillWait = false;
	}

	returnValue->Caller = caller;

	return returnValue;
}

void CommandStreamManager::LoadConfiguration(std::string filename)
{
	std::string key;
	std::string value;
	std::ifstream input(filename);

	while (input)
	{
		//load key/value pair
		std::getline(input, key, '=');
		std::getline(input, value, '\n');

		//scrub \r out just in case this is a DOS/Windows format file.
		key.erase(std::remove(key.begin(), key.end(), '\r'), key.end());
		value.erase(std::remove(value.begin(), value.end(), '\r'), value.end());

		//Handle leading and trailing spaces.
		Trim(key);
		Trim(value);

		mConfiguration[key] = value;
	}

	input.close();
}