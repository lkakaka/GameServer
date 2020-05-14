
# Generated by tool.  DO NOT EDIT!

from google.protobuf import descriptor_pb2  # 如果不先import，协议序列化(SerializeToString)会崩溃!!
import proto.server_only.login_srv_pb2
import proto.login_pb2
import proto.scene_pb2
import proto.server_only.common_pb2
import proto.test_pb2


class Message:
    MSG_ID_CLIENT_DISCONNECT = 1
    MSG_ID_CREATE_ROLE_REQ = 2
    MSG_ID_CREATE_ROLE_RSP = 3
    MSG_ID_DISCONNECT_REQ = 4
    MSG_ID_DISCONNECT_RSP = 5
    MSG_ID_ENTER_GAME = 6
    MSG_ID_ENTER_GAME_RSP = 7
    MSG_ID_ENTER_SCENE_REQ = 8
    MSG_ID_ENTER_SCENE_RSP = 9
    MSG_ID_LOAD_ROLE_LIST_REQ = 10
    MSG_ID_LOAD_ROLE_LIST_RSP = 11
    MSG_ID_LOAD_ROLE_REQ = 12
    MSG_ID_LOAD_ROLE_RSP = 13
    MSG_ID_LOGIN_REQ = 14
    MSG_ID_LOGIN_RSP = 15
    MSG_ID_RPC_MSG = 16
    MSG_ID_RPC_MSG_RSP = 17
    MSG_ID_TEST_REQ = 18

    _MsgId2Msg = {
        MSG_ID_CLIENT_DISCONNECT: proto.server_only.login_srv_pb2.ClientDisconnect,
        MSG_ID_CREATE_ROLE_REQ: proto.login_pb2.CreateRoleReq,
        MSG_ID_CREATE_ROLE_RSP: proto.login_pb2.CreateRoleRsp,
        MSG_ID_DISCONNECT_REQ: proto.login_pb2.DisconnectReq,
        MSG_ID_DISCONNECT_RSP: proto.login_pb2.DisconnectRsp,
        MSG_ID_ENTER_GAME: proto.login_pb2.EnterGame,
        MSG_ID_ENTER_GAME_RSP: proto.login_pb2.EnterGameRsp,
        MSG_ID_ENTER_SCENE_REQ: proto.scene_pb2.EnterSceneReq,
        MSG_ID_ENTER_SCENE_RSP: proto.scene_pb2.EnterSceneRsp,
        MSG_ID_LOAD_ROLE_LIST_REQ: proto.server_only.login_srv_pb2.LoadRoleListReq,
        MSG_ID_LOAD_ROLE_LIST_RSP: proto.login_pb2.LoadRoleListRsp,
        MSG_ID_LOAD_ROLE_REQ: proto.server_only.login_srv_pb2.LoadRoleReq,
        MSG_ID_LOAD_ROLE_RSP: proto.server_only.login_srv_pb2.LoadRoleRsp,
        MSG_ID_LOGIN_REQ: proto.login_pb2.LoginReq,
        MSG_ID_LOGIN_RSP: proto.login_pb2.LoginRsp,
        MSG_ID_RPC_MSG: proto.server_only.common_pb2.RpcMsg,
        MSG_ID_RPC_MSG_RSP: proto.server_only.common_pb2.RpcMsgRsp,
        MSG_ID_TEST_REQ: proto.test_pb2.TestReq,
    }
    
    _Msg2MsgId = {
        proto.server_only.login_srv_pb2.ClientDisconnect: MSG_ID_CLIENT_DISCONNECT,
        proto.login_pb2.CreateRoleReq: MSG_ID_CREATE_ROLE_REQ,
        proto.login_pb2.CreateRoleRsp: MSG_ID_CREATE_ROLE_RSP,
        proto.login_pb2.DisconnectReq: MSG_ID_DISCONNECT_REQ,
        proto.login_pb2.DisconnectRsp: MSG_ID_DISCONNECT_RSP,
        proto.login_pb2.EnterGame: MSG_ID_ENTER_GAME,
        proto.login_pb2.EnterGameRsp: MSG_ID_ENTER_GAME_RSP,
        proto.scene_pb2.EnterSceneReq: MSG_ID_ENTER_SCENE_REQ,
        proto.scene_pb2.EnterSceneRsp: MSG_ID_ENTER_SCENE_RSP,
        proto.server_only.login_srv_pb2.LoadRoleListReq: MSG_ID_LOAD_ROLE_LIST_REQ,
        proto.login_pb2.LoadRoleListRsp: MSG_ID_LOAD_ROLE_LIST_RSP,
        proto.server_only.login_srv_pb2.LoadRoleReq: MSG_ID_LOAD_ROLE_REQ,
        proto.server_only.login_srv_pb2.LoadRoleRsp: MSG_ID_LOAD_ROLE_RSP,
        proto.login_pb2.LoginReq: MSG_ID_LOGIN_REQ,
        proto.login_pb2.LoginRsp: MSG_ID_LOGIN_RSP,
        proto.server_only.common_pb2.RpcMsg: MSG_ID_RPC_MSG,
        proto.server_only.common_pb2.RpcMsgRsp: MSG_ID_RPC_MSG_RSP,
        proto.test_pb2.TestReq: MSG_ID_TEST_REQ,
    }
    
    @staticmethod
    def create_msg_by_id(msg_id):
        msg_cls = Message._MsgId2Msg.get(msg_id, None)
        if msg_cls is None:
            return
        return msg_cls()
        
    @staticmethod
    def get_msg_id(msg):
        return Message._Msg2MsgId.get(type(msg), None)
