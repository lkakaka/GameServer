#pragma once
#include "ServiceAddr.h"

class IMessageHandler {
public:
	virtual void onRecvMessage(ServiceAddr* sender, char* data, int dataLen) = 0;
};

