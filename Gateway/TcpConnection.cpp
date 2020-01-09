#include "TcpConnection.h"
#include "MessageHandler.h"

TcpConnection::TcpConnection(boost::asio::io_service& io, int connID, closeFuncType closeFunc):
	m_connID(connID),
	m_socket(io),
	m_closeFunc(closeFunc)
{

}

TcpConnection::~TcpConnection()
{
	try {
		this->m_socket.close();
	}catch (boost::system::system_error e) {
		Logger::logError("socket close error, %s", e.what());
	}
	printf("delete TcpConnection\n");
}

tcp::socket& TcpConnection::getSocket()
{
	return m_socket;
}

int TcpConnection::getConnID() const
{
	return m_connID;
}

void TcpConnection::doRead()
{	
	m_vecData.resize(1024);
	m_vecData.assign(m_vecData.size(), 0);
	auto buf = boost::asio::buffer(m_vecData, m_vecData.size());
	m_socket.async_receive(buf, [buf, this](const boost::system::error_code& error, size_t datLen) {
		if (error)
		{
			const std::string err_str = error.message();
			Logger::logError("$close connection, %s", err_str.data());
			m_closeFunc(getConnID());
			return;
		}
		if (datLen > 0)
		{
			//Logger::logInfo("$receive data, len:%d, %s", datLen, m_vecData.data());
			auto iter = m_vecData.begin();
			std::advance(iter, datLen);
			std::copy(m_vecData.begin(), iter, std::back_inserter(m_readData));
			parseRecvData();
		}
		else {
			Logger::logInfo("$receive data len is 0");
		}
		this->doRead();
	});
}

void TcpConnection::parseRecvData()
{
	int len = 0;
	do {
		len = MessageHandler::parseProtoData(m_connID, &m_readData);
		if (len > 0) {
			auto removeIter = m_readData.begin();
			std::advance(removeIter, len);
			m_readData.erase(m_readData.begin(), removeIter);
		}
	} while (len > 0);
}

void TcpConnection::sendData(std::vector<char>&& dat, size_t datLen)
{
	boost::asio::const_buffer buf(&dat.front(), datLen);
	m_socket.async_write_some(buf, [](boost::system::error_code err_code, size_t datLen) {
		if (err_code)
		{
			const std::string err_str = err_code.message();
			Logger::logError("$send data error, %s", err_str.data());
			//Logger::logInfo("$send data len:%d", datLen);
		}
	});
}

void TcpConnection::doShutDown()
{
	try {
		this->m_socket.shutdown(m_socket.shutdown_both);
	}
	catch (boost::system::system_error e) {
		Logger::logError("socket shutdown error, %s", e.what());
	}
}