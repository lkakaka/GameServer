
#include "CmdLine.h"
#include <thread>
#include <iostream>
#include "../Common/PyCommon.h"

static std::thread cmd_thread;

void waitInput() {
	std::string cmd;
	while (true) {
		std::cin >> cmd;
		PyGILState_STATE py_state = PyGILState_Ensure();
		PyObject* cmdObj = PyUnicode_FromString(cmd.c_str());
		if (cmdObj != NULL) {
			PyObject* args = PyTuple_New(1);
			PyTuple_SetItem(args, 0, cmdObj);
			callPyFunction("cmd_line.CmdLine", "on_cmd", args);
		}
		PyGILState_Release(py_state);
	}
}

void startCmd() {
	cmd_thread = std::thread(waitInput);
}