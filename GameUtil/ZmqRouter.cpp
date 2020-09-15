#include "ZmqRouter.h"
#include "MyBuffer.h"
#include "Const.h"

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
		char src_name[MAX_PEER_NAME_LEN]{ 0 };
		char dst_name[MAX_PEER_NAME_LEN]{ 0 };
		char msg[MAX_MSG_LEN + 1]{ 0 };

		while (1) {
			int len = zmq_recv(m_socket, src_name, sizeof(src_name), 0);
			if (len <= 0 || len >= MAX_PEER_NAME_LEN) {
				Logger::logError("$recv src name len(%d) error!", len);
				continue;
			}
			src_name[len] = '\0';
			len = zmq_recv(m_socket, msg, MAX_MSG_LEN, 0);
			if (len <= 0 || len > MAX_MSG_LEN) {
				Logger::logError("$recv msg len(%d) error!", len);
				continue;
			}
			int dstNameLen = strlen(msg);
			if (dstNameLen == 0 || dstNameLen >= MAX_PEER_NAME_LEN) {
				Logger::logError("$dstNameLen (%d) error!", dstNameLen);
				continue;
			}
			if (dstNameLen + 1 >= len) {
				Logger::logError("$msg len error, dstNameLen:%d, recv_len:%d", dstNameLen, len);
				continue;
			}
			memcpy(dst_name, msg, dstNameLen);
			dst_name[dstNameLen] = '\0';
			int iMsgBodyIdx = dstNameLen + 1;
			int iMsgBodyLen = len - iMsgBodyIdx;
			Logger::logInfo("$recv msg, src_name:%s, dst_name:%s, msg_len:%d", src_name, dst_name, iMsgBodyLen);

			zmq_send(m_socket, dst_name, strlen(dst_name), ZMQ_SNDMORE);
			int srcNameLen = strlen(src_name);

			MyBuffer buffer;
			buffer.writeString(src_name, strlen(src_name));
			buffer.writeByte('\0');
			buffer.writeString(&msg[iMsgBodyIdx], iMsgBodyLen);
			zmq_send(m_socket, buffer.data(), buffer.size(), 0);
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
