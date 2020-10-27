#include "PyTypeBase.h"
#include "Logger.h"

PyTypeBase::PyTypeBase(const char* mod_name, const char* obj_name, Py_ssize_t tp_size) :
	mod_name(mod_name), obj_name(obj_name), tp_basicsize(tp_size)
{

}

void PyTypeBase::initPyObj_Type()
{
	memset(&PyObj_Type, 0, sizeof(PyObj_Type));
	PyObj_Type.ob_base = { PyObject_HEAD_INIT(NULL) 0 };
	char tp_name[128]{ 0 };
	sprintf(tp_name, "%s.%s", mod_name, obj_name);
	PyObj_Type.tp_name = tp_name;
	PyObj_Type.tp_basicsize = tp_basicsize;
	PyObj_Type.tp_getattro = PyObject_GenericGetAttr;
	PyObj_Type.tp_flags = Py_TPFLAGS_DEFAULT;
	PyObj_Type.tp_methods = getMethod();
	PyObj_Type.tp_members = getMember();
	PyObj_Type.tp_new = getNewFunc();
	PyObj_Type.tp_free = getFreeFunc();
	PyObj_Type.tp_repr = getReprFunc();
}

static PyMethodDef tp_methods[] = {
	{NULL, NULL}           /* sentinel */
};

static PyMemberDef tp_members[] = {
	{NULL}
};

PyMethodDef* PyTypeBase::getMethod() {
	return tp_methods;
}

PyMemberDef* PyTypeBase::getMember() {
	return tp_members;
}

newfunc PyTypeBase::getNewFunc() {
	return PyType_GenericNew;
}

freefunc PyTypeBase::getFreeFunc() {
	return PyObject_Del;
}

reprfunc PyTypeBase::getReprFunc() {
	return PyObject_Repr;
}

bool PyTypeBase::addPyTypeObj(PyObject* module) {
	initPyObj_Type();
	if (PyType_Ready(&PyObj_Type) < 0) {
		Logger::logError("$add py type obj %s error, ready type failed", obj_name);
		return false;
	}

	Py_INCREF(&PyObj_Type);
	if (PyModule_AddObject(module, obj_name, (PyObject*)&PyObj_Type) < 0) {
		Py_DECREF(&PyObj_Type);
		Logger::logError("$add py type obj %s error, add failed", obj_name);
		return false;
	}
	return true;
}
