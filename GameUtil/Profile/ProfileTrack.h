#pragma once
#include <stdio.h>
#include <string>

#include "../GameUtil.h"

#define PROFILE_TRACK(name) _PROFILE_TRACK(name, __COUNTER__)
#define _PROFILE_TRACK(name, val) __PROFILE_TRACK(name, val)
#define __PROFILE_TRACK(name, val) ProfileTrack l_profile_##val(name)


class GAMEUTIL_API ProfileTrack
{
	int64_t m_startTime;
	std::string m_name;
public:
	inline ProfileTrack(const char* name);

	inline ~ProfileTrack();
};

