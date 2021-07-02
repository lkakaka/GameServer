#pragma once
#include "CommEntity.h"

class IMessageHandler {
public:
	virtual void onRecvMessage(ServiceAddr* sender, char* data, int dataLen) = 0;
};

