require("game.scene.game_actor")
require("util.logger")
require("game.scene.player.item_mgr")
require("game.scene.player.msg_handler")
require("base.service_type")

clsGamePlayer = clsGameActor:Inherit("clsGamePlayer")

clsGamePlayer.index_roleId = "role_id"
clsGamePlayer.index_connId = "conn_id"

function clsGamePlayer:__init__(game_scene, engine_obj, role_id, name)
    Super(clsGamePlayer).__init__(self, game_scene, engine_obj:getActorId(), {clsGamePlayer.index_roleId, clsGamePlayer.index_connId})
    self.engineObj = engine_obj
    self.role_id = role_id
    self.name = name
    self.conn_id = engine_obj:getConnId()
    self:_init_mgr()
end

function clsGamePlayer:_init_mgr()
    self._item_mgr = clsItemMgr:New(self)
    self.msg_handler = clsMsgHandler:New(self)
end

function clsGamePlayer:init_player_data(tbls)
    -- # print(tables)
    local tb_player = tbls.player.rows[1]
    self.name = tb_player.role_name
    self.account = tb_player.account

    self._item_mgr:on_load_item(tbls.item)
end

function clsGamePlayer:is_player()
    return true
end

function clsGamePlayer:send_msg_to_client(msgId, msg)
    -- self.game_scene.service:sendMsgToClient(self.conn_id, msgId, msg)
    local data = encodeMsg(msgId, msg)
    self.engineObj:sendToClient(msgId, data, string.len(data))
end

function clsGamePlayer:send_msg_to_sight(msgId, msg)
    -- self.game_scene.service:sendMsgToClient(self.conn_id, msgId, msg)
    local data = encodeMsg(msgId, msg)
    self.engineObj:sendToSight(msgId, data, string.len(data))
end

function clsGamePlayer:send_msg_to_client_kcp(msgId, msg)
    self.game_scene.service:sendMsgToClientKCP(self.conn_id, msgId, msg)
end

function clsGamePlayer:send_msg_to_service(dst_srv, msgId, msg)
    self.game_scene.service:sendMsgToService(dst_srv, msgId, msg)
end

function clsGamePlayer:pack_born_info(msg)
    msg.player_list = msg.player_list or {}
    local player_info = {}
    player_info.actor_id = self.actor_id
    player_info.name = self.name
    table.insert(msg.player_list, player_info)
end

function clsGamePlayer:on_actor_enter_sight(actor)
    if not self:check_can_see(actor) then return end
    local msg = {}
    actor:pack_born_info(msg)
    self:send_msg_to_client(MSG_ID_ACTOR_BORN, msg)
end

function clsGamePlayer:on_actor_leave_sight(actor)
    local msg = {actor_ids = {}}
    table.insert(msg.actor_ids, actor.actor_id)
    self:send_msg_to_client(MSG_ID_ACTOR_DISSOLVE, msg)
    print("on_actor_leave_sight")
end

function clsGamePlayer:on_enter_scene()
    local game_scene = self.game_scene
    game_scene.service:callRpc(LOCAL_SERVICE_SCENE_CTRL, "Player_EnterScene", -1, {role_id=self.role_id,
                                scene_uid=game_scene.scene_uid, scene_id=game_scene.scene_id})
end

function clsGamePlayer:on_reconnect(conn_id)
    self:change_conn_id(conn_id)
    logger.logInfo("reconnect, role_id:%d, conn_id:%d", self.role_id, conn_id)
end

function clsGamePlayer:change_conn_id(conn_id)
    self:change_index_attr_value(clsGamePlayer.index_connId, conn_id)
    self.conn_id = conn_id
    self.engineObj:setConnId(conn_id)
end

function clsGamePlayer:on_leave_scene()
end

function clsGamePlayer:on_leave_game()
    self.game_scene.service:callRpc(LOCAL_SERVICE_SCENE_CTRL, "Player_LeaveGame", -1, {role_id=self.role_id})
end

function clsGamePlayer:try_switch_scene(scene_id)
    if self.game_scene.scene_id == scene_id then
        logger.logInfo("try switch scene fail, cur scene id=%d, role_id:%d", scene_id, self.role_id)
        return
    end
    local role_id = self.role_id

    local function _on_finish(err_code, scene_uid)
        if err_code ~= ErrorCode.OK then
            logger.logError("switch scene failed, role_id:%d, scene_id:%d, err:%d", role_id, scene_id, err_code)
            return
        end
        self:switch_scene(scene_uid)
    end

    local future = self.game_scene.service:callRpc(LOCAL_SERVICE_SCENE_CTRL, "Player_SwitchSceneReq", -1,
                                            {scene_id=scene_id, role_id=role_id})
    future:regCallback(_on_finish)
end

function clsGamePlayer:switch_scene(scene_uid)
    logger.logInfo("switch scene, role_id:%d, scene_uid:%d", self.role_id, scene_uid)
    self.game_scene.service:callRpc(LOCAL_SERVICE_SCENE_CTRL, "Player_SwitchScene", -1,
                                    {conn_id=self.conn_id, role_id=self.role_id, scene_uid=scene_uid})
    self:leave_scene("switch_scene")
end

function clsGamePlayer:leave_scene(reason)
    self.game_scene:remove_player(self.role_id, reason)
end

function clsGamePlayer:sendGMCmdRsp(cmd, result)
    local rsp_msg = {}
    rsp_msg.cmd = cmd
    rsp_msg.msg = result or ""
    self:send_msg_to_client(MSG_ID_GM_CMD_RSP, rsp_msg)
end