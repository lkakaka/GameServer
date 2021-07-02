#include "GatewayNet.h"
#include "GatewayConnection.h"

INIT_SINGLETON_CLASS(GatewayNet)

GatewayNet::GatewayNet()
{
}

ServerConnection* GatewayNet::onAccept(tcp::socket& socket) {
	int connId = allocConnID();
	GatewayConnection* conn = new GatewayConnection(connId, socket, std::bind(&ServerNetwork::closeConnection, this, std::placeholders::_1, std::placeholders::_2));
	return conn;
}

void GatewayNet::onCloseConnection(ServerConnection* conn, const char* reason) {

}
