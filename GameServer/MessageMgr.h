#pragma once
#include <vector>
#include <google/protobuf/message.h>
#include "Network.h"

class MessageMgr
{
public:
	static void sendToClient(int connID, int msgId, google::protobuf::Message* msg);
	static void sendToClient(int connID, int msgId, const char* msg, int msgLen);
	static void sendToServer(const char* serviceName, int msgId, const char* msg, int msgLen);
	static void onRecvData(char* sender, char* data, int dataLen);
	static void onGatewayRecvData(char* sender, char* data, int dataLen);
};

