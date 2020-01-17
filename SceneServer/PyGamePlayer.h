#pragma once

#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "GamePlayer.h"

class PyGamePlayer
{
public:
	PyObject_HEAD
	GamePlayer* gamePlayer;
};

bool addPyGamePlayer(PyObject* module);

