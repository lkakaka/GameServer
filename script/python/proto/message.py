
import proto.login_pb2
import proto.server_only.login_srv_pb2
import proto.server_only.common_pb2
import proto.test_pb2


class Message:
    MSG_ID_DISCONNECT = 1
    MSG_ID_LOAD_ROLE_REQ = 2
    MSG_ID_LOAD_ROLE_RSP = 3
    MSG_ID_LOGIN_REQ = 4
    MSG_ID_LOGIN_RSP = 5
    MSG_ID_RPC_MSG = 6
    MSG_ID_RPC_MSG_RSP = 7
    MSG_ID_TEST_REQ = 8

    _MsgId2Msg = {
        MSG_ID_DISCONNECT: proto.login_pb2.Disconnect,
        MSG_ID_LOAD_ROLE_REQ: proto.server_only.login_srv_pb2.LoadRoleReq,
        MSG_ID_LOAD_ROLE_RSP: proto.login_pb2.LoadRoleRsp,
        MSG_ID_LOGIN_REQ: proto.login_pb2.LoginReq,
        MSG_ID_LOGIN_RSP: proto.login_pb2.LoginRsp,
        MSG_ID_RPC_MSG: proto.server_only.common_pb2.RpcMsg,
        MSG_ID_RPC_MSG_RSP: proto.server_only.common_pb2.RpcMsgRsp,
        MSG_ID_TEST_REQ: proto.test_pb2.TestReq,
    }
    
    @staticmethod
    def create_msg_by_id(msg_id):
        msg_cls = Message._MsgId2Msg.get(msg_id, None)
        if msg_cls is None:
            return
        return msg_cls()
