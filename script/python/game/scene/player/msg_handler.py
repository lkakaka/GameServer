
import weakref
import game.util.cmd_util
from proto.pb_message import Message
from game.util import logger
from game.service.service_addr import LOCAL_DB_SERVICE_ADDR
from game.db.tbl.tbl_player import TblPlayer


class MsgHandler(object):
    _c_cmd = game.util.cmd_util.CmdDispatch("c_player")

    def __init__(self, player):
        self._player = weakref.ref(player)

    @property
    def player(self):
        return self._player()

    def on_recv_client_msg(self, msg_id, msg_data):
        msg = Message.create_msg_by_id(msg_id)
        msg.ParseFromString(msg_data)
        func = MsgHandler._c_cmd.get_cmd_func(msg_id)
        if func is None:
            logger.log_error("player on_recv_client_msg error, not found cmd func, msgId:{}", msg_id)
            return
        func(self, msg_id, msg)

    @_c_cmd.reg_cmd(Message.MSG_ID_DISCONNECT_REQ)
    def _on_recv_disconnect(self, msg_id, msg):
        logger.log_error("player disconnect, role_id:{},reason:{}", self.player.role_id, msg.reason)
        self.player.game_scene.tick_player(self.player.role_id, "client req disconnect")

    @_c_cmd.reg_cmd(Message.MSG_ID_GM_CMD)
    def _on_recv_gm_cmd(self, msg_id, msg):
        from game.gm.gm_handler import GMParam
        param = GMParam()
        param.player = self.player
        param.args = msg.args
        result = self.player.game_scene.service.gm_handler.handle_gm_cmd(msg.cmd, param)
        game.util.logger.log_info("exe gm cmd:{}, args:{}, result:\n{}", msg.cmd, msg.args, result)
        rsp_msg = Message.create_msg_by_id(Message.MSG_ID_GM_CMD_RSP)
        rsp_msg.cmd = msg.cmd
        rsp_msg.msg = result if result is None else ""
        self.player.send_msg_to_client(rsp_msg)

    @_c_cmd.reg_cmd(Message.MSG_ID_TEST_REQ)
    def _on_recv_test_req(self, msg_id, msg):
        print("$player recv test req, role_id:{}, msg:{}", self.player.role_id, msg)
        rsp_msg = Message.create_msg_by_id(Message.MSG_ID_TEST_REQ)
        rsp_msg.id = 10
        rsp_msg.msg = "welcome to game world, " + self.player.name
        self.player.send_msg_to_client(rsp_msg)
        self.player.send_msg_to_service(LOCAL_DB_SERVICE_ADDR, msg)

        def on_load_cb(err_code, lst):
            print("on_load_cb----", lst)

        future = self.player.game_scene.service.db_proxy.load("player", role_id=1)
        future.on_fin += on_load_cb

        tbl_player = TblPlayer()
        tbl_player.role_id = 6
        tbl_player.role_name = "rename"
        # tbl_player.account = "aa"
        self.player.game_scene.service.db_proxy.update(tbl_player)
        start_pos = (15, 10, -47)
        end_pos = (43, 10, -1)
        # start_pos = (-665610, 0, -689073)
        # end_pos = (-641419, 0, -709361)
        path = self.player.game_scene.scene_obj.findPath(start_pos, end_pos)
        logger.log_info("find path {0}->{1}, {2}", start_pos, end_pos, path)
