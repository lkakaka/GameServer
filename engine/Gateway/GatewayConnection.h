#pragma once
#include "boost/asio.hpp"
#include "Logger.h"
#include "MyBuffer.h"
#include "../Common/ServerExports.h"
#include <mutex>
#include "Network/CommEntity.h"
#include "Network/ServerConnection.h"

USE_NS_GAME_NET

class GatewayConnection : public ServerConnection
{
private:
	int m_sceneServiceId; // ËùÔÚ³¡¾°

protected:
	void parseMessage();

public:
	GatewayConnection(int connID, std::shared_ptr<tcp::socket> sock, ConnCloseCallback closeCallback);
	~GatewayConnection();

	void parsePacket();
	void dispatchClientMsg(int msgId, int msgLen, const char* msgData);
	void sendMsgToService(int msgId, int msgLen, const char* msgData, ServiceAddr* addr);
	void sendMsgToClient(int msgId, char* data, int dataLen);

	void setSceneServiceId(int sceneServiceId);
};

