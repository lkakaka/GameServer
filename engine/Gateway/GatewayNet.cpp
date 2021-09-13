#include "GatewayNet.h"
#include "GatewayConnection.h"
#include "proto.h"

INIT_SINGLETON_CLASS(GatewayNet)

#define UDP_BUFF_SIZE 2048

GatewayNet::GatewayNet(boost::asio::io_service* io) : ServerNetwork(io), m_udp(NULL)
{
}

ServerConnection* GatewayNet::onAccept(std::shared_ptr<tcp::socket> sock) {
	int connId = allocConnID();
	GatewayConnection* conn = new GatewayConnection(connId, sock, std::bind(&ServerNetwork::closeConnection, this, std::placeholders::_1, std::placeholders::_2));
	return conn;
}

void GatewayNet::onCloseConnection(ServerConnection* conn, const char* reason) {
	GatewayConnection* gatewayConn = (GatewayConnection*)conn;
	ClientDisconnect msg;
	msg.set_conn_id(conn->getConnID());
	msg.set_reason(reason);
	std::string msgData;
	msg.SerializeToString(&msgData);
	gatewayConn->dispatchClientMsg(MSG_ID_CLIENT_DISCONNECT, msgData.length(), msgData.c_str());
}

void GatewayNet::startUdp(int udpPort) {
	try {
		udp::endpoint ep(udp::v4(), udpPort);
		m_udp.reset(new udp::socket(*getIOService(), ep));
		m_udp->set_option(boost::asio::ip::udp::socket::reuse_address(true));
		//m_udp->open(ep.protocol());
		//m_udp->bind(udp::endpoint(udp::v4(), udpPort));
	}
	catch (std::exception& e) {
		Logger::logError("$%s, port:%d", e.what(), udpPort);
		throw(e);
	}
	m_udpReadBuf.assign(UDP_BUFF_SIZE, 0);
	recvUdpMsg();
}

void GatewayNet::recvUdpMsg() {
	auto buf = boost::asio::buffer(m_udpReadBuf, m_udpReadBuf.size());
	udp::endpoint remotePoint;
	m_udp->async_receive_from(buf, remotePoint, [buf, &remotePoint, this](const boost::system::error_code& error, size_t bytes_transferred) {
		if (error)
		{
			const std::string err_str = error.message();
			Logger::logError("$upd msg error, %s", err_str.c_str());
			return;
		}

		if (bytes_transferred > 0)
		{
			/*if (remotePoint.address().is_v6()) {
				Logger::logDebug("$ipv6");
			}*/
			std::string clientIp = remotePoint.address().to_string();
			unsigned short clientPort = remotePoint.port();
			Logger::logDebug("$receive udp data from ip:%s, port:%d, len=%d", clientIp.c_str(), clientPort, bytes_transferred);

			handleUdpMsg(bytes_transferred);
		}
		else {
			Logger::logInfo("$receive udp data len is 0");
		}

		//try {
		//	char* s = "nihao";
		//	auto sbuf = boost::asio::buffer(buf, bytes_transferred);
		//	m_udp->send_to(sbuf, udp::endpoint(boost::asio::ip::address_v4::from_string("127.0.0.1"), 8888));
		//	//m_udp->send_to(sbuf, remotePoint);
		//}
		//catch (std::exception& e) {
		//	Logger::logError("$%s", e.what());
		//	//throw(e);
		//}

		recvUdpMsg();
	});
}

void GatewayNet::handleUdpMsg(int len) {
	if (len <= 4) return;
	// KCP»á»°ID
	int connId = *((unsigned int*)m_udpReadBuf.data());
	GatewayConnection* conn = (GatewayConnection*)getConnection(connId);
	if (conn == NULL) {
		Logger::logError("$udp msg, not found connection: %d", connId);
		return;
	}
	conn->onRecvKCPMsg(m_udpReadBuf.data(), len);
}
