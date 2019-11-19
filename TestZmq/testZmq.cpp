
#include "zmq.h"
#include <string>

int main()
{
	void* zmq_context = zmq_init(1);

	//  与服务端通信的套接字
	void* conn_socket = zmq_socket(zmq_context, ZMQ_REQ);
	zmq_connect(conn_socket, "tcp://localhost:5555");
	
	zmq_msg_t reply;
	zmq_msg_init_size(&reply, 5);
	memcpy(zmq_msg_data(&reply), "Hello", 5);
	zmq_msg_send(&reply, conn_socket, ZMQ_DONTWAIT);
	zmq_msg_close(&reply);

	zmq_msg_t msg;
	zmq_msg_init_size(&msg, 5);
	memcpy(zmq_msg_data(&msg), "World", 5);
	zmq_msg_send(&msg, conn_socket, ZMQ_DONTWAIT);
	zmq_msg_close(&msg);

	while (1) {
		//  等待客户端请求
		zmq_msg_t request;
		zmq_msg_init(&request);
		zmq_msg_recv(&request, conn_socket, 0);
		size_t len = zmq_msg_size(&request);
		char* buff = new char[len + 1];
		memcpy(buff, zmq_msg_data(&request), len);
		buff[len] = '\0';
		printf("收到 %s\n", buff);
		zmq_msg_close(&request);
		break;
	}

	getchar();

	return 0;
}