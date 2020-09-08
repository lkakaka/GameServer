#pragma once

#include "../Common/ServerExports.h"
#include <assert.h>


template<typename T>
class LazySingleton
{
public:
	static T* _lazy_singleon;

public:
	~LazySingleton(void) {
		assert(_lazy_singleon);
		if (_lazy_singleon != NULL) {
			delete _lazy_singleon;
		}
		_lazy_singleon = NULL;
	}
	static T* getSingleton() {
		if (_lazy_singleon == NULL) {
			_lazy_singleon = new T();
		}
		return _lazy_singleon;
	}
};

#define INIT_LAZY_SINGLETON_CLASS(cls) cls* LazySingleton<cls>::_singleon = NULL;

