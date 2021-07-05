#pragma once
#include <vector>
//#include <google/protobuf/message.h>
#include "Network/ServiceCommEntity.h"
#include "Network/IMessageHandler.h"
#include "Singleton.h"

class ServiceMessageHandler : public IMessageHandler, public Singleton<ServiceMessageHandler>
{
private:
	int handleServiceMessage(ServiceAddr* srcAddr, char* data, int dataLen);
	bool handleEngineServiceMsg(int msgId, char* data, int dataLen);

	int handleGatewayMessage(ServiceAddr* srcAddr, char* data, int dataLen);
	bool handleEngineGatewayMsg(int connId, int msgId, char* data, int dataLen);

public:
	void onRecvMessage(ServiceAddr* sender, char* data, int dataLen);
};

