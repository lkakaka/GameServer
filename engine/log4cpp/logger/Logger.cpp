#include "../include/Logger.h"
#include "log4cpp/Category.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/SimpleLayout.hh"
#include "log4cpp/PropertyConfigurator.hh"
#include <log4cpp/Portability.hh>
#include <log4cpp/PatternLayout.hh>
#include "log4cpp/DailyRollingFileAppender.hh"
#include "log4cpp/OstreamAppender.hh"

#include "../Common/ServerMacros.h"


#define FORMAT_BUFF_SIZE 1024
#define MAX_LOG_FILE_DAYS 15 // 日志文件保存天数


std::vector<std::string> Logger::m_logs;


int Logger::initLog(int serverId, const char* logFile)
{
	log4cpp::Category& root = log4cpp::Category::getRoot();
	log4cpp::Category& console = log4cpp::Category::getInstance("console");
#ifdef _DEBUG
	//root.setPriority(log4cpp::Priority::NOTSET);
	console.setPriority(log4cpp::Priority::DEBUG);
#else
	root.setPriority(log4cpp::Priority::INFO);
	console.setPriority(log4cpp::Priority::INFO);
#endif
	std::string logFileName = logFile;
	char logDirName[64]{ 0 };
	sprintf(logDirName, "../log/%d/", serverId);
	//std::string logDirName = "../log/" + serverId;
	if (M_ACCESS(logDirName, 0) == -1 && M_MKDIR(logDirName, 0755) == -1) {
		printf("create log dir failed\n");
	}
	logFileName = logDirName + logFileName + ".log";
	log4cpp::Appender* rootAppender = new log4cpp::DailyRollingFileAppender("MyServer", logFileName.c_str(), MAX_LOG_FILE_DAYS);
	root.addAppender(rootAppender);
	log4cpp::Appender* consoleAppender = new log4cpp::OstreamAppender("MyServer", &std::cout);
	console.addAppender(consoleAppender);

	log4cpp::PatternLayout* patternLayout = new log4cpp::PatternLayout();
	patternLayout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S.%l} thread_id:%t [%p] %m %n");
	rootAppender->setLayout(patternLayout);

	log4cpp::PatternLayout* consolePatternLayout = new log4cpp::PatternLayout();
	consolePatternLayout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S.%l} thread_id:%t [%p] %m %n");
	consoleAppender->setLayout(consolePatternLayout);

	return 1;
}

//void Logger::_writeLog() {
//	while (true) {
//		if (Logger::m_logs.empty()) continue;
//		for (auto iter = Logger::m_logs.begin(); iter != Logger::m_logs.end(); iter++) {
//			if (Logger::isPrint(iter->c_str()))
//			{
//				//formatLog(buff, &fmt[1], args);
//				log4cpp::Category& sub = log4cpp::Category::getInstance("sub1");
//				//sub.notice(&fmt[1], args);
//				//std::string s = buff;
//				sub.notice(iter->erase(0, 1));
//			}
//			else {
//				//formatLog(buff, fmt, args);
//				log4cpp::Category& root = log4cpp::Category::getRoot();
//				//root.notice(fmt, args);
//				//std::string s = buff;
//				root.notice(*iter);
//			}
//		}
//	}
//}
//std::shared_ptr<std::thread> Logger::m_thread = std::make_shared<std::thread>(Logger::_writeLog);


bool Logger::isPrint(const char* fmt)
{
	if (NULL == fmt || strlen(fmt) <= 1)
		return false;
	return (fmt[0] == '$');
}

void Logger::formatLog(char* buff, const char* fmt, va_list va)
{
	//_vsnprintf_s(buff, FORMAT_BUFF_SIZE-1, _TRUNCATE, fmt, va);
	vsnprintf(buff, FORMAT_BUFF_SIZE-1, fmt, va);
	//Logger::m_logs.emplace_back(buff);
}

void Logger::logDebug(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	log4cpp::Category& root = log4cpp::Category::getRoot();
	root.debug(fmt, args);
	va_end(args);
}

void Logger::logWarning(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	log4cpp::Category& root = log4cpp::Category::getRoot();
	root.warn(fmt, args);
	va_end(args);
}

void Logger::logInfo(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	log4cpp::Category& root = log4cpp::Category::getRoot();
	root.info(fmt, args);
	va_end(args);
}

void Logger::logError(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	log4cpp::Category& root = log4cpp::Category::getRoot();
	root.error(fmt, args);
	va_end(args);
}

void Logger::logDebugWithConsole(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	log4cpp::Category& console = log4cpp::Category::getInstance("console");
	console.debug(fmt, args);
	va_end(args);
}

void Logger::logWarningWithConsole(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	log4cpp::Category& console = log4cpp::Category::getInstance("console");
	console.warn(fmt, args);
	va_end(args);
}

void Logger::logInfoWithConsole(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	log4cpp::Category& console = log4cpp::Category::getInstance("console");
	console.info(fmt, args);
	va_end(args);
}

void Logger::logErrorWithConsole(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	log4cpp::Category& console = log4cpp::Category::getInstance("console");
	console.error(fmt, args);
	va_end(args);
}

void Logger::test()
{
	log4cpp::Category& root = log4cpp::Category::getRoot();
	root.setPriority(log4cpp::Priority::NOTICE);
	log4cpp::Category& sub = log4cpp::Category::getInstance("sub1");

	log4cpp::Appender* rootAppender = new log4cpp::DailyRollingFileAppender("test", "test.log");
	root.addAppender(rootAppender);
	log4cpp::Appender* rootAppender1 = new log4cpp::OstreamAppender("test1", &std::cout);
	root.addAppender(rootAppender1);

	rootAppender->setThreshold(log4cpp::Priority::NOTICE);
	log4cpp::Appender* subAppender = new log4cpp::OstreamAppender("subtest", &std::cout);
	sub.addAppender(subAppender);

	//log4cpp::PropertyConfigurator::configure("logTest.Property");

	log4cpp::PatternLayout* patternLayout = new log4cpp::PatternLayout();
	patternLayout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S.%l} %t [%c] [%p] %m %n");
	rootAppender->setLayout(patternLayout);
	log4cpp::PatternLayout* subPatternLayout = new log4cpp::PatternLayout();
	subPatternLayout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S.%l} %t [%c] [%p] %m %n");
	subAppender->setLayout(subPatternLayout);

	root.notice("root1, %d,%s", 1, "1234");
	sub.notice("sub1");

	//Log::logInfo("$root1, %d,%s", 1, "1234");
}

