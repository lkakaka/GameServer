#include "ZmqCommEntity.h"
#include "../MyBuffer.h"
#include "Logger.h"
#include "../Const.h"

//ZmqInst* ZmqInst::zmqInstance = NULL;
//#ifndef WIN32
INIT_SINGLETON_CLASS(ZmqCommEntity)
//#endif

//ZmqInst::ZmqInst(std::string& name, std::string& router_addr) : m_name(name), m_router_addr(router_addr),
//	zmq_context(NULL), conn_socket(NULL), work_thread(NULL), m_recvCallback(NULL)
//{
//	//startZmqInst();
//}

ZmqCommEntity::ZmqCommEntity(ServiceAddr& addr, const char* serverIp, int serverPort) : IServiceCommEntity(addr),
	m_serverIp(serverIp), m_serverPort(serverPort),
	zmq_context(NULL), conn_socket(NULL), work_thread(NULL)
{
	//startZmqInst();
	m_name = *addr.getName();
}

ZmqCommEntity::~ZmqCommEntity()
{
	destory();
}
//
//void ZmqInst::initZmqInstance(const char* name, const char* router_addr) 
//{
//	if (zmqInstance != NULL) {
//		return;
//	}
//	ZmqInst::zmqInstance = new ZmqInst();
//	ZmqInst::zmqInstance->startZmqInst(name, router_addr);
//}
//

//void ZmqInst::sendData(const char* dstName, char* data, int datLen)
//{
//	MyBuffer buffer;
//	buffer.writeString(dstName, strlen(dstName));
//	buffer.writeByte('\0');
//	buffer.writeString(data, datLen);
//	zmq_send(conn_socket, buffer.data(), buffer.size(), 0);
//}

void ZmqCommEntity::sendToService(ServiceAddr* dstAddr, char* msg, int msgLen) {
	MyBuffer buffer;
	//dstAddr->serialize(&buffer);
	std::string* addrName = dstAddr->getName();
	buffer.writeString(addrName->c_str(), addrName->size());
	buffer.writeByte('\0');
	buffer.writeString(msg, msgLen);
	zmq_send(conn_socket, buffer.data(), buffer.size(), 0);
}

void ZmqCommEntity::run()
{
	Logger::logInfo("$zmq intance running...");
	//while (1) {
	//	//  �ȴ��ͻ�������
	//	zmq_msg_t request;
	//	zmq_msg_init(&request);
	//	zmq_msg_recv(&request, bind_socket, 0);
	//	size_t len = zmq_msg_size(&request);
	//	char *buff = new char[len + 1];
	//	memcpy(buff, zmq_msg_data(&request), len);
	//	buff[len] = '\0';
	//	printf("�յ� %s\n", buff);
	//	zmq_msg_close(&request);


	//	//  ����Ӧ��
	//	zmq_msg_t reply;
	//	zmq_msg_init_size(&reply, 5);
	//	memcpy(zmq_msg_data(&reply), "World", 5);
	//	zmq_msg_send(&reply, bind_socket, 0);
	//	zmq_msg_close(&reply);
	//}
	auto threadFunc = [this]() {
		char srcAddr[128]{ 0 };
		char msg[MAX_MSG_LEN + 1]{ 0 };
		while (1) {
			int len = zmq_recv(this->conn_socket, msg, MAX_MSG_LEN, 0);
			if (len <= 0 || len > MAX_MSG_LEN) {
				Logger::logError("$recv msg len(%d) error", len);
				continue;
			}
			int srcAddrLen = strlen(msg);
			if (srcAddrLen == 0 || srcAddrLen + 1 >= len) {
				Logger::logError("$recv msg len error, srcAddrLen:%d, len:%d", srcAddrLen, len);
				continue;
			}
			memcpy(srcAddr, &msg, srcAddrLen);
			srcAddr[srcAddrLen] = '\0';
			int iMsgBodyIdx = srcAddrLen + 1;
			int iMsgBodyLen = len - iMsgBodyIdx;
			ServiceAddr srcServiceAddr;
			srcServiceAddr.parseAddr(srcAddr);
			if (m_recvCallback != NULL) {
				m_recvCallback(&srcServiceAddr, &msg[iMsgBodyIdx], iMsgBodyLen);
			}
		}

		Logger::logInfo("$zmq intance exit");
	};
	work_thread = new std::thread(threadFunc);
}

void ZmqCommEntity::start()
{
	zmq_context = zmq_init(1);

	////  ��ͻ���ͨ�ŵ��׽���
	//conn_socket = zmq_socket(zmq_context, ZMQ_REP);
	//zmq_bind(bind_socket, "tcp://*:5555");

	//work_thread = new std::thread(std::bind(&ZmqInst::run, this));
	///*std::thread t1(std::bind(&ZmqInst::run, this));
	//t1.join();*/

	conn_socket = zmq_socket(zmq_context, ZMQ_DEALER);

	//char* pName = "client1";
	if (zmq_setsockopt(conn_socket, ZMQ_IDENTITY, m_name.c_str(), m_name.size()) < 0)
	{
		zmq_close(conn_socket);
		zmq_ctx_destroy(zmq_context);
		return;
	}

	char routerAddr[64]{ 0 };
	sprintf(routerAddr, "tcp://%s:%d", m_serverIp.c_str(), m_serverPort);
	//std::string routerAddr = "tcp://" + m_router_addr;
	//routerAddr.append(router_addr);
	if (zmq_connect(conn_socket, routerAddr) < 0) {
		zmq_close(conn_socket);
		zmq_ctx_destroy(zmq_context);
		return;
	}

	int major, minor, patch;
	zmq_version(&major, &minor, &patch);
	Logger::logInfo("$Current ZMQ version is %d.%d.%d", major, minor, patch);

	run();

	Logger::logInfo("$create zmq instance, name: %s, router addr:%s:%d", m_name.c_str(), m_serverIp.c_str(), m_serverPort);
}

void ZmqCommEntity::destory()
{
	if (zmq_context != NULL) {
		zmq_close(conn_socket);
		zmq_term(zmq_context);
		zmq_context = NULL;
		conn_socket = NULL;
	}
	if (work_thread != NULL) {
		delete work_thread;
		work_thread = NULL;
	}
}

void zmqTest() {
	Logger::logInfo("$zmq test start");
	void* context = zmq_init(1);

	//  ��ͻ���ͨ�ŵ��׽���
	void* responder = zmq_socket(context, ZMQ_REP);
	zmq_bind(responder, "tcp://*:5555");

	while (1) {
		//  �ȴ��ͻ�������
		zmq_msg_t request;
		zmq_msg_init(&request);
		zmq_recv(responder, &request, 0, 0);
		printf("�յ� Hello\n");
		zmq_msg_close(&request);

		//  ��Щ��������
		//sleep(1);

		//  ����Ӧ��
		zmq_msg_t reply;
		zmq_msg_init_size(&reply, 5);
		memcpy(zmq_msg_data(&reply), "World", 5);
		zmq_send(responder, &reply, 0, 0);
		zmq_msg_close(&reply);
	}
	//  ���򲻻����е��������ֻ����ʾ����Ӧ����ν���
	zmq_close(responder);
	zmq_term(context);
}