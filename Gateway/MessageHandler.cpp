#include "MessageHandler.h"
#include "Network.h"

#include "proto.h"
#include "Logger.h"
#include "ZmqInst.h"


std::shared_ptr<google::protobuf::Message> createMessage(int msgId, char* data, int dataLen)
{
	std::shared_ptr<google::protobuf::Message> msg = CreateMsgById(msgId);
	if (msg == NULL) {
		Logger::logError("$create proto msg error, msgId:%d", msgId);
		return msg;
	}
	msg->ParseFromArray(data, dataLen);
	return msg;
}

void MessageHandler::onRecvData(char* sender, char* data, int dataLen) {
	if (dataLen <= 8) {
		Logger::logError("$recv msg format error, data len < 8");
		return;
	}
	MyBuffer buffer(data, dataLen);
	int connId = buffer.readInt(true);
	int msgId = buffer.readInt(true);
	if (msgId == MSG_ID_CLIENT_DISCONNECT) {
		std::shared_ptr<google::protobuf::Message> msg = createMessage(msgId, &data[8], dataLen - 8);
		ClientDisconnect* recvMsg = (ClientDisconnect*)msg.get();
		Network::getNetworkInstance()->removeConnection(recvMsg->conn_id(), recvMsg->reason().c_str());
	}
	else {
		TcpConnection* connection = Network::getConnById(connId);
		if (connection == NULL) {
			Logger::logError("$send packet error, connId(%d) is not exist, msgId:%d", connId, msgId);
			return;
		}
		connection->sendMsgToClient(msgId, &data[8], dataLen - 8);
	}

	Logger::logInfo("$recv msg, sender:%s,  msgId:%d", sender, msgId);
}

