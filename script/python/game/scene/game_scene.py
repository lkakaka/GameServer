import weakref

import Scene
import game.scene.game_player
from util import logger
from proto.pb_message import Message
import util.cmd_util
import util.db_util
from util.const import ErrorCode


class GameScene:

    def __init__(self, service, scene_id):
        self.service = service
        self.scene_id = scene_id
        self.scene_obj = Scene.SceneObj(scene_id, self)
        self.player_dict = {}
        self.player_conn_dict = {}
        # print("scene obj =", self.scene_obj, self.scene_obj.scene_uid)

    @property
    def scene_uid(self):
        return self.scene_obj.scene_uid

    def prepare_enter_scene(self, conn_id, role_id):
        # msg = Message.create_msg_by_id(Message.MSG_ID_LOAD_ROLE_REQ)
        # msg.role_id = role_id
        # msg.conn_id = conn_id
        # self.service.send_msg_to_service("db", msg)
        tbls = self._add_load_tb(role_id)
        future = self.service.db_proxy.load_multi(tbls)

        def on_load_role(err_code, tbls):
            self.on_load_player(err_code, conn_id, role_id, tbls)

        future.on_fin += on_load_role
        future.on_timeout += on_load_role
        logger.log_info("prepare enter scene, role_id:{}, scene_uid:{}, scene_id:%d", role_id, self.scene_uid, self.scene_id)

    def _add_load_tb(self, role_id):
        tbls = []
        # player
        tbl_player = util.db_util.create_tbl_obj("player")
        tbl_player.role_id = role_id
        tbls.append(tbl_player)
        # item
        tbl_item = util.db_util.create_tbl_obj("item")
        tbl_item.role_id = role_id
        tbls.append(tbl_item)

        return tbls

    def on_load_player(self, err_code, conn_id, role_id, tbls):
        # conn_id = msg.conn_id
        # role_id = msg.role_info.role_id
        # name = msg.role_info.role_name

        if err_code != ErrorCode.OK:
            logger.log_error("load player data error, role_id:{}, ", role_id)
            return

        sorted_tbls = {}
        for tbl in tbls:
            tb_name = tbl["__tb_name"]
            tbl_obj = util.db_util.create_tbl_obj(tb_name)
            tbl_obj.assign(tbl)
            if tb_name not in sorted_tbls:
                sorted_tbls[tb_name] = [tbl_obj,]
            else:
                sorted_tbls[tb_name].append(tbl_obj)

        game_player = self.create_player(conn_id, role_id, sorted_tbls)
        self.player_dict[role_id] = game_player
        self.player_conn_dict[conn_id] = weakref.ref(game_player)
        self.on_player_enter(game_player)

    def on_player_enter(self, game_player):
        self.scene_obj.onPlayerEnter(game_player.actor_id)
        logger.log_info("player enter scene, role_id:{}, scene_uid:{}, name:{}", game_player.role_id, self.scene_uid, game_player.name)

    def create_player(self, conn_id, role_id, sorted_tbls):
        tb_player = sorted_tbls["player"][0]
        player_info = self.scene_obj.createPlayer(conn_id, role_id, tb_player.role_name, 0, 0)
        game_player = game.scene.game_player.GamePlayer(player_info, self, conn_id, role_id)
        game_player.init_player_data(sorted_tbls)
        return game_player

    def remove_player(self, role_id, reason):
        player = self.get_player_by_role_id(role_id)
        if player is None:
            return
        # self.scene_obj.removePlayer()
        self.scene_obj.removeActor(player.actor_id)
        self.player_dict.pop(role_id, None)
        self.player_conn_dict.pop(player.conn_id, None)
        self.service.on_remove_player(player.conn_id)

        logger.log_info("remove player, reason:{}", reason)

    def tick_player(self, role_id, reason):
        player = self.get_player_by_role_id(role_id)
        if player is None:
            return
        self.remove_player(role_id, reason)
        msg = Message.create_msg_by_id(Message.MSG_ID_CLIENT_DISCONNECT)
        msg.conn_id = player.conn_id
        msg.reason = reason
        self.service.send_msg_to_client(player.conn_id, msg)

    def get_player_by_conn_id(self, conn_id):
        weak_player = self.player_conn_dict.get(conn_id)
        if weak_player is None:
            return None
        return weak_player()

    def get_player_by_role_id(self, role_id):
        return self.player_dict.get(role_id)

    def on_recv_client_msg(self, conn_id, msg_id, msg_data):
        player = self.get_player_by_conn_id(conn_id)
        if player is None:
            logger.log_error("on_recv_service_msg error, not found player, msgId:{}", msg_id)
            return

        player.on_recv_client_msg(msg_id, msg_data)

    def on_actor_enter(self, actor_id, enter_ids):
        print("on_actor_enter", actor_id, enter_ids)

    def on_actor_leave(self, actor_id, leave_ids):
        print("on_actor_leave", actor_id, leave_ids)

    def on_actor_move(self, actor_id, enter_ids, leave_ids):
        print("on_actor_move", actor_id, enter_ids, leave_ids)

