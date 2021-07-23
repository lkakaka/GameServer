//#define TEST_SOL

#ifdef TEST_SOL
#define SOL_ALL_SAFETIES_ON 1

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <sol/sol.hpp>

#include <iostream>

struct two_things {
	int a;
	bool b;
};

template <typename Handler>
bool sol_lua_check(sol::types<two_things>, lua_State* L, int index, Handler&& handler, sol::stack::record& tracking) {
	// indices can be negative to count backwards from the top of the stack,
	// rather than the bottom up
	// to deal with this, we adjust the index to
	// its absolute position using the lua_absindex function
	int absolute_index = lua_absindex(L, index);
	// Check first and second second index for being the proper types
	bool success = sol::stack::check<int>(L, absolute_index, handler) && sol::stack::check<bool>(L, absolute_index + 1, handler);
	tracking.use(2);
	std::cout << "=== sol_lua_check ===" << std::endl;
	return success;
}

two_things sol_lua_get(sol::types<two_things>, lua_State* L, int index, sol::stack::record& tracking) {
	int absolute_index = lua_absindex(L, index);
	// Get the first element
	int a = sol::stack::get<int>(L, absolute_index);
	// Get the second element,
	// in the +1 position from the first
	bool b = sol::stack::get<bool>(L, absolute_index + 1);
	// we use 2 slots, each of the previous takes 1
	tracking.use(2);
	std::cout << "=== sol_lua_get ===" << std::endl;
	return two_things{ a, b };
}

int sol_lua_push(lua_State* L, const two_things& things) {
	int amount = sol::stack::push(L, things.a);
	// amount will be 1: int pushes 1 item
	amount += sol::stack::push(L, things.b);
	// amount 2 now, since bool pushes a single item
	// Return 2 things
	std::cout << "=== sol_lua_push ===" << std::endl;
	return amount;
}

int main() {
	std::cout << "=== customization ===" << std::endl;
	std::cout << std::boolalpha;

	sol::state lua;
	lua.open_libraries(sol::lib::base);
	// Create a pass-through style of function
	lua.script("function f ( a, b ) print(a, b) return a, b end");

	// get the function out of Lua
	sol::function f = lua["f"];

	two_things things = f(two_things{ 24, false });
	assert(things.a == 24);
	assert(things.b == false);
	// things.a == 24
	// things.b == true

	std::cout << "things.a: " << things.a << std::endl;
	std::cout << "things.b: " << things.b << std::endl;
	std::cout << std::endl;

	return 0;
}

//
//int main() {
//	std::cout << "=== self_call ===" << std::endl;
//
//	sol::state lua;
//	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table);
//
//	// a small script using 'self' syntax
//	lua.script(R"(
//	some_table = { some_val = 100 }
//	function some_table:add_to_some_val(value)
//	    self.some_val = self.some_val + value
//	end
//	function print_some_val()
//	    print("some_table.some_val = " .. some_table.some_val)
//	end
//	)");
//
//	lua.script("for k,v in pairs(_G) do print(k,v) end");
//
//	// do some printing
//	lua["print_some_val"]();
//	// 100
//
//	sol::table self = lua["some_table"];
//	self["add_to_some_val"](self, 10);
//	lua["print_some_val"]();
//
//	std::cout << std::endl;
//
//	return 0;
//}

#endif
