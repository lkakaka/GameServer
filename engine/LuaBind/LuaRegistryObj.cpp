#include "LuaRegistryObj.h"
#include "Logger.h"

std::map<int, sol::object> LuaRegistryObj::m_registryObjs;
int LuaRegistryObj::m_curId = 1;

std::vector<int> LuaRegistryObj::addRegistryObj(sol::object obj) {
	m_curId++;
	m_registryObjs.emplace(m_curId, obj);
	auto iter = m_registryObjs.find(m_curId);
	int index = iter->second.registry_index();
	return std::vector<int>{ m_curId , index};
}

void LuaRegistryObj::removeRegistryObj(int objId) {
	m_registryObjs.erase(objId);
}



