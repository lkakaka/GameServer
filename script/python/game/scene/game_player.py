# -*- encoding:utf-8 -*-
import Scene
from proto.pb_message import Message
from game.util import logger
import game.util.db_util
from game.util.id_mgr import IDMgr

from game.db.tbl.tbl_player import TblPlayer
import game.db.tbl.tbl_item

import game.scene.player.item_mgr
import game.scene.player.msg_handler

import game.scene.game_actor
from game.service.service_addr import LOCAL_DB_SERVICE_ADDR
from game.service.service_addr import LOCAL_SCENE_CTRL_SERVICE_ADDR
from game.service.service_addr import LOCAL_SERVICE_GROUP
from game.service.service_addr import ServiceAddr
from game.util.const import ErrorCode
from game.util.multi_index_container import MultiIndexElement


class GamePlayer(game.scene.game_actor.GameActor, MultiIndexElement):
    # _c_cmd = game.util.cmd_util.CmdDispatch("c_player")

    ATTR_ROLE_ID = "role_id"
    ATTR_CONN_ID = "conn_id"

    @staticmethod
    def define_multi_index_attr_names():
        return GamePlayer.ATTR_ROLE_ID, GamePlayer.ATTR_CONN_ID

    def __init__(self, e_player, game_scene, conn_id, role_id):
        MultiIndexElement.__init__(self)
        game.scene.game_actor.GameActor.__init__(self, e_player[1])
        self.native_obj = Scene.Player(e_player, self)
        # self.actor_id = e_player[1]
        self.game_scene = game_scene
        self.conn_id = conn_id
        self.role_id = role_id
        self.server_id = IDMgr.get_server_id_by_uid(role_id)
        self._init_mgr()

    def _init_mgr(self):
        self._item_mgr = game.scene.player.item_mgr.ItemMgr(self)
        self.msg_handler = game.scene.player.msg_handler.MsgHandler(self)

    def init_player_data(self, tables):
        # print(tables)
        tb_player = tables["player"][0]
        self.name = tb_player.role_name
        self.account = tb_player.account

        self._item_mgr.on_load_item(tables.get("item"))

    def is_player(self):
        return True

    def send_msg_to_client(self, msg):
        self.game_scene.service.send_msg_to_client(self.conn_id, msg)

    def send_msg_to_client_kcp(self, msg):
        self.game_scene.service.send_msg_to_client_kcp(self.conn_id, msg)

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
        # print("on_actor_leave_sight")

    def on_enter_scene(self):
        game_scene = self.game_scene
        game_scene.service.rpc_call(LOCAL_SCENE_CTRL_SERVICE_ADDR, "Player_EnterScene", role_id=self.role_id,
                                    scene_uid=game_scene.scene_uid, scene_id=game_scene.scene_id)

    def on_leave_scene(self):
        pass

    def on_leave_game(self):
        self.game_scene.service.rpc_call(LOCAL_SCENE_CTRL_SERVICE_ADDR, "Player_LeaveGame", role_id=self.role_id)

    def try_switch_scene(self, server_id, scene_id):
        if LOCAL_SERVICE_GROUP == server_id and self.game_scene.scene_id == scene_id:
            logger.log_info("try switch scene fail, cur scene id={0}, role_id:{1}", scene_id, self.role_id)
            return

        if LOCAL_SERVICE_GROUP == server_id:
            self._local_switch_scene(server_id, scene_id)
        else:
            self._remote_switch_scene(server_id, scene_id)

    def _local_switch_scene(self, server_id, scene_id):
        def _on_finish(err_code, scene_uid):
            if err_code != ErrorCode.OK:
                logger.log_error("switch scene failed, role_id:%d, server_id:%d, scene_id:%d, err:%d", self.role_id, server_id,
                                scene_id, err_code)
                return
            self.switch_local_scene(scene_uid)

        scene_ctrl_addr = ServiceAddr.make_scene_ctrl_addr(server_id)
        future = self.game_scene.service.rpc_call(scene_ctrl_addr, "Player_SwitchSceneReq", -1, scene_id=scene_id, role_id=self.role_id)
        future.on_fin += _on_finish
        future.on_timeout += _on_finish

    def _remote_switch_scene(self, server_id, scene_id):
        def _on_finish(err_code, server_id=None, scene_uid=None, gateway_addr=None, token=None):
            if err_code != ErrorCode.OK:
                logger.log_error("switch remote scene failed, role_id:{}, server_id:{}, scene_id:{}, err:{}", self.role_id,
                                server_id, scene_id, err_code)
                return
            self.switch_remote_scene(server_id, scene_uid, gateway_addr, token)

        scene_ctrl_addr = ServiceAddr.make_scene_ctrl_addr(server_id)
        future = self.game_scene.service.rpc_call(scene_ctrl_addr, "Player_RemoteSwitchSceneReq", -1, scene_id=scene_id, role_id=self.role_id)
        future.on_fin += _on_finish
        future.on_timeout += _on_finish

    def switch_remote_scene(self, server_id, scene_uid, gateway_addr, token):
        logger.log_info("switch remote scene, role_id:{}, server_id:{}, scene_uid:{}, gateway_addr:{}",
                       self.role_id, server_id, scene_uid, gateway_addr)
        scene_ctrl_addr = ServiceAddr.make_scene_ctrl_addr(self.server_id)
        self.game_scene.service.rpc_call(scene_ctrl_addr, "Player_SwitchToRemoteScene", -1,
                                         role_id=self.role_id, server_id=server_id, scene_uid=scene_uid)
        msg = Message.create_msg_by_id(Message.MSG_ID_SWITCH_REMOTE_SERVER)
        msg.remote_ip = gateway_addr["ip"]
        msg.remote_port = gateway_addr["port"]
        msg.remote_udp_port = gateway_addr["udp_port"]
        msg.token = token
        self.send_msg_to_client(msg)

    def switch_local_scene(self, scene_uid):
        logger.log_info("switch local scene, role_id:{}, scene_uid:{}", self.role_id, scene_uid)
        self.game_scene.service.rpc_call(LOCAL_SCENE_CTRL_SERVICE_ADDR, "Player_SwitchScene", -1,
                                         conn_id=self.conn_id, role_id=self.role_id, scene_uid=scene_uid)
        self.leave_scene("switch_scene")

    def leave_scene(self, reason):
        self.game_scene.remove_player(self.role_id, reason)
