#include "LuaDB.h"
#include "Logger.h"
#include "DBMgr.h"


LuaDB::LuaDB(const char* dbName) {
	m_dbHandler = DBMgr::getSingleton()->createDBHander(dbName);
}

void LuaDB::bindLuaDB(std::shared_ptr<sol::state> lua) {
	sol::usertype<LuaDB> luaDB_type = lua->new_usertype<LuaDB>("LuaDB",
		// 3 constructors
		sol::constructors<LuaDB(const char*)>());

	luaDB_type["executeSql"] = &LuaDB::executeSql;
	luaDB_type["initTable"] = &LuaDB::initTable;
	luaDB_type["insertRow"] = &LuaDB::insertRow;
	luaDB_type["getRow"] = &LuaDB::getRow;
	luaDB_type["updateRow"] = &LuaDB::updateRow;
	luaDB_type["replaceRows"] = &LuaDB::replaceRows;
	luaDB_type["deleteRow"] = &LuaDB::deleteRow;
}

sol::object LuaDB::executeSql(const char* sql, sol::this_state s) {
	StatementPtr ptr = m_dbHandler->executeSql(sql);
	if (ptr == NULL) sol::nil;
	//sql::Statement* st = ptr->getStatement();
	bool isResultSet = ptr->isResultSet();

	sol::state_view lua(s);
	sol::table result = sol::table::create_with(s.lua_state());
	/*LOG_INFO("execute sql return!!");*/
	while (true) {
		if (isResultSet) {
			sql::ResultSet* rs = ptr->getResultSet();
			sql::ResultSetMetaData* metaData = rs->getMetaData();
			int colCount = metaData->getColumnCount();
			//sol::table fieldTuple = sol::table::create_with(s.lua_state());
			//for (int i = 1; i <= colCount; i++) {
			//	//std::string fieldName = metaData->getColumnLabel(i).c_str();
			//	/*const char* name = metaData->getColumnLabel(i).c_str();*/

			//	//sol::object obj = sol::make_object(lua, metaData->getColumnLabel(i).c_str());
			//	fieldTuple[i] = sol::make_object(lua, metaData->getColumnLabel(i).c_str());
			//	//fieldTuple[i] = metaData->getColumnLabel(i);
			//}
			//result.add(fieldTuple);

			while (rs->next()) {
				sol::table dataTuple = sol::table::create_with(s.lua_state());
				for (int i = 1; i <= colCount; i++) {
					sol::object colName = sol::make_object(lua, metaData->getColumnLabel(i).c_str());

					int colType = metaData->getColumnType(i);
					if (colType >= sql::DataType::BIT && colType <= sql::DataType::BIGINT) {
						dataTuple[colName] = sol::make_object(lua, rs->getInt(i));
					}
					else if (colType >= sql::DataType::REAL && colType <= sql::DataType::NUMERIC) {
						dataTuple[colName] = sol::make_object(lua, rs->getDouble(i));
					}
					else {
						dataTuple[colName] = sol::make_object(lua, rs->getString(i).c_str());
					}
				}
				result.add(dataTuple);
			}
		}
		else {
			int64_t updateCount = ptr->getUpdateCount();
			if (updateCount < 0) {
				break;
			}
			
			sol::table filedTuple = sol::table::create_with(s.lua_state());
			filedTuple[1] = sol::make_object(lua, "update_count");
			result.add(filedTuple);
			sol::table dataTuple = sol::table::create_with(s.lua_state());
			dataTuple[1] = updateCount;
			result.add(dataTuple);
		}

		isResultSet = ptr->getMoreResults();
	}

	return result;
}

static bool _initTable(DBHandler* dbHandler, sol::table tblObj) {
	Table tbl;
	const char* tbName = ((sol::object)tblObj["tb_name"]).as<const char*>();

	tbl.tableName = tbName;
	sol::table colTuple = tblObj["columns"];
	bool hasKey = false;
	for (int col = 1; col <= colTuple.size(); col++) {
		
		sol::table colObj = colTuple[col];
		const char* colName = ((sol::object)colObj["name"]).as<const char*>();
		sol::object defaultObj = colObj["default"];
		int type = ((sol::object)colObj["type"]).as<int>();

		MAKE_TABLE_FIELD(field);
		field->fieldName = colName;
		field->type = (TableField::FieldType)type;

		if (colObj["old_name"] != sol::nil) {
			field->oldName = ((sol::object)colObj["old_name"]).as<const char*>();
		}
		if (colObj["is_del"] != sol::nil) {
			field->isDel = ((sol::object)colObj["is_del"]).as<bool>();
		}

		switch (type)
		{
		case TableField::FieldType::TYPE_INT:
		case TableField::FieldType::TYPE_BIGINT:
		{
			if (defaultObj != sol::nil) {
				long defVal = defaultObj.as<int>();
				field->defaut_val = defVal;
			}
			break;
		}
		case TableField::FieldType::TYPE_DOUBLE:
		{
			if (defaultObj != sol::nil) {
				double defVal = defaultObj.as<double>();
				field->defaut_val = defVal;
			}
			break;
		}
		case TableField::FieldType::TYPE_VCHAR:
		{
			sol::object lenObj = colObj["length"];
			if (lenObj != sol::nil) {
				long len = lenObj.as<int>();
				field->length = len;
			}
			if (defaultObj != sol::nil) {
				const char* defVal = defaultObj.as<const char*>();
				if (strcmp(defVal, "") != 0) {
					field->defaut_val.append("'").append(defVal).append("'");
				}
			}
			break;
		}
		case TableField::FieldType::TYPE_TEXT:
		{
			if (defaultObj != sol::nil) {
				const char* defVal = defaultObj.as<const char*>();
				field->defaut_val.append("'").append(defVal).append("'");
			}
			break;
		}
		default:
			LOG_ERROR("not support table col type %ld, table:%s", type, tbName);
			return false;
		}
		sol::object keyObj = colObj["key"];
		if (keyObj != sol::nil && keyObj.as<bool>()) {
			if (hasKey) {
				LOG_ERROR("creat table %s failed, has set primary key", tbName);
				return false;
			}
			tbl.priKeyName = colName;
			sol::object autoIncrObj = colObj["auto_incr"];
			if (autoIncrObj != sol::nil && autoIncrObj.as<bool>()) {
				tbl.isAutoIncr = true;
			}
			hasKey = true;
		}

		tbl.addField(field);
	}

	if (!hasKey) {
		LOG_ERROR("creat table %s failed, not set primary key", tbName);
		return false;
	}

	// ³õÊ¼»¯Ë÷Òý
	sol::table tbIndexTuple = tblObj["indexs"];
	if (tbIndexTuple != sol::nil) {
		for (int idx = 1; idx <= tbIndexTuple.size(); idx++) {
			TableIndex tblIndex;
			sol::table idxObj = tbIndexTuple[idx];
			sol::table colsObj = idxObj["cols"];
			tblIndex.isUnique = ((sol::object)idxObj["is_unique"]).as<bool>();
			for (int idxCol = 1; idxCol <= colsObj.size(); idxCol++) {
				sol::object colNameObj = colsObj[idxCol];
				tblIndex.cols.emplace_back(colNameObj.as<const char*>());
			}
			tbl.tableIndexs.emplace_back(tblIndex);
		}
	}

	return dbHandler->initTable(&tbl);
}

sol::object LuaDB::initTable(sol::table tblDefs, sol::this_state s) {
	sol::state_view lua(s);

	for (auto& kv : tblDefs) {
		//sol::object key = kv.first;
		sol::table tbObj = kv.second;
		if (!_initTable(m_dbHandler, tbObj)) {
			sol::object tbName = tbObj["tb_name"];
			const char* strName = tbName.as<const char*>();
			LOG_ERROR("table %s init failed", strName);
			return sol::make_object(lua, false);
		}
	}
	m_dbHandler->initTableSchema();
	return sol::make_object(lua, true);
}

sol::object LuaDB::insertRow(sol::table obj, sol::this_state s) {
	sol::object tableNameObj = obj["table_name"];
	sol::object priKeyObj = obj["pri_key"];
	sol::table fields = obj["fields"];
	const char* tableName = tableNameObj.as<const char*>();
	long priKeyVal = 0;
	if (priKeyObj != sol::nil) {
		long priKeyVal = priKeyObj.as<long>();
	}

	Table tbl;
	tbl.tableName = tableName;
	tbl.priKeyVal = priKeyVal;
	for (auto& kv : fields) {
		sol::object key = kv.first;
		sol::object val = kv.second;
		const char* fieldName = key.as<const char*>();
		DataBase::TableField* fieldDesc = m_dbHandler->getTableField(tableName, fieldName);
		if (fieldDesc == NULL) {
			return sol::nil;
		}

		MAKE_TABLE_FIELD(field);
		field->fieldName = fieldName;
		field->type = fieldDesc->type;
		switch (fieldDesc->type)
		{
		case TableField::FieldType::TYPE_INT:
		case TableField::FieldType::TYPE_BIGINT:
		{
			field->lval = val.as<long>();
			break;
		}
		case TableField::FieldType::TYPE_DOUBLE:
		{
			field->dval = val.as<double>();
			break;
		}
		case TableField::FieldType::TYPE_VCHAR:
		case TableField::FieldType::TYPE_TEXT:
		{
			field->sval = val.as<const char*>();
			break;
		}
		default:
		{
			LOG_ERROR("not support field type, table:%s, field:%s", tableName, fieldName);
			return sol::nil;
		}
		}
		tbl.addField(field);
	}

	if (!m_dbHandler->insertRow(&tbl))
	{
		return sol::nil;
	}
	sol::state_view lua(s);
	return sol::make_object(lua, tbl.priKeyVal);
}

void LuaDB::luaTableToTable(sol::table luaTbl, Table* tbl) {
	sol::object tableNameObj = luaTbl["table_name"];
	const char* tableName = tableNameObj.as<const char*>();
	tbl->tableName = tableName;

	sol::table row = luaTbl["row"];
	for (auto& kv : row) {
		const char* fieldName = kv.first.as<const char*>();
		sol::object valObj = kv.second;
		DataBase::TableField* fieldDesc = m_dbHandler->getTableField(tbl->tableName.c_str(), fieldName);
		if (fieldDesc == NULL) {
			// todo:ÈÝ´í
			LOG_ERROR("not found table field, table:%s, field:%s", tableName, fieldName);
			continue;
		}

		MAKE_TABLE_FIELD(tbField);
		tbField->fieldName = fieldName;
		tbField->type = fieldDesc->type;
		switch (tbField->type)
		{
			case TableField::FieldType::TYPE_INT:
			case TableField::FieldType::TYPE_BIGINT:
			{
				tbField->lval = valObj.as<long>();
				break;
			}
			case TableField::FieldType::TYPE_DOUBLE:
			{
				tbField->defaut_val = valObj.as<double>();
				break;
			}
			case TableField::FieldType::TYPE_VCHAR:
			case TableField::FieldType::TYPE_TEXT:
			{
				tbField->sval = valObj.as<const char*>();
				break;
			}
			default:
				LOG_ERROR("not support table col type %ld, table:%s", tbField->type, tbl->tableName.c_str());
		}
		tbl->addField(tbField);
	}
}
 
sol::table LuaDB::tableToLuaTable(Table* tbl, sol::this_state s) {
	std::string tableName = tbl->tableName;
	sol::state_view lua(s);
	sol::table tblObj = sol::table::create(s.lua_state());
	for (auto iter : tbl->fields) {
		std::string colName = iter.first;
		std::shared_ptr<TableField> field = iter.second;
		long type = field->type;
		switch (type)
		{
			case TableField::FieldType::TYPE_INT:
			case TableField::FieldType::TYPE_BIGINT:
			{
				tblObj[colName] = sol::make_object(lua, field->lval);
				break;
			}
			case TableField::FieldType::TYPE_DOUBLE:
			{
				tblObj[colName] = sol::make_object(lua, field->dval);
				break;
			}
			case TableField::FieldType::TYPE_VCHAR:
			case TableField::FieldType::TYPE_TEXT:
			{
				tblObj[colName] = sol::make_object(lua, field->sval.c_str());
				break;
			}
			default:
				LOG_ERROR("not support table col type %ld, table:%s", type, tbl->tableName.c_str());
		}
	}
	return tblObj;
}

sol::object LuaDB::getRow(sol::table tblObj, sol::this_state s) {
	Table tbl;
	luaTableToTable(tblObj, &tbl);
	std::vector<Table> result;
	if (!m_dbHandler->getRow(&tbl, result)) {
		return sol::nil;
	}

	sol::table lst = sol::table::create_with(s.lua_state());
	for (Table tbl : result) {
		sol::table obj = tableToLuaTable(&tbl, s);
		lst.add(obj);
	}

	return lst;
}

sol::object LuaDB::updateRow(sol::table tblObj, sol::this_state s) {
	Table tbl;
	luaTableToTable(tblObj, &tbl);
	sol::state_view lua(s);
	if (!m_dbHandler->updateRow(&tbl))
	{
		return sol::make_object(s, false);
	}
	return sol::make_object(s, true);
}

sol::object LuaDB::deleteRow(sol::table tblObj, sol::this_state s) {
	Table tbl;
	luaTableToTable(tblObj, &tbl);

	if (!m_dbHandler->deleteRow(&tbl))
	{
		return sol::make_object(s, false);
	}
	return sol::make_object(s, true);
}

sol::object LuaDB::replaceRows(sol::table tblObj, sol::this_state s) {
	for (int i = 1; i <= tblObj.size(); i++) {
		sol::table row = tblObj[i];
		Table tbl;
		luaTableToTable(row, &tbl);
		if (!m_dbHandler->replaceRow(&tbl))
		{
			return sol::make_object(s, false);
		}
	}

	return sol::make_object(s, true);
}


