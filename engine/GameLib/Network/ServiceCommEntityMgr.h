#pragma once
#include "ServiceCommEntity.h"
#include "boost/asio.hpp"
#include "Network.h"
#include "../Singleton.h"

class CommEntityMgr : public Singleton<CommEntityMgr> {
private:
	IServiceCommEntity* m_commEntity;

	IServiceCommEntity* createZmqCommEntity(ServiceAddr& addr, const char* serverIp, int serverPort);
	IServiceCommEntity* createTcpCommEntity(ServiceAddr& addr, const char* serverIp, int serverPort);
public:
	IServiceCommEntity* createCommEntity(ServiceAddr& addr, const char* serverIp, int serverPort);
	inline IServiceCommEntity* getCommEntity() { return m_commEntity; }
};

