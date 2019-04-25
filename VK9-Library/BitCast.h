#pragma once

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

template <class TargetType, class SourceType>
inline TargetType bit_cast(const SourceType& source) noexcept
{
	static_assert(!std::is_same<TargetType, SourceType>::value, "You've made a mistake, you don't need to bit cast if the types are the same.");
	constexpr int32_t size = std::min(sizeof(TargetType), sizeof(SourceType));
	TargetType returnValue;
	memcpy(&returnValue, &source, size);
	return returnValue;
}

inline DWORD bit_cast(const float& source) noexcept
{
	DWORD returnValue;
	memcpy(&returnValue, &source, sizeof(DWORD));
	return returnValue;
}

inline float bit_cast(const DWORD& source) noexcept
{
	float returnValue;
	memcpy(&returnValue, &source, sizeof(float));
	return returnValue;
}