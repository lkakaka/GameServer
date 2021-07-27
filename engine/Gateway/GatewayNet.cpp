#include "GatewayNet.h"
#include "GatewayConnection.h"
#include "proto.h"

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
	GatewayConnection* gatewayConn = (GatewayConnection*)conn;
	ClientDisconnect msg;
	msg.set_conn_id(conn->getConnID());
	msg.set_reason(reason);
	std::string msgData;
	msg.SerializeToString(&msgData);
	gatewayConn->dispatchClientMsg(MSG_ID_CLIENT_DISCONNECT, msgData.length(), msgData.c_str());
}
