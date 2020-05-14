#include "ZmqRouter.h"

#define MAX_MSG_LEN (64 * 1024)

ZmqRouter* ZmqRouter::zmqRouter = NULL;

ZmqRouter::ZmqRouter() : zmq_context(NULL), m_socket(NULL), work_thread(NULL)
{

}

ZmqRouter::~ZmqRouter()
{
	destory();
}

void ZmqRouter::initZmqRouter(const char* name, int port)
{
	if (zmqRouter != NULL) {
		return;
	}
	ZmqRouter::zmqRouter = new ZmqRouter();
	ZmqRouter::zmqRouter->startZmqRouter(name, port);
}

ZmqRouter* ZmqRouter::getZmqRouter()
{
	return ZmqRouter::zmqRouter;
}


void ZmqRouter::run()
{
	Logger::logInfo("$zmq router running...");
	auto threadFunc = [this]() {
		char src_name[128]{ 0 };
		char dst_name[128]{ 0 };
		char msg[MAX_MSG_LEN + 1]{ 0 };

		while (1) {
			zmq_recv(m_socket, src_name, sizeof(src_name), 0);
			//zmq_recv(router_socket, NULL, 0, 0);
			zmq_recv(m_socket, dst_name, sizeof(dst_name), 0);
			int msgLen = 0;
			zmq_recv(m_socket, (void*)&msgLen, 4, 0);
			if (msgLen <= 0 || msgLen > MAX_MSG_LEN) {
				Logger::logError("$error msg len(%d), src_name:%s, dst_name:%s", msgLen, src_name, dst_name);
				continue;
			}
			//zmq_recv(router_socket, NULL, 0, 0);
			zmq_recv(m_socket, msg, msgLen, 0);
			msg[msgLen] = '\0';

			Logger::logInfo("$recv msg, src_name:%s, dst_name:%s, msg:%s", src_name, dst_name, msg);

			zmq_send(m_socket, dst_name, strlen(dst_name), ZMQ_SNDMORE);
			//zmq_send(router_socket, "", 0, ZMQ_SNDMORE);
			zmq_send(m_socket, src_name, strlen(src_name), ZMQ_SNDMORE);
			zmq_send(m_socket, (void*)&msgLen, 4, ZMQ_SNDMORE);
			//zmq_send(router_socket, "", 0, ZMQ_SNDMORE);
			zmq_send(m_socket, msg, msgLen, 0);

			memset(src_name, 0, sizeof(src_name));
			memset(dst_name, 0, sizeof(dst_name));
		}

		Logger::logInfo("$zmq router exit");
	};

	work_thread = new std::thread(threadFunc);
}

void ZmqRouter::startZmqRouter(const char* name, int port)
{
	zmq_context = zmq_init(1);
	m_socket = zmq_socket(zmq_context, ZMQ_ROUTER);

	if (zmq_setsockopt(m_socket, ZMQ_IDENTITY, name, strlen(name)) < 0)
	{
		zmq_close(m_socket);
		zmq_ctx_destroy(zmq_context);
		Logger::logError("$zmq router set identity error, name: %s", name);
		return;
	}

	char addr[128]{ 0 };
	sprintf(addr, "tcp://*:%d", port);
	if (zmq_bind(m_socket, addr) < 0) {
		zmq_close(m_socket);
		zmq_ctx_destroy(zmq_context);
		Logger::logError("$zmq router bind error, port: %d", port);
		return ;
	}

	int major, minor, patch;
	zmq_version(&major, &minor, &patch);
	Logger::logInfo("$Current ZMQ version is %d.%d.%d", major, minor, patch);

	run();

	Logger::logInfo("$create zmq router, name: %s, router addr:%s", name, addr);
}

void ZmqRouter::destory()
{
	if (zmq_context != NULL) {
		zmq_close(m_socket);
		zmq_term(zmq_context);
		zmq_context = NULL;
		m_socket = NULL;
	}
	if (work_thread != NULL) {
		delete work_thread;
		work_thread = NULL;
	}
}
