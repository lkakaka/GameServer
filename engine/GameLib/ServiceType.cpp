#pragma once
#include "ServiceType.h"
#include <map>

static std::map<std::string, ServiceType> serviceName2Type = {
	{"router", ServiceType::SERVICE_TYPE_ROUTER},
	{"gateway", ServiceType::SERVICE_TYPE_GATEWAY},
	{"login", ServiceType::SERVICE_TYPE_LOGIN},
	{"db", ServiceType::SERVICE_TYPE_DB},
	{"scene", ServiceType::SERVICE_TYPE_SCENE},
	{"scene_ctrl", ServiceType::SERVICE_TYPE_SCENE_CTRL},
};

ServiceType getServiceType(std::string serviceName) {
	auto iter = serviceName2Type.find(serviceName);
	if (iter == serviceName2Type.end()) {
		return ServiceType::SERVICE_TYPE_UNKNOW;
	}
	return iter->second;
}
