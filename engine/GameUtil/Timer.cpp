#include "Timer.h"
#include "Logger.h"
#include "boost/bind.hpp"
//#include "boost/asio/error.hpp"
#include "Profile/ProfileTrack.h"

static TimerMgr* g_timerMgr = NULL;

TimerMgr::TimerMgr(boost::asio::io_service* io): m_curTimerId(1), m_io(io)
{
}

long TimerMgr::allocTimerId()
{
	return m_curTimerId++;
}

void  TimerMgr::onTimer(const boost::system::error_code& e, long timerId) {
	//Logger::logInfo("$onTimer------%d", e.value());
	if (e.value() == 0) {
		auto iter = m_timerMap.find(timerId);
		if (iter == m_timerMap.end()) {
			Logger::logError("$onTimer error, not found timer");
			return;
		}
		Timer* timer = &iter->second;
		timer->triggerCnt++;
		if (timer->loopCnt >= 0 && timer->triggerCnt >= timer->loopCnt) {
			if (timer->m_callback != NULL) {
				PROFILE_TRACK_WITH_TIME("timer", 10);
				timer->m_callback(timerId);
			}
			removeTimer(timerId, false);
		}
		else {
			timer->timer->expires_at(timer->timer->expires_at() + boost::posix_time::milliseconds(timer->interval));
			timer->timer->async_wait(boost::bind(&TimerMgr::onTimer, this, boost::asio::placeholders::error, timerId));
			if (timer->m_callback != NULL) {
				PROFILE_TRACK_WITH_TIME("timer", 10);
				timer->m_callback(timerId);
			}
		}
	}
	else {
		removeTimer(timerId, false);
		if (e.value() != boost::asio::error::operation_aborted) {
			Logger::logInfo("$onTimer error:%d", e.value());
		}
	}
}

long TimerMgr::addTimer(int firstInterval, int interval, int loopCnt, TimerCallback callback)
{
	//boost::asio::deadline_timer t(*m_io, boost::posix_time::seconds(time));
	boost::asio::deadline_timer *t = new boost::asio::deadline_timer(*m_io, boost::posix_time::milliseconds(firstInterval));
	Timer timer;
	timer.m_callback = callback;
	timer.timer = t;
	timer.interval = interval;
	timer.loopCnt = loopCnt;
	timer.triggerCnt = 0;
	std::unique_lock<std::mutex> lock(m_timerMutex);
	long timerId = allocTimerId();
	if (m_timerMap.find(timerId) != m_timerMap.end()) {
		delete t;
		Logger::logInfo("$addTimer error, timer id repeated, timerId:%d", timerId);
		return -1;
	}
	m_timerMap.insert(std::make_pair(timerId, timer));
	t->async_wait(boost::bind(&TimerMgr::onTimer, this, boost::asio::placeholders::error, timerId));
	return timerId;
}

void TimerMgr::removeTimer(long timerId, bool needCancel)
{
	std::unique_lock<std::mutex> lock(m_timerMutex);
	auto iter = m_timerMap.find(timerId);
	if (iter == m_timerMap.end()) {
		return;
	}
	
	Timer timer = iter->second;
	m_timerMap.erase(timerId);
	if (needCancel) {
		timer.timer->cancel();
	}
	delete timer.timer;
}

TimerMgr* TimerMgr::getTimerInstance()
{
	return g_timerMgr;
}

void TimerMgr::initTimerMgr(boost::asio::io_service* io) 
{
	g_timerMgr = new TimerMgr(io);
}
