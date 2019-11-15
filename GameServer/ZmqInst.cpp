#include "ZmqInst.h"
#include <functional>


ZmqInst::ZmqInst() : zmq_context(NULL), bind_socket(NULL), work_thread(NULL)
{
}

ZmqInst::~ZmqInst()
{
	destory();
}

void ZmqInst::run() 
{
	Logger::logInfo("$zmq intance running...");
	while (1) {
		//  �ȴ��ͻ�������
		zmq_msg_t request;
		zmq_msg_init(&request);
		zmq_recv(bind_socket, &request, 0, 0);
		printf("�յ� Hello\n");
		zmq_msg_close(&request);

		//  ��Щ������
		//sleep(1);

		//  ����Ӧ��
		zmq_msg_t reply;
		zmq_msg_init_size(&reply, 5);
		memcpy(zmq_msg_data(&reply), "World", 5);
		zmq_send(bind_socket, &reply, 0, 0);
		zmq_msg_close(&reply);
	}
	Logger::logInfo("$zmq intance exit");
}

void ZmqInst::startZmqInst()
{
	zmq_context = zmq_init(1);

	//  ��ͻ���ͨ�ŵ��׽���
	bind_socket = zmq_socket(zmq_context, ZMQ_REP);
	zmq_bind(bind_socket, "tcp://*:5555");

	work_thread = new std::thread(std::bind(&ZmqInst::run, this));
	/*std::thread t1(std::bind(&ZmqInst::run, this));
	t1.join();*/
	Logger::logInfo("$create zmq instance");
}

void ZmqInst::destory() 
{
	if (zmq_context != NULL) {
		zmq_close(bind_socket);
		zmq_term(zmq_context);
		zmq_context = NULL;
		bind_socket = NULL;
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

		//  ��Щ������
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