#include "MessageMgr.h"

#include "proto.h"
#include "Logger.h"
#include "ZmqInst.h"
#include "PythonPlugin.h"

extern std::string g_service_name;

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

bool handleMsg(int connId, int msgId, char* data, int dataLen)
{
	switch (msgId)
	{
	/*case MSG_ID_TEST:
	{
		std::shared_ptr<google::protobuf::Message> msg = createMessage(msgId, data, dataLen);
		Test* recvMsg = (Test*)msg.get();
		Logger::logInfo("$receive test proto, id:%d, msg:%s", recvMsg->id(), recvMsg->msg().data());

		Test resp_msg;
		resp_msg.set_id(1);
		resp_msg.set_msg("world");
		MessageMgr::sendToClient(connId, MSG_ID_TEST, &resp_msg);
		return true;
	}
	case MSG_ID_LOGIN:
	{
		std::shared_ptr<google::protobuf::Message> msg = createMessage(msgId, data, dataLen);
		Login* recvMsg = (Login*)msg.get();
		Logger::logInfo("$receive login proto, account:%s, pwd:%s", recvMsg->account().c_str(), recvMsg->pwd().data());

		LoginRsp resp_msg;
		resp_msg.set_account(recvMsg->account());
		resp_msg.set_user_id(1);
		MessageMgr::sendToClient(connId, MSG_ID_LOGINRSP, &resp_msg);
		return true;
	}*/
	default:
		/*Logger::logError("$receive unknown proto, msgId:%d", msgId);*/
		break;
	}
	return false;
}

bool handleServiceMsg(int msgId, char* data, int dataLen)
{
	return false;
}

void MessageMgr::onRecvData(char* sender, char* data, int dataLen) {
	if (dataLen <= 8) {
		Logger::logError("$recv msg format error, data len < 8");
		return;
	}

	int msgId = 0;
	if (strcmp(sender, "gateway") == 0) {
		int connId = readInt(data);
		msgId = readInt(&data[4]);
		////google::protobuf::Message* msg = (google::protobuf::Message*)CreateMsgById(msgId);
		//std::shared_ptr<google::protobuf::Message> msg = CreateMsgById(msgId);
		//if (msg == NULL) {
		//	Logger::logError("$create proto msg error, msgId:%d", msgId);
		//	return;
		//}
		//msg->ParseFromArray(&data[8], dataLen - 8);
		char* msgData = &data[8];
		int msgLen = dataLen - 8;
		if (!handleMsg(connId, msgId, msgData, msgLen)) {
			auto py_state = PyGILState_Ensure();
			PyObject* arg = PyTuple_New(4);
			PyTuple_SetItem(arg, 0, Py_BuildValue("s", g_service_name.c_str()));
			PyTuple_SetItem(arg, 1, PyLong_FromLong(connId));
			PyTuple_SetItem(arg, 2, PyLong_FromLong(msgId));
			PyTuple_SetItem(arg, 3, Py_BuildValue("y#", msgData, msgLen));
			callPyFunction("main", "on_recv_client_msg", arg);
			PyGILState_Release(py_state);
		}
	}
	else {
		msgId = readInt(data);
		char* msgData = &data[4];
		int msgLen = dataLen - 4;
		if (!handleServiceMsg(msgId, msgData, msgLen)) {
			PyObject* arg = PyTuple_New(4);
			auto py_state = PyGILState_Ensure();
			PyTuple_SetItem(arg, 0, Py_BuildValue("s", g_service_name.c_str()));
			PyTuple_SetItem(arg, 1, Py_BuildValue("s", sender));
			PyTuple_SetItem(arg, 2, PyLong_FromLong(msgId));
			PyTuple_SetItem(arg, 3, Py_BuildValue("y#", msgData, msgLen));
			callPyFunction("main", "on_recv_service_msg", arg);
			PyGILState_Release(py_state);
		}
	}

	Logger::logDebug("$recv msg, sender:%s,  msgId:%d", sender, msgId);
}

void MessageMgr::sendToClient(int connID, int msgId, google::protobuf::Message* msg) {
	std::string msgData;
	msg->SerializeToString(&msgData);
	sendToClient(connID, msgId, msgData.c_str(), msgData.length());
	/*int msgLen = msgData.size() + 8;
	std::vector<char> data;
	writeIntEx(&data, connID);
	writeIntEx(&data, msgId);
	std::copy(msgData.begin(), msgData.end(), std::back_inserter(data));*/
	
	//ZmqInst::getZmqInstance()->sendData("gateway", data.data(), msgLen);
}

void MessageMgr::sendToClient(int connID, int msgId, const char* msg, int msgLen) {
	msgLen += 8;
	std::vector<char> data;
	writeIntEx(&data, connID);
	writeIntEx(&data, msgId);
	std::copy(msg, msg + msgLen, std::back_inserter(data));

	ZmqInst::getZmqInstance()->sendData("gateway", data.data(), msgLen);
}

void MessageMgr::sendToServer(const char *serviceName, int msgId, const char* msg, int msgLen)
{
	std::vector<char> data;
	writeIntEx(&data, msgId);
	std::copy(msg, msg + msgLen, std::back_inserter(data));
	ZmqInst::getZmqInstance()->sendData(serviceName, data.data(), msgLen + 4);
}

