#pragma once
#include "../Common/ServerExports.h"
#include <stdint.h>

class SERVER_EXPORT_API TimeUtil
{
public:
	static int64_t nowSec();
	static int64_t nowMillSec();
};

