#include "ScriptBind.h"
#include "PythonPlugin.h"
#include "LuaPlugin.h"
#include "Logger.h"

IScript* ScriptBind::bindScript(const char* funcName) {
	IScript* script = NULL;
	if (funcName == NULL || strlen(funcName) == 0) return script;
#ifdef USE_PYTHON_SCRIPT
	script = new PythonPlugin(funcName);
#else
	script = new LuaPlugin(funcName);
#endif // USE_PYTHON_SCRIPT
	return script;
}

