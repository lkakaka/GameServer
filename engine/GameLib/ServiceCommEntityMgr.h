#pragma once
#include "CommEntity.h"
#include "boost/asio.hpp"

//#define USE_ZMQ_ENTITY

class CommEntityMgr : public Singleton<CommEntityMgr> {
private:
	CommEntityInf* m_commEntity;

	CommEntityInf* createZmqCommEntity(ServiceAddr& addr, const char* serverIp, int serverPort);
	CommEntityInf* createTcpCommEntity(boost::asio::io_context* io, ServiceAddr& addr, const char* serverIp, int serverPort);
public:
	CommEntityInf* createCommEntity(boost::asio::io_context* io, ServiceAddr& addr, const char* serverIp, int serverPort);
	inline CommEntityInf* getCommEntity() { return m_commEntity; }
};

