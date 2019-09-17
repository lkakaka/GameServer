#pragma once
#include "boost/asio.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/system/error_code.hpp"

#include "TcpConnection.h"
#include "Log.h"
#include <map>
#include <unordered_map>

using boost::asio::ip::tcp;
using boost::system::error_code;

class Network : std::enable_shared_from_this<Network>
{
private:
	int m_curConnId = 0;

	tcp::acceptor m_acceptor;
	boost::asio::io_service* m_io;
	
	std::unordered_map<int, std::shared_ptr<TcpConnection>> m_connMap;

	inline int allocConnID() { return m_curConnId++; }

public:
	Network(boost::asio::io_service* io);
	int startListen();
	void doAccept();
	void acceptHandler(std::shared_ptr<TcpConnection> conn, error_code ec);
	void onConnectionClose(int connID);
	void closeConnection(int connID);
};

