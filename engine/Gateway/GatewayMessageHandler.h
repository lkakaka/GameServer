#pragma once
#include "IMessageHandler.h"
#include "Singleton.h"

class GatewayMessageHandler : public IMessageHandler, Singleton<GatewayMessageHandler> {
public:
	void onRecvMessage(char* sender, char* data, int dataLen);
};
