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
	int m_sceneServiceId; // ���ڳ���
	ikcpcb* m_kcp;
	long m_kcpTimerId;
	bool m_kcpEnabled;  // �Ƿ���Կ���KCP
	std::string m_kcpToken; // KCP����token
	int64_t m_kcpTokenTs; // KCPtoken����Чʱ���

	std::string m_clientUdpIP;
	int m_clientUdpPort;

protected:
	void parseMessage();

	static int udp_output(const char* buf, int len, ikcpcb* kcp, void* user);

public:
	GatewayConnection(int connID, std::shared_ptr<tcp::socket> sock, ConnCloseCallback closeCallback);
	~GatewayConnection();

	void parsePacket();
	void dispatchClientMsg(int msgId, int msgLen, const char* msgData);
	//void sendMsgToService(int msgId, int msgLen, const char* msgData, ServiceAddr* addr);
	void sendMsgToClient(send_type type, int msgId, char* data, int dataLen);

	void setSceneServiceId(int sceneServiceId);
	inline int getSceneServiceId() { return m_sceneServiceId; }

	inline void setClientUdpAddr(std::string& ip, int port) { m_clientUdpIP = ip; m_clientUdpPort = port; }
	inline const char* getClientUdpIP() const { return m_clientUdpIP.c_str(); }
	inline int getClientUdpPort() const { return m_clientUdpPort; }
	void onRecvKCPMsg(char* buff, int len);
	void enableKCP(std::string& token);
	bool isKcpTokenValid(std::string& token);
	inline bool isKCPStarted() { return m_kcp != NULL; }
	void startKCP();
	void stopKCP();
};

