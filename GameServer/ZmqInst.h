#pragma once

#include "zmq.h"
#include "Logger.h"

#include <thread>

class ZmqInst {
private:
	void* zmq_context;
	void* bind_socket;
	std::thread* work_thread;

	void run();
public:
	ZmqInst();
	~ZmqInst();
	void startZmqInst();
	void destory();
};