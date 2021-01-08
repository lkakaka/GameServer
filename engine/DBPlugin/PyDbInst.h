#pragma once

#include "PyTypeBase.h"

#include "../Common/ServerExports.h"
#include "PyCustomObj.h"
#include "DBHandler.h"

USING_DATA_BASE;

class PyDbObject : public PyCustomObjBase {
public:
	//PyObject_HEAD
	DBHandler* db_inst;
	PyObject* name;
};

class PyTypeDbInst : public PyTypeBase
{
public:
	DECLARE_CONSTRUTOR(PyTypeDbInst);
	TYPE_DECALARE_ALL
	TYPE_REPRFUNC_DECLARE;
};


//bool addPyRedisObj(PyObject* module);
