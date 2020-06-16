
#include "hiredis.h"
#include <string>

void parseReply(redisReply* reply);

int main()
{
	std::string ip = "127.0.0.1";
	int port = 6379;
	redisContext* redisContext = redisConnect(ip.c_str(), port);
	if (redisContext->err != 0) {
		printf("cannot connect redis server, ip:%s, port:%d", ip.c_str(), port);
		return 1;
	}

	redisReply* reply = (redisReply*)redisCommand(redisContext, "hgetall test:3");
	parseReply(reply);
	
	return 0;
}

void parseReply(redisReply* reply) {
	printf("result type:%d\n", reply->type);
	switch (reply->type) {
		case REDIS_REPLY_INTEGER:
		{
			int x = reply->integer;
			break;
		}
		case REDIS_REPLY_DOUBLE:
		{
			double d = reply->dval;
			break;
		}
		case REDIS_REPLY_STRING:
		case REDIS_REPLY_ERROR:
		case REDIS_REPLY_STATUS:
		{
			std::string str;
			std::copy(reply->str, reply->str + reply->len, std::back_inserter(str));
			break;
		}
		case REDIS_REPLY_ARRAY:
		case REDIS_REPLY_MAP:
		case REDIS_REPLY_SET:
		{
			printf("result array size:%d\n", reply->elements);
			for (int i = 0; i < reply->elements; i++) {
				redisReply* subReply = reply->element[i];
				parseReply(subReply);
			}
			break;
		}
	}
}