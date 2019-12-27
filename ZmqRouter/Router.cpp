#include "zmq.h"
#include "Logger.h"
#include "Config.h"


int main(int argc, char** argv)
{
	Logger::initLog();
	int major, minor, patch;
	zmq_version(&major, &minor, &patch);
	Logger::logInfo("$Current ZMQ version is %d.%d.%d\n", major, minor, patch);
	Logger::logInfo("$===========================================\n\n");

	if (argc < 2) {
		Logger::logError("$arg count error");
		return 0;
	}
	char* cfgName = argv[1];
	if (!Config::checkFileExist(cfgName)) {
		Logger::logError("$cfg file not exist, file name: %s", cfgName);
		return 0;
	}

	std::string serviceName = Config::getConfigStr(cfgName, "service_name");
	if (serviceName == "") {
		Logger::logError("$not config zmq addr, cfg name: %s", cfgName);
		return 0;
	}
	int port = Config::getConfigInt(cfgName, "port");
	if (port <= 0) {
		Logger::logError("$not config zmq router port, cfg name: %s", cfgName);
		return 0;
	}
	
	void* zmq_context = zmq_init(1);
	void* router_socket = zmq_socket(zmq_context, ZMQ_ROUTER);

	if (zmq_setsockopt(router_socket, ZMQ_IDENTITY, serviceName.c_str(), serviceName.size()) < 0)
	{
		zmq_close(router_socket);
		zmq_ctx_destroy(zmq_context);
		return 0;
	}

	char addr[128]{0};
	sprintf(addr, "tcp://*:%d", port);
	if (zmq_bind(router_socket, addr) < 0) {
		zmq_close(router_socket);
		zmq_ctx_destroy(zmq_context);
		return 0;
	}

	Logger::logInfo("$router name:%s, addr: %s", serviceName.c_str(), addr);

	char src_name[128]{ 0 };
	char dst_name[128]{ 0 };

	while (1) {
		zmq_recv(router_socket, src_name, sizeof(src_name), 0);
		//zmq_recv(router_socket, NULL, 0, 0);
		zmq_recv(router_socket, dst_name, sizeof(dst_name), 0);
		int msgLen = 0;
		zmq_recv(router_socket, (void*)&msgLen, 4, 0);
		//zmq_recv(router_socket, NULL, 0, 0);
		char* msg = new char[msgLen];
		zmq_recv(router_socket, msg, msgLen, 0);

		Logger::logInfo("$recv msg, src_name:%s, dst_name:%s, msg:%s", src_name, dst_name, msg);
		
		zmq_send(router_socket, dst_name, strlen(dst_name), ZMQ_SNDMORE);
		//zmq_send(router_socket, "", 0, ZMQ_SNDMORE);
		zmq_send(router_socket, src_name, strlen(src_name), ZMQ_SNDMORE);
		zmq_send(router_socket, (void*)&msgLen, 4, ZMQ_SNDMORE);
		//zmq_send(router_socket, "", 0, ZMQ_SNDMORE);
		zmq_send(router_socket, msg, msgLen, 0);

		memset(src_name, 0, sizeof(src_name));
		memset(dst_name, 0, sizeof(dst_name));
		delete[] msg;
	}

	return 1;
}