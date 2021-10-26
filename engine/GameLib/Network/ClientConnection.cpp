#include "ClientConnection.h"
#include "Logger.h"
#include "../Timer.h"
#include "../AsioService.h"

#define READ_BUFFER_SIZE 1024

#define SEND_BUFFER_WARNING_SIZE 16 * 1024 * 1024

USE_NS_GAME_NET

ClientConnection::ClientConnection(const char* server_ip, int server_port) : 
	ConnectionBase(NULL, false),
	m_serverIp(server_ip), m_serverPort(server_port)
{
	m_readBuff.resize(READ_BUFFER_SIZE);
	m_readBuff.assign(m_readBuff.size(), 0);
	m_socket.reset(new tcp::socket(MAIN_IO));

	//TimerMgr::getSingleton()->addTimer(1000, 1000, -1, [](int timerId) { LOG_INFO("timer"); });
}

bool ClientConnection::connect() {
	if (m_isConnected) return true;
	try {
		tcp::endpoint target(boost::asio::ip::make_address(m_serverIp), m_serverPort);
		m_socket->async_connect(target, std::bind(&ClientConnection::connectHandler, this, std::placeholders::_1));

		/*m_socket->connect(target);
		m_isConnected = true;*/
	}
	catch (std::exception& e) {
		LOG_ERROR("connect failed, %s", e.what());
		return false;
	}
	/*onConnect();
	_read();
	_trySend();*/
	return true;
}

void ClientConnection::connectHandler(boost::system::error_code ec) {
	if (ec) {
		//boost::asio::error::connection_refused;
		LOG_ERROR("connect failed, %d, %s", ec.value(), ec.message());
		connect();
		return;
	}
	m_isConnected = true;
	onConnect();
	_read();
	_trySend();
}

void ClientConnection::tryConnect() {
	//if (connect()) return;
	/*std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	tryConnect();*/

	/*m_connectTimer = TimerMgr::getSingleton()->addTimer(1000, 1000, -1, [this](int timerId) {
		if (connect()) {
			TimerMgr::getSingleton()->removeTimer(timerId, true);
		}
	});*/
	m_connectThread.reset(new std::thread([this] {
		while (1) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			if (connect()) {
				break;
			}
		}
		// 必须先detach,析构joinable的线程会crash
		m_connectThread.get()->detach();
		m_connectThread.reset();
	}));
}

void ClientConnection::_read() {
	m_readBuff.assign(m_readBuff.size(), 0);
	auto buf = boost::asio::buffer(m_readBuff, m_readBuff.size());
	m_socket->async_receive(buf, [this, buf](const boost::system::error_code error, size_t datLen) {
		if (error.value())
		{
			const std::string err_str = error.message();
			LOG_ERROR("close connection, %s", err_str.data());
			disConnect();
			return;
		}
		if (datLen > 0)
		{
			LOG_DEBUG("receive data, len:%d, %s", datLen, buf.data());
			onRecvData(m_readBuff, datLen);
		}
		if (m_socket->is_open()) _read();
	});
}

void ClientConnection::disConnect() {
	m_socket->shutdown(boost::asio::socket_base::shutdown_type::shutdown_both);
	m_isConnected = false;
	m_socket.reset(new tcp::socket(MAIN_IO));
	connect();
}
