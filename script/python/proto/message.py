
import proto.common_pb2
import proto.login_pb2
import proto.test_pb2

class message:
    MSG_ID_LOGIN_REQ = 1
    MSG_ID_LOGIN_RSP = 2
    MSG_ID_RPC_MSG = 3
    MSG_ID_RPC_MSG_RSP = 4
    MSG_ID_TEST_REQ = 5

    _MsgId2Msg = {
        MSG_ID_LOGIN_REQ: proto.login_pb2.LoginReq,
        MSG_ID_LOGIN_RSP: proto.login_pb2.LoginRsp,
        MSG_ID_RPC_MSG: proto.common_pb2.RpcMsg,
        MSG_ID_RPC_MSG_RSP: proto.common_pb2.RpcMsgRsp,
        MSG_ID_TEST_REQ: proto.test_pb2.TestReq,
    }
    
    @staticmethod
    def create_msg_by_id(msg_id):
        msg_cls = message._MsgId2Msg.get(msg_id, None)
        if msg_cls is None:
            return
        return msg_cls()
