#pragma once
#define PY_SSIZE_T_CLEAN
#include "Python.h"

#include "reply.hpp"

http::server::reply_ptr getHttpReply(PyObject* resp);