#pragma once
#include "ClientConnection.h"
#include "../MyBuffer.h"
#include <functional>
#include "CommEntity.h"

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
	TcpCommEntity(boost::asio::io_context* io, ServiceAddr& addr, const char* serverIp, int port);

	void sendToService(ServiceAddr* dstAddr, char* msg, int msgLen);
	void start() { tryConnect(); }
	void onConnect();
};

