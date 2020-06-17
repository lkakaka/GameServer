#pragma once
#ifdef WIN32
#include "win/hiredis.h"
#else
#include "linux/hiredis.h"
#endif