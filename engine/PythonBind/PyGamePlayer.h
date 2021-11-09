#pragma once

#include "PyTypeBase.h"
#include "PlayerEntity.h"

class PyGamePlayer
{
public:
	PyObject_HEAD
	PlayerEntity* gamePlayer;
};

class PyTypeGamePlayer : public PyTypeBase
{
public:
	DECLARE_CONSTRUTOR(PyTypeGamePlayer);
	TYPE_METHOD_DECLARE;
	TYPE_NEWFUNC_DECLARE;
	TYPE_FREEFUNC_DECLARE;

};

