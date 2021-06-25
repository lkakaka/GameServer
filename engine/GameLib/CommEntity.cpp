#include "CommEntity.h"
#include "Logger.h"
#include "Const.h"
#include "StrUtil.h"

ServiceAddr::ServiceAddr() : serviceGroup(0), serviceType(0), serviceId(0) {
	addrName = toString();
};

ServiceAddr::ServiceAddr(int serviceGroup, int serviceType, int serviceId) : serviceGroup(serviceGroup), serviceType(serviceType), serviceId(serviceId) {
	addrName = toString();
};

void ServiceAddr::serialize(MyBuffer* buffer) {
	buffer->writeInt(this->serviceGroup);
	buffer->writeInt(this->serviceType);
	buffer->writeInt(this->serviceId);
}

void ServiceAddr::parseAddr(char* addr) {
	std::vector<std::string> vec = StrUtil::split(addr, ".");
	if (vec.size() != 3) {
		Logger::logError("service addr(%s) is error", addr);
	}
	serviceGroup = atoi(vec[0].c_str());
	serviceType = atoi(vec[1].c_str());
	serviceId = atoi(vec[2].c_str());
	addrName = toString();
}


std::string ServiceAddr::toString() {
	char str[1024]{ 0 };
	sprintf(str, "%d.%d.%d", this->serviceGroup, this->serviceType, this->serviceId);
	return str;
}
