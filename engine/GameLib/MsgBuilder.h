#pragma once
#include "MyBuffer.h"
#include <set>

class MsgBuilder {
public:
	static MyBuffer buildServiceMsg(int serviceType, int msgId, const char* msg, int msgLen);
	static MyBuffer buildClientTcpMsg(int connId, int msgId, const char* msg, int msgLen);
	static MyBuffer buildClientBroadcastTcpMsg(std::set<int>& connIds, int msgId, const char* msg, int msgLen);

	static MyBuffer buildClientKcpMsg(int connId, int msgId, const char* msg, int msgLen);
	static MyBuffer buildClientBroadcastKcpMsg(std::set<int>& connIds, int msgId, const char* msg, int msgLen);
};
