#pragma once

#include "ServerNet.h"
#include "SCConnection.h"
#include "Singleton.h"

USE_NS_GAME_NET

class SCNet : public ServerNetwork, public Singleton<SCNet> {
private:
	std::unordered_map<int, SCConnection*> m_conns;
	std::unordered_map<std::string, SCConnection*> m_serviceConns;

protected:
	Connection* onAccept(tcp::socket& socket);
public:
	SCNet();

	void closeConnection(void* conn, const char* reason);
	void addServiceConnection(const char* serviceAddr, SCConnection* conn);
	SCConnection* getServiceConnection(const char* serviceAdd);
};
