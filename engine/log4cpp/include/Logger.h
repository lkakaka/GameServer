#pragma once
#include <iostream>
#include <string.h>
#include <stdarg.h>
#include "../../Common/ServerExports.h"
#include <thread>
#include <vector>

#ifndef LOG_NOT_ON_CONSOLE
#define LOG_DEBUG(fmt, ...) do { Logger::logDebugWithConsole(fmt, ##__VA_ARGS__); } while(0)
#define LOG_INFO(fmt, ...)  do { Logger::logInfoWithConsole(fmt, ##__VA_ARGS__); } while(0)
#define LOG_WARN(fmt, ...)  do { Logger::logWarningWithConsole(fmt, ##__VA_ARGS__); } while(0)
#define LOG_ERROR(fmt, ...) do { Logger::logErrorWithConsole(fmt, ##__VA_ARGS__); } while(0)
#else
#define LOG_DEBUG(fmt, ...) do { Logger::logDebug(fmt, ##__VA_ARGS__); } while(0)
#define LOG_INFO(fmt, ...)  do { Logger::logInfo(fmt, ##__VA_ARGS__); } while(0)
#define LOG_WARN(fmt, ...)  do { Logger::logWarning(fmt, ##__VA_ARGS__); } while(0)
#define LOG_ERROR(fmt, ...) do { Logger::logError(fmt, ##__VA_ARGS__); } while(0)
#endif


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
	static void logInfo(const char* fmt, ...);
	static void logError(const char* fmt, ...);

	static void logDebugWithConsole(const char* fmt, ...);
	static void logWarningWithConsole(const char* fmt, ...);
	static void logInfoWithConsole(const char* fmt, ...);
	static void logErrorWithConsole(const char* fmt, ...);
};

