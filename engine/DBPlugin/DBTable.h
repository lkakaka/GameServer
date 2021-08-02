#pragma once
#include <vector>
#include "DataBase.h"

DATA_BASE_BEGIN

#define MAKE_TABLE_FIELD(field) std::shared_ptr<TableField> field = std::make_shared<TableField>()
#define MAKE_TABLE_FIELD_SCHEMA(fieldSchema) std::shared_ptr<TableFieldSchema> field = std::make_shared<TableFieldSchema>()

struct TableField
{
public:
	std::string fieldName;
	int64_t lval;
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
	int64_t priKeyVal;
	bool isAutoIncr;  // 主键是否自增
	std::map<std::string, std::shared_ptr<TableField>> fields;
	std::vector<std::string> colNames;

	std::vector<TableIndex> tableIndexs; // 索引信息

	Table():priKeyVal(0), isAutoIncr(false){}

	std::string redisKey() { 
		char buf[128]{0};
		snprintf(buf, 128, "%I64d", priKeyVal);
		return tableName + ":" + buf;
	}

	void addField(std::shared_ptr<TableField> field) {
		colNames.emplace_back(field->fieldName);
		fields.emplace(field->fieldName, field);
	}

	TableField* getField(const char* fieldName) {
		auto iter = fields.find(fieldName);
		if (iter == fields.end()) return NULL;
		return iter->second.get();
	}

	static std::string redisKey(const char* tableName, int64_t keyVal) {
		std::string redis_key = tableName;
		char buf[128];
		snprintf(buf, 128, "%I64d", keyVal);
		return redis_key + ":" + buf;
	}
};


struct TableFieldSchema
{
public:
	std::string fieldName;
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

	TableFieldSchema() : length(0), isDel(false) {}
};

class TableSchema
{
public:
	std::string tableName;
	std::string priKeyName;
	std::map<std::string, TableField> fields;
	std::vector<std::string> colNames;

	//std::vector<TableIndex> tableIndexs; // 索引信息

	void addFieldSchema(TableField field) {
		colNames.emplace_back(field.fieldName);
		fields.emplace(field.fieldName, field);
	}

	TableField* getFieldSchema(const char* fieldName) {
		auto iter = fields.find(fieldName);
		if (iter == fields.end()) return NULL;
		return &iter->second;
	}
};

DATA_BASE_END
