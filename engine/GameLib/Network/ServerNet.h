#pragma once

#include "Network.h"
#include "boost/asio.hpp"
#include <unordered_map>
#include "ServerConnection.h"

using boost::asio::ip::tcp;

NS_GAME_NET_BEGIN

class ServerNetwork {
private:
	boost::asio::io_service* m_ioService;
	std::shared_ptr<tcp::acceptor> m_acceptor;
	std::unordered_map<int, std::shared_ptr<ServerConnection>> m_conns;

	int m_curConnId = 128;

private:
	void doAccept();
	void acceptHandler(std::shared_ptr<tcp::socket> sock, boost::system::error_code error);

protected:
	inline int allocConnID() { return m_curConnId++; }
	virtual ServerConnection* onAccept(std::shared_ptr<tcp::socket> sock) = 0;
	virtual void onCloseConnection(ServerConnection* conn, const char* reason) = 0;

public:
	ServerNetwork(boost::asio::io_service* io);
	void start(int port);
	ServerConnection* getConnection(int connId);
	void removeConnection(int connId, const char* reason);
	void closeConnection(void* conn, const char* reason);
};

NS_GAME_NET_END
