#pragma once
#include <map>
#include <string>
#include <functional>
#include <vector>


enum FieldType
{
	TYPE_INT,
	TYPE_STRING,
};

class Field
{
public:
	std::string name;
	int type;
	void* addr;
};

class Type
{
public:
	std::string name;
	std::map<std::string, Field> fields;
};

class Method
{
	std::string name;
};

class ReflectObject
{
private:
	std::map<std::string, Field> fieldMap;
	std::map<std::string, Method> methodMap;
protected:
	Type type;
	
	void registerField(std::string fieldName, int fieldType, void* addr)
	{
		Field field;
		field.name = fieldName;
		field.type = fieldType;
		field.addr = addr;
		type.fields.insert(std::make_pair(fieldName, field));
	}

public:
	void setField(std::string fieldName, int val)
	{
		auto iter = type.fields.find(fieldName);
		Field field = (Field)iter->second;
		*(int*)field.addr = val;
		//*addr = val;
		//*field.addr = val;
	}

	void setField(std::string fieldName, std::string val)
	{
		auto iter = type.fields.find(fieldName);
		Field field = (Field)iter->second;
		*(std::string*)field.addr = val;
		//*addr = val;
		//std::copy(val.begin(), val.end(), std::back_inserter(*field));
	}
};

class ReflectObjectFactory
{
	static std::map<std::string, std::function<ReflectObject* (void)>> classMap;
public:
	static void registerClass(std::string className, std::function<ReflectObject* (void)> createFunc) 
	{
		classMap.insert(std::make_pair(className, createFunc));
	}

	static ReflectObject* createClass(std::string className)
	{
		auto iter = classMap.find(className);
		if (iter == classMap.end()) {
			return NULL;
		}
		std::function<ReflectObject* (void)> func = iter->second;
		return func();
	}
};

class RelectTest : public ReflectObject
{
public:
	int id = 0;
	std::string name = "";

	RelectTest()
	{
		registerField("id", TYPE_INT, &id);
		registerField("name", TYPE_STRING, &name);
	}
};
