#include "Timer.h"
#include "Logger.h"
#include "boost/bind.hpp"


TimerMgr::TimerMgr(boost::asio::io_service* io): m_cur_timer_id(1), m_io(io)
{
}

long TimerMgr::allocTimerId()
{
	return m_cur_timer_id++;
}

void  TimerMgr::onTimer(const boost::system::error_code& e) {
	Logger::logInfo("$onTimer------%d", e.value());
}

long TimerMgr::addTimer(int time, int interval, int loopCnt, TimerCallback callback)
{
	//boost::asio::deadline_timer t(*m_io, boost::posix_time::seconds(time));
	boost::asio::deadline_timer *t = new boost::asio::deadline_timer(*m_io, boost::posix_time::milliseconds(time));
	long timerId = allocTimerId();
	Timer timer;
	timer.m_callback = callback;
	timer.timer = t;
	m_timer_map.emplace(std::make_pair(timerId, timer));
	t->async_wait(boost::bind(&TimerMgr::onTimer, this, boost::asio::placeholders::error));
	return timerId;
}

void TimerMgr::removeTimer(long timerId)
{
	auto iter = m_timer_map.find(timerId);
	if (iter == m_timer_map.end()) {
		return;
	}
	
	m_timer_map.erase(timerId);
	//iter->second.cancel();
}