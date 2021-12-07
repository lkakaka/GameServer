#pragma once
#include "../Common/ServerExports.h"
#include "Network/ServiceAddr.h"

class IScript
{
public:
	virtual void initScript() = 0;
	virtual void dispatchClientMsgToScript(int connId, int msgId, const char* data, int len) = 0;
	virtual void dispatchServiceMsgToScript(ServiceAddr* srcAddr, int msgId, const char* data, int len) = 0;
};

