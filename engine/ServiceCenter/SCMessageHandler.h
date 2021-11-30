#pragma once
#include "Network/IMessageHandler.h"
#include "Singleton.h"
#include "SCConnection.h"
#include "MyBuffer.h"

typedef enum {
	OK = 0,
	VERIFY_FAIL = 1,
} MSG_ERROR_CODE;


typedef struct _Message {
	MyBuffer buffer;
} _Message;

class SCMessageHandler : public IMessageHandler, public Singleton<SCMessageHandler> {
private:
	std::map<std::string, std::vector<MyBuffer>> msgCaches;

	void dispatchServiceMsg(SCConnection* conn, ServiceAddr* dst, char* data, int len);
	int handleVerifyMsg(SCConnection* conn, ServiceAddr* sender, char* data, int len);
	void addMsgCache(ServiceAddr* dst, MyBuffer* buffer);
	void dispatchCacheMsg(SCConnection* conn);
	void sendServiceMsg(SCConnection* dstConn, MyBuffer* buffer, ServiceAddr* srcAddr);
public:
	void onRecvMessage(ServiceAddr* sender, char* data, int dataLen);
	int onRecvConnectionMessage(SCConnection* conn, ServiceAddr* dstAddr, char* data, int dataLen);
};
