#include "ServiceCommEntityMgr.h"
#include "Logger.h"

#include "ZmqCommEntity.h"
#include "TcpCommEntity.h"

INIT_SINGLETON_CLASS(CommEntityMgr)

IServiceCommEntity* CommEntityMgr::createZmqCommEntity(ServiceAddr& addr, const char* serverIp, int serverPort) {
	return new ZmqCommEntity(addr, serverIp, serverPort);
}

IServiceCommEntity* CommEntityMgr::createTcpCommEntity(ServiceAddr& addr, const char* serverIp, int serverPort) {
	return new TcpCommEntity(addr, serverIp, serverPort);
}

IServiceCommEntity* CommEntityMgr::createCommEntity(const char* commName, ServiceAddr& addr, const char* serverIp, int serverPort) {
	if (m_commEntity.count(commName) != 0) {
		LOG_ERROR("comm entity already exist, %s", commName);
		return NULL;
	}
	IServiceCommEntity* commEntity = NULL;
#ifdef SERVICE_COMM_ENTITY_ZMQ
	commEntity = createZmqCommEntity(addr, serverIp, serverPort);
#else
	commEntity = createTcpCommEntity(addr, serverIp, serverPort);
#endif // SERVICE_COMM_ENTITY_ZMQ
	m_commEntity.emplace(commName, commEntity);
	return commEntity;
}
