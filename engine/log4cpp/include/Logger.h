#pragma once
#include <iostream>
#include <string.h>
#include <stdarg.h>
#include "../../Common/ServerExports.h"
#include <thread>
#include <vector>


class SERVER_EXPORT_API Logger
{
private:
	static std::shared_ptr<std::thread> m_thread;
	static std::vector<std::string> m_logs;

	static bool isPrint(const char* fmt);
	static void _writeLog();
public:
	static Logger* g_log;
	//static std::list < std::
	static void test();
	static int initLog(const char* serverName);
	static void formatLog(char* buff, const char* fmt, va_list va);
	static void logDebug(const char* fmt, ...);
	static void logWarning(const char* fmt, ...);
	static void logInfo(char* fmt, ...);
	static void logError(const char* fmt, ...);
};

