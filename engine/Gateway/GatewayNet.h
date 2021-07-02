#pragma once
#include "boost/asio.hpp"

#include "Network/ServerNet.h"
#include "Singleton.h"
#include "Logger.h"
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

USE_NS_GAME_NET

class GatewayNet : public ServerNetwork, public Singleton<GatewayNet> {
protected:
	ServerConnection* onAccept(tcp::socket& socket);
public:
	GatewayNet();

	void onCloseConnection(ServerConnection* conn, const char* reason);
};

