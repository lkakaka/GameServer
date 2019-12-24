#include "ProtoBufferMgr.h"

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

void handleMsg(int connId, int msgId, std::shared_ptr<google::protobuf::Message> msg)
{
	switch (msgId)
	{
	case MSG_ID_TEST:
	{
		Test* recvMsg = (Test*)msg.get();
		Logger::logInfo("$receive test proto, id:%d, msg:%s", recvMsg->id(), recvMsg->msg().data());

		Test resp_msg;
		resp_msg.set_id(1);
		resp_msg.set_msg("world");
		ProtoBufferMgr::sendPacket(connId, MSG_ID_TEST, &resp_msg);
		break;
	}
	case MSG_ID_LOGIN:
	{
		Login* recvMsg = (Login*)msg.get();
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

void ProtoBufferMgr::onRecvData(char* sender, char* data, int dataLen) {
	if (dataLen <= 8) {
		Logger::logError("$recv msg format error, data len < 8");
		return;
	}
	int connId = readInt(data);
	int msgId = readInt(&data[4]);
	//google::protobuf::Message* msg = (google::protobuf::Message*)CreateMsgById(msgId);
	std::shared_ptr<google::protobuf::Message> msg = CreateMsgById(msgId);
	if (msg == NULL) {
		Logger::logError("$create proto msg error, msgId:%d", msgId);
		return;
	}
	msg->ParseFromArray(&data[8], dataLen - 8);
	handleMsg(connId, msgId, msg);
	Logger::logDebug("$recv msg, sender:%s,  msgId:%d", sender, msgId);
}

void ProtoBufferMgr::sendPacket(int connID, int msgId, google::protobuf::Message* msg) {
	std::string msgData;
	msg->SerializeToString(&msgData);
	int msgLen = msgData.size() + 8;
	std::vector<char> data;
	writeIntEx(&data, connID);
	writeIntEx(&data, msgId);
	std::copy(msgData.begin(), msgData.end(), std::back_inserter(data));
	
	ZmqInst::getZmqInstance()->sendData("gateway", data.data(), msgLen);
	/*TcpConnection* connection = Network::getConnById(connID);
	if (connection == NULL) {
		return;
	}
	connection->sendData(std::move(data), data.size());*/
}
