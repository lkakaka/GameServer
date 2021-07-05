#pragma once

#include "zmq.h"
#include "Logger.h"
//#include "GameUtil.h"
#include "../../Common/ServerExports.h"
#include "../Singleton.h"
#include "ServiceCommEntity.h"

#include <thread>
#include <functional>

class ZmqCommEntity : public Singleton<ZmqCommEntity>, public IServiceCommEntity {
private:
	//static ZmqInst* zmqInstance;
	void* zmq_context;
	void* conn_socket;
	std::thread* work_thread;
	std::string m_name;
	std::string m_serverIp;
	int m_serverPort;

	void run();
public:
	//ZmqInst(std::string& name, std::string& router_addr);
	ZmqCommEntity(ServiceAddr& addr, const char* serverIp, int serverPort);
	~ZmqCommEntity();

	//static void initZmqInstance(const char* name, const char* router_addr);

	//void sendData(const char* dstName, char* data, int datLen);

	void sendToService(ServiceAddr* dstAddr, char* msg, int msgLen);

	void start();
	void destory();
};

//#ifdef WIN32
//#ifdef SERVER_EXPORT
//INIT_SINGLETON_CLASS(ZmqInst);
//#endif // SERVER_EXPORT
//#endif // WIN32


