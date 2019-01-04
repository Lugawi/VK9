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

#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>

#include "d3d9.h"

#include <vulkan/vk_sdk_platform.h>
#include <vulkan/vulkan.hpp>
#include <spirv.hpp>
#include <GLSL.std.450.h>

enum SeverityLevel
{
	trace,
	debug,
	info,
	warning,
	error,
	fatal
};

class NullBuffer
	: public std::streambuf
{
public:
	int overflow(int c) { return c; }
};

class NullStream
	: public std::ostream
{
private:
	NullBuffer obj;
public:
	NullStream()
		: std::ostream(&obj)
	{
	}
};

class LockedStream
{
	std::unique_lock<std::mutex> mLock;
	std::ostream* mStream;

public:
	LockedStream(std::ostream& stream, std::mutex& mutant)
		: mStream(&stream),
		mLock(mutant)
	{
	}

	LockedStream(LockedStream&& obj)
		: mLock(std::move(obj.mLock))
		, mStream(obj.mStream)
	{
		obj.mStream = nullptr;
	}

	//LockedStream& operator<<(ostream& (*manip)(ostream&))
	//{
	//	(*mStream) << manip;
	//	return *this;
	//}

	//template <class T>
	//LockedStream& operator<<(T&& x)
	//{
	//	(*mStream) << std::forward<T>(x);
	//	return *this;
	//}

	friend LockedStream&& operator << (LockedStream&& ls, std::ostream& (*argument)(std::ostream&))
	{
		(*ls.mStream) << argument;
		return std::move(ls);
	}

	friend LockedStream&& operator << (LockedStream&& ls, uint32_t&& argument)
	{
		(*ls.mStream) << argument;
		return std::move(ls);
	}

	template <typename ArgumentType>
	friend LockedStream&& operator << (LockedStream&& ls, ArgumentType&& argument)
	{
		(*ls.mStream) << std::forward<ArgumentType>(argument);
		return std::move(ls);
	}
};

class LogManager
{
private:
	static LogManager* mInstance;

	std::mutex mMutex;
	std::ofstream mFileStream;
	NullStream mNullStream;
	SeverityLevel mSeverityLevel;

public:
	LogManager(const std::string& filename, SeverityLevel severityLevel);
	~LogManager();
	static void Create(const std::string& filename, SeverityLevel severityLevel);
	static void Destroy();

	friend LockedStream Log(SeverityLevel severityLevel);
};

LockedStream Log(SeverityLevel severityLevel);

#endif // LOGMANAGER_H
