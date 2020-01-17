#include "MessageHandler.h"
#include "Network.h"

#include "proto.h"
#include "Logger.h"
#include "ZmqInst.h"


int readInt(char* data) {
	return *(int*)data;
}

void writeInt(std::vector<char>* data, int val) {
	char* p = (char*)& val;
	for (int i = 3; i >= 0; i--) {
		data->push_back(p[i]);
	}
}

void writeIntEx(std::vector<char>* data, int val) {
	char* p = (char*)& val;
	for (int i = 0; i < 4; i++) {
		data->push_back(p[i]);
	}
}

void MessageHandler::onRecvData(char* sender, char* data, int dataLen) {
	if (dataLen <= 8) {
		Logger::logError("$recv msg format error, data len < 8");
		return;
	}
	int connId = readInt(data);
	int msgId = readInt(&data[4]);
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

