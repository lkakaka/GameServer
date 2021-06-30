
#include "Connection.h"
#include "Logger.h"

USE_NS_GAME_NET

Connection::Connection(int connID, tcp::socket& socket, ConnCloseCallback closeCallback) :
	m_isClosed(false), m_isSending(false),
	m_connID(connID),
	m_socket(std::move(socket)),
	m_closeCallback(closeCallback)
{
	
}

Connection::~Connection()
{
	/*try {
		this->m_socket.close();
	}catch (boost::system::system_error e) {
		Logger::logError("socket close error, %s", e.what());
	}*/
	//printf("delete TcpConnection\n");
}

tcp::socket& Connection::getSocket()
{
	return m_socket;
}

int Connection::getConnID() const
{
	return m_connID;
}

void Connection::doRead()
{
	m_readBuf.resize(1024);
	m_readBuf.assign(m_readBuf.size(), 0);
	auto buf = boost::asio::buffer(m_readBuf, m_readBuf.size());
	m_socket.async_receive(buf, [buf, this](const boost::system::error_code& error, size_t bytes_transferred) {
		if (error)
		{
			const std::string err_str = error.message();
			Logger::logError("$close connection, %s", err_str.c_str());
			m_closeCallback(this, "client disconnected");
			return;
		}
		if (bytes_transferred > 0)
		{
			//Logger::logInfo("$receive data, len:%d, %s", datLen, m_vecData.data());
			//m_readBuf.append(m_vecData, bytes_transferred);
			onRead(m_readBuf.data(), bytes_transferred);
		}
		else {
			Logger::logInfo("$receive data len is 0");
		}
		if (!m_isClosed) {
			this->doRead();
		}
	});
}

void Connection::send(const char* data, int len) {
	std::copy(data, data + len, std::back_inserter(m_sendBuf));
	_send();
}

void Connection::_send() {
	if (m_sendBuf.size() == 0) return;
	boost::asio::const_buffer buf(&m_sendBuf.front(), m_sendBuf.size());
	size_t len = m_socket.write_some(buf);
	if (len > 0) {
		m_sendBuf.erase(m_sendBuf.begin(), m_sendBuf.begin() + len);
		_send();
	}

	/*if (m_isSending) return;
	if (m_sendBuf.size() == 0) return;
	m_isSending = true;
	boost::asio::const_buffer buf(&m_sendBuf.front(), m_sendBuf.size());
	m_socket.async_write_some(buf, [this](const boost::system::error_code err_code, size_t datLen) {
		m_isSending = false;
		if (err_code)
		{
			const std::string err_str = err_code.message();
			Logger::logError("$send data error, %s", err_str.data());
			return;
		}

		if (datLen > 0) {
			m_sendBuf.erase(m_sendBuf.begin(), m_sendBuf.begin() + datLen);
			Logger::logDebug("$send data, len:%d", datLen);
			_send();
		}
		});*/
}

void Connection::close() {
	m_isClosed = true;
	try {
		m_socket.shutdown(m_socket.shutdown_both);
	}
	catch (boost::system::system_error e) {
		Logger::logError("socket shutdown error, %s", e.what());
	}
	Logger::logInfo("$connection close, id:%d", m_connID);
}
