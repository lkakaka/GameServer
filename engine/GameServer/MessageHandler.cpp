#include "MessageHandler.h"

#include "proto.h"
#include "Logger.h"
#include "Network/ServiceCommEntityMgr.h"
#include "GameService.h"
#include "MyBuffer.h"
#include "SceneMgr.h"
#include "proto.h"
#include "ServiceType.h"
#include "Network/ServiceCommEntity.h"
#include "ServiceInfo.h"


INIT_SINGLETON_CLASS(ServiceMessageHandler)

bool ServiceMessageHandler::handleEngineGatewayMsg(int connId, int msgId, char* data, int dataLen)
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

int ServiceMessageHandler::handleGatewayMessage(ServiceAddr* srcAddr, char* data, int dataLen) {
	if (dataLen < 9) {
		Logger::logError("$recv %s msg format error, data len < 9", srcAddr->getName()->c_str());
		return 0;
	}
	MyBuffer buffer(data, dataLen);
	bool isClientMsg = (buffer.readByte() == 0);
	int connId = buffer.readInt();
	int msgId = buffer.readInt();
	char* msgData = (char*)buffer.data();
	int msgLen = buffer.size();
	if (handleEngineGatewayMsg(connId, msgId, msgData, msgLen)) return msgId;

	if (isClientMsg) {
		GameService::g_gameService->dispatchClientMsgToScript(connId, msgId, msgData, msgLen);
	}
	else {
		GameService::g_gameService->dispatchServiceMsgToScript(srcAddr, msgId, msgData, msgLen);
	}
	return msgId;
}

bool ServiceMessageHandler::handleEngineServiceMsg(int msgId, char* data, int dataLen)
{
	return false;
}

int ServiceMessageHandler::handleServiceMessage(ServiceAddr* srcAddr, char* data, int dataLen) {
	if (dataLen <= 4) {
		Logger::logError("$recv %s msg format error, data len <= 4", srcAddr->getName()->c_str());
		return 0;
	}
	MyBuffer buffer(data, dataLen);
	int msgId = buffer.readInt();
	char* msgData = (char*)buffer.data();
	int msgLen = buffer.size();
	if (handleEngineServiceMsg(msgId, msgData, msgLen)) return msgId;

	GameService::g_gameService->dispatchServiceMsgToScript(srcAddr, msgId, msgData, msgLen);
	return msgId;
}

void ServiceMessageHandler::onRecvMessage(ServiceAddr* srcAddr, char* data, int dataLen) {
	int msgId = 0;
	if (srcAddr->getServiceType() == SERVICE_TYPE_GATEWAY) 
	{
		handleGatewayMessage(srcAddr, data, dataLen);
	}
	else {
		handleServiceMessage(srcAddr, data, dataLen);
	}

	Logger::logDebug("$recv msg, sender:%s,  msgId:%d", srcAddr->getName()->c_str(), msgId);
}
