#include "LuaService.h"
#include "Logger.h"
#include "MyBuffer.h"
#include "ServiceType.h"
#include "Network/ServiceCommEntityMgr.h"
#include "../Common/ServerMacros.h"
#include "GameService.h"
#include "MsgBuilder.h"

void LuaService::bindLuaService(std::shared_ptr<sol::state> lua) {
	sol::table service = lua->create_named_table("Service");
	service["sendMsgToService"] = &LuaService::sendMsgToService;
	service["sendMsgToClient"] = &LuaService::sendMsgToClient;
	service["broadcastMsgToClient"] = &LuaService::broadcastMsgToClient;
	service["sendMsgToClientKCP"] = &LuaService::sendMsgToClientKCP;
}

bool LuaService::sendMsgToService(sol::table dstAddr, int msgId, const char* msg, int msgLen) {
	sol::object serviceGroupObj = dstAddr["serviceGroup"];
	sol::object serviceTypeObj = dstAddr["serviceType"];
	sol::object serviceIdObj = dstAddr["serviceId"];
	sol::type serviceGroupObjType = serviceGroupObj.get_type();
	sol::type serviceGroupTypeType = serviceTypeObj.get_type();
	sol::type serviceGroupIdType = serviceIdObj.get_type();
	if (serviceGroupObjType != sol::type::number && serviceGroupTypeType != sol::type::number && serviceGroupIdType != sol::type::number) {
		LOG_ERROR("dst addr type error");
		return false;
	}
	int serviceGroup = serviceGroupObj.as<int>();
	int serviceType = serviceTypeObj.as<int>();
	int serviceId = serviceIdObj.as<int>();
	ServiceAddr addr(serviceGroup, serviceType, serviceId);

	//MyBuffer buffer;

	//if (SERVICE_TYPE != SERVICE_TYPE_GATEWAY) {
	//	if (serviceType == SERVICE_TYPE_GATEWAY) {
	//		//// �������ݸ�ʽ, ����gateway����Ϣ����Ҫһ��connId
	//		//buffer.writeInt(1);
	//		//buffer.writeInt(-1);
	//		//buffer.writeByte(SEND_TYPE_TCP);
	//		buffer.writeByte(0); // �Ƿ��Ƿ����ͻ��˵���Ϣ
	//	}
	//	buffer.writeInt(msgId);
	//} else {
	//	// gateway���������������Ϣ
	//	buffer.writeByte(1); // �Ƿ��Ƿ�������Ϣ
	//	buffer.writeInt(0);  // conn ID����������Ϣ����Ҫ����-1
	//	buffer.writeInt(msgId);
	//}
	//buffer.writeString(msg, msgLen);
	
	MyBuffer buffer = MsgBuilder::buildServiceMsg(serviceType, msgId, msg, msgLen);

	SERVER_CENTER_COMM_ENTITY->sendToService(&addr, (char*)buffer.data(), buffer.size());

	LOG_INFO("send msg to service %s, msgId:%d", addr.getName(), msgId);
	return true;
}

bool LuaService::sendMsgToClient(int connId, int msgId, const char* msg, int msgLen) {
	//MyBuffer buffer;
	//buffer.writeByte(1); // �Ƿ��Ƿ����ͻ��˵���Ϣ
	//buffer.writeInt(msgId);
	//buffer.writeInt(1);
	//buffer.writeInt(connId);
	//buffer.writeByte(SEND_TYPE_TCP);
	//buffer.writeString(msg, msgLen);

	MyBuffer buffer = MsgBuilder::buildClientTcpMsg(connId, msgId, msg, msgLen);

	ServiceAddr addr(SERVICE_GROUP, ServiceType::SERVICE_TYPE_GATEWAY, 0);
	SERVER_CENTER_COMM_ENTITY->sendToService(&addr, (char*)buffer.data(), buffer.size());
	return true;
}

bool LuaService::broadcastMsgToClient(std::set<int> connIds, int msgId, const char* msg, int msgLen) {
	//MyBuffer buffer;
	//buffer.writeByte(1); // �Ƿ��Ƿ����ͻ��˵���Ϣ
	//buffer.writeInt(msgId);
	//buffer.writeInt(connIds.size());
	//for (int connId : connIds) {
	//	buffer.writeInt(connId);
	//}
	//buffer.writeByte(SEND_TYPE_TCP);
	//buffer.writeString(msg, msgLen);

	MyBuffer buffer = MsgBuilder::buildClientBroadcastTcpMsg(connIds, msgId, msg, msgLen);

	ServiceAddr addr(SERVICE_GROUP, ServiceType::SERVICE_TYPE_GATEWAY, 0);
	SERVER_CENTER_COMM_ENTITY->sendToService(&addr, (char*)buffer.data(), buffer.size());
	return true;
}

bool LuaService::sendMsgToClientKCP(int connId, int msgId, const char* msg, int msgLen) {
	//MyBuffer buffer;
	//buffer.writeByte(1); // �Ƿ��Ƿ����ͻ��˵���Ϣ
	//buffer.writeInt(msgId);
	//buffer.writeInt(1);
	//buffer.writeInt(connId);
	//buffer.writeByte(SEND_TYPE_KCP);
	//buffer.writeString(msg, msgLen);

	MyBuffer buffer = MsgBuilder::buildClientKcpMsg(connId, msgId, msg, msgLen);

	ServiceAddr addr(SERVICE_GROUP, ServiceType::SERVICE_TYPE_GATEWAY, 0);
	SERVER_CENTER_COMM_ENTITY->sendToService(&addr, (char*)buffer.data(), buffer.size());
	return true;
}

