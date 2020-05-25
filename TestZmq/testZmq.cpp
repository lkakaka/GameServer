
#include "zmq.h"
#include <string>
#include <string.h>
#include <set>

#include "Device.h"
#include <thread>
#include <iostream>

void testReq() 
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
}

void test(char* pName, char* dstName)
{
	void* zmq_context = zmq_init(1);
	void* socket = zmq_socket(zmq_context, ZMQ_DEALER);

	//char* pName = "client1";
	if (zmq_setsockopt(socket, ZMQ_IDENTITY, pName, strlen(pName)) < 0)
	{
		zmq_close(socket);
		zmq_ctx_destroy(zmq_context);
		return;
	}

	if (zmq_connect(socket, "tcp://localhost:5555") < 0) {
		zmq_close(socket);
		zmq_ctx_destroy(zmq_context);
		return;
	}

	std::thread t([socket]() {
		char src_name[128]{ 0 };
		char msg[1024]{ 0 };
		while (1) {
			//zmq_recv(socket, NULL, 0, 0);
			zmq_recv(socket, src_name, sizeof(src_name), 0);
			//zmq_recv(socket, NULL, 0, 0);
			zmq_recv(socket, msg, sizeof(msg), 0);

			printf("recv msg from %s, msg:%s\n", src_name, msg);

			memset(src_name, 0, sizeof(src_name));
			memset(msg, 0, sizeof(msg));
		}
	});

	std::thread t1([socket, dstName]() {
		char msg[1024]{ 0 };
		while (1) {
			std::string x;
			std::cout << "input:\n";
			std::cin >> x;
			//zmq_send(socket, "", 0, ZMQ_SNDMORE);
			zmq_send(socket, dstName, strlen(dstName), ZMQ_SNDMORE);
			//zmq_send(socket, "", 0, ZMQ_SNDMORE);
			zmq_send(socket, x.c_str(), x.length(), 0);
		}
	});

	printf("start client %s, %s", pName, dstName);

	t.join();
	t1.join();
}

int main(int argc, char**args)
{
	//startDevice("*:5555", "*:5556");
	char buf[10]{0};
	int n = snprintf(buf, 10, "'%%Index_%%'");
	n = snprintf(buf, 10, "s%d", 1);

	std::set<std::string> st;
	st.emplace("1");
	st.emplace("2");
	n = st.erase("0");
	
	std::string columns = "age,sex";
	int npos;
	while ((npos = columns.find_first_of(',', 0)) >= 0) {
		std::string x = columns.substr(0, npos);
		columns = columns.substr(npos + 1, columns.size() - npos);
	}
	//test(args[1], args[2]);
	getchar();

	return 0;
}
