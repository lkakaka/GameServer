#pragma once
#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include <memory>
#include <thread>
#include "server.hpp"


class PyHttpServer
{
public:
	PyObject_HEAD
	http::server::server* http_server;
	std::shared_ptr<std::thread> http_thread;
};

bool addPyHttpServer(PyObject* module);

