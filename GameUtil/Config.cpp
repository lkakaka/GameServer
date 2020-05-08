#include "Config.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>

bool Config::checkFileExist(char* fileName)
{
	std::ifstream in(fileName);
	return in.good();
}

std::string Config::getConfigStr(char* configFileName, char* key)
{
	std::ifstream in(configFileName);
	//char line[128]{0};

	std::string line;
	while (getline(in, line))
	{
		if (line.size() == 0 || line[0] == '#') {
			continue;
		}
		int index = line.find('=', 0);
		if (index <= 0) {
			continue;
		}

		std::string line_key = line.substr(0, index);
		if (strcmp(line_key.c_str(), key) == 0) {
			return line.substr(index + 1, line.length() - 1);
		}
	}
	return "";
}

int Config::getConfigInt(char* configFileName, char* key)
{
	std::string val = getConfigStr(configFileName, key);
	if (val.length() == 0) {
		return ERROR_CONFIG_INT;
	}
	return atoi(val.c_str());
}
