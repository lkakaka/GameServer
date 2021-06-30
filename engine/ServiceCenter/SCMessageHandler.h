#pragma once
#include "IMessageHandler.h"
#include "Singleton.h"
#include "SCConnection.h"

class SCMessageHandler : public IMessageHandler, public Singleton<SCMessageHandler> {
public:
	void onRecvMessage(ServiceAddr* sender, char* data, int dataLen);
	void onRecvConnectionMessage(SCConnection* conn, ServiceAddr* sender, char* data, int dataLen);
};
