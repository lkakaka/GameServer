#include "Log.h"

int Log::initLog()
{
	log4cpp::Category& root = log4cpp::Category::getRoot();
	log4cpp::Category& sub = log4cpp::Category::getInstance("sub1");
#ifdef _DEBUG
	//root.setPriority(log4cpp::Priority::NOTSET);
	sub.setPriority(log4cpp::Priority::NOTSET);
#else
	root.setPriority(log4cpp::Priority::NOTICE);
	sub.setPriority(log4cpp::Priority::NOTICE);
#endif

	log4cpp::Appender* rootAppender = new log4cpp::DailyRollingFileAppender("MyServer", "Server.log");
	root.addAppender(rootAppender);
	log4cpp::Appender* subAppender = new log4cpp::OstreamAppender("MyServer", &std::cout);
	sub.addAppender(subAppender);

	log4cpp::PatternLayout* patternLayout = new log4cpp::PatternLayout();
	patternLayout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S.%l} [%p] %m %n");
	rootAppender->setLayout(patternLayout);

	log4cpp::PatternLayout* subPatternLayout = new log4cpp::PatternLayout();
	subPatternLayout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S.%l} [%p] %m %n");
	subAppender->setLayout(subPatternLayout);

	return 1;
}

bool Log::isPrint(const char* fmt)
{
	if (NULL == fmt || strlen(fmt) <= 1)
		return false;
	return (fmt[0] == '%');
}

void Log::logDebug(const char* fmt, ...)
{
	log4cpp::Category& root = log4cpp::Category::getRoot();
	va_list args;
	va_start(args, fmt);
	if (Log::isPrint(fmt))
	{
		root.debug(static_cast<const char*>(&fmt[1]), args);
		log4cpp::Category& sub = log4cpp::Category::getInstance("sub1");
		sub.debug(fmt, args);
	}else {
		root.debug(fmt, args);
	}
	va_end(args);
}

void Log::logInfo(char* fmt, ...)
{
	log4cpp::Category& root = log4cpp::Category::getRoot();
	va_list args;
	va_start(args, fmt);
	if (Log::isPrint(fmt))
	{
		root.notice(static_cast<const char*>(&fmt[1]), args);
		log4cpp::Category& sub = log4cpp::Category::getInstance("sub1");
		sub.notice(fmt, args);
	}else {
		root.notice(fmt, args);
	}
	va_end(args);
}

void Log::logError(const char* fmt, ...)
{
	log4cpp::Category& root = log4cpp::Category::getRoot();
	va_list args;
	va_start(args, fmt);
	if (Log::isPrint(fmt))
	{
		root.error(static_cast<const char*>(&fmt[1]), args);
		log4cpp::Category& sub = log4cpp::Category::getInstance("sub1");
		sub.error(fmt, args);
	}else {
		root.error(fmt, args);
	}
	va_end(args);
}

void Log::test()
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

	root.notice("root1");
	sub.notice("sub1");
}

