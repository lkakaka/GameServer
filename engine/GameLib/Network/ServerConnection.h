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
	tcp::socket m_socket;
	std::vector<char> m_readBuf;
	std::vector<char> m_sendBuf;
	bool m_isClosed;
	bool m_isSending;
	ConnCloseCallback m_closeCallback;

private:
	void _send();
	void _read();
	void checkRecvBufferSize();
	
protected:
	MyBuffer m_recvBuffer; // ���ܵ�δ����������
	
	// ���ദ��, �����ش��������ݳ���
	virtual void parseMessage() = 0;
	void close(const char* reason);

public:
	ServerConnection(int connID, tcp::socket& socket, ConnCloseCallback closeCallback);
	virtual ~ServerConnection();

	tcp::socket& getSocket();
	int getConnID() const;

	inline void startRead() { _read(); }
	void send(const char* data, int len);

	//void destroy();
	
};

NS_GAME_NET_END
