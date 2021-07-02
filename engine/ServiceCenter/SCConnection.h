#pragma once

#include "Network/ServerConnection.h"
#include "MyBuffer.h"
#include "Network/CommEntity.h"

USE_NS_GAME_NET

class SCConnection : public ServerConnection {
private:
	bool m_isVerify;
	ServiceAddr m_serviceAddr;
private:
	void parse();
public:
	SCConnection(int connID, tcp::socket& socket, ConnCloseCallback closeCallback);

	inline bool isVerify() { return m_isVerify; }
	inline void setVerify(bool isVerify) { m_isVerify = isVerify; }

	inline void setServiceAddr(ServiceAddr& addr) { m_serviceAddr = addr; }
	inline ServiceAddr* getServiceAddr() { return &m_serviceAddr; } 

protected:
	void parseMessage();
};

