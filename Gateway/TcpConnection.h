#pragma once
#include "boost/asio.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "Logger.h"
#include "MyBuffer.h"
#include "../Common/ServerExports.h"

typedef std::function<void(int, const char*)> ConnCloseFunc;

using boost::asio::ip::tcp;

class SERVER_EXPORT_API TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
private:
	tcp::socket m_socket;
	std::vector<char> m_vecData;
	//std::vector<char> m_readBuf;
	MyBuffer m_readBuf;
	std::vector<char> m_sendBuf;
	int m_connID;
	bool m_isClosed;
	//std::shared_ptr<Network> m_network;
	ConnCloseFunc m_closeFunc;

	void doSend();

public:
	TcpConnection(boost::asio::io_service& io, int connID, ConnCloseFunc closeFunc);
	~TcpConnection();

	tcp::socket& getSocket();
	int getConnID() const;
	void doRead();
	void parsePacket();
	void dispatchClientMsg(int msgId, int msgLen, const char* msgData);
	void sendMsgToService(int msgId, int msgLen, const char* msgData, const char* serviceName);
	void sendMsgToClient(int msgId, char* data, int dataLen);
	void sendData(std::vector<char>& dat);
	void doShutDown(const char* reason);
	void close(const char* reason);
};

