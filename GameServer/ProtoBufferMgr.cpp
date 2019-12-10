#include "ProtoBufferMgr.h"
#include "Network.h"

#include "proto/proto.h"
#include "Logger.h"


inline int readInt(unsigned char* data) {
	return *(int*)data;
}

void writeInt(std::vector<unsigned char>* data, int val) {
	char* p = (char*)& val;
	for (int i = 3; i >= 0; i--) {
		data->push_back(p[i]);
	}
}

void handleMsg(int connId, int msgId, google::protobuf::Message* msg)
{
	switch (msgId)
	{
	case MSG_ID_TEST:
	{
		Test* recvMsg = (Test*)msg;
		Logger::logInfo("$receive test proto, id:%d, msg:%s", recvMsg->id(), recvMsg->msg().data());

		Test resp_msg;
		resp_msg.set_id(1);
		resp_msg.set_msg("world");
		ProtoBufferMgr::sendPacket(connId, MSG_ID_TEST, &resp_msg);
		break;
	}
	case MSG_ID_LOGIN:
	{
		Login* recvMsg = (Login*)msg;
		Logger::logInfo("$receive login proto, account:%s, pwd:%s", recvMsg->account().c_str(), recvMsg->pwd().data());

		LoginRsp resp_msg;
		resp_msg.set_account(recvMsg->account());
		resp_msg.set_user_id(1);
		ProtoBufferMgr::sendPacket(connId, MSG_ID_LOGINRSP, &resp_msg);
		break;
	}
	default:
		Logger::logError("$receive unknown proto, msgId:%d", msgId);
		break;
	}
}

int ProtoBufferMgr::parseProtoData(int connId, std::vector<unsigned char> *recData)
{
	unsigned char* data = recData->data();
	int dataLen = recData->size();
	if (dataLen < 8) {
		return 0;
	}
	int msgLen = readInt(&data[4]);
	if (dataLen - 8 < msgLen) {
		return 0;
	}
	int msgId = readInt(data);
	google::protobuf::Message* msg = (google::protobuf::Message*)CreateMsgById(msgId);
	if (msg == NULL) {
		Logger::logError("$create proto msg error, msgId:%d", msgId);
		return msgLen + 8;
	}
	msg->ParseFromArray(&data[8], msgLen);
	handleMsg(connId, msgId, msg);
	return msgLen + 8;
}

void ProtoBufferMgr::sendPacket(int connID, int msgId, google::protobuf::Message* msg) {
	std::string msgData;
	msg->SerializeToString(&msgData);
	int msgLen = msgData.size() + 8;
	std::vector<unsigned char> data;
	writeInt(&data, msgLen);
	writeInt(&data, msgId);
	std::copy(msgData.begin(), msgData.end(), std::back_inserter(data));
	TcpConnection* connection = Network::getConnById(connID);
	if (connection == NULL) {
		return;
	}
	connection->sendData(std::move(data), data.size());
}
