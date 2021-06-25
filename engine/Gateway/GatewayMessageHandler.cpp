#include "GatewayMessageHandler.h"
#include "proto.h"
#include "Logger.h"
#include "Network.h"


INIT_SINGLETON_CLASS(GatewayMessageHandler)


void handMsg_ClientDisconnect(int msgId, int connId, char* data, int dataLen) {
	std::shared_ptr<google::protobuf::Message> msg = createMessage(msgId, data, dataLen);
	ClientDisconnect* recvMsg = (ClientDisconnect*)msg.get();
	Network::getNetworkInstance()->removeConnection(recvMsg->conn_id(), recvMsg->reason().c_str());
	Logger::logInfo("recv client disconnect msg, connId:%d", connId);
}

void handMsg_SwitchSceneService(int msgId, int connId, char* data, int dataLen) {
	std::shared_ptr<google::protobuf::Message> msg = createMessage(msgId, data, dataLen);
	SwitchSceneService* recvMsg = (SwitchSceneService*)msg.get();
	TcpConnection* conn = Network::getNetworkInstance()->getConnById(recvMsg->conn_id());
	if (conn == NULL) Logger::logError("$SwitchSceneService not found conn:%d", recvMsg->conn_id());
	conn->setSceneServiceId(recvMsg->scene_service_id());
}

// 消息默认处理发给客户端
void handMsg_Default(int msgId, int connId, char* data, int dataLen) {
	TcpConnection* connection = Network::getConnById(connId);
	if (connection == NULL) {
		Logger::logError("$send msg fail, connId(%d) is not exist, msgId:%d", connId, msgId);
		return;
	}
	connection->sendMsgToClient(msgId, data, dataLen);
	Logger::logInfo("$send msg to client, msgId:%d, connId:%d", msgId, connId);
}

void handMsg(int msgId, int connId, char* data, int dataLen) {
	switch (msgId)
	{
		case MSG_ID_CLIENT_DISCONNECT:
			handMsg_ClientDisconnect(msgId, connId, data, dataLen);
			break;
		case MSG_ID_SWITCH_SCENE_SERVICE:
			handMsg_SwitchSceneService(msgId, connId, data, dataLen);
			break;
		default:
			handMsg_Default(msgId, connId, data, dataLen);
			break;
	}
}

void GatewayMessageHandler::onRecvMessage(char* sender, char* data, int dataLen) {
	MyBuffer buffer(data, dataLen);
	if (dataLen < 8) {
		int connId = -1;
		if (dataLen >= 4) connId = buffer.readInt();
		Logger::logError("$recv %s msg format error, data len(%d) < 8, connId:%d", sender, dataLen, connId);
		return;
	}
	int connId = buffer.readInt();
	int msgId = buffer.readInt();
	
	Logger::logInfo("$recv msg, sender:%s, msgId:%d, connId:%d", sender, msgId, connId);
	// 网关处理的消息
	handMsg(msgId, connId, &data[8], dataLen - 8);
}