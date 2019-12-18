#pragma once
#include <vector>
#include <google/protobuf/message.h>

class ProtoBufferMgr
{
public:
	static void sendPacket(int connID, int msgId, google::protobuf::Message* msg);
	static void onRecvData(char* sender, char* data, int dataLen);
};

