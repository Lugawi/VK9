// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
Copyright(c) 2019 Christopher Joseph Dean Schaefer

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

#include "LogManager.h"

//#include "Utilities.h"

static char SeverityText[6][20] =
{
	"[TRACE] ",
	"[DEBUG] ",
	"[INFO] ",
	"[WARNING] ",
	"[ERROR] ",
	"[FATAL] "
};

LogManager* LogManager::mInstance;

LogManager::LogManager(const std::string& filename, SeverityLevel severityLevel)
	: mFileStream(filename),
	mSeverityLevel(severityLevel)
{
}

LogManager::~LogManager() 
{
}

void LogManager::Create(const std::string& filename, SeverityLevel severityLevel)
{
	LogManager::mInstance = new LogManager(filename, severityLevel);
}

void LogManager::Destroy()
{
	delete LogManager::mInstance;
}

LockedStream Log(SeverityLevel severityLevel)
{
	if (severityLevel > LogManager::mInstance->mSeverityLevel)
	{
		return LockedStream(LogManager::mInstance->mFileStream, LogManager::mInstance->mMutex);
	}
	else
	{
		return LockedStream(LogManager::mInstance->mNullStream, LogManager::mInstance->mMutex);
	}
};