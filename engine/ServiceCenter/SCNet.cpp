#include "SCNet.h"
#include "Logger.h"

INIT_SINGLETON_CLASS(SCNet)

SCNet::SCNet()
{

}

Connection* SCNet::onAccept(tcp::socket& socket) {
	int connId = allocConnID();
	SCConnection* conn = new SCConnection(connId, socket, std::bind(&SCNet::closeConnection, this, std::placeholders::_1, std::placeholders::_2));
	m_conns.emplace(std::make_pair(connId, conn));
	conn->startRead();
	return conn;
}

void SCNet::closeConnection(void* conn, const char* reason)
{
	SCConnection* scConn = (SCConnection*)conn;
	int connId = scConn->getConnID();
	Logger::logInfo("$close connection(%d), reason:%s", connId, reason);
	//decltype(m_conns.begin()->second) conn;

	auto iter1 = m_serviceConns.find(scConn->getServiceAddr()->getName()->c_str());
	if (iter1 != m_serviceConns.end())
	{
		m_serviceConns.erase(iter1);
	}

	auto iter = m_conns.find(connId);
	if (iter != m_conns.end()) {
		m_conns.erase(iter);
	}
	
	scConn->destroy();

	delete scConn;
}

void SCNet::addServiceConnection(const char* serviceAddr, SCConnection* conn) {
	if (m_serviceConns.find(serviceAddr) != m_serviceConns.end()) {
		Logger::logError("$service connection exist!!, %s", serviceAddr);
		return;
	}
	m_serviceConns.emplace(std::make_pair(serviceAddr, conn));
}

SCConnection* SCNet::getServiceConnection(const char* serviceAddr) {
	auto iter = m_serviceConns.find(serviceAddr);
	if (iter != m_serviceConns.end())
	{
		return iter->second;
	}
	return NULL;
}