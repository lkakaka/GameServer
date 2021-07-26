#pragma once

#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "structmember.h"
#include "../Common/ServerExports.h"

class SERVER_EXPORT_API PyTypeBase
{
private:
	PyTypeObject PyObj_Type;
	const char* mod_name;
	const char* obj_name;
	Py_ssize_t tp_basicsize;
	/*newfunc new_func;
	freefunc free_func;*/

	void initPyObj_Type();

public:
	PyTypeBase(const char* mod_name, const char* obj_name, Py_ssize_t tp_size);
	virtual PyMethodDef* getMethod();
	virtual PyMemberDef* getMember();
	virtual newfunc getNewFunc();
	virtual freefunc getFreeFunc();
	virtual reprfunc getReprFunc();

	bool addPyTypeObj(PyObject* module);
};


#define MAKE_PY_OBJ_TYPE(type, mod_name, obj_name, tp_size) new type(mod_name, obj_name, tp_size)

#define DECLARE_CONSTRUTOR(type) type(const char* mod_name, const char* obj_name, Py_ssize_t tp_size)
#define TYPE_CONSTRUTOR(type) type::type(const char* mod_name, const char* obj_name, Py_ssize_t tp_size):PyTypeBase(mod_name, obj_name, tp_size)

#define TYPE_MEMBER_DECLARE PyMemberDef* getMember()
#define TYPE_METHOD_DECLARE PyMethodDef* getMethod()
#define TYPE_NEWFUNC_DECLARE newfunc getNewFunc()
#define TYPE_FREEFUNC_DECLARE freefunc getFreeFunc()
#define TYPE_REPRFUNC_DECLARE reprfunc getReprFunc()

#define TYPE_DECALARE_ALL TYPE_MEMBER_DECLARE; TYPE_METHOD_DECLARE; TYPE_NEWFUNC_DECLARE; TYPE_FREEFUNC_DECLARE;

#define TYPE_MEMBER(type, tp_member) PyMemberDef* type::getMember() { return tp_member;}
#define TYPE_METHOD(type, tp_method) PyMethodDef* type::getMethod() { return tp_method;}
#define TYPE_NEWFUNC(type, new_func) newfunc type::getNewFunc() { return new_func;}
#define TYPE_FREEFUNC(type, free_func) freefunc type::getFreeFunc() { return free_func;}
#define TYPE_REPRFUNC(type, repr_func) reprfunc type::getReprFunc() { return repr_func;}

