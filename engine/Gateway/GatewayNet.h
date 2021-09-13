#pragma once
#include "boost/asio.hpp"

#include "Network/ServerNet.h"
#include "Singleton.h"
#include "Logger.h"
#include "../Common/ServerExports.h"
#include "Network/ikcp.h"

/*
#ifdef GATEWAY_EXPORT
#ifndef GATEWAY_API
#define GATEWAY_API extern "C" __declspec(dllexport)
#endif
#else
#define GATEWAY_API _declspec(dllimport)
#endif
*/

USE_NS_GAME_NET
using boost::asio::ip::udp;

class GatewayNet : public ServerNetwork, public Singleton<GatewayNet> {
private:
	std::shared_ptr<udp::socket> m_udp;
	std::vector<char> m_udpReadBuf;
protected:
	ServerConnection* onAccept(std::shared_ptr<tcp::socket> socket);
	void recvUdpMsg();
	void handleUdpMsg(int len);
public:
	GatewayNet(boost::asio::io_service* io);
	void startUdp(int udpPort);
	void onCloseConnection(ServerConnection* conn, const char* reason);
};

