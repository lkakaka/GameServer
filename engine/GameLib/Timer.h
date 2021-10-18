#pragma once

#include <unordered_map>
#include <mutex>
#include "boost/asio.hpp"

//#include "GameUtil.h"
#include "../Common/ServerExports.h"
#include "Singleton.h"

//typedef void (*TimerCallback)();
typedef std::function<void(int)> TimerCallback;

typedef struct
{
	TimerCallback m_callback;
	boost::asio::deadline_timer* timer;
	int interval;
	int loopCnt;
	int triggerCnt;
}Timer;

class TimerMgr : public Singleton<TimerMgr>
{
private:
	long m_curTimerId;
	std::unordered_map<long, Timer> m_timerMap;

	std::mutex m_timerMutex;
public:
	TimerMgr();
	long allocTimerId();
	long addTimer(int nextTime, int interval, int loopCnt, TimerCallback callback);
	void removeTimer(long timerId, bool needCancel);

	void onTimer(const boost::system::error_code& e, long timerId);

	static void initTimerMgr();
	//static TimerMgr* getTimerInstance();
};

