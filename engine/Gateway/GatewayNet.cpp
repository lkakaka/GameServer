#include "GatewayNet.h"
#include "GatewayConnection.h"

INIT_SINGLETON_CLASS(GatewayNet)

GatewayNet::GatewayNet(boost::asio::io_service* io) : ServerNetwork(io)
{
}

ServerConnection* GatewayNet::onAccept(std::shared_ptr<tcp::socket> sock) {
	int connId = allocConnID();
	GatewayConnection* conn = new GatewayConnection(connId, sock, std::bind(&ServerNetwork::closeConnection, this, std::placeholders::_1, std::placeholders::_2));
	return conn;
}

void GatewayNet::onCloseConnection(ServerConnection* conn, const char* reason) {

}
