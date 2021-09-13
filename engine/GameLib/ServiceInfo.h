#pragma once

#include "Singleton.h"
#include "ServiceType.h"

#define SERVICE_GROUP ServiceInfo::getSingleton()->getServiceGroup()
#define SERVICE_TYPE ServiceInfo::getSingleton()->getServiceType()
#define SERVICE_ID ServiceInfo::getSingleton()->getServiceId()

class ServiceInfo : public Singleton<ServiceInfo> {
private:
	int serviceGroup;
	ServiceType serviceType;
	int serviceId;
public:
	ServiceInfo(int serviceGroup, ServiceType serviceType, int serviceId);
	inline int getServiceGroup() { return serviceGroup; }
	inline int getServiceType() { return serviceType; }
	inline int getServiceId() { return serviceId; }
};


