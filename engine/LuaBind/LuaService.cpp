#include "LuaService.h"
#include "Logger.h"
#include "MyBuffer.h"
#include "ServiceType.h"
#include "Network/ServiceCommEntityMgr.h"
#include "ServiceInfo.h"
#include "../Common/ServerMacros.h"

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

	MyBuffer buffer;
	buffer.writeInt(msgId);
	if (serviceType == SERVICE_TYPE_GATEWAY) {
		// 补齐数据格式, 发往gateway的消息都需要一个connId
		buffer.writeInt(1);
		buffer.writeInt(-1);
		buffer.writeByte(SEND_TYPE_TCP);
	}
	buffer.writeString(msg, msgLen);
	CommEntityMgr::getSingleton()->getCommEntity()->sendToService(&addr, (char*)buffer.data(), buffer.size());

	LOG_INFO("send msg to service %s, msgId:%d", addr.getName(), msgId);
	return true;
}

bool LuaService::sendMsgToClient(int connId, int msgId, const char* msg, int msgLen) {
	MyBuffer buffer;
	buffer.writeInt(msgId);
	buffer.writeInt(1);
	buffer.writeInt(connId);
	buffer.writeByte(SEND_TYPE_TCP);
	buffer.writeString(msg, msgLen);
	ServiceAddr addr(ServiceInfo::getSingleton()->getServiceGroup(), ServiceType::SERVICE_TYPE_GATEWAY, 0);
	CommEntityMgr::getSingleton()->getCommEntity()->sendToService(&addr, (char*)buffer.data(), buffer.size());
	return true;
}

bool LuaService::broadcastMsgToClient(std::set<int> connIds, int msgId, const char* msg, int msgLen) {
	MyBuffer buffer;
	buffer.writeInt(msgId);
	buffer.writeInt(connIds.size());
	for (int connId : connIds) {
		buffer.writeInt(connId);
	}
	buffer.writeByte(SEND_TYPE_TCP);
	buffer.writeString(msg, msgLen);
	ServiceAddr addr(ServiceInfo::getSingleton()->getServiceGroup(), ServiceType::SERVICE_TYPE_GATEWAY, 0);
	CommEntityMgr::getSingleton()->getCommEntity()->sendToService(&addr, (char*)buffer.data(), buffer.size());
	return true;
}

bool LuaService::sendMsgToClientKCP(int connId, int msgId, const char* msg, int msgLen) {
	MyBuffer buffer;
	buffer.writeInt(msgId);
	buffer.writeInt(1);
	buffer.writeInt(connId);
	buffer.writeByte(SEND_TYPE_KCP);
	buffer.writeString(msg, msgLen);
	ServiceAddr addr(ServiceInfo::getSingleton()->getServiceGroup(), ServiceType::SERVICE_TYPE_GATEWAY, 0);
	CommEntityMgr::getSingleton()->getCommEntity()->sendToService(&addr, (char*)buffer.data(), buffer.size());
	return true;
}

