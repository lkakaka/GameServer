#pragma once
#include <vector>

class MessageHandler
{
public:
	static void onRecvData(char* sender, char* data, int dataLen);
};



