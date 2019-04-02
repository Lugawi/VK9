#pragma once

/*
Copyright(c) 2018-2019 Christopher Joseph Dean Schaefer

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

#include <atomic>
#include "WorkItemType.h"
#include "d3d9.h"

struct WorkItem
{
	WorkItemType WorkItemType = WorkItemType::None;
	size_t Id = 0;
	void* Argument1 = nullptr;
	void* Argument2 = nullptr;
	void* Argument3 = nullptr;
	void* Argument4 = nullptr;
	void* Argument5 = nullptr;
	void* Argument6 = nullptr;

	IUnknown* Caller = nullptr;

	bool WillWait = false;
	HANDLE WaitHandle;

	WorkItem()
	{
		WaitHandle = CreateEvent(
			NULL,               // default security attributes
			TRUE,               // manual-reset event
			FALSE,              // initial state is nonsignaled
			TEXT("WorkCompleteEvent")  // object name
		);
	}

	~WorkItem()
	{
		CloseHandle(WaitHandle);
	}
};
