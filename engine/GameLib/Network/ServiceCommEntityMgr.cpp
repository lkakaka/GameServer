#include "ServiceCommEntityMgr.h"
#include "Logger.h"

#include "ZmqCommEntity.h"
#include "TcpCommEntity.h"

INIT_SINGLETON_CLASS(CommEntityMgr)

IServiceCommEntity* CommEntityMgr::createZmqCommEntity(ServiceAddr& addr, const char* serverIp, int serverPort) {
	m_commEntity = new ZmqCommEntity(addr, serverIp, serverPort);
	return m_commEntity;
}

IServiceCommEntity* CommEntityMgr::createTcpCommEntity(boost::asio::io_context* io, ServiceAddr& addr, const char* serverIp, int serverPort) {
	m_commEntity = new TcpCommEntity(io, addr, serverIp, serverPort);
	return m_commEntity;
}

IServiceCommEntity* CommEntityMgr::createCommEntity(boost::asio::io_context* io, ServiceAddr& addr, const char* serverIp, int serverPort) {
#ifdef USE_ZMQ_ENTITY
	return createZmqCommEntity(addr, serverIp, serverPort);
#else
	return createTcpCommEntity(io, addr, serverIp, serverPort);
#endif // USE_ZMQ_ENTITY

}
