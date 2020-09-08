#pragma once

#include "zmq.h"
#include "Logger.h"
//#include "GameUtil.h"
#include "../Common/ServerExports.h"
#include "Singleton.h"

#include <thread>
#include <functional>

#define ZmqRecvCallback std::function<void(char*, char*, int)>

class SERVER_EXPORT_API ZmqInst : public Singleton<ZmqInst> {
private:
	//static ZmqInst* zmqInstance;
	void* zmq_context;
	void* conn_socket;
	std::thread* work_thread;
	ZmqRecvCallback m_recvCallback;
	std::string m_name;
	std::string m_router_addr;
	void run();
public:
	ZmqInst(std::string& name, std::string& router_addr);
	~ZmqInst();

	static ZmqInst* getZmqInstance();
	//static void initZmqInstance(const char* name, const char* router_addr);
	void setRecvCallback(ZmqRecvCallback callback);

	void sendData(const char* dstName, void* data, int datLen);

	void startZmqInst();
	void destory();
};

#ifdef WIN32
#ifdef SERVER_EXPORT
INIT_SINGLETON_CLASS(ZmqInst);
#endif // SERVER_EXPORT
#endif // WIN32


