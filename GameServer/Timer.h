#pragma once

#include <unordered_map>
#include "boost/asio.hpp"

typedef void* (*TimerCallback)();

class Timer
{
public:
	TimerCallback m_callback;
	boost::asio::deadline_timer* timer;
};

class TimerMgr
{
private:
	boost::asio::io_service* m_io;
	long m_cur_timer_id;
	std::unordered_map<long, Timer> m_timer_map;
public:
	TimerMgr(boost::asio::io_service* io);
	long allocTimerId();
	long addTimer(int nextTime, int interval, int loopCnt, TimerCallback callback);
	void removeTimer(long timerId);

	void onTimer(const boost::system::error_code& e);
};

