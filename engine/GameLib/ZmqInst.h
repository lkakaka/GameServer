#pragma once

#include "zmq.h"
#include "Logger.h"
//#include "GameUtil.h"
#include "../Common/ServerExports.h"
#include "Singleton.h"
#include "CommEntity.h"

#include <thread>
#include <functional>

#define ZmqRecvCallback std::function<void(char*, char*, int)>

class ZmqInst : public Singleton<ZmqInst>, public CommEntityInf {
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
	//ZmqInst(std::string& name, std::string& router_addr);
	ZmqInst(ServiceAddr& addr, std::string& routerAddr);
	~ZmqInst();

	static ZmqInst* getZmqInstance();
	//static void initZmqInstance(const char* name, const char* router_addr);
	void setRecvCallback(ZmqRecvCallback callback);

	//void sendData(const char* dstName, char* data, int datLen);

	void sendToService(ServiceAddr* dstAddr, char* msg, int msgLen);
	void onRecvServiceMsg(ServiceAddr* srcAddr, char* msg, int msgLen);

	void startZmqInst();
	void destory();
};

//#ifdef WIN32
//#ifdef SERVER_EXPORT
//INIT_SINGLETON_CLASS(ZmqInst);
//#endif // SERVER_EXPORT
//#endif // WIN32


