#pragma once
#include <iostream>
#include "log4cpp/Category.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/SimpleLayout.hh"
#include "log4cpp/PropertyConfigurator.hh"
#include <log4cpp/Portability.hh>
#include <log4cpp/PatternLayout.hh>
#include "log4cpp/DailyRollingFileAppender.hh"
#include "log4cpp/OstreamAppender.hh"

class Log
{
private:
	static Log* g_log;
	static const log4cpp::Category& root;

	static void test();
	static bool isPrint(const char* fmt);
public:
	static int initLog();
	static void logDebug(const char* fmt, ...);
	static void logInfo(char* fmt, ...);
	static void logError(const char* fmt, ...);

};

