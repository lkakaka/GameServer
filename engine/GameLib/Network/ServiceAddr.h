#pragma once

#include "../../Common/ServerExports.h"
#include "../MyBuffer.h"

class ServiceAddr {
private:
	int serviceGroup;  // 服务器ID
	int serviceType;  // 服务类型
	int serviceId;  // 服务编号

	std::string addrName;

	std::string toString();

public:
	ServiceAddr();
	ServiceAddr(int serviceGroup, int serviceType, int serviceId);
	void serialize(MyBuffer* buffer); 
	
	inline const char* getName() { return addrName.c_str(); }
	inline int getServiceGroup() { return serviceGroup; }
	inline int getServiceType() { return serviceType; }
	inline int getServiceId() { return serviceId; }

	inline void changeServiceAddr(int serviceType, int serviceId) {
		this->serviceType = serviceType;
		this->serviceId = serviceId;
		addrName = toString();
	}

	void parseAddr(char* addr);
};


