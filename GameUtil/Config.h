#pragma once
#include <string>
#include "GameUtil.h"

#define ERROR_CONFIG_INT -0x0FFFFFFF

class GAMEUTIL_API Config
{
public:
	static bool checkFileExist(char* fileName);
	static std::string getConfigStr(char* configFileName, char* key);
	static int getConfigInt(char* configFileName, char* key);
};

