#pragma once
#include <vector>
#include <set>
#include <functional>
#include <memory>
#include "../Common/ServerExports.h"
#include "../Redis.h"
#include "../DataBase.h"

DATA_BASE_BEGIN


class SERVER_EXPORT_API MongoDBHandler
{
private:
	std::string m_dbUrl;
	std::string m_dbName;

public:
	MongoDBHandler(std::string& dbUrl, std::string& dbName);
	~MongoDBHandler();
};


DATA_BASE_END
