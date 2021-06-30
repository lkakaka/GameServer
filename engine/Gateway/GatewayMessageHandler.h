#pragma once
#include "IMessageHandler.h"
#include "Singleton.h"

class GatewayMessageHandler : public IMessageHandler, public Singleton<GatewayMessageHandler> {
public:
	void onRecvMessage(ServiceAddr* sender, char* data, int dataLen);
};
