#pragma once

#include "PyTypeBase.h"

#include "../Common/ServerExports.h"
#include "Redis.h"

USING_DATA_BASE;

class PyRedisObj {
public:
	PyObject_HEAD
	Redis* redis;
};

class PyTypeRedis : public PyTypeBase
{
public:
	DECLARE_CONSTRUTOR(PyTypeRedis);

	TYPE_METHOD_DECLARE;
	TYPE_NEWFUNC_DECLARE;
	TYPE_FREEFUNC_DECLARE;
};


//bool addPyRedisObj(PyObject* module);
