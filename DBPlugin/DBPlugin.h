#pragma once

#include <stdio.h>
#include "Logger.h"
#include "DBExport.h"

class DBPLUNGIN_API DBPlugin
{
private:
	//sql::Connection* m_dbConn;
public:
	void initDBPlugin(std::string dbDir);
};

