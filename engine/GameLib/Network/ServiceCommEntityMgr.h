#pragma once
#include "CommEntity.h"
#include "boost/asio.hpp"

//#define USE_ZMQ_ENTITY

class CommEntityMgr : public Singleton<CommEntityMgr> {
private:
	IServiceCommEntity* m_commEntity;

	IServiceCommEntity* createZmqCommEntity(ServiceAddr& addr, const char* serverIp, int serverPort);
	IServiceCommEntity* createTcpCommEntity(boost::asio::io_context* io, ServiceAddr& addr, const char* serverIp, int serverPort);
public:
	IServiceCommEntity* createCommEntity(boost::asio::io_context* io, ServiceAddr& addr, const char* serverIp, int serverPort);
	inline IServiceCommEntity* getCommEntity() { return m_commEntity; }
};

