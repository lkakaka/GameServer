#pragma once

#include "zmq.h"
#include "Logger.h"
#include "GameUtil.h"

#include <thread>
#include <functional>

#define ZmqRecvCallback std::function<void(char*, char*, int)>

class GAMEUTIL_API ZmqInst {
private:
	static ZmqInst* zmqInstance;
	void* zmq_context;
	void* conn_socket;
	std::thread* work_thread;
	ZmqRecvCallback m_recvCallback;
	ZmqInst();
	void run();
public:
	~ZmqInst();

	static ZmqInst* getZmqInstance();
	static void initZmqInstance(const char* name, const char* router_addr);
	void setRecvCallback(ZmqRecvCallback callback);

	void sendData(const char* dstName, void* data, int datLen);

	void startZmqInst(const char* name, const char* router_addr);
	void destory();
};
