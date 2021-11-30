#pragma once

#include "Network/ServerNet.h"
#include "SCConnection.h"
#include "Singleton.h"
#include "Network/TcpCommEntity.h"

USE_NS_GAME_NET

class SCNet : public ServerNetwork, public Singleton<SCNet> {
private:
	std::unordered_map<std::string, std::shared_ptr<ServerConnection>> m_serviceConns;



protected:
	ServerConnection* onAccept(std::shared_ptr<tcp::socket> socket);
public:
	SCNet(boost::asio::io_service* io);

	void onCloseConnection(ServerConnection* conn, const char* reason);
	void addServiceConnection(const char* serviceAddr, SCConnection* conn);
	SCConnection* getServiceConnection(const char* serviceAdd);
};
