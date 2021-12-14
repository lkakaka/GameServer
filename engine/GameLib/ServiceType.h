#pragma once
#include <string>

enum ServiceType {
	SERVICE_TYPE_UNKNOW = -1,
	SERVICE_TYPE_START = 0,
	// �������Ͷ���
	SERVICE_TYPE_GROUP_CENTER = 0,
	SERVICE_TYPE_CENTER = 1,
	SERVICE_TYPE_GATEWAY = 2,
	SERVICE_TYPE_LOGIN = 3,
	SERVICE_TYPE_DB = 4,
	SERVICE_TYPE_SCENE = 5,
	SERVICE_TYPE_SCENE_CTRL = 6,
	// �������Ͷ������(����������Ҫ�޸�SERVICE_TYPE_END)
	SERVICE_TYPE_END = 7,
};

ServiceType getServiceType(std::string& serviceName);
