#include "GatewayMessageHandler.h"
#include "proto.h"
#include "Logger.h"
#include "GatewayNet.h"
#include "GatewayConnection.h"
#include "../Common/ServerMacros.h"

INIT_SINGLETON_CLASS(GatewayMessageHandler)

void handMsg_ClientDisconnect(int msgId, int connId, char* data, int dataLen) {
	std::shared_ptr<google::protobuf::Message> msg = createMessage(msgId, data, dataLen);
	ClientDisconnect* recvMsg = (ClientDisconnect*)msg.get();
	GatewayNet::getSingleton()->removeConnection(recvMsg->conn_id(), recvMsg->reason().c_str());
	Logger::logInfo("recv client disconnect msg, connId:%d", connId);
}

void handMsg_SwitchSceneService(int msgId, int connId, char* data, int dataLen) {
	std::shared_ptr<google::protobuf::Message> msg = createMessage(msgId, data, dataLen);
	SwitchSceneService* recvMsg = (SwitchSceneService*)msg.get();
	GatewayConnection* conn = (GatewayConnection*)GatewayNet::getSingleton()->getConnection(recvMsg->conn_id());
	if (conn == NULL) Logger::logError("$SwitchSceneService not found conn:%d", recvMsg->conn_id());
	conn->setSceneServiceId(recvMsg->scene_service_id());
}

// 消息默认处理发给客户端
void handMsg_Default(int msgId, int connId, send_type type, char* data, int dataLen) {
	GatewayConnection* conn = (GatewayConnection*)GatewayNet::getSingleton()->getConnection(connId);
	if (conn == NULL) {
		Logger::logError("$send msg fail, connId(%d) is not exist, msgId:%d", connId, msgId);
		return;
	}
	conn->sendMsgToClient(type, msgId, data, dataLen);
	Logger::logInfo("$send msg to client, msgId:%d, connId:%d", msgId, connId);
}

void handMsg(int msgId, int connId, send_type type, char* data, int dataLen) {
	switch (msgId)
	{
		case MSG_ID_CLIENT_DISCONNECT:
			handMsg_ClientDisconnect(msgId, connId, data, dataLen);
			break;
		case MSG_ID_SWITCH_SCENE_SERVICE:
			handMsg_SwitchSceneService(msgId, connId, data, dataLen);
			break;
		default:
			handMsg_Default(msgId, connId, type, data, dataLen);
			break;
	}
}

void GatewayMessageHandler::onRecvMessage(ServiceAddr* sender, char* data, int dataLen) {
	MyBuffer buffer(data, dataLen);
	if (dataLen < 9) {
		/*int connId = -1;
		if (dataLen >= 4) connId = buffer.readInt();*/
		Logger::logError("$recv %s msg format error, data len(%d) < 9", sender->getName(), dataLen);
		return;
	}
	int msgId = buffer.readInt();
	int connId = buffer.readInt();
	send_type sendType = buffer.readByte();
	
	Logger::logInfo("$recv msg, sender:%s, msgId:%d, connId:%d", sender->getName(), msgId, connId);
	// 网关处理的消息
	handMsg(msgId, connId, sendType, &data[9], dataLen - 9);
}