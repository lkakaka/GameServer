#pragma once

#include "sol/sol.hpp"
#include "DBHandler.h"

USING_DATA_BASE;

class LuaDB {
private:
	DBHandler* m_dbHandler;

	void luaTableToTable(sol::table luaTbl, Table* tbl);
	sol::table tableToLuaTable(Table* tbl, sol::this_state s);
public:
	LuaDB(const char* dbName);
	static void bindLuaDB(std::shared_ptr<sol::state>);
	
	sol::object executeSql(const char* sql, sol::this_state s);
	sol::object initTable(sol::table tblDefs, sol::this_state s);
	sol::object insertRow(sol::table obj, sol::this_state s);
	sol::object getRow(sol::table tblObj, sol::this_state s);
	sol::object updateRow(sol::table tblObj, sol::this_state s);
	sol::object deleteRow(sol::table tblObj, sol::this_state s);
	sol::object replaceRows(sol::table tblObj, sol::this_state s);
};
