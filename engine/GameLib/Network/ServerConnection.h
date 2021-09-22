#pragma once

#include "boost/asio.hpp"
#include <unordered_map>
#include "Network.h"
#include "../MyBuffer.h"


using boost::asio::ip::tcp;

typedef std::function<void(void*, const char*)> ConnCloseCallback;

NS_GAME_NET_BEGIN

class ServerConnection : public std::enable_shared_from_this<ServerConnection> {
private:
	int m_connID;
	std::shared_ptr<tcp::socket> m_socket;
	std::vector<char> m_readBuf;
	std::vector<char> m_sendBuf;
	bool m_isClosed;
	bool m_waitClosed;
	std::string m_waitCloseReason;
	bool m_isSending;
	ConnCloseCallback m_closeCallback;
	std::string m_clientIp;

private:
	void _send();
	void _read();
	void checkRecvBufferSize();
	
protected:
	MyBuffer m_recvBuffer; // 接受到未处理的数据
	
	// 子类处理, 并返回处理的数据长度
	virtual void parseMessage() = 0;
	void close(const char* reason);

public:
	ServerConnection(int connID, std::shared_ptr<tcp::socket> socket, ConnCloseCallback closeCallback);
	virtual ~ServerConnection();

	std::shared_ptr<tcp::socket> getSocket();
	int getConnID() const;
	inline const char* getClientIp() const { return m_clientIp.c_str(); }
	inline void setClientIp(std::string& ip) { m_clientIp = ip; }

	inline void startRead() { _read(); }
	void send(const char* data, int len);

	void setWaitClose(const char* reason);

	//void destroy();
	
};

NS_GAME_NET_END

