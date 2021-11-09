
#include "ServerConnection.h"
#include "Logger.h"

USE_NS_GAME_NET

#define READ_BUFF_SIZE 1024

ServerConnection::ServerConnection(int connID, std::shared_ptr<tcp::socket> socket, ConnCloseCallback closeCallback) :
	ConnectionBase(socket, true),
	m_waitClosed(false),
	m_connID(connID),
	m_closeCallback(closeCallback)
{
	m_readBuf.resize(READ_BUFF_SIZE);
}

ServerConnection::~ServerConnection()
{
	try {
		m_isConnected = false;
		this->m_socket->close();
	}catch (boost::system::system_error e) {
		LOG_ERROR("socket close error, %s", e.what());
	}
	LOG_DEBUG("delete ServerConnection: connId:%d", m_connID);
}

int ServerConnection::getConnID() const
{
	return m_connID;
}

void ServerConnection::_read()
{
	m_readBuf.assign(READ_BUFF_SIZE, 0);
	auto buf = boost::asio::buffer(m_readBuf, READ_BUFF_SIZE);
	m_socket->async_receive(buf, [buf, this](const boost::system::error_code& error, size_t bytes_transferred) {
		if (error)
		{
			const std::string err_str = error.message();
			LOG_ERROR("close connection, %s", err_str.c_str());
			close("client disconnected");
			return;
		}

		if (bytes_transferred > 0)
		{
			//LOG_INFO("receive data, len:%d, %s", datLen, m_vecData.data());
			//m_readBuf.append(m_vecData, bytes_transferred);
			//onRead(m_readBuf.data(), bytes_transferred);
			LOG_DEBUG("receive data, len=%d", bytes_transferred);
			m_recvBuffer.writeString(m_readBuf.data(), bytes_transferred);
			parseMessage();
		} else {
			LOG_ERROR("receive data len is 0");
		}

		checkRecvBufferSize();

		if (m_waitClosed) {
			close(m_waitCloseReason.c_str());
			return;
		}

		if (m_isConnected) {
			this->_read();
		}
	});
}

void ServerConnection::checkRecvBufferSize() {
	int size = m_recvBuffer.size();
	if (size >= SERVER_RECV_BUFF_WARN_SIZE) {
		LOG_WARN("cache too much recv data!!!, size:%d", size);
	}

	if (size >= SERVER_RECV_MAX_BUFF_SIZE) {
		setWaitClose("cache too much data");
	}
}

void ServerConnection::setWaitClose(const char* reason) { 
	m_waitCloseReason = reason;  
	m_waitClosed = true; 
}

void ServerConnection::close(const char* reason) {
	if (!m_isConnected) return;
	m_isConnected = false;
	m_closeCallback(this, reason);
	//LOG_INFO("connection close, id:%d, reason:%s", m_connID, reason);
}
//
//void ServerConnection::destroy() {
//	try {
//		m_socket.shutdown(m_socket.shutdown_both);
//	}
//	catch (boost::system::system_error e) {
//		LOG_ERROR("socket shutdown error, %s", e.what());
//	}
//}
