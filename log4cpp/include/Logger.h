#pragma once
#include <iostream>
#include <string.h>
#include <stdarg.h>
//#include "log4cpp/Category.hh"
//#include "log4cpp/FileAppender.hh"
//#include "log4cpp/SimpleLayout.hh"
//#include "log4cpp/PropertyConfigurator.hh"
//#include <log4cpp/Portability.hh>
//#include <log4cpp/PatternLayout.hh>
//#include "log4cpp/DailyRollingFileAppender.hh"
//#include "log4cpp/OstreamAppender.hh"
#include "../../Common/ServerExports.h"

class SERVER_EXPORT_API Logger
{
private:
	static Logger* g_log;
	//static const log4cpp::Category& root;

	static bool isPrint(const char* fmt);
public:
	static void test();
	static int initLog();
	static void formatLog(char* buff, const char* fmt, va_list va);
	static void logDebug(const char* fmt, ...);
	static void logInfo(char* fmt, ...);
	static void logError(const char* fmt, ...);
};

