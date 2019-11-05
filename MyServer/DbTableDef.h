#pragma once
#include "Reflect.h"

class TblPlayer : ReflectObject {
public:
	int id = 0;
	std::string name = "";
	int age = 0;

	TblPlayer()
	{
		setTypeName("TblPlayer");
		registerField("id", TYPE_INT, &id);
		registerField("name", TYPE_STRING, &name);
		registerField("age", TYPE_INT, &age);
	}
};