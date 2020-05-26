#pragma once
#include <vector>
#include "DataBase.h"

DATA_BASE_BEGIN

struct TableField
{
public:
	std::string fieldName;
	long lval;
	double dval;
	std::string sval;
	std::string defaut_val;
	int length;  // 字段长度,TYPE_VCHAR有效

	// 创建表使用
	std::string oldName;
	bool isDel;

	enum FieldType
	{
		TYPE_INT = 0,
		TYPE_BIGINT = 1,
		TYPE_DOUBLE = 2,
		TYPE_VCHAR = 3,
		TYPE_TEXT = 4,
	}type;

	TableField() : lval(0), dval(0.0), length(0), isDel(false){}
};

struct TableIndex
{
	std::vector<std::string> cols;
	bool isUnique;
	std::string indexName;
	
	std::string redisKey;
};

class Table
{
public:
	std::string tableName;
	std::string priKeyName;
	long priKeyVal;  // 主键只能是自增
	std::map<std::string, TableField> fields;
	std::vector<std::string> colNames;

	std::vector<TableIndex> tableIndexs; // 索引信息

	std::string redisKey() { 
		char buf[64];
		snprintf(buf, 64, "%ld", priKeyVal);
		return tableName + ":" + buf;
	}

	void addField(TableField field) {
		colNames.emplace_back(field.fieldName);
		fields.emplace(field.fieldName, field);
	}

	static std::string redisKey(const char* tableName, long keyVal) {
		std::string redis_key = tableName;
		char buf[64];
		snprintf(buf, 64, "%ld", keyVal);
		return redis_key + ":" + buf;
	}
};

DATA_BASE_END
