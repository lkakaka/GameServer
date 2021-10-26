#pragma once
#include "ClientConnection.h"
#include "../MyBuffer.h"
#include "ServiceCommEntity.h"
#include "../Singleton.h"

USE_NS_GAME_NET

class TcpCommEntity : public ClientConnection, public Singleton<TcpCommEntity>, public IServiceCommEntity {
private:
	std::string m_serverIp;
	int m_serverPort;

	MyBuffer m_recvBuf;
private:
	void _parse();
protected:
	void onRecvData(std::vector<char>& data, int len);
public:
	TcpCommEntity(ServiceAddr& addr, const char* serverIp, int port);

	void sendToService(ServiceAddr* dstAddr, char* msg, int msgLen);
	void start() { connect(); }
	void onConnect();
};

