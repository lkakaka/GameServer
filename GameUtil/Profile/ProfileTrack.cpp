#include "ProfileTrack.h"
#include "../TimeUtil.h"

ProfileTrack::ProfileTrack(const char* name)
{
	m_startTime = TimeUtil::getCurrentStamp();
	m_name = name;
	m_trackTime = 0;
	//printf("enter profile track, %s\n", name);
}

ProfileTrack::ProfileTrack(const char* name, int trackTime)
{
	m_startTime = TimeUtil::getCurrentStamp();
	m_name = name;
	m_trackTime = trackTime;
	//printf("enter profile track, %s\n", name);
}

ProfileTrack::~ProfileTrack()
{
	long costTime = TimeUtil::getCurrentStamp() - m_startTime;
	if (costTime >= m_trackTime) {
		printf("profile track, %s, cost time: %ldms\n", m_name.c_str(), costTime);
	}
}