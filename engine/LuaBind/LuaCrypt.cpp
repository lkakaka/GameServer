#include "LuaCrypt.h"
#include "Logger.h"

void LuaCrypt::bindLuaCrypt(std::shared_ptr<sol::state> lua) {
	sol::usertype<MD5> luaMD5_type = lua->new_usertype<MD5>("MD5");

	// typical member function that returns a variable
	luaMD5_type["update"] = sol::overload(sol::resolve<void(const char*, unsigned int)>(&MD5::update));
	luaMD5_type["hexdigest"] = &MD5::hexdigest;
	luaMD5_type["finalize"] = &MD5::finalize;

	sol::table disorderId = lua->create_named_table("DisorderID");
	disorderId["genDisorderId"] = &DisorderID::generate;
}
