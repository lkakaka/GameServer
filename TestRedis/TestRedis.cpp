
#include "hiredis.h"
#include <string>
#include <map>
#include <set>

void parseReply(redisReply* reply);

int main()
{
	std::set<int> st;
	st.insert(1);
	st.insert(1);
	st.erase(2);

	std::string ip = "127.0.0.1";
	int port = 6379;
	redisContext* redisContext = redisConnect(ip.c_str(), port);
	if (redisContext->err != 0) {
		printf("cannot connect redis server, ip:%s, port:%d", ip.c_str(), port);
		return 1;
	}

	redisReply* reply;
	/*printf("\n------exec hset------\n");
	redisReply* reply = (redisReply*)redisCommand(redisContext, "HSET test id 1 name test");
	parseReply(reply);
	freeReplyObject(reply);

	printf("\n------exec hget------\n");
	reply = (redisReply*)redisCommand(redisContext, "hget test name");
	parseReply(reply);
	freeReplyObject(reply);*/
	
	printf("\n------exec hgetall------\n");
	reply = (redisReply*)redisCommand(redisContext, "HGETALL %s", "test");
	printf("size:(%d)\n", reply->elements);
	parseReply(reply);
	freeReplyObject(reply);
	/*
	printf("\n------exec hkeys------\n");
	reply = (redisReply*)redisCommand(redisContext, "hkeys test");
	parseReply(reply);
	freeReplyObject(reply);
	
	printf("\n------exec zadd------\n");
	reply = (redisReply*)redisCommand(redisContext, "zadd test_st 0 id");
	parseReply(reply);
	freeReplyObject(reply);
	
	printf("\n ------exec zrange------\n");
	reply = (redisReply*)redisCommand(redisContext, "zrange test_st 0 -1 withscores");
	parseReply(reply);
	freeReplyObject(reply);
*/
	return 0;
}

void parseReply(redisReply* reply) {
	printf("result type:%d\n", reply->type);
	switch (reply->type) {
		case REDIS_REPLY_INTEGER:
		{
			int x = reply->integer;
			printf("result int:%d\n", x);
			break;
		}
		case REDIS_REPLY_DOUBLE:
		{
			double d = reply->dval;
			printf("result double:%lf\n", d);
			break;
		}
		case REDIS_REPLY_STRING:
		case REDIS_REPLY_ERROR:
		case REDIS_REPLY_STATUS:
		{
			std::string str;
			std::copy(reply->str, reply->str + reply->len, std::back_inserter(str));
			printf("result string:%s\n", str.c_str());
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
		default:
			printf("unkown type:%d\n", reply->type);
	}
}
