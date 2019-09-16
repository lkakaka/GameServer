#pragma once
#include "boost/asio.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/system/error_code.hpp"

#include "TcpConnection.h"
#include <map>
#include <unordered_map>

using boost::asio::ip::tcp;
using boost::system::error_code;

class Network
{
private:
	int m_curConnId;

	tcp::acceptor m_acceptor;
	boost::asio::io_service* m_io;
	
	std::unordered_map<int, TcpConnection> m_connMap;


public:
	Network(boost::asio::io_service* io);
	int startListen();
	void doAccept();
	void acceptHandler(boost::shared_ptr<tcp::socket> psocket, error_code ec);
};

