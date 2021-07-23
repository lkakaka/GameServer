#include "LuaConfig.h"
#include "Logger.h"

void LuaConfig::bindLuaConfig(std::shared_ptr<sol::state> lua) {
	/*sol::table timer = lua->create_named_table("Timer");
	timer["addTimer"] = &LuaTimer::addTimer;
	timer["removeTimer"] = &LuaTimer::removeTimer;*/

	sol::usertype<Config> luaConfig_type = lua->new_usertype<Config>("UTConfig");
	lua->set("Config", Config::getSingleton());
	//(*(lua->get()))["_Config"] = *(Config::getSingleton());

	// typical member function that returns a variable
	luaConfig_type["getConfigStr"] = &Config::getConfigStr;
	luaConfig_type["getConfigInt"] = &Config::getConfigInt;
}