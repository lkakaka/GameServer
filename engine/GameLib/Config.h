#pragma once
#include <string>
//#include "GameUtil.h"
#include "../Common/ServerExports.h"
#include "Singleton.h"

#define ERROR_CONFIG_INT -0x0FFFFFFF

class Config : public Singleton<Config>
{
private:
	std::string cfgFile;
public:
	Config(const char* cfgFile);
	static bool checkFileExist(const char* fileName);
	std::string getConfigStr(const char* key);
	int getConfigInt(const char* key);
};

