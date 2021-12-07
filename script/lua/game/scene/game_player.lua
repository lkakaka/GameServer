require("game.scene.scene_entity")
require("util.logger")
require("game.scene.player.item_mgr")
require("game.scene.player.msg_handler")
require("base.service_type")

clsGamePlayer = clsSceneEntity:Inherit("clsGamePlayer")

clsGamePlayer.index_roleId = "role_id"
clsGamePlayer.index_connId = "conn_id"

function clsGamePlayer:__init__(game_scene, engine_obj, role_id, name)
    Super(clsGamePlayer).__init__(self, game_scene, engine_obj:getEntityId(), {clsGamePlayer.index_roleId, clsGamePlayer.index_connId})
    self.engineObj = engine_obj
    self.role_id = role_id
    self.server_id = IDMgr.get_server_id_by_uid(role_id)
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
    player_info.actor_id = self.entity_id
    player_info.name = self.name
    table.insert(msg.player_list, player_info)
end

function clsGamePlayer:on_entity_enter_sight(entity)
    if not self:check_can_see(entity) then return end
    local msg = {}
    entity:pack_born_info(msg)
    self:send_msg_to_client(MSG_ID_ACTOR_BORN, msg)
end

function clsGamePlayer:on_entity_leave_sight(entity)
    local msg = {actor_ids = {}}
    table.insert(msg.actor_ids, entity.entity_id)
    self:send_msg_to_client(MSG_ID_ACTOR_DISSOLVE, msg)
    print("on_entity_leave_sight")
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

function clsGamePlayer:try_switch_scene(server_id, scene_id)
    if SERVER_GROUP_ID == server_id and self.game_scene.scene_id == scene_id then
        logger.logInfo("try switch scene fail, cur scene id=%d, role_id:%d", scene_id, self.role_id)
        return
    end
    local role_id = self.role_id

    -- local function _on_finish(err_code, scene_uid)
    --     if err_code ~= ErrorCode.OK then
    --         logger.logError("switch scene failed, role_id:%d, server_id:%d, scene_id:%d, err:%d", role_id, server_id, scene_id, err_code)
    --         return
    --     end
    --     self:switch_scene(server_id, scene_uid)
    -- end

    -- local scene_ctrl_addr = make_scene_ctrl_addr(server_id)

    if SERVER_GROUP_ID == server_id then
        self:_local_switch_scene(server_id, scene_id)
        -- local future = self.game_scene.service:callRpc(scene_ctrl_addr, "Player_SwitchSceneReq", -1, {scene_id=scene_id, role_id=role_id})
        -- future:regCallback(_on_finish)
    else
        self:_remote_switch_scene(server_id, scene_id)
        -- local future = self.game_scene.service:callRpc(scene_ctrl_addr, "Player_RemoteSwitchSceneReq", -1, {scene_id=scene_id, role_id=role_id})
        -- future:regCallback(_on_finish)
    end
end

-- 在本服切换场景
function clsGamePlayer:_local_switch_scene(server_id, scene_id)
    local function _on_finish(err_code, scene_uid)
        if err_code ~= ErrorCode.OK then
            logger.logError("switch scene failed, role_id:%d, server_id:%d, scene_id:%d, err:%d", role_id, server_id, scene_id, err_code)
            return
        end
        self:switch_local_scene(server_id, scene_uid)
    end

    local scene_ctrl_addr = make_scene_ctrl_addr(server_id)
    local future = self.game_scene.service:callRpc(scene_ctrl_addr, "Player_SwitchSceneReq", -1, {scene_id=scene_id, role_id=self.role_id})
    future:regCallback(_on_finish)
end

-- 切换到其他服场景
function clsGamePlayer:_remote_switch_scene(server_id, scene_id)
    local function _on_finish(err_code, result)
        if err_code ~= ErrorCode.OK then
            logger.logError("switch remote scene failed, role_id:%d, server_id:%d, scene_id:%d, err:%d", self.role_id, server_id, scene_id, err_code)
            return
        end
        local server_id = result.server_id
        local scene_uid = result.scene_uid
        local gateway_addr = result.gateway_addr
        local token = result.token
        self:switch_remote_scene(server_id, scene_uid, gateway_addr, token)
    end

    local scene_ctrl_addr = make_scene_ctrl_addr(server_id)
    local future = self.game_scene.service:callRpc(scene_ctrl_addr, "Player_RemoteSwitchSceneReq", -1, {scene_id=scene_id, role_id=self.role_id})
    future:regCallback(_on_finish)
end

function clsGamePlayer:is_local_server()
    return self.server_id == SERVER_GROUP_ID
end

function clsGamePlayer:switch_local_scene(server_id, scene_uid)
    logger.logInfo("switch scene, role_id:%d, server_id:%d, scene_uid:%d", self.role_id, server_id, scene_uid)
    self.game_scene.service:callRpc(LOCAL_SERVICE_SCENE_CTRL, "Player_SwitchScene", -1,
                                        {conn_id=self.conn_id, role_id=self.role_id, scene_uid=scene_uid})
    self:leave_scene("switch_scene")
end

function clsGamePlayer:switch_remote_scene(server_id, scene_uid, gateway_addr, token)
    logger.logInfo("switch remote scene, role_id:%d, server_id:%d, scene_uid:%d, gateway_addr:%s", 
                        self.role_id, server_id, scene_uid, StrUtil.tableToStr(gateway_addr))
    local scene_ctrl_addr = make_scene_ctrl_addr(self.server_id)
    self.game_scene.service:callRpc(scene_ctrl_addr, "Player_SwitchToRemoteScene", -1,
                                        {role_id=self.role_id, server_id=server_id, scene_uid=scene_uid})
    local msg = {}
    msg.remote_ip = gateway_addr.ip
    msg.remote_port = gateway_addr.port
    msg.remote_udp_port = gateway_addr.udp_port
    msg.token = token
    self:send_msg_to_client(MSG_ID_SWITCH_REMOTE_SERVER, msg)
    -- self:leave_scene("switch_remote_scene")
    -- self.game_scene:tick_player(self.role_id, "switch_remote_scene")
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