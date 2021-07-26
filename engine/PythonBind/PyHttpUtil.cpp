#include "PyHttpUtil.h"

http::server::reply_ptr getHttpReply(PyObject* resp) {
	int status = PyLong_AsLong(PyObject_GetAttrString(resp, "status"));
	const char* body = PyUnicode_AsUTF8(PyObject_GetAttrString(resp, "body"));
	PyObject* headers = PyObject_GetAttrString(resp, "headers");
	http::server::reply_ptr rep = std::make_shared<http::server::reply>();
	rep->status = (http::server::reply::status_type)status;
	rep->content = body;

	Py_ssize_t pos = 0;
	PyObject* key, * val;
	while (PyDict_Next(headers, &pos, &key, &val)) {
		http::server::header h;
		h.name = PyUnicode_AsUTF8(key);
		h.value = PyUnicode_AsUTF8(val);
		rep->headers.emplace_back(h);
	}
	return rep;
}