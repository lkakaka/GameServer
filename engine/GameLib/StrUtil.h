#pragma once
#include "../Common/ServerExports.h"
#include <stdint.h>
#include <vector>
#include <string>

class SERVER_EXPORT_API StrUtil
{
public:
	static std::vector<std::string> split(char* str, const char* delimiters);
};

