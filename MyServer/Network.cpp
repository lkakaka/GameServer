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
	boost::shared_ptr<tcp::socket> psocket(new tcp::socket(*m_io));
	m_acceptor.async_accept(*psocket, std::bind(&Network::acceptHandler, this, psocket, std::placeholders::_1));
}

void Network::acceptHandler(boost::shared_ptr<tcp::socket> psocket, error_code ec)
{
	if (!ec) {
		TcpConnection* conn = new TcpConnection(psocket.get());
		m_connMap.insert(std::make_pair(m_curConnId++, *conn));
	}
	doAccept();
}