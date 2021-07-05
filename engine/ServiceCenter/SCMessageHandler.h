#pragma once
#include "Network/IMessageHandler.h"
#include "Singleton.h"
#include "SCConnection.h"
#include "MyBuffer.h"

typedef struct _Message {
	MyBuffer buffer;
} _Message;

class SCMessageHandler : public IMessageHandler, public Singleton<SCMessageHandler> {
private:
	std::map<std::string, std::vector<MyBuffer>> msgCaches;

	void dispatchServiceMsg(SCConnection* conn, ServiceAddr* dst, char* data, int len);
	void handleVerifyMsg(SCConnection* conn, ServiceAddr* sender, char* data, int len);
	void addMsgCache(ServiceAddr* dst, MyBuffer* buffer);
	void dispatchCacheMsg(SCConnection* conn);
public:
	void onRecvMessage(ServiceAddr* sender, char* data, int dataLen);
	void onRecvConnectionMessage(SCConnection* conn, ServiceAddr* sender, char* data, int dataLen);
};
