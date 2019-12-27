
import proto.login_pb2
import proto.test_pb2

class message:
    MSG_ID_LOGIN = 1
    MSG_ID_LOGINRSP = 2
    MSG_ID_TEST = 3

    _MsgId2Msg = {
        MSG_ID_LOGIN: proto.login_pb2.Login,
        MSG_ID_LOGINRSP: proto.login_pb2.LoginRsp,
        MSG_ID_TEST: proto.test_pb2.Test,
    }
    
    @staticmethod
    def create_msg_by_id(msg_id):
        msg_cls = message._MsgId2Msg.get(msg_id, None)
        if msg_cls is None:
            return
        return msg_cls()
