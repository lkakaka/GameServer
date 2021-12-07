#include "GatewayMessageHandler.h"
#include "proto.h"
#include "Logger.h"
#include "GatewayNet.h"
#include "GatewayConnection.h"
#include "../Common/ServerMacros.h"
#include "GameService.h"

INIT_SINGLETON_CLASS(GatewayMessageHandler)

void handMsg_ClientDisconnect(int msgId, char* data, int dataLen) {
	std::shared_ptr<google::protobuf::Message> msg = createMessage(msgId, data, dataLen);
	ClientDisconnect* recvMsg = (ClientDisconnect*)msg.get();
	int connId = recvMsg->conn_id();
	GatewayNet::getSingleton()->removeConnection(connId, recvMsg->reason().c_str());
	LOG_INFO("recv client disconnect msg, connId:%d", connId);
}

void handMsg_SwitchSceneService(int msgId, char* data, int dataLen) {
	std::shared_ptr<google::protobuf::Message> msg = createMessage(msgId, data, dataLen);
	SwitchSceneService* recvMsg = (SwitchSceneService*)msg.get();
	GatewayConnection* conn = (GatewayConnection*)GatewayNet::getSingleton()->getConnection(recvMsg->conn_id());
	if (conn == NULL) {
		LOG_ERROR("SwitchSceneService not found conn:%d", recvMsg->conn_id());
		return;
	}
	conn->setSceneServiceId(recvMsg->scene_service_id());
}

void handMsg_StartKCP(send_type type, int msgId, int connId, char* data, int dataLen) {
	std::shared_ptr<google::protobuf::Message> msg = createMessage(msgId, data, dataLen);
	StartKcp* recvMsg = (StartKcp*)msg.get();
	GatewayConnection* conn = (GatewayConnection*)GatewayNet::getSingleton()->getConnection(connId);
	if (conn == NULL) {
		LOG_ERROR("StartKCP not found conn:%d", connId);
		return;
	}
	conn->enableKCP(const_cast<std::string&>(recvMsg->token()));
	conn->sendMsgToClient(type, msgId, data, dataLen);
	LOG_INFO("send msg to client, msgId:%d, connId:%d", msgId, connId);
}


// 消息默认处理发给客户端
void handMsg_Default(int msgId, int connId, send_type type, char* data, int dataLen) {
	GatewayConnection* conn = (GatewayConnection*)GatewayNet::getSingleton()->getConnection(connId);
	if (conn == NULL) {
		LOG_ERROR("send msg fail, connId(%d) is not exist, msgId:%d", connId, msgId);
		return;
	}
	conn->sendMsgToClient(type, msgId, data, dataLen);
	LOG_INFO("send msg to client, msgId:%d, connId:%d", msgId, connId);
}

void handClientMsg(int msgId, int connId, send_type type, char* data, int dataLen) {
	switch (msgId)
	{
		/*case MSG_ID_CLIENT_DISCONNECT:
			handMsg_ClientDisconnect(msgId, data, dataLen);
			break;
		case MSG_ID_SWITCH_SCENE_SERVICE:
			handMsg_SwitchSceneService(msgId, data, dataLen);
			break;*/
		case MSG_ID_START_KCP:
			handMsg_StartKCP(type, msgId, connId, data, dataLen);
			break;
		default:
			handMsg_Default(msgId, connId, type, data, dataLen);
			break;
	}
}

void handServiceMsg(ServiceAddr* sender, int msgId, char* data, int dataLen) {
	switch (msgId)
	{
	case MSG_ID_CLIENT_DISCONNECT:
		handMsg_ClientDisconnect(msgId, data, dataLen);
		break;
	case MSG_ID_SWITCH_SCENE_SERVICE:
		handMsg_SwitchSceneService(msgId, data, dataLen);
		break;
	default:
		GameService::getSingleton()->dispatchServiceMsgToScript(sender, msgId, data, dataLen);
		break;
	}
}

void GatewayMessageHandler::onRecvMessage(ServiceAddr* sender, char* data, int dataLen) {
	MyBuffer buffer(data, dataLen);
	if (dataLen < 5) {
		/*int connId = -1;
		if (dataLen >= 4) connId = buffer.readInt();*/
		LOG_ERROR("recv %s msg format error, data len(%d) < 5", sender->getName(), dataLen);
		return;
	}
	bool isToClient = (buffer.readByte() == 1);
	int msgId = buffer.readInt();
	
	if (isToClient) {
		int connCount = buffer.readInt();
		if (connCount < 0 || dataLen < 10 + connCount * 4) {
			LOG_ERROR("recv %s msg format error, data len:%d, connCount:%d", sender->getName(), dataLen, connCount);
			return;
		}

		int iDataOffset = 10;
		std::vector<int> connIds;
		for (int i = 0; i < connCount; i++) {
			connIds.emplace_back(buffer.readInt());
			iDataOffset += 4;
		}
		//int connId = buffer.readInt();
		send_type sendType = buffer.readByte();

		LOG_INFO("recv msg, sender:%s, msgId:%d, connCount:%d", sender->getName(), msgId, connCount);
		// 网关处理的消息
		for (int connId : connIds) {
			handClientMsg(msgId, connId, sendType, &data[iDataOffset], dataLen - iDataOffset);
		}
	}
	else {
		// TODO: 处理服务器之间的消息
		char* msgData = (char*)buffer.data();
		int msgLen = buffer.size();
		LOG_DEBUG("recv msg, sender:%s,  msgId:%d, msgLen:%d", sender->getName(), msgId, msgLen);
		handServiceMsg(sender, msgId, msgData, msgLen);
	}
}