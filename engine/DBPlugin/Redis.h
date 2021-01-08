#pragma once
#include "../Common/ServerExports.h"
#include <string>
#include "hiredis.h"
#include "DataBase.h"
#include <memory>

DATA_BASE_BEGIN;

#define REDIS_REPLY_PTR std::shared_ptr<RedisReply>

class RedisReply {
private:
	redisReply* m_reply;
public:
	RedisReply(redisReply* reply) : m_reply(reply) {
		//printf("redisReply constructor------\n");
	}
	~RedisReply() {
		//printf("redisReply free------\n");
		freeReplyObject(m_reply);
	}
	inline redisReply* getReply() { return m_reply; }

};

class Redis
{
private:
	std::string ip;
	int port;
	redisContext* m_redisContext;
public:

	Redis(std::string ip, int port);
	void parseReply(redisReply* reply);
	std::shared_ptr<RedisReply> execRedisCmd(const char* format, ...);
};

DATA_BASE_END

