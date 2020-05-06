
// Generated by the make proto tool

#pragma once

#include "common.pb.h"
#include "login.pb.h"
#include "test.pb.h"

#ifdef PROTO_BUFFER_EXPORT
#ifndef PROTO_BUFFER_API
#define PROTO_BUFFER_API __declspec(dllexport)
#endif
#else
#define PROTO_BUFFER_API
#endif

enum PROTO_MSG_ID {
	MSG_ID_DISCONNECT=1,
	MSG_ID_LOGIN_REQ=2,
	MSG_ID_LOGIN_RSP=3,
	MSG_ID_RPC_MSG=4,
	MSG_ID_RPC_MSG_RSP=5,
	MSG_ID_TEST_REQ=6,
	
};

typedef PROTO_MSG_ID MSG_ID;

PROTO_BUFFER_API std::shared_ptr<google::protobuf::Message> CreateMsgById(int msgId);