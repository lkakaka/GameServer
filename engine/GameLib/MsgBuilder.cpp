
#include "MsgBuilder.h"
#include "GameService.h"
#include "../Common/ServerMacros.h"

MyBuffer MsgBuilder::buildServiceMsg(int serviceType, int msgId, const char* msg, int msgLen) {
	MyBuffer buffer;

	if (SERVICE_TYPE != SERVICE_TYPE_GATEWAY) {
		if (serviceType == SERVICE_TYPE_GATEWAY) {
			//// �������ݸ�ʽ, ����gateway����Ϣ����Ҫһ��connId
			//buffer.writeInt(1);
			//buffer.writeInt(-1);
			//buffer.writeByte(SEND_TYPE_TCP);
			buffer.writeByte(0); // �Ƿ��Ƿ����ͻ��˵���Ϣ
		}
		buffer.writeInt(msgId);
	}
	else {
		// gateway���������������Ϣ
		buffer.writeByte(1); // �Ƿ��Ƿ�������Ϣ
		buffer.writeInt(0);  // conn ID����������Ϣ����Ҫ����-1
		buffer.writeInt(msgId);
	}
	buffer.writeString(msg, msgLen);
	return buffer;
}

MyBuffer MsgBuilder::buildClientTcpMsg(int connId, int msgId, const char* msg, int msgLen) {
	MyBuffer buffer;
	buffer.writeByte(1); // �Ƿ��Ƿ����ͻ��˵���Ϣ
	buffer.writeInt(msgId);
	buffer.writeInt(1);
	buffer.writeInt(connId);
	buffer.writeByte(SEND_TYPE_TCP);
	buffer.writeString(msg, msgLen);
	return buffer;
}

MyBuffer MsgBuilder::buildClientBroadcastTcpMsg(std::set<int>& connIds, int msgId, const char* msg, int msgLen) {
	MyBuffer buffer;
	buffer.writeByte(1); // �Ƿ��Ƿ����ͻ��˵���Ϣ
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
	buffer.writeByte(1); // �Ƿ��Ƿ����ͻ��˵���Ϣ
	buffer.writeInt(msgId);
	buffer.writeInt(1);
	buffer.writeInt(connId);
	buffer.writeByte(SEND_TYPE_KCP);
	buffer.writeString(msg, msgLen);
	return buffer;
}

MyBuffer MsgBuilder::buildClientBroadcastKcpMsg(std::set<int>& connIds, int msgId, const char* msg, int msgLen) {
	MyBuffer buffer;
	buffer.writeByte(1); // �Ƿ��Ƿ����ͻ��˵���Ϣ
	buffer.writeInt(msgId);
	buffer.writeInt(connIds.size());
	for (int connId : connIds) {
		buffer.writeInt(connId);
	}
	buffer.writeByte(SEND_TYPE_KCP);
	buffer.writeString(msg, msgLen);
	return buffer;
}
