#pragma once
#include <string>
//#include "GameUtil.h"
#include "../Common/ServerExports.h"

#define ERROR_CONFIG_INT -0x0FFFFFFF

class Config
{
public:
	static bool checkFileExist(char* fileName);
	static std::string getConfigStr(const char* configFileName, char* key);
	static int getConfigInt(const char* configFileName, char* key);
};

