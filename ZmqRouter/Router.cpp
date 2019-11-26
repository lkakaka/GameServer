#include "zmq.h"
#include "Logger.h"


int main()
{
	Logger::initLog();
	int major, minor, patch;
	zmq_version(&major, &minor, &patch);
	Logger::logInfo("$Current 0MQ version is %d.%d.%d\n", major, minor, patch);
	Logger::logInfo("$===========================================\n\n");
	
	void* zmq_context = zmq_init(1);
	void* router_socket = zmq_socket(zmq_context, ZMQ_ROUTER);

	char* pName = "router";
	if (zmq_setsockopt(router_socket, ZMQ_IDENTITY, pName, strlen(pName)) < 0)
	{
		zmq_close(router_socket);
		zmq_ctx_destroy(zmq_context);
		return 0;
	}

	if (zmq_bind(router_socket, "tcp://*:5555") < 0) {
		zmq_close(router_socket);
		zmq_ctx_destroy(zmq_context);
		return 0;
	}

	char src_name[128]{ 0 };
	char dst_name[128]{ 0 };
	char msg[1024]{ 0 };

	while (1) {
		zmq_recv(router_socket, src_name, sizeof(src_name), 0);
		//zmq_recv(router_socket, NULL, 0, 0);
		zmq_recv(router_socket, dst_name, sizeof(dst_name), 0);
		//zmq_recv(router_socket, NULL, 0, 0);
		zmq_recv(router_socket, msg, sizeof(msg), 0);

		printf("recv msg, src_name:%s, dst_name:%s, msg:%s\n", src_name, dst_name, msg);
		
		zmq_send(router_socket, dst_name, strlen(dst_name), ZMQ_SNDMORE);
		//zmq_send(router_socket, "", 0, ZMQ_SNDMORE);
		zmq_send(router_socket, src_name, strlen(src_name), ZMQ_SNDMORE);
		//zmq_send(router_socket, "", 0, ZMQ_SNDMORE);
		zmq_send(router_socket, msg, strlen(msg), 0);

		memset(src_name, 0, sizeof(src_name));
		memset(dst_name, 0, sizeof(dst_name));
		memset(msg, 0, sizeof(msg));
	}

	return 1;
}