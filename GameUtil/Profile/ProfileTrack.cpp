#include "ProfileTrack.h"
#include "boost/date_time/posix_time/posix_time.hpp"

static int64_t getCurrentStamp64()
{
	boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));
	boost::posix_time::time_duration time_from_epoch = boost::posix_time::microsec_clock::universal_time() - epoch;
	return time_from_epoch.total_microseconds() / 1000;
}

ProfileTrack::ProfileTrack(const char* name)
{
	m_startTime = getCurrentStamp64();
	m_name = name;
	printf("enter profile track, %s\n", name);
}

ProfileTrack::~ProfileTrack()
{
	printf("leave profile track, %s, cost time: %lldms\n", m_name.c_str(), getCurrentStamp64() - m_startTime);
}