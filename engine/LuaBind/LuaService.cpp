#include "LuaService.h"
#include "Logger.h"
#include "MyBuffer.h"
#include "ServiceType.h"
#include "Network/ServiceCommEntityMgr.h"
#include "ServiceInfo.h"

void LuaService::bindLuaService(std::shared_ptr<sol::state> lua) {
	sol::table timer = lua->create_named_table("Service");
	timer["sendMsgToService"] = &LuaService::sendMsgToService;
	timer["sendMsgToClient"] = &LuaService::sendMsgToClient;
}

bool LuaService::sendMsgToService(sol::table dstAddr, int msgId, const char* msg, int msgLen) {
	sol::object serviceGroupObj = dstAddr["serviceGroup"];
	sol::object serviceTypeObj = dstAddr["serviceType"];
	sol::object serviceIdObj = dstAddr["serviceId"];
	sol::type serviceGroupObjType = serviceGroupObj.get_type();
	sol::type serviceGroupTypeType = serviceTypeObj.get_type();
	sol::type serviceGroupIdType = serviceIdObj.get_type();
	if (serviceGroupObjType != sol::type::number && serviceGroupTypeType != sol::type::number && serviceGroupIdType != sol::type::number) {
		Logger::logError("$dst addr type error");
		return false;
	}
	int serviceGroup = serviceGroupObj.as<int>();
	int serviceType = serviceTypeObj.as<int>();
	int serviceId = serviceIdObj.as<int>();
	ServiceAddr addr(serviceGroup, serviceType, serviceId);

	MyBuffer buffer;
	buffer.writeInt(msgId);
	// ����gateway����Ϣ����Ҫһ��connId
	if (serviceType == SERVICE_TYPE_GATEWAY) buffer.writeInt(-1);
	buffer.writeString(msg, msgLen);
	CommEntityMgr::getSingleton()->getCommEntity()->sendToService(&addr, (char*)buffer.data(), buffer.size());

	Logger::logInfo("$send msg to service %s, msgId:%d", addr.getName(), msgId);
	return true;
}

bool LuaService::sendMsgToClient(int connId, int msgId, const char* msg, int msgLen) {
	MyBuffer buffer;
	buffer.writeInt(msgId);
	buffer.writeInt(connId);
	buffer.writeString(msg, msgLen);
	ServiceAddr addr(ServiceInfo::getSingleton()->getServiceGroup(), ServiceType::SERVICE_TYPE_GATEWAY, 0);
	CommEntityMgr::getSingleton()->getCommEntity()->sendToService(&addr, (char*)buffer.data(), buffer.size());
	return true;
}
