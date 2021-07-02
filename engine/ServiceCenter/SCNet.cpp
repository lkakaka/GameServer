#include "SCNet.h"
#include "Logger.h"

INIT_SINGLETON_CLASS(SCNet)

SCNet::SCNet()
{

}

ServerConnection* SCNet::onAccept(tcp::socket& socket) {
	int connId = allocConnID();
	SCConnection* conn = new SCConnection(connId, socket, std::bind(&ServerNetwork::closeConnection, this, std::placeholders::_1, std::placeholders::_2));
	return conn;
}

void SCNet::onCloseConnection(ServerConnection* conn, const char* reason)
{
	SCConnection* scConn = (SCConnection*)conn;
	auto iter = m_serviceConns.find(scConn->getServiceAddr()->getName()->c_str());
	if (iter != m_serviceConns.end())
	{
		m_serviceConns.erase(iter);
	}
}

void SCNet::addServiceConnection(const char* serviceAddr, SCConnection* conn) {
	if (m_serviceConns.find(serviceAddr) != m_serviceConns.end()) {
		Logger::logError("$service connection exist!!, %s", serviceAddr);
		return;
	}
	m_serviceConns.emplace(serviceAddr, conn->shared_from_this());
}

SCConnection* SCNet::getServiceConnection(const char* serviceAddr) {
	auto iter = m_serviceConns.find(serviceAddr);
	if (iter != m_serviceConns.end())
	{
		return (SCConnection*)iter->second.get();
	}
	return NULL;
}