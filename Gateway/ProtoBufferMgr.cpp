#include "ProtoBufferMgr.h"
#include "Network.h"

#include "proto.h"
#include "Logger.h"
#include "ZmqInst.h"


inline int readInt(char* data) {
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

int ProtoBufferMgr::parseProtoData(int connId, std::vector<char> *recData)
{
	char* data = recData->data();
	int dataLen = recData->size();
	if (dataLen < 8) {
		return 0;
	}
	int msgLen = readInt(&data[4]);
	if (dataLen - 8 < msgLen) {
		return 0;
	}
	int msgId = readInt(data);
	dispatchMsg(connId, msgId, msgLen, recData);
	Logger::logInfo("$receive client msg, connId:%d, msgId:%d", connId, msgId);
	return msgLen + 8;
}

void ProtoBufferMgr::dispatchMsg(int connId, int msgId, int msgLen, std::vector<char>* recData) {
	if (msgId == MSG_ID_LOGIN) {

	}
	LoginRsp rsp;
	std::vector<char> tmp;
	writeIntEx(&tmp, connId);
	writeIntEx(&tmp, msgId);
	auto iter1 = recData->begin();
	std::advance(iter1, 8);
	auto iter2 = recData->begin();
	std::advance(iter2, msgLen + 8);
	std::copy(iter1, iter2, std::back_inserter(tmp));
	ZmqInst::getZmqInstance()->sendData("scene", tmp.data(), msgLen + 8);
}

void ProtoBufferMgr::onRecvData(char* sender, char* data, int dataLen) {
	if (dataLen <= 8) {
		Logger::logError("$recv msg format error, data len < 8");
		return;
	}
	int connId = readInt(data);
	int msgId = readInt(&data[4]);
	sendPacket(connId, msgId, &data[8], dataLen - 8);
	Logger::logInfo("$recv msg, sender:%s,  msgId:%d", sender, msgId);
}

void ProtoBufferMgr::sendPacket(int connID, int msgId, char* data, int dataLen) {
	int msgLen = dataLen + 8;
	std::vector<char> buff;
	writeInt(&buff, msgLen);
	writeInt(&buff, msgId);
	std::copy(data, data+dataLen, std::back_inserter(buff));
	TcpConnection* connection = Network::getConnById(connID);
	if (connection == NULL) {
		Logger::logError("$send packet error, connId(%d) is not exist, msgId:%d", connID, msgId);
		return;
	}
	connection->sendData(std::move(buff), buff.size());
}
