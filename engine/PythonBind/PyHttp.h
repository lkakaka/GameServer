#pragma once
#include "../Common/ServerExports.h"
#include "request.hpp"
#include "server.hpp"

using namespace http::server;

SERVER_EXPORT_API void initHttpModule();
reply_ptr onRecvHttpReq(void* server, int conn_id, const http::server::request& req);


