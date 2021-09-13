#pragma once
#include "boost/asio.hpp"
#include "Logger.h"
#include "MyBuffer.h"
#include "../Common/ServerExports.h"
#include "../Common/ServerMacros.h"
#include <mutex>
#include "Network/ServiceCommEntity.h"
#include "Network/ServerConnection.h"
#include "Network/ikcp.h"

USE_NS_GAME_NET

class GatewayConnection : public ServerConnection
{
private:
	int m_sceneServiceId; // 所在场景
	ikcpcb* m_kcp;
	long m_kcpTimerId;
	int m_remoteUdpPort; // 客户端UDP监听端口

protected:
	void parseMessage();

	void startKCP();
	void stopKCP();
	static int udp_output(const char* buf, int len, ikcpcb* kcp, void* user);

public:
	GatewayConnection(int connID, std::shared_ptr<tcp::socket> sock, ConnCloseCallback closeCallback);
	~GatewayConnection();

	void parsePacket();
	void dispatchClientMsg(int msgId, int msgLen, const char* msgData);
	//void sendMsgToService(int msgId, int msgLen, const char* msgData, ServiceAddr* addr);
	void sendMsgToClient(send_type type, int msgId, char* data, int dataLen);

	void setSceneServiceId(int sceneServiceId);
	inline int getSceneServiceId() { return m_sceneServiceId; };

	inline int getRemoteUdpPort() const { return m_remoteUdpPort; }
	void onRecvKCPMsg(char* buff, int len);
};

