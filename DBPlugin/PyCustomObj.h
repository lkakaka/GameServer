#pragma once
#include "Python.h"

class PyCustomObjBase {
public:
	PyObject_HEAD
};

class PyModuleObj {
public:
	PyTypeObject pyTypeObj;
	const char* obj_name;
	int custom_type_size;
	const char* tp_name;
	Py_ssize_t tp_basicsize, tp_itemsize; /* For allocation */
	reprfunc tp_repr;
	getattrofunc tp_getattro;
	struct PyMethodDef* tp_methods;
	struct PyMemberDef* tp_members;
	newfunc tp_new;
	freefunc tp_free; /* Low-level free-memory routine */

	PyModuleObj();
	bool addToModule(PyObject* mod);

private:
	void initPyObj_Type();
};

//bool addModuleObj(PyObject* mod, const char* obj_name, PyModuleObj* mod_obj);
