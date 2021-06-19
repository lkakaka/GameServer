# -*- encoding:utf-8 -*-
import Scene
from proto.pb_message import Message
from game.util import logger
import game.util.db_util

from game.db.tbl.tbl_player import TblPlayer
import game.db.tbl.tbl_item

import game.scene.player.item_mgr
import game.scene.player.gm_mgr

import game.scene.game_actor
from game.service.service_addr import LOCAL_DB_SERVICE_ADDR


class GamePlayer(game.scene.game_actor.GameActor):
    _c_cmd = game.util.cmd_util.CmdDispatch("c_player")

    def __init__(self, e_player, game_scene, conn_id, role_id):
        game.scene.game_actor.GameActor.__init__(self, e_player[1])
        self.native_obj = Scene.Player(e_player, self)
        # self.actor_id = e_player[1]
        self.game_scene = game_scene
        self.conn_id = conn_id
        self.role_id = role_id

        self._gm_mgr = game.scene.player.gm_mgr.GMMgr(self)
        self._item_mgr = game.scene.player.item_mgr.ItemMgr(self)

    def init_player_data(self, tables):
        # print(tables)
        tb_player = tables["player"][0]
        self.name = tb_player.role_name
        self.account = tb_player.account

        self._item_mgr.on_load_item(tables.get("item"))

    def is_player(self):
        return True

    def on_recv_client_msg(self, msg_id, msg_data):
        msg = Message.create_msg_by_id(msg_id)
        msg.ParseFromString(msg_data)
        func = GamePlayer._c_cmd.get_cmd_func(msg_id)
        if func is None:
            logger.log_error("player on_recv_client_msg error, not found cmd func, msgId:{}", msg_id)
            return
        func(self, msg_id, msg)

    def send_msg_to_client(self, msg):
        self.game_scene.service.send_msg_to_client(self.conn_id, msg)

    def send_msg_to_service(self, dst_srv, msg):
        self.game_scene.service.send_msg_to_service(dst_srv, msg)

    def pack_born_info(self, msg):
        player_info = msg.player_list.add()
        player_info.actor_id = self.actor_id
        player_info.name = self.name

    def on_actor_enter_sight(self, actor):
        if not self.check_can_see(actor):
            return
        msg = Message.create_msg_by_id(Message.MSG_ID_ACTOR_BORN)
        actor.pack_born_info(msg)
        self.send_msg_to_client(msg)

    def on_actor_leave_sight(self, actor):
        msg = Message.create_msg_by_id(Message.MSG_ID_ACTOR_DISSOLVE)
        msg.actor_ids.append(actor.actor_id)
        self.send_msg_to_client(msg)
        print("on_actor_leave_sight")

    @_c_cmd.reg_cmd(Message.MSG_ID_DISCONNECT_REQ)
    def _on_recv_disconnect(self, msg_id, msg):
        logger.log_error("player disconnect, role_id:{},reason:{}", self.role_id, msg.reason)
        self.game_scene.tick_player(self.role_id, "client req disconnect")

    @_c_cmd.reg_cmd(Message.MSG_ID_GM_CMD)
    def _on_recv_gm_cmd(self, msg_id, msg):
        result = self._gm_mgr.on_recv_gm_cmd(msg.cmd, msg.args)
        game.util.logger.log_info("exe gm cmd:{}, args:{}, result:\n{}", msg.cmd, msg.args, result)
        rsp_msg = Message.create_msg_by_id(Message.MSG_ID_GM_CMD_RSP)
        rsp_msg.cmd = msg.cmd
        rsp_msg.msg = result if result is None else ""
        self.send_msg_to_client(rsp_msg)

    @_c_cmd.reg_cmd(Message.MSG_ID_TEST_REQ)
    def _on_recv_test_req(self, msg_id, msg):
        print("$player recv test req, role_id:{}, msg:{}", self.role_id, msg)
        rsp_msg = Message.create_msg_by_id(Message.MSG_ID_TEST_REQ)
        rsp_msg.id = 10
        rsp_msg.msg = "welcome to game world, " + self.name
        self.send_msg_to_client(rsp_msg)
        self.send_msg_to_service(LOCAL_DB_SERVICE_ADDR, msg)

        def on_load_cb(err_code, lst):
            print("on_load_cb----", lst)

        future = self.game_scene.service.db_proxy.load("player", role_id=1)
        future.on_fin += on_load_cb

        tbl_player = TblPlayer()
        tbl_player.role_id = 6
        tbl_player.role_name = "rename"
        # tbl_player.account = "aa"
        self.game_scene.service.db_proxy.update(tbl_player)
        # start_pos = (15, 10, -47)
        # end_pos = (43, 10, -1)
        start_pos = (-665610, 0, -689073)
        end_pos = (-641419, 0, -709361)
        path = self.game_scene.scene_obj.findPath(start_pos, end_pos)
        logger.log_info("find path {0}->{1}, {2}", start_pos, end_pos, path)
