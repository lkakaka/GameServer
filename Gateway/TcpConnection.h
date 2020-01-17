#pragma once
#include "boost/asio.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "Logger.h"

typedef std::function<void(int, const char*)> ConnCloseFunc;

using boost::asio::ip::tcp;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
private:
	tcp::socket m_socket;
	std::vector<char>	m_vecData;
	std::vector<char> m_readData;
	int m_connID;
	//std::shared_ptr<Network> m_network;
	ConnCloseFunc m_closeFunc;

	void parseRecvData();

public:
	TcpConnection(boost::asio::io_service& io, int connID, ConnCloseFunc closeFunc);
	~TcpConnection();

	tcp::socket& getSocket();
	int getConnID() const;
	void doRead();
	int parseProtoData();
	void dispatchMsg(int msgId, int msgLen, const char* msgData);
	void sendMsgToClient(int msgId, char* data, int dataLen);
	void sendData(std::vector<char>&& dat, size_t datLen);
	void doShutDown(const char* reason);
};

