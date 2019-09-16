#pragma once
#include "boost/asio/ip/tcp.hpp"
#include "Log.h"

using boost::asio::ip::tcp;

class TcpConnection
{
private:
	tcp::socket* m_socket;
	char* m_vecData;

public:
	TcpConnection(tcp::socket* socket);
	~TcpConnection();

	void read();
};

