#pragma once

#include "../../Common/ServerExports.h"
#include "IMessageHandler.h"
#include "ServiceAddr.h"

class IServiceCommEntity {
protected:
	ServiceAddr addr;
	IMessageHandler* messageHandler;
public:
	IServiceCommEntity(ServiceAddr& addr): addr(addr), messageHandler(NULL) { };
	inline void setMessageHandler(IMessageHandler* handler) { messageHandler = handler; }
	virtual void sendToService(ServiceAddr* dstAddr, char* msg, int msgLen) = 0;
	virtual void start() = 0;
};

