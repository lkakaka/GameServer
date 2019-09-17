#include "TcpConnection.h"

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
		Log::logError("socket close error, %s", e.what());
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
			Log::logError("$close connection, %s", err_str);
			m_closeFunc(getConnID());
			return;
		}
		if (datLen > 0)
		{
			Log::logInfo("$receive data, len:%d, %s\n", datLen, m_vecData.data());

			std::string echo = "server echo:";
			std::vector<unsigned char>data;
			std::copy(echo.begin(), echo.end(), std::back_inserter(data));

			sendData(std::move(data), data.size());
		}
		else {
			Log::logInfo("$receive data len is 0");
		}
		this->doRead();
	});
}

void TcpConnection::sendData(std::vector<unsigned char>&& dat, size_t datLen)
{
	boost::asio::const_buffer buf(&dat.front(), datLen);
	m_socket.async_write_some(buf, [](boost::system::error_code err_code, size_t datLen) {
		Log::logInfo("$send data len:%d", datLen);
	});
}

void TcpConnection::doShutDown()
{
	try {
		this->m_socket.shutdown(m_socket.shutdown_both);
	}
	catch (boost::system::system_error e) {
		Log::logError("socket shutdown error, %s", e.what());
	}
}