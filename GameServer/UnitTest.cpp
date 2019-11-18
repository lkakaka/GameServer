#include "UnitTest.h"
#include "Timer.h"
#include "PythonPlugin.h"
#include "../Math/MathFunction.h"

#define TEST_TIMER_TREAD 0
#define TEST_PYTHON_TREAD 0

#pragma comment(lib, "MathFunction.lib")

void pyThreadFunc()
{
	Logger::logInfo("$Thread Start");
	auto py_state = PyGILState_Ensure();
	callPyFunction("main", "thread_test");
	PyGILState_Release(py_state);
	Logger::logInfo("$Thread End");
}

void print(int timerId)
{
	Logger::logInfo("$Timer trigger, timerId:%d", timerId);

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

void UnitTest::test()
{	
	double x = power(2, 3);
	/*boost::asio::io_service io;
	boost::asio::deadline_timer t(io, boost::posix_time::seconds(5));
	t.wait();*/
	
	std::vector<std::thread*> threads;
	testTimerThead(&threads);
	testPythonThead(&threads);
	
	for (auto iter = threads.begin(); iter != threads.end(); iter++) {
		std::thread* t = (std::thread*)*iter;
		t->join();
	}

	for (auto iter = threads.begin(); iter != threads.end(); iter++) {
		delete *iter;
	}
}