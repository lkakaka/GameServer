#pragma once

#define NS_GAME_NET_BEGIN namespace GameNet{
#define NS_GAME_NET_END }

#define USE_NS_GAME_NET using namespace GameNet;

#define SERVER_RECV_BUFF_WARN_SIZE (1024 * 1024 * 8)  // ��������ӻ������ݱ�����С 1MB
#define SERVER_RECV_MAX_BUFF_SIZE (10 * 1024 * 1024 * 8)  // ��������ӻ����������ֵ 10MB