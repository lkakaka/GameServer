#pragma once

#include "crypt/md5.h"
#include "sol/sol.hpp"

class LuaCrypt {
public:
	static void bindLuaCrypt(std::shared_ptr<sol::state>);
};
