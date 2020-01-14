#include "PyCustomObj.h"

//bool addModuleObj(PyObject* mod, const char* obj_name, PyModuleObj* mod_obj) {
//	PyTypeObject* pyTypeObj = mod_obj->pyTypeObj;
//	if (PyType_Ready(pyTypeObj) < 0) {
//		assert(false, "assert");
//		return false;
//	}
//
//	Py_INCREF(pyTypeObj);
//	//PyObject* obj = PyObject_New(PyObject, &PyDb_Type);;
//	if (PyModule_AddObject(mod, obj_name, (PyObject*)pyTypeObj) < 0) {
//		Py_DECREF(pyTypeObj);
//		return false;
//	}
//
//	return true;
//}

PyModuleObj::PyModuleObj() : obj_name(""), tp_name(""), custom_type_size(0), tp_basicsize(0), tp_itemsize(0),
tp_members(NULL), tp_methods(NULL), tp_free(PyObject_Del), tp_new(PyType_GenericNew), tp_repr(NULL), tp_getattro(PyObject_GenericGetAttr)
{
}

void PyModuleObj::initPyObj_Type()
{
	memset(&pyTypeObj, 0, sizeof(pyTypeObj));
	pyTypeObj.ob_base = { PyObject_HEAD_INIT(NULL) 0 };
	pyTypeObj.tp_name = tp_name;
	pyTypeObj.tp_basicsize = custom_type_size;
	pyTypeObj.tp_repr = tp_repr;
	pyTypeObj.tp_getattro = tp_getattro;
	pyTypeObj.tp_flags = Py_TPFLAGS_DEFAULT;
	pyTypeObj.tp_methods = tp_methods;
	pyTypeObj.tp_members = tp_members;
	pyTypeObj.tp_new = tp_new;
	pyTypeObj.tp_free = tp_free;
}

bool PyModuleObj::addToModule(PyObject* mod)
{
	initPyObj_Type();
	if (PyType_Ready(&pyTypeObj) < 0) {
		assert(false, "assert");
		return false;
	}

	Py_INCREF(&pyTypeObj);
	//PyObject* obj = PyObject_New(PyObject, &PyDb_Type);;
	if (PyModule_AddObject(mod, obj_name, (PyObject*)&pyTypeObj) < 0) {
		Py_DECREF(&pyTypeObj);
		return false;
	}
	return true;
}