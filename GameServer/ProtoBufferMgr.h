#pragma once
#include <vector>
#include <google/protobuf/message.h>

class ProtoBufferMgr
{
public:
	static int parseProtoData(int connId, std::vector<unsigned char>* data);
	static void sendPacket(int connID, int msgId, google::protobuf::Message* msg);
};

