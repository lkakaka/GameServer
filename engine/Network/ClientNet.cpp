#include "ClientNet.h"
#include "Logger.h"

#define READ_BUFFER_SIZE 1024

#define SEND_BUFFER_WARNING_SIZE 16 * 1024 * 1024

USE_NS_GAME_NET

ClientNet::ClientNet(boost::asio::io_context* io, const char* server_ip, int server_port) : m_io(io), m_serverIp(server_ip), m_serverPort(server_port), 
	m_isConnected(false), m_isSending(false)
{
	m_readBuff.resize(READ_BUFFER_SIZE);
	m_readBuff.assign(m_readBuff.size(), 0);
	m_socket.reset(new tcp::socket(*m_io));
}

bool ClientNet::connect() {
	if (m_isConnected) return true;
	try {
		tcp::endpoint target(boost::asio::ip::make_address(m_serverIp), m_serverPort);
		m_socket->connect(target);
		m_isConnected = true;
	}
	catch (std::exception e) {
		Logger::logError("$connect failed, %s", e.what());
		return false;
	}
	onConnect();
	_read();
	_send();
	return true;
}

void ClientNet::tryConnect() {
	if (connect()) return;
	m_connectThread.reset(new std::thread([this] {
		while (1) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			if (connect()) break;
		}
	}));
}

void ClientNet::_read() {
	m_readBuff.assign(m_readBuff.size(), 0);
	auto buf = boost::asio::buffer(m_readBuff, m_readBuff.size());
	m_socket->async_receive(buf, [this, buf](const boost::system::error_code error, size_t datLen) {
		if (error.value())
		{
			const std::string err_str = error.message();
			Logger::logError("$close connection, %s", err_str.data());
			disConnect();
			return;
		}
		if (datLen > 0)
		{
			Logger::logDebug("$receive data, len:%d, %s", datLen, buf.data());
			onRecvData(m_readBuff, datLen);
		}
		if (m_socket->is_open()) _read();
	});
}

void ClientNet::disConnect() {
	m_socket->shutdown(boost::asio::socket_base::shutdown_type::shutdown_both);
	m_isConnected = false;
}


bool ClientNet::send(const char* data, int len) {
	if (m_sendBuff.size() >= SEND_BUFFER_WARNING_SIZE) {
		Logger::logWarning("$send buffer too large, size£º%d", m_sendBuff.size());
	}
	std::copy(data, data + len, std::back_inserter(m_sendBuff));
	_send();
	return true;
}

void ClientNet::_send() {
	if (m_sendBuff.size() == 0) return;
	boost::asio::const_buffer buf(&m_sendBuff.front(), m_sendBuff.size());
	size_t len = m_socket->write_some(buf);
	if (len > 0) {
		m_sendBuff.erase(m_sendBuff.begin(), m_sendBuff.begin() + len);
		_send();
	}

	/*if (m_isSending) return;
	if (m_sendBuff.size() == 0) return;
	m_isSending = true;
	boost::asio::const_buffer buf(&m_sendBuff.front(), m_sendBuff.size());
	m_socket->async_write_some(buf, [this](const boost::system::error_code err_code, size_t datLen) {
		m_isSending = false;
		if (err_code)
		{
			const std::string err_str = err_code.message();
			Logger::logError("$send data error, %s", err_str.data());
			return;
		}

		if (datLen > 0) {
			m_sendBuff.erase(m_sendBuff.begin(), m_sendBuff.begin() + datLen);
			Logger::logDebug("$send data, len:%d", datLen);
			_send();
		}
	});*/
}