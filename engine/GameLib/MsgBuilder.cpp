
#include "MsgBuilder.h"
#include "GameService.h"
#include "../Common/ServerMacros.h"

MyBuffer MsgBuilder::buildServiceMsg(int serviceType, int msgId, const char* msg, int msgLen) {
	MyBuffer buffer;

	if (SERVICE_TYPE != SERVICE_TYPE_GATEWAY) {
		if (serviceType == SERVICE_TYPE_GATEWAY) {
			//// 补齐数据格式, 发往gateway的消息都需要一个connId
			//buffer.writeInt(1);
			//buffer.writeInt(-1);
			//buffer.writeByte(SEND_TYPE_TCP);
			buffer.writeByte(0); // 是否是发给客户端的消息
		}
		buffer.writeInt(msgId);
	}
	else {
		// gateway发往其他服务的消息
		buffer.writeByte(1); // 是否是服务器消息
		buffer.writeInt(0);  // conn ID，服务器消息不需要，填-1
		buffer.writeInt(msgId);
	}
	buffer.writeString(msg, msgLen);
	return buffer;
}

MyBuffer MsgBuilder::buildClientTcpMsg(int connId, int msgId, const char* msg, int msgLen) {
	MyBuffer buffer;
	buffer.writeByte(1); // 是否是发给客户端的消息
	buffer.writeInt(msgId);
	buffer.writeInt(1);
	buffer.writeInt(connId);
	buffer.writeByte(SEND_TYPE_TCP);
	buffer.writeString(msg, msgLen);
	return buffer;
}

MyBuffer MsgBuilder::buildClientBroadcastTcpMsg(std::set<int>& connIds, int msgId, const char* msg, int msgLen) {
	MyBuffer buffer;
	buffer.writeByte(1); // 是否是发给客户端的消息
	buffer.writeInt(msgId);
	buffer.writeInt(connIds.size());
	for (int connId : connIds) {
		buffer.writeInt(connId);
	}
	buffer.writeByte(SEND_TYPE_TCP);
	buffer.writeString(msg, msgLen);
	return buffer;
}

MyBuffer MsgBuilder::buildClientKcpMsg(int connId, int msgId, const char* msg, int msgLen) {
	MyBuffer buffer;
	buffer.writeByte(1); // 是否是发给客户端的消息
	buffer.writeInt(msgId);
	buffer.writeInt(1);
	buffer.writeInt(connId);
	buffer.writeByte(SEND_TYPE_KCP);
	buffer.writeString(msg, msgLen);
	return buffer;
}

MyBuffer MsgBuilder::buildClientBroadcastKcpMsg(std::set<int>& connIds, int msgId, const char* msg, int msgLen) {
	MyBuffer buffer;
	buffer.writeByte(1); // 是否是发给客户端的消息
	buffer.writeInt(msgId);
	buffer.writeInt(connIds.size());
	for (int connId : connIds) {
		buffer.writeInt(connId);
	}
	buffer.writeByte(SEND_TYPE_KCP);
	buffer.writeString(msg, msgLen);
	return buffer;
}
