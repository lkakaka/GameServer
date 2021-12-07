#pragma once
#include <string>
#include "Singleton.h"

#include "Network/ServiceCommEntity.h"
#include "ServiceType.h"
#include "IScript.h"

#define SERVICE_GROUP GameService::getSingleton()->getServiceGroup()
#define SERVICE_TYPE GameService::getSingleton()->getServiceType()
#define SERVICE_ID GameService::getSingleton()->getServiceId()

class GameService : public Singleton<GameService> {
private:
	int m_serviceGroup;  // 服务器ID
	ServiceType m_serviceType; // 服务类型
	int m_serviceId; // 服务ID
	IScript* m_script;

public:
	GameService(int serverId, ServiceType serviceType, int serviceId, IScript* script);
	~GameService();

	inline int getServiceGroup() { return m_serviceGroup; }
	inline ServiceType getServieType() { return m_serviceType; }
	inline int getServiceId() { return m_serviceId; }

	inline void dispatchClientMsgToScript(int connId, int msgId, const char* data, int len) {
		m_script->dispatchClientMsgToScript(connId, msgId, data, len);
	}

	inline void dispatchServiceMsgToScript(ServiceAddr* srcAddr, int msgId, const char* data, int len) {
		m_script->dispatchServiceMsgToScript(srcAddr, msgId, data, len);
	}
};


