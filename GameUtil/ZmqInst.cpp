#include "ZmqInst.h"

//ZmqInst* ZmqInst::zmqInstance = NULL;
//INIT_SINGLETON_CLASS(ZmqInst)

ZmqInst::ZmqInst(std::string& name, std::string& router_addr) : m_name(name), m_router_addr(router_addr),
	zmq_context(NULL), conn_socket(NULL), work_thread(NULL), m_recvCallback(NULL)
{
	//startZmqInst();
}

ZmqInst::~ZmqInst()
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
ZmqInst* ZmqInst::getZmqInstance()
{
	//return ZmqInst::zmqInstance;
	return ZmqInst::getSingleton();
}

void ZmqInst::setRecvCallback(ZmqRecvCallback callback)
{
	m_recvCallback = callback;
}

void ZmqInst::sendData(const char* dstName, void* data, int datLen)
{
	zmq_send(conn_socket, dstName, strlen(dstName), ZMQ_SNDMORE);
	zmq_send(conn_socket, (char*)&datLen, 4, ZMQ_SNDMORE);
	zmq_send(conn_socket, data, datLen, 0);
}

void ZmqInst::run() 
{
	Logger::logInfo("$zmq intance running...");
	//while (1) {
	//	//  等待客户端请求
	//	zmq_msg_t request;
	//	zmq_msg_init(&request);
	//	zmq_msg_recv(&request, bind_socket, 0);
	//	size_t len = zmq_msg_size(&request);
	//	char *buff = new char[len + 1];
	//	memcpy(buff, zmq_msg_data(&request), len);
	//	buff[len] = '\0';
	//	printf("收到 %s\n", buff);
	//	zmq_msg_close(&request);


	//	//  返回应答
	//	zmq_msg_t reply;
	//	zmq_msg_init_size(&reply, 5);
	//	memcpy(zmq_msg_data(&reply), "World", 5);
	//	zmq_msg_send(&reply, bind_socket, 0);
	//	zmq_msg_close(&reply);
	//}
	auto threadFunc = [this]() {
		char src_name[128]{ 0 };
		while (1) {
			//zmq_recv(socket, NULL, 0, 0);
			zmq_recv(this->conn_socket, src_name, sizeof(src_name), 0);
			//zmq_recv(socket, NULL, 0, 0);
			int msg_len = 0;
			zmq_recv(this->conn_socket, &msg_len, sizeof(msg_len), 0);
			if (msg_len <= 0) {
				continue;
			}
			char* msg = new char[msg_len];
			zmq_recv(this->conn_socket, msg, msg_len, 0);
			if (m_recvCallback != NULL) {
				m_recvCallback(src_name, msg, msg_len);
			}
			//printf("recv msg from %s, msg:%s\n", src_name, msg);

			memset(src_name, 0, sizeof(src_name));
			delete[] msg;
			//memset(msg, 0, sizeof(msg));
		}

		Logger::logInfo("$zmq intance exit");
	};
	work_thread = new std::thread(threadFunc);
}

void ZmqInst::startZmqInst()
{
	zmq_context = zmq_init(1);

	////  与客户端通信的套接字
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

	std::string routerAddr = "tcp://" + m_router_addr;
	//routerAddr.append(router_addr);
	if (zmq_connect(conn_socket, routerAddr.c_str()) < 0) {
		zmq_close(conn_socket);
		zmq_ctx_destroy(zmq_context);
		return;
	}

	int major, minor, patch;
	zmq_version(&major, &minor, &patch);
	Logger::logInfo("$Current ZMQ version is %d.%d.%d", major, minor, patch);

	run();

	Logger::logInfo("$create zmq instance, name: %s, router addr:%s", m_name.c_str(), routerAddr.c_str());
}

void ZmqInst::destory() 
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

	//  与客户端通信的套接字
	void* responder = zmq_socket(context, ZMQ_REP);
	zmq_bind(responder, "tcp://*:5555");

	while (1) {
		//  等待客户端请求
		zmq_msg_t request;
		zmq_msg_init(&request);
		zmq_recv(responder, &request, 0, 0);
		printf("收到 Hello\n");
		zmq_msg_close(&request);

		//  做些“处理”
		//sleep(1);

		//  返回应答
		zmq_msg_t reply;
		zmq_msg_init_size(&reply, 5);
		memcpy(zmq_msg_data(&reply), "World", 5);
		zmq_send(responder, &reply, 0, 0);
		zmq_msg_close(&reply);
	}
	//  程序不会运行到这里，以下只是演示我们应该如何结束
	zmq_close(responder);
	zmq_term(context);
}