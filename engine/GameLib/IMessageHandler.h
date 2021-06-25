#pragma once

class IMessageHandler {
public:
	virtual void onRecvMessage(char* sender, char* data, int dataLen) = 0;
};

