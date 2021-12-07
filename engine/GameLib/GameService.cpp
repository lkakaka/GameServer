#include "GameService.h"
#include "Logger.h"


INIT_SINGLETON_CLASS(GameService)

GameService::GameService(int serverId, ServiceType serviceType, int serviceId, IScript* script) :
	m_serviceGroup(serverId), m_serviceType(serviceType), m_serviceId(serviceId), m_script(script)
{
	
}

GameService::~GameService() {
	
}
