#include "MessageMgr.h"

#include "proto.h"
#include "Logger.h"
#include "ZmqInst.h"
#include "PythonPlugin.h"
#include "GameService.h"
#include "MyBuffer.h"

extern std::string g_service_name;


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
	MyBuffer buffer(data, dataLen);
	if (strcmp(sender, "gateway") == 0) {
		int connId = buffer.readIntEx();
		msgId = buffer.readIntEx();
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
			PyObject* arg = PyTuple_New(3);
			//PyTuple_SetItem(arg, 0, Py_BuildValue("s", g_service_name.c_str()));
			PyTuple_SetItem(arg, 0, PyLong_FromLong(connId));
			PyTuple_SetItem(arg, 1, PyLong_FromLong(msgId));
			PyTuple_SetItem(arg, 2, Py_BuildValue("y#", msgData, msgLen));
			//callPyFunction("main", "on_recv_client_msg", arg);
			GameService::g_gameService->callPyFunction("on_recv_client_msg", arg);
			PyGILState_Release(py_state);
		}
	}
	else {
		msgId = buffer.readIntEx();
		char* msgData = &data[4];
		int msgLen = dataLen - 4;
		if (!handleServiceMsg(msgId, msgData, msgLen)) {
			auto py_state = PyGILState_Ensure();
			PyObject* arg = PyTuple_New(3);
			PyTuple_SetItem(arg, 0, Py_BuildValue("s", sender));
			PyTuple_SetItem(arg, 1, PyLong_FromLong(msgId));
			PyTuple_SetItem(arg, 2, Py_BuildValue("y#", msgData, msgLen));
			//callPyFunction("main", "on_recv_service_msg", arg);
			//Py_INCREF(arg);
			GameService::g_gameService->callPyFunction("on_recv_service_msg", arg);
			//Py_DECREF(arg);
			PyGILState_Release(py_state);
		}
	}

	Logger::logDebug("$recv msg, sender:%s,  msgId:%d", sender, msgId);
}

void MessageMgr::sendToClient(int connID, int msgId, google::protobuf::Message* msg) {
	std::string msgData;
	msg->SerializeToString(&msgData);
	sendToClient(connID, msgId, msgData.c_str(), msgData.length());
}

void MessageMgr::sendToClient(int connID, int msgId, const char* msg, int msgLen) {
	MyBuffer buffer;
	buffer.writeInt(connID);
	buffer.writeInt(msgId);
	buffer.writeString(msg, msgLen);
	ZmqInst::getZmqInstance()->sendData("gateway", buffer.data(), buffer.size());
}

void MessageMgr::sendToServer(const char *serviceName, int msgId, const char* msg, int msgLen)
{
	MyBuffer buffer;
	buffer.writeInt(msgId);
	buffer.writeString(msg, msgLen);
	ZmqInst::getZmqInstance()->sendData(serviceName, buffer.data(), buffer.size());
}

