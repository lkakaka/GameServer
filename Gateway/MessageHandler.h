#pragma once
#include <vector>

class MessageHandler
{
public:
	static void onRecvData(char* sender, char* data, int dataLen);
};

int readInt(char* data);
void writeInt(std::vector<char>* data, int val);
void writeIntEx(std::vector<char>* data, int val);


