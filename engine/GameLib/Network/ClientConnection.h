#pragma once

//#include "boost/asio.hpp"
#include "ConnectionBase.h"
#include <mutex>

using boost::asio::ip::tcp;

NS_GAME_NET_BEGIN

// 网络连接的客户端，非游戏客户端

class ClientConnection : public ConnectionBase {
private:
	std::string m_serverIp;
	int m_serverPort;
	std::vector<char> m_readBuff;
	std::shared_ptr<std::thread> m_connectThread;
	long m_connectTimer;
	long m_lastConnectTime;

	std::mutex m_sendLock;

private:
	void _read();

protected:
	virtual void onRecvData(std::vector<char>& data, int len) = 0;
	virtual void onConnect() {};

	void connectHandler(boost::system::error_code ec);

public:
	ClientConnection(const char* server_ip, int server_port);

	bool connect();
	void tryConnect();
	void disConnect();
};

NS_GAME_NET_END
