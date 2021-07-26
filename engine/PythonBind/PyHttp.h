#pragma once
#include "../Common/ServerExports.h"
#include "request.hpp"
#include "server.hpp"

using namespace http::server;

SERVER_EXPORT_API void initHttpModule();
reply_ptr onRecvHttpReq(void* script_obj, int conn_id, const request& req);


