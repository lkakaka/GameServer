#pragma once

#include "boost/asio.hpp"
#include "Network.h"

using boost::asio::ip::tcp;

NS_GAME_NET_BEGIN

// 网络连接的客户端，非游戏客户端

class ClientConnection {
private:
	std::string m_serverIp;
	int m_serverPort;
	bool m_isConnected;
	bool m_isSending;
	boost::asio::io_context* m_io;

	std::shared_ptr<tcp::socket> m_socket;
	std::vector<char> m_readBuff;
	std::vector<unsigned char> m_sendBuff;
	
	std::shared_ptr<std::thread> m_connectThread;
	long m_connectTimer;

private:
	void _read();
	void _send();

protected:
	virtual void onRecvData(std::vector<char>& data, int len) = 0;
	virtual void onConnect() {};

	void connectHandler(boost::system::error_code ec);

public:
	ClientConnection(boost::asio::io_context* io, const char* server_ip, int server_port);

	bool connect();
	void tryConnect();
	void disConnect();

	bool send(const char* data, int len);
};

NS_GAME_NET_END
