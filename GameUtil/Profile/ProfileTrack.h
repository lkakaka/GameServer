#pragma once
#include <stdio.h>
#include <string>

#include "../../Common/ServerExports.h"

#ifdef OPEN_PROFILE_TRACK
#define PROFILE_TRACK(name) _PROFILE_TRACK(name, 0, __COUNTER__)
#define PROFILE_TRACK_WITH_TIME(name, trackTime) _PROFILE_TRACK(name, trackTime, __COUNTER__)
#define _PROFILE_TRACK(name, trackTime, val) __PROFILE_TRACK(name, trackTime, val)
#define __PROFILE_TRACK(name, trackTime, val) ProfileTrack profile_track_##val(name, trackTime)
#else
#define PROFILE_TRACK(name)
#define PROFILE_TRACK_WITH_TIME(name, time)
#endif // OPEN_PROFILE_TRACK


class SERVER_EXPORT_API ProfileTrack
{
	long m_startTime;
	std::string m_name;
	int m_trackTime;
public:
	inline ProfileTrack(const char* name);
	inline ProfileTrack(const char* name, int trackTime);
	inline ~ProfileTrack();
};

