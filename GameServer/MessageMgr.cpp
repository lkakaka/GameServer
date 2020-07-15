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
	int msgId = 0;
	MyBuffer buffer(data, dataLen);
	if (strcmp(sender, "gateway") == 0) {
		if (dataLen <= 9) {
			Logger::logError("$recv %s msg format error, data len <= 9", sender);
			return;
		}
		bool isClientMsg = (buffer.readByte(true) == 0);
		int connId = buffer.readInt(true);
		msgId = buffer.readInt(true);
		////google::protobuf::Message* msg = (google::protobuf::Message*)CreateMsgById(msgId);
		//std::shared_ptr<google::protobuf::Message> msg = CreateMsgById(msgId);
		//if (msg == NULL) {
		//	Logger::logError("$create proto msg error, msgId:%d", msgId);
		//	return;
		//}
		//msg->ParseFromArray(&data[8], dataLen - 8);
		char* msgData = buffer.data();
		int msgLen = buffer.size();
		if (!handleMsg(connId, msgId, msgData, msgLen)) {
			auto py_state = PyGILState_Ensure();
			PyObject* arg = PyTuple_New(3);
			//PyTuple_SetItem(arg, 0, Py_BuildValue("s", g_service_name.c_str()));
			if (isClientMsg) {
				PyTuple_SetItem(arg, 0, PyLong_FromLong(connId));
			}
			else {
				PyTuple_SetItem(arg, 0, Py_BuildValue("s", sender));
			}
			PyTuple_SetItem(arg, 1, PyLong_FromLong(msgId));
			PyTuple_SetItem(arg, 2, Py_BuildValue("y#", msgData, msgLen));
			if (isClientMsg) {
				GameService::g_gameService->callPyFunc("on_recv_client_msg", arg);
			}
			else {
				GameService::g_gameService->callPyFunc("on_recv_service_msg", arg);
			}
			PyGILState_Release(py_state);
		}
	}
	else {
		if (dataLen <= 4) {
			Logger::logError("$recv %s msg format error, data len <= 4", sender);
			return;
		}
		msgId = buffer.readInt(true);
		char* msgData = buffer.data();
		int msgLen = buffer.size();
		if (!handleServiceMsg(msgId, msgData, msgLen)) {
			auto py_state = PyGILState_Ensure();
			PyObject* arg = PyTuple_New(3);
			PyTuple_SetItem(arg, 0, Py_BuildValue("s", sender));
			PyTuple_SetItem(arg, 1, PyLong_FromLong(msgId));
			PyTuple_SetItem(arg, 2, Py_BuildValue("y#", msgData, msgLen));
			GameService::g_gameService->callPyFunc("on_recv_service_msg", arg);
			PyGILState_Release(py_state);
		}
	}

	Logger::logDebug("$recv msg, sender:%s,  msgId:%d", sender, msgId);
}

void MessageMgr::onGatewayRecvData(char* sender, char* data, int dataLen) {
	MyBuffer buffer(data, dataLen);
	if (dataLen < 8) {
		int connId = -1;
		if (dataLen >= 4) connId = buffer.readInt(true);
		Logger::logError("$recv %s msg format error, data len(%d) < 8, connId:%d", sender, dataLen, connId);
		return;
	}
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

