#include "UnitTest.h"
#include "Timer.h"
#include "PythonPlugin.h"
#include "../Math/MathFunction.h"
#include "../Common/PyCommon.h"

#define TEST_TIMER_TREAD 0
#define TEST_PYTHON_TREAD 0
#define TEST_DB_PLUGIN 0
#define TEST_PROFILE 0
#define TEST_CONFIG 1

#if TEST_DB_PLUGIN
#include "DBMgr.h"
#endif

#if TEST_PROFILE
#include "Profile/ProfileTrack.h"
#endif

#if TEST_CONFIG
#include "Config.h"
#endif

#pragma comment(lib, "MathFunction.lib")

void pyThreadFunc()
{
	Logger::logInfo("$[UnitTest]Thread Start");
	auto py_state = PyGILState_Ensure();
	callPyFunction("main", "thread_test", NULL);
	PyGILState_Release(py_state);
	Logger::logInfo("$[UnitTest]Thread End");
}

void print(int timerId)
{
	Logger::logInfo("$[UnitTest]Timer trigger, timerId:%d", timerId);

}

void threadTimer()
{
	for (int i = 0; i < 1000; i++) {
		TimerMgr::getTimerInstance()->addTimer(5000, 5000, 1, std::bind(print, std::placeholders::_1));
	}
}

static void testTimerThead(std::vector<std::thread*> *threads) {
#if TEST_TIMER_TREAD
	// 定时器多线程测试
	std::thread* t_timer1 = new std::thread(threadTimer);
	threads->push_back(t_timer1);
	std::thread* t_timer2 = new std::thread(threadTimer);
	threads->push_back(t_timer2);
#endif
}

static void testPythonThead(std::vector<std::thread*>* threads) {
#if TEST_PYTHON_TREAD
	std::thread* t = new std::thread(pyThreadFunc);
	threads->push_back(t);
#endif
}

static void testDbPlugin() {
#if TEST_DB_PLUGIN
	DBMgr* dbMgr = DBMgr::getDBMgrInstance();
	if (dbMgr == NULL) {
		Logger::logError("$[UnitTest]DBMgr test failed, dbMgr Instance is NULL");
		return;
	}
	dbMgr->createDBHander("test2");
	Logger::logInfo("$[UnitTest]DB test success!!!");
#endif
}

static void testProfile() {
#if TEST_PROFILE
	{
		PROFILE_TRACK("test");
		for (int i = 0; i < 100000000; i++) {
			int x = sin(1.0) * 20;
		}
	}
	PROFILE_TRACK("test1");
#endif
}

static void testConfig() {
#if TEST_CONFIG
	int port = Config::getConfigInt("../../conf/gateway.cfg", "port");
	int a = 0x8fffffff;
	int b = a << 1;
	int c = a << 2;
	//assert(port == 20000);
	//_ASSERT(port == 200000);

#endif
}

void UnitTest::test()
{	
	double x = power(2, 3);
	/*boost::asio::io_service io;
	boost::asio::deadline_timer t(io, boost::posix_time::seconds(5));
	t.wait();*/
	
	std::vector<std::thread*> threads;
	testTimerThead(&threads);
	testPythonThead(&threads);

	testDbPlugin();
	testProfile();
	testConfig();
	
	for (auto iter = threads.begin(); iter != threads.end(); iter++) {
		std::thread* t = (std::thread*)*iter;
		t->join();
	}

	for (auto iter = threads.begin(); iter != threads.end(); iter++) {
		delete *iter;
	}
}