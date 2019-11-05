#pragma once

#include <stdio.h>
#include "Logger.h"


#ifdef DBPLUGIN_EXPORT
	#define DBPLUNGIN_API __declspec(dllexport)
#else
	#define DBPLUNGIN_API
#endif

class DBPLUNGIN_API DBPlugin
{
private:
	//sql::Connection* m_dbConn;
public:
	void initDBPlugin(std::string dbDir);
};

