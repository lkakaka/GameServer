#pragma once
#include "ServiceInfo.h"

INIT_SINGLETON_CLASS(ServiceInfo)

ServiceInfo::ServiceInfo(int serviceGroup, ServiceType serviceType, int serviceId) : 
	serviceGroup(serviceGroup), serviceType(serviceType), serviceId(serviceId)
{

}