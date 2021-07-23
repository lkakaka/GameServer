#pragma once

#include "sol/sol.hpp"
#include <map>

class LuaScene {
private:
	
public:
	static void bindLuaScene(std::shared_ptr<sol::state>);

	static sol::object createScene(int sceneId, sol::table script, sol::this_state s);
	
};
