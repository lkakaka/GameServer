#pragma once

#define NS_GAME_NET_BEGIN namespace GameNet{
#define NS_GAME_NET_END }

#define USE_NS_GAME_NET using namespace GameNet;

#define SERVER_RECV_BUFF_WARN_SIZE (1024 * 1024 * 8)  // 服务端连接缓存数据报警大小 1MB
#define SERVER_RECV_MAX_BUFF_SIZE (10 * 1024 * 1024 * 8)  // 服务端连接缓存数据最大值 10MB