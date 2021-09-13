#pragma once
#include <string>
//#include "GameUtil.h"
#include "../Common/ServerExports.h"
#include "Singleton.h"

#define ERROR_CONFIG_INT -0x0FFFFFFF

#define GET_CONFG_INT(key) Config::getSingleton()->getConfigInt(key)
#define GET_CONFG_STR(key) Config::getSingleton()->getConfigStr(key)

class Config : public Singleton<Config>
{
private:
	std::string cfgFile;
public:
	Config(const char* cfgFile);
	static bool checkFileExist(const char* fileName);
	std::string getConfigStr(const char* key);
	int getConfigInt(const char* key);
	inline const char* getConfigFileName() { return cfgFile.c_str(); }
};

