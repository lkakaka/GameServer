#pragma once
#include <iostream>
#include <string.h>
#include <stdarg.h>
#include "../../Common/ServerExports.h"

class SERVER_EXPORT_API Logger
{
private:
	static Logger* g_log;

	static bool isPrint(const char* fmt);
public:
	static void test();
	static int initLog(const char* serverName);
	static void formatLog(char* buff, const char* fmt, va_list va);
	static void logDebug(const char* fmt, ...);
	static void logWarning(const char* fmt, ...);
	static void logInfo(char* fmt, ...);
	static void logError(const char* fmt, ...);
};

