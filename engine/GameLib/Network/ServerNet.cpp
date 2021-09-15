
#include "ServerNet.h"
#include "Logger.h"

USE_NS_GAME_NET

ServerNetwork::ServerNetwork(boost::asio::io_service* io) : m_acceptor(NULL), m_ioService(io)
{
	
}

void ServerNetwork::start(int port) {
	try {
		m_acceptor.reset(new tcp::acceptor(*m_ioService, tcp::endpoint(tcp::v4(), port)));
		doAccept();
	}
	catch (std::exception& e) {
		Logger::logError("$%s, port:%d", e.what(), port);
		throw(e);
	}
	Logger::logInfo("$start listen, port:%d", port);
}

void ServerNetwork::doAccept()
{
	std::shared_ptr<tcp::socket> psocket(new tcp::socket(*m_ioService));

	/*std::shared_ptr<TcpConnection> conn(new TcpConnection(*m_io, allocConnID(), std::bind(&Network::closeConnection, this, std::placeholders::_1, std::placeholders::_2)));
	std::shared_ptr<TcpConnection> conn;*/
	m_acceptor->async_accept(*psocket, std::bind(&ServerNetwork::acceptHandler, this, psocket, std::placeholders::_1));
}

static void closeInvalidSocket(std::shared_ptr<tcp::socket> sock) {
	try {
		sock->shutdown(sock->shutdown_both);
	}
	catch (std::exception& e) {
		Logger::logError("$invalid socket shutdown error, %s", e.what());
	}
	Logger::logInfo("$close invalid socket!!!");
}

void ServerNetwork::acceptHandler(std::shared_ptr<tcp::socket> sock, boost::system::error_code ec) {
	if (!ec) {
		//TcpConnection* conn = new TcpConnection(std::move(psocket.get()));
		ServerConnection* conn = onAccept(sock);
		/*std::string rsp = "hello world";
		std::vector<char> buf;
		std::copy(rsp.begin(), rsp.end(), std::back_inserter(buf));
		conn->sendData(buf);*/
		if (conn != NULL) {
			m_conns.emplace(std::make_pair(conn->getConnID(), conn));
			conn->startRead();
			try {
				std::string remoteIP = sock->remote_endpoint().address().to_string();
				conn->setClientIp(remoteIP);
				unsigned short clientPort = sock->remote_endpoint().port();
				Logger::logInfo("$client connected, ip:%s, port:%d", remoteIP.c_str(), clientPort);
			}
			catch (std::exception& e) {
				Logger::logInfo("$client connected, cannot get remote addr , e:%s", e.what());
			}
		} else { 
			closeInvalidSocket(sock);
		}
	}
	doAccept();
}

ServerConnection* ServerNetwork::getConnection(int connId) {
	auto iter = m_conns.find(connId);
	if (iter == m_conns.end()) return NULL;
	return iter->second.get();
}

void ServerNetwork::removeConnection(int connId, const char* reason) {
	auto iter = m_conns.find(connId);
	if (iter == m_conns.end()) return;
	closeConnection(iter->second.get(), reason);
}

void ServerNetwork::closeConnection(void* conn, const char* reason)
{
	ServerConnection* s_conn = (ServerConnection*)conn;
	int connId = s_conn->getConnID();
	Logger::logInfo("$close connection(%d), reason:%s", connId, reason);
	//decltype(m_conns.begin()->second) conn;

	onCloseConnection(s_conn, reason);

	auto iter = m_conns.find(connId);
	if (iter != m_conns.end()) {
		m_conns.erase(iter);
	}
	// 从上面的map移除后，connection就会析构
}
