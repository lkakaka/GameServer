# -*- encoding:utf-8 -*-
import weakref
import os

import Scene
import Config
from game.scene.game_player import GamePlayer
from game.util import logger
from proto.pb_message import Message
from game.util.const import ErrorCode

from game.data import cfg_scene
from game.service.service_addr import LOCAL_GATEWAY_SERVICE_ADDR
from game.service.service_addr import LOCAL_SCENE_CTRL_SERVICE_ADDR
import game.util.multi_index_container
from game.util.id_mgr import IDMgr


class GameScene:

    def __init__(self, service, scene_id):
        self.service = service
        self.scene_service_id = Config.getConfigInt("service_id")
        self.scene_id = scene_id
        self.scene_obj = Scene.SceneObj(scene_id, self)
        self._actors = {}
        self._mic_player = game.util.multi_index_container.MultiIndexContainer(GamePlayer)
        # print("scene obj =", self.scene_obj, self.scene_obj.scene_uid)
        scene_cfg = cfg_scene.find(scene_id)
        self.det_file_name = os.path.dirname(os.path.abspath(__file__)) + "/../../../../res/" + scene_cfg.nav_name
        # self.det_file_name = os.path.dirname(os.path.abspath(__file__)) + "/../../../../res/" + "all_tiles_navmesh.bin"
        logger.log_info("scene det_file:{0}", self.det_file_name)
        self.scene_obj.loadNavMesh(self.det_file_name)

    @property
    def scene_uid(self):
        return self.scene_obj.scene_uid

    def prepare_enter_scene(self, conn_id, role_id):
        tbls = self._add_load_tb(role_id)
        server_id = IDMgr.get_server_id_by_uid(role_id)
        print("server_id = ", role_id, server_id)
        future = self.service.db_proxy.load_multi(server_id, tbls)

        def on_load_role(err_code, tbls):
            self.on_load_player(err_code, conn_id, role_id, tbls)

        future.on_fin += on_load_role
        future.on_timeout += on_load_role
        logger.log_info("prepare enter scene, role_id:{}, scene_uid:{}, scene_id:{}", role_id, self.scene_uid, self.scene_id)

    def _add_load_tb(self, role_id):
        tbls = []
        # player
        tbl_player = game.db.tbl.tbl_player.TblPlayer()
        tbl_player.role_id = role_id
        tbls.append(tbl_player)
        # item
        tbl_item = game.db.tbl.tbl_item.TblItem()
        tbl_item.role_id = role_id
        tbls.append(tbl_item)

        return tbls

    def on_load_player(self, err_code, conn_id, role_id, tbls):
        # conn_id = msg.conn_id
        # role_id = msg.role_info.role_id
        # name = msg.role_info.role_name

        if err_code != ErrorCode.OK:
            logger.log_error("load player data error, role_id:{0}, conn_id:{1}", role_id, conn_id)
            return

        print(tbls)

        sorted_tbls = {}
        for tbl in tbls:
            tb_name = tbl["__tb_name"]
            tbl_obj = game.util.db_util.create_tbl_obj(tb_name)
            tbl_obj.assign(tbl)
            if tb_name not in sorted_tbls:
                sorted_tbls[tb_name] = [tbl_obj,]
            else:
                sorted_tbls[tb_name].append(tbl_obj)

        game_player = self.create_player(conn_id, role_id, sorted_tbls)
        self.add_player(game_player)
        self.on_player_enter(game_player)
        game_player.on_enter_scene()

    def on_player_enter(self, game_player):
        msg = Message.create_msg_by_id(Message.MSG_ID_SWITCH_SCENE_SERVICE)
        msg.conn_id = game_player.conn_id
        msg.scene_service_id = self.scene_service_id
        self.service.send_msg_to_service(LOCAL_GATEWAY_SERVICE_ADDR, msg)
        self.scene_obj.onPlayerEnter(game_player.actor_id)
        logger.log_info("player enter scene, role_id:{}, scene_uid:{}, name:{}", game_player.role_id, self.scene_uid, game_player.name)

    def create_player(self, conn_id, role_id, sorted_tbls):
        tb_player = sorted_tbls["player"][0]
        x, y = 0, 0
        move_speed = 20
        player_info = self.scene_obj.createPlayer(conn_id, role_id, tb_player.role_name, x, y, move_speed)
        game_player = GamePlayer(player_info, self, conn_id, role_id)
        game_player.init_player_data(sorted_tbls)
        return game_player

    def add_player(self, game_player):
        if self.get_player_by_role_id(game_player.role_id) is not None:
            logger.log_warn("player exist, role_id:{0}", game_player.role_id)
            self.remove_player(game_player.role_id, "repeated")
        self._mic_player.add_elem(game_player)
        self._actors[game_player.actor_id] = game_player

    def remove_player(self, role_id, reason):
        player = self.get_player_by_role_id(role_id)
        if player is None:
            return
        # self.scene_obj.removePlayer()
        self.scene_obj.removeActor(player.actor_id)
        self._mic_player.remove_elem(player)
        self._actors.pop(player.actor_id, None)
        self.service.on_remove_player(player.conn_id)
        player.on_leave_scene()

        logger.log_info("remove player, role_id:{0}, reason:{1}", role_id, reason)

    def tick_player(self, role_id, reason):
        player = self.get_player_by_role_id(role_id)
        if player is None:
            return
        self.remove_player(role_id, reason)
        player.on_leave_game()
        msg = Message.create_msg_by_id(Message.MSG_ID_CLIENT_DISCONNECT)
        msg.conn_id = player.conn_id
        msg.reason = reason
        self.service.send_msg_to_client(player.conn_id, msg)

    def get_player_by_conn_id(self, conn_id):
        return self._mic_player.get_one_elem(GamePlayer.ATTR_CONN_ID, conn_id)

    def get_player_by_role_id(self, role_id):
        return self._mic_player.get_one_elem(GamePlayer.ATTR_ROLE_ID, role_id)

    def get_actor(self, actor_id):
        return self._actors.get(actor_id, None)

    def on_recv_client_msg(self, conn_id, msg_id, msg_data):
        player = self.get_player_by_conn_id(conn_id)
        if player is None:
            logger.log_error("on_recv_service_msg error, not found player, msgId:{}", msg_id)
            return

        player.msg_handler.on_recv_client_msg(msg_id, msg_data)

    def _on_enter_sight(self, actor, enter_ids):
        if not enter_ids:
            return
        msg = Message.create_msg_by_id(Message.MSG_ID_ACTOR_BORN) if actor.is_player() else None
        for actor_id in enter_ids:
            enter_actor = self.get_actor(actor_id)
            if enter_actor is None:
                continue
            if msg is not None:
                enter_actor.pack_born_info(msg)
            enter_actor.on_actor_enter_sight(actor)

        if msg is not None:
            actor.send_msg_to_client(msg)

    def _on_leave_sight(self, actor, leave_ids, is_actor_leave):
        if not leave_ids:
            return
        msg = None
        if not is_actor_leave and actor.is_player():
            msg = Message.create_msg_by_id(Message.MSG_ID_ACTOR_DISSOLVE)
        for actor_id in leave_ids:
            leave_actor = self.get_actor(actor_id)
            if leave_actor is None:
                continue
            if msg is not None:
                msg.actor_ids.append(leave_actor.actor_id)
            leave_actor.on_actor_leave_sight(actor)

        if msg is not None:
            actor.send_msg_to_client(msg)

    def after_actor_enter(self, actor_id, enter_ids):
        actor = self.get_actor(actor_id)
        if actor is None:
            return
        self._on_enter_sight(actor, enter_ids)

    def after_actor_leave(self, actor_id, leave_ids):
        actor = self.get_actor(actor_id)
        if actor is None:
            return
        self._on_leave_sight(actor, leave_ids, True)

    def after_actor_move(self, actor_id, enter_ids, leave_ids):
        actor = self.get_actor(actor_id)
        if actor is None:
            return
        self._on_enter_sight(actor, enter_ids)
        self._on_leave_sight(actor, leave_ids, False)

    def destroy(self):
        self.service.remove_scene(self.scene_uid)

    def on_destroy(self):
        self.service.rpc_call(LOCAL_SCENE_CTRL_SERVICE_ADDR, "UnRegScene", scene_uid=self.scene_uid)

