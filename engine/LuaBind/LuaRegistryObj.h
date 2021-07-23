#pragma once

#include "sol/sol.hpp"
#include <map>

class LuaRegistryObj {
private:
	static int m_curId;
	static std::map<int, sol::object> m_registryObjs;
public:
	static std::vector<int> addRegistryObj(sol::object);
	static void removeRegistryObj(int ref);
};
