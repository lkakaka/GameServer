#pragma once

#include "Connection.h"
#include "MyBuffer.h"
#include "CommEntity.h"

USE_NS_GAME_NET

class SCConnection : public Connection {
private:
	MyBuffer m_recvBuffer;
	bool m_isVerify;
	ServiceAddr m_serviceAddr;
public:
	SCConnection(int connID, tcp::socket& socket, ConnCloseCallback closeCallback);
	inline void startRead() { doRead(); }

	inline bool isVerify() { return m_isVerify; }
	inline void setVerify(bool isVerify) { m_isVerify = isVerify; }

	inline void setServiceAddr(ServiceAddr& addr) { m_serviceAddr = addr; }
	inline ServiceAddr* getServiceAddr() { return &m_serviceAddr; } 

protected:
	void onRead(char* data, int len);
};

