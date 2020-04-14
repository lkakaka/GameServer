#include "Network.h"


static Network* g_network = NULL;

Network::Network(boost::asio::io_service* io, int port)
	: m_io(io),
	m_acceptor(*io, tcp::endpoint(tcp::v4(), port)),
	m_curConnId(0)
{
	
}

void Network::initNetwork(boost::asio::io_service* io, int port)
{
	g_network = new Network(io, port);
	g_network->startListen();
	Logger::logInfo("$start listening on port %d", port);
}

Network* Network::getNetworkInstance()
{
	return g_network;
}

int Network::startListen()
{
	doAccept();
	return 1;
}

void Network::doAccept()
{
	//boost::shared_ptr<tcp::socket> psocket(new tcp::socket(*m_io));
	
	std::shared_ptr<TcpConnection> conn(new TcpConnection(*m_io, allocConnID(), std::bind(&Network::closeConnection, this, std::placeholders::_1, std::placeholders::_2)));
	m_acceptor.async_accept(conn->getSocket(), std::bind(&Network::acceptHandler, this, conn, std::placeholders::_1));
}

void Network::acceptHandler(std::shared_ptr<TcpConnection> conn, error_code ec)
{
	if (!ec) {
		//TcpConnection* conn = new TcpConnection(std::move(psocket.get()));
		m_connMap.insert(std::make_pair(conn->getConnID(), conn));
		conn->doRead();

		/*std::string rsp = "hello world";
		std::vector<char> buf;
		std::copy(rsp.begin(), rsp.end(), std::back_inserter(buf));
		conn->sendData(buf);*/

		Logger::logDebug("$client connected, %s", conn->getSocket().remote_endpoint().address().to_string().c_str());
	}
	doAccept();
}

void Network::doCloseConnection(int connID, const char* reason)
{
	decltype(m_connMap.begin()->second) conn;
	auto iter = m_connMap.find(connID);
	if (iter != m_connMap.end())
	{
		conn = iter->second;
		m_connMap.erase(iter);
	}

	if (conn)
	{
		conn->doShutDown(reason);
	}

	Logger::logInfo("$close connection(%d), reason:%s", connID, reason);
}

void Network::closeConnection(int connID, const char* reason)
{
	doCloseConnection(connID, reason);
}

void Network::removeConnection(int connID, const char* reason) {
	doCloseConnection(connID, reason);
}

TcpConnection* Network::getConnById(int connId)
{
	Network* network = getNetworkInstance();
	auto iter = network->m_connMap.find(connId);
	if (iter == network->m_connMap.end()) {
		return NULL;
	}
	return iter->second.get();
}

void startNetwork(boost::asio::io_service* io, int port)
{
	//Logger::initLog();
	Network::initNetwork(io, port);
}
