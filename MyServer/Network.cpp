#include "Network.h"

Network::Network(boost::asio::io_service* io)
	: m_io(io),
	m_acceptor(*io, tcp::endpoint(tcp::v4(), 20000)),
	m_curConnId(0)
{
	
}

int Network::startListen()
{
	doAccept();
	return 1;
}

void Network::doAccept()
{
	//boost::shared_ptr<tcp::socket> psocket(new tcp::socket(*m_io));
	
	std::shared_ptr<TcpConnection> conn(new TcpConnection(*m_io, allocConnID(), std::bind(&Network::closeConnection, this, std::placeholders::_1)));
	m_acceptor.async_accept(conn->getSocket(), std::bind(&Network::acceptHandler, this, conn, std::placeholders::_1));
}

void Network::acceptHandler(std::shared_ptr<TcpConnection> conn, error_code ec)
{
	if (!ec) {
		//TcpConnection* conn = new TcpConnection(std::move(psocket.get()));
		m_connMap.insert(std::make_pair(conn->getConnID(), conn));
		conn->doRead();
		Log::logDebug("$client connected, %s", conn->getSocket().remote_endpoint().address());
	}
	doAccept();
}

void Network::onConnectionClose(int connID)
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
		conn->doShutDown();
	}
}

void Network::closeConnection(int connID)
{
	onConnectionClose(connID);
}
