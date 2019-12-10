
#include "login.pb.h"
#include "test.pb.h"

enum PROTO_MSG_ID {
	MSG_ID_LOGIN=1,
	MSG_ID_LOGINRSP=2,
	MSG_ID_TEST=3,
	
};

typedef PROTO_MSG_ID MSG_ID;

void* CreateMsgById(int msgId)
{
	switch (msgId)
	{
		case MSG_ID_LOGIN:
		return new Login();
		case MSG_ID_LOGINRSP:
		return new LoginRsp();
		case MSG_ID_TEST:
		return new Test();
	}
	return NULL;
}