#pragma once

#include "Network.h"
#include "boost/asio.hpp"
#include <unordered_map>
#include "Connection.h"

using boost::asio::ip::tcp;

NS_GAME_NET_BEGIN

class ServerNetwork {
private:
	std::shared_ptr<tcp::acceptor> m_acceptor;
	//std::unordered_map<int, std::shared_ptr<TcpConnection>> m_conns;

	int m_curConnId = 1;

private:
	void doAccept();
	void acceptHandler(boost::system::error_code error, tcp::socket socket);

protected:
	inline int allocConnID() { return m_curConnId++; }
	virtual Connection* onAccept(tcp::socket& socket) = 0;

public:
	ServerNetwork();
	void start(boost::asio::io_service& io, int port);
	void removeConnection();
};

NS_GAME_NET_END
