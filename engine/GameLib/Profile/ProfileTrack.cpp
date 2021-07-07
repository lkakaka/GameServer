#include "ProfileTrack.h"
#include "../TimeUtil.h"
#include "Logger.h"

ProfileTrack::ProfileTrack(const char* name)
{
	m_startTime = TimeUtil::getCurrentTime();
	m_name = name;
	m_trackTime = 0;
	//Logger::logDebug("$enter profile track, %s", name);
}

ProfileTrack::ProfileTrack(const char* name, int trackTime)
{
	m_startTime = TimeUtil::getCurrentTime();
	m_name = name;
	m_trackTime = trackTime;
	//Logger::logDebug("enter profile track, %s", name);
}

ProfileTrack::~ProfileTrack()
{
	long costTime = TimeUtil::getCurrentTime() - m_startTime;
	if (costTime >= m_trackTime) {
		Logger::logWarning("$profile track, %s, cost too long time: %ldms", m_name.c_str(), costTime);
	}
}