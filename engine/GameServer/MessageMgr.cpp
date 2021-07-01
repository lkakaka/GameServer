#include "MessageMgr.h"

#include "proto.h"
#include "Logger.h"
#include "ServiceCommEntityMgr.h"
#include "PythonPlugin.h"
#include "GameService.h"
#include "MyBuffer.h"
#include "SceneMgr.h"
#include "proto.h"
#include "ServiceType.h"
#include "CommEntity.h"
#include "ServiceInfo.h"


bool handleMsg(int connId, int msgId, char* data, int dataLen)
{
	if (GameService::g_gameService->getServieType() == SERVICE_TYPE_SCENE) {
		if (SceneMgr::getSceneMgr()->handleClientMsg(connId, msgId, data, dataLen)) {
			return true;
		}
	}
	
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

void MessageMgr::onRecvData(ServiceAddr* srcAddr, char* data, int dataLen) {
	/*ServiceAddr srcAddr;
	srcAddr.parseAddr(sender);*/
	int msgId = 0;
	MyBuffer buffer(data, dataLen);
	if (srcAddr->getServiceType() == SERVICE_TYPE_GATEWAY) {
		if (dataLen < 9) {
			Logger::logError("$recv %s msg format error, data len < 9", srcAddr->getName()->c_str());
			return;
		}
		bool isClientMsg = (buffer.readByte() == 0);
		int connId = buffer.readInt();
		msgId = buffer.readInt();
		char* msgData = (char*)buffer.data();
		int msgLen = buffer.size();
		if (!handleMsg(connId, msgId, msgData, msgLen)) {
			auto py_state = PyGILState_Ensure();
			PyObject* arg = PyTuple_New(3);
			if (isClientMsg) {
				PyTuple_SetItem(arg, 0, PyLong_FromLong(connId));
				PyTuple_SetItem(arg, 1, PyLong_FromLong(msgId));
				PyTuple_SetItem(arg, 2, Py_BuildValue("y#", msgData, msgLen));
				GameService::g_gameService->callPyFunc("on_recv_client_msg", arg);
			}
			else {
				PyObject* pArgs = Py_BuildValue("iii", srcAddr->getServiceGroup(), srcAddr->getServiceType(), srcAddr->getServiceId());
				PyObject* pyObj = GameService::g_gameService->callPyFunc("create_service_addr", pArgs);
				PyTuple_SetItem(arg, 0, pyObj);
				PyTuple_SetItem(arg, 1, PyLong_FromLong(msgId));
				PyTuple_SetItem(arg, 2, Py_BuildValue("y#", msgData, msgLen));
				GameService::g_gameService->callPyFunc("on_recv_service_msg", arg);
				Py_INCREF(pArgs);
				Py_INCREF(pyObj);
			}
			Py_INCREF(arg);
			PyGILState_Release(py_state);
		}
	}
	else {
		if (dataLen <= 4) {
			Logger::logError("$recv %s msg format error, data len <= 4", srcAddr->getName()->c_str());
			return;
		}
		msgId = buffer.readInt();
		char* msgData = (char*)buffer.data();
		int msgLen = buffer.size();
		if (!handleServiceMsg(msgId, msgData, msgLen)) {
			auto py_state = PyGILState_Ensure();
			PyObject* arg = PyTuple_New(3);

			//PyObject* pModule = PyImport_ImportModule("game.service.service_addr");//这里是要调用的文件名
			//PyObject* pFunc = PyObject_GetAttrString(pModule, "ServiceAddr");//这里是要调用的函数名
			PyObject* pArgs = Py_BuildValue("iii", srcAddr->getServiceGroup(), srcAddr->getServiceType(), srcAddr->getServiceId());
			//PyObject* pyObj = PyEval_CallObject(pFunc, pArgs);//调用函数

			PyObject* pyObj = GameService::g_gameService->callPyFunc("create_service_addr", pArgs);

			PyTuple_SetItem(arg, 0, pyObj);
			PyTuple_SetItem(arg, 1, PyLong_FromLong(msgId));
			PyTuple_SetItem(arg, 2, Py_BuildValue("y#", msgData, msgLen));
			GameService::g_gameService->callPyFunc("on_recv_service_msg", arg);
			Py_INCREF(pArgs);
				Py_INCREF(pyObj);
			PyGILState_Release(py_state);
		}
	}

	Logger::logDebug("$recv msg, sender:%s,  msgId:%d", srcAddr->getName()->c_str(), msgId);
}

//void MessageMgr::onGatewayRecvData(char* sender, char* data, int dataLen) {
//	MyBuffer buffer(data, dataLen);
//	if (dataLen < 8) {
//		int connId = -1;
//		if (dataLen >= 4) connId = buffer.readInt();
//		Logger::logError("$recv %s msg format error, data len(%d) < 8, connId:%d", sender, dataLen, connId);
//		return;
//	}
//	int connId = buffer.readInt();
//	int msgId = buffer.readInt();
//	if (msgId == MSG_ID_CLIENT_DISCONNECT) {
//		std::shared_ptr<google::protobuf::Message> msg = createMessage(msgId, &data[8], dataLen - 8);
//		ClientDisconnect* recvMsg = (ClientDisconnect*)msg.get();
//		Network::getNetworkInstance()->removeConnection(recvMsg->conn_id(), recvMsg->reason().c_str());
//	}
//	else {
//		TcpConnection* connection = Network::getConnById(connId);
//		if (connection == NULL) {
//			Logger::logError("$send packet error, connId(%d) is not exist, msgId:%d", connId, msgId);
//			return;
//		}
//		connection->sendMsgToClient(msgId, &data[8], dataLen - 8);
//	}
//
//	Logger::logInfo("$recv msg, sender:%s,  msgId:%d", sender, msgId);
//}

//void MessageMgr::sendToClient(int connID, int msgId, google::protobuf::Message* msg) {
//	std::string msgData;
//	msg->SerializeToString(&msgData);
//	sendToClient(connID, msgId, msgData.c_str(), msgData.length());
//}

void MessageMgr::sendToClient(int connID, int msgId, const char* msg, int msgLen) {
	MyBuffer buffer;
	buffer.writeInt(msgId);
	buffer.writeInt(connID);
	buffer.writeString(msg, msgLen);
	ServiceAddr addr(ServiceInfo::getSingleton()->getServiceGroup(), ServiceType::SERVICE_TYPE_GATEWAY, 0);
	CommEntityMgr::getSingleton()->getCommEntity()->sendToService(&addr, (char*)buffer.data(), buffer.size());
}

void MessageMgr::sendToServer(ServiceAddr* addr, int msgId, const char* msg, int msgLen)
{
	MyBuffer buffer;
	buffer.writeInt(msgId);
	// 发往gateway的消息都需要一个connId
	if (addr->getServiceType() == SERVICE_TYPE_GATEWAY) buffer.writeInt(-1);
	buffer.writeString(msg, msgLen);
	CommEntityMgr::getSingleton()->getCommEntity()->sendToService(addr, (char*)buffer.data(), buffer.size());
}

