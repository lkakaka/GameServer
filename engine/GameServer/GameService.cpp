#include "GameService.h"
#include "../Common/PyCommon.h"
#include <map>


GameService* GameService::g_gameService = NULL;

static std::map<std::string, ServiceType> serviceName2Type = { 
	{"router", ServiceType::SERVIE_TYPE_ROUTER},
	{"gateway", ServiceType::SERVIE_TYPE_GATEWAY},
	{"login", ServiceType::SERVIE_TYPE_LOGIN},
	{"db", ServiceType::SERVIE_TYPE_DB},
	{"scene", ServiceType::SERVIE_TYPE_SCENE},
	{"scene_ctrl", ServiceType::SERVIE_TYPE_SCENE_CTRL},
};

ServiceType getServiceType(std::string serviceName) {
	auto iter = serviceName2Type.find(serviceName);
	if (iter == serviceName2Type.end()) {
		return ServiceType::SERVICE_TYPE_UNKNOW;
	}
	return iter->second;
}

GameService::GameService(std::string service_name, ServiceType serviceType, PyObject* scriptObj) :
	service_name(service_name), m_serviceType(serviceType), m_scriptObj(scriptObj)
{

}

PyObject* GameService::callPyFunc(const char* funcName, PyObject* args) {
	auto func = PyObject_GetAttrString(m_scriptObj, funcName);
	PyObject* obj = PyObject_Call(func, args, NULL);
	if (obj == NULL) {
		//PyErr_Print();
		logPyException();
	}
	return obj;
}