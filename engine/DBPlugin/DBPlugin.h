#pragma once

#include <stdio.h>
#include "Logger.h"
#include "../Common/ServerExports.h"

class SERVER_EXPORT_API DBPlugin
{
private:
	//sql::Connection* m_dbConn;
public:
	void initDBPlugin(std::string dbDir);
};

