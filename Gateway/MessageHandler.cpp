#include "MessageHandler.h"
#include "Network.h"

#include "proto.h"
#include "Logger.h"
#include "ZmqInst.h"


void MessageHandler::onRecvData(char* sender, char* data, int dataLen) {
	if (dataLen <= 8) {
		Logger::logError("$recv msg format error, data len < 8");
		return;
	}
	MyBuffer buffer(data, dataLen);
	int connId = buffer.readIntEx();
	int msgId = buffer.readIntEx();
	TcpConnection* connection = Network::getConnById(connId);
	if (connection == NULL) {
		Logger::logError("$send packet error, connId(%d) is not exist, msgId:%d", connId, msgId);
		return;
	}
	connection->sendMsgToClient(msgId, &data[8], dataLen - 8);

	if (msgId == MSG_ID_DISCONNECT) {
		Network::getNetworkInstance()->removeConnection(connId, "server disconnect");
	}
	Logger::logInfo("$recv msg, sender:%s,  msgId:%d", sender, msgId);
}

