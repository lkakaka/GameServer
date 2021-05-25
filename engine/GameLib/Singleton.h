#pragma once

#include "../Common/ServerExports.h"
#include <assert.h>
#include <cstddef>


template<typename T>
class Singleton
{
public:
	static T* _singleon;

public:
	Singleton() {
		assert(_singleon == NULL);
		_singleon = static_cast<T*>(this);
	};
	~Singleton(void) { 
		assert(_singleon);
		if (_singleon != NULL) {
			delete _singleon;
		}
		_singleon = NULL; 
	}
	static T* getSingleton() {
		assert(_singleon != NULL);
		/*if (_singleon == NULL) {
			_singleon = new T();
		}*/
		return _singleon;
	}
};

#define INIT_SINGLETON_CLASS(cls) template<> cls* Singleton<cls>::_singleon = NULL;

