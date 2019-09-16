#include "TcpConnection.h"

TcpConnection::TcpConnection(tcp::socket* socket):
	m_socket(socket)
{
	m_vecData = new char[1024];
	memset(m_vecData, 0, 1024);
}

TcpConnection::~TcpConnection()
{
	delete[] m_vecData;
}

void TcpConnection::read()
{
	auto buf = boost::asio::buffer(m_vecData, 1024);
	m_socket->async_receive(buf, [buf](const boost::system::error_code& error, size_t datLen) {
		Log::logInfo("%receive data, len:%d, %s", datLen, buf);
	});
}