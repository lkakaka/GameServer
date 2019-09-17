#pragma once
#include "boost/asio.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "Log.h"

typedef std::function<void(int)> closeFuncType;

using boost::asio::ip::tcp;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
private:
	tcp::socket m_socket;
	std::vector<unsigned char>	m_vecData;
	int m_connID;
	//std::shared_ptr<Network> m_network;
	closeFuncType m_closeFunc;

public:
	TcpConnection(boost::asio::io_service& io, int connID, closeFuncType closeFunc);
	~TcpConnection();

	tcp::socket& getSocket();
	int getConnID() const;
	void doRead();
	void sendData(std::vector<unsigned char>&& dat, size_t datLen);
	void doShutDown();
};

