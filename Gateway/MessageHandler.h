#pragma once
#include <vector>

class MessageHandler
{
public:
	static int parseProtoData(int connId, std::vector<char>* data);
	static void sendPacket(int connID, int msgId, char* data, int dataLen);
	static void onRecvData(char* sender, char* data, int dataLen);
	static void dispatchMsg(int connId, int msgId, int msgLen, std::vector<char>* recData);
};

