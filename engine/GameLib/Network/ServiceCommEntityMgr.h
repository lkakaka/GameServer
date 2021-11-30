#pragma once
#include "ServiceCommEntity.h"
#include "boost/asio.hpp"
#include "Network.h"
#include "../Singleton.h"

// ���������ڵ����ķ�ͨѶʵ����
#define SERVER_CENTER_COMM_NAME "SERVER_CENTER_COMM"
// �������������ķ�ͨѶʵ����
#define GROUP_CENTER_COMM_NAME "GROUP_CENTER_COMM"

// ���������ڵ����ķ�ͨѶʵ��
#define SERVER_CENTER_COMM_ENTITY CommEntityMgr::getSingleton()->getCommEntity(SERVER_CENTER_COMM_NAME)
// �������������ķ�ͨѶʵ��
#define GROUP_CENTER_COMM_ENTITY CommEntityMgr::getSingleton()->getCommEntity(GROUP_CENTER_COMM_NAME)

class CommEntityMgr : public Singleton<CommEntityMgr> {
private:
	std::map<std::string, IServiceCommEntity*> m_commEntity;

	IServiceCommEntity* createZmqCommEntity(ServiceAddr& addr, const char* serverIp, int serverPort);
	IServiceCommEntity* createTcpCommEntity(ServiceAddr& addr, const char* serverIp, int serverPort);
public:
	IServiceCommEntity* createCommEntity(const char* commName, ServiceAddr& addr, const char* serverIp, int serverPort);
	inline IServiceCommEntity* getCommEntity(const char* commName) {
		auto iter = m_commEntity.find(commName);
		if (iter == m_commEntity.end()) return NULL;
		return iter->second;
	}
};

