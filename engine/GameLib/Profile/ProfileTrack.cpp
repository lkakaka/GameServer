#include "ProfileTrack.h"
#include "../TimeUtil.h"
#include "Logger.h"

ProfileTrack::ProfileTrack(const char* name)
{
	m_startTime = TimeUtil::nowMillSec();
	m_name = name;
	m_trackTime = 0;
	//LOG_DEBUG("enter profile track, %s", name);
}

ProfileTrack::ProfileTrack(const char* name, int64_t trackTime)
{
	m_startTime = TimeUtil::nowMillSec();
	m_name = name;
	m_trackTime = trackTime;
	//LOG_DEBUG("enter profile track, %s", name);
}

ProfileTrack::~ProfileTrack()
{
	long costTime = TimeUtil::nowMillSec() - m_startTime;
	if (costTime >= m_trackTime) {
		LOG_WARN("profile track, %s, cost too long time: %ldms", m_name.c_str(), costTime);
	}
}