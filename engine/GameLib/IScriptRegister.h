#pragma once

typedef void (*CallScripFunc)(void* scripRegister, ...);

class IScriptRegister {
protected:
	// py bind
	void* m_scriptObj;
	// lua bind
	int m_luaObjId;
	int m_luaRef;

	CallScripFunc m_func;

public:
	IScriptRegister() : m_scriptObj(nullptr), m_luaObjId(-1), m_luaRef(-1), m_func(nullptr){ }

	inline void bindPyScriptObject(void* scriptObj, CallScripFunc func) {
		m_scriptObj = scriptObj; 
		m_func = func;
	}
	inline void* getScriptObject() { return m_scriptObj; }

	inline void bindLuaScriptObject(int luaObjId, int luaRef, CallScripFunc func) {
		m_luaObjId = luaObjId;
		m_luaRef = luaRef;
		m_func = func;
	}

	inline int getLuaObjId() { return m_luaObjId; }
	inline int getLuaRef() { return m_luaRef; }

	inline CallScripFunc getCallScriptFunc() { return m_func; }
};
