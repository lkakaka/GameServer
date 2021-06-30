#pragma once

#include "boost/asio.hpp"
#include <unordered_map>
#include "Network.h"


using boost::asio::ip::tcp;

typedef std::function<void(void*, const char*)> ConnCloseCallback;

NS_GAME_NET_BEGIN

class Connection : public std::enable_shared_from_this<Connection> {
private:
	tcp::socket m_socket;
	std::vector<char> m_readBuf;
	std::vector<char> m_sendBuf;
	int m_connID;
	bool m_isClosed;
	bool m_isSending;
	ConnCloseCallback m_closeCallback;

private:
	void _send();

protected:
	void doRead();
	// 子类处理
	virtual void onRead(char* data, int len) = 0;

public:
	Connection(int connID, tcp::socket& socket, ConnCloseCallback closeCallback);
	~Connection();

	tcp::socket& getSocket();
	int getConnID() const;

	void send(const char* data, int len);

	void close();
};

NS_GAME_NET_END

