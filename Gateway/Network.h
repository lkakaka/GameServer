#pragma once
#include "boost/asio.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/system/error_code.hpp"

#include "TcpConnection.h"
#include "Logger.h"
#include <map>
#include <unordered_map>
#include "../Common/ServerExports.h"

/*
#ifdef GATEWAY_EXPORT
#ifndef GATEWAY_API
#define GATEWAY_API extern "C" __declspec(dllexport)
#endif
#else
#define GATEWAY_API _declspec(dllimport)
#endif
*/

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
	void doCloseConnection(int connID, const char* reason);

public:
	Network(boost::asio::io_service* io, int port);

	static void initNetwork(boost::asio::io_service* io, int port);
	static Network* getNetworkInstance();
	static TcpConnection* getConnById(int connId);

	int startListen();
	void doAccept();
	void acceptHandler(std::shared_ptr<TcpConnection> conn, error_code ec);
	void closeConnection(int connID, const char* reason);
	void removeConnection(int connID, const char* reason);
};

SERVER_EXPORT_API void startNetwork(boost::asio::io_service* io, int port);

