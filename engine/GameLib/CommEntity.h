#pragma once

#include "Logger.h"
#include "../Common/ServerExports.h"
#include "MyBuffer.h"
#include "Config.h"
#include "ServiceType.h"

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
	
	inline std::string* getName() { return &addrName; }
	inline int getServiceGroup() { return serviceGroup; }
	inline int getServiceType() { return serviceType; }
	inline int getServiceId() { return serviceId; }

	void parseAddr(char* addr);
};

class CommEntityInf {
protected:
	ServiceAddr addr;
public:
	CommEntityInf(ServiceAddr& addr): addr(addr) { };
	virtual void sendToService(ServiceAddr* dstAddr, char* msg, int msgLen) = 0;
	virtual void onRecvServiceMsg(ServiceAddr* srcAddr, char* msg, int msgLen) = 0;
};


