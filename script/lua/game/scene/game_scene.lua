
require("base.object")
require("util.logger")
require("game.scene.game_player")
require("game.scene.game_npc")
require("util.multi_index_container")
require("base.service_type")
require("data.cfg_scene")

clsGameScene = clsObject:Inherit("clsGameScene")

function clsGameScene:__init__(service, scene_id)
    self.service = service
    self.scene_id = scene_id
    self.scene_service_id = Config:getConfigInt("service_id")
    print("clsGameScene:__init__", scene_id)
    self._engineObj = SceneMgr.createScene(scene_id, self)
    self.scene_uid = self._engineObj:getSceneUid()
    self._actors = {}
    self._mic_player = clsMultIndexContainer:New({clsGamePlayer.index_roleId, clsGamePlayer.index_connId})

    local cfg_scene = CfgScene.find(scene_id)
    local navMeshName = "../res/" .. cfg_scene.nav_name
    -- local navMeshName = "../res/" .. "all_tiles_navmesh.bin"
    -- print(type(self._engineObj))
    self._engineObj:loadNavMesh(navMeshName)
    -- print("game scene c++ obj", self._engineObj)
    logger.logInfo("create scene, scene_id=%d, scene_uid=%d", self.scene_id, self.scene_uid)
end

function clsGameScene:prepare_enter_scene(conn_id, role_id)
    local tbls = self:_add_load_tb(role_id)
    local future = self.service.db_proxy:loadMulti(tbls)

    local function on_load_role(err_code, result)
        print("on_load_role", StrUtil.tableToStr(result))
        self:on_load_player(err_code, conn_id, role_id, result)
    end

    future:regCallback(on_load_role)
    logger.logInfo("prepare enter scene, role_id:%d, scene_uid:%d, scene_id:%d", role_id, self.scene_uid, self.scene_id)
end

function clsGameScene:_add_load_tb(role_id)
    assert(role_id > 0, "role id error")
    local tbls = {}
    -- # player
    local tbl_player = {table_name = "player", row = { role_id = role_id }}
    table.insert(tbls, tbl_player)
    -- # item
    local tbl_item = {table_name = "item", row = { role_id = role_id }}
    table.insert(tbls, tbl_item)

    return tbls
end

function clsGameScene:on_load_player(err_code, conn_id, role_id, tbls)
    if err_code ~= ErrorCode.OK then
        logger.logError("load player data error, role_id:%d, conn_id:%d", role_id, conn_id)
        return
    end

    local game_player = self:create_player(conn_id, role_id, tbls)
    self:add_player(game_player)
    self:on_player_enter(game_player)
    game_player:on_enter_scene()
end

function clsGameScene:on_player_enter(game_player)
    local msg = {}
    msg.conn_id = game_player.conn_id
    msg.scene_service_id = self.scene_service_id
    self.service:sendMsgToService(LOCAL_SERVICE_GATEWAY, MSG_ID_SWITCH_SCENE_SERVICE, msg)
    self._engineObj:onActorEnter(game_player.actor_id)
    logger.logInfo("player enter scene, role_id:%d, scene_uid:%d, name:%s", game_player.role_id, self.scene_uid, game_player.name)
end

function clsGameScene:create_player(conn_id, role_id, tbls)
    local tb_player = tbls.player.rows[1]
    local x, y = 0, 0
    local move_speed = 10
    local player_obj = self._engineObj:createPlayer(conn_id, role_id, tb_player.role_name, x, y, move_speed)
    local game_player = clsGamePlayer:New(self, player_obj, role_id, tb_player.role_name)
    game_player:init_player_data(tbls)
    return game_player
end

function clsGameScene:add_player(game_player)
    if self:get_player_by_role_id(game_player.role_id) ~= nil then
        logger.logWarn("player exist, role_id:{0}", game_player.role_id)
        self:remove_player(game_player.role_id, "repeated")
    end
    self._mic_player:addElem(game_player)
    self._actors[game_player.actor_id] = game_player
end

function clsGameScene:remove_player(role_id, reason)
    local player = self:get_player_by_role_id(role_id)
    if player == nil then return end
    -- # self.scene_obj.removePlayer()
    self._engineObj:removeActor(player.actor_id)
    self._mic_player:removeElem(player)
    self._actors[player.actor_id] = nil
    self.service:on_remove_player(player.conn_id)
    player:on_leave_scene()

    logger.logInfo("remove player, role_id:%d, reason:%s", role_id, reason)
end

function clsGameScene:tick_player(role_id, reason)
    local player = self:get_player_by_role_id(role_id)
    if player == nil then return end
    self:remove_player(role_id, reason)
    player:on_leave_game()
    local msg = {}
    msg.conn_id = player.conn_id
    msg.reason = reason
    self.service:sendMsgToClient(player.conn_id, MSG_ID_CLIENT_DISCONNECT, msg)
end

function clsGameScene:get_player_by_conn_id(conn_id)
    return self._mic_player:getOneElem(clsGamePlayer.index_connId, conn_id)
end

function clsGameScene:get_player_by_role_id(role_id)
    return self._mic_player:getOneElem(clsGamePlayer.index_roleId, role_id)
end

function clsGameScene:get_actor(actor_id)
    return self._actors[actor_id]
end

function clsGameScene:create_npc(npc_id, x, y)
    local npc_obj = self._engineObj:createNpc(npc_id, x, y, 0)
    local game_npc = clsGameNpc:New(npc_id, self, npc_obj)
    self._actors[game_npc.actor_id] = game_npc
    self._engineObj:onActorEnter(game_npc.actor_id)
    game_npc:on_leave_scene()
    logger.logInfo("create npc, npc_id:%d, scene_id:%d", npc_id, self.scene_id)
    return game_npc
end

function clsGameScene:remove_npc(actor_id)
    local npc = self:get_actor(actor_id)
    if npc == nil then
        logger.logWarn("remove npc error, not found npc:%d", actor_id)
        return
    end

    if not npc:is_npc() then
        logger.logError("remove not a npc, actor_id:%d", actor_id)
        return
    end

    self._engineObj:removeActor(actor_id)
    self._actors[actor_id] = nil
end

function clsGameScene:on_recv_client_msg(conn_id, msg_id, msg)
    local player = self:get_player_by_conn_id(conn_id)
    if player == nil then
        logger.logError("on_recv_service_msg error, not found player, msgId:%d", msg_id)
        return
    end

    player.msg_handler:on_recv_client_msg(msg_id, msg)
end

function clsGameScene:_on_enter_sight(actor, enter_ids)
    if TableUtil.isEmpty(enter_ids) then return end
    local msg = nil
    if actor.is_player() then msg = {} end

    for _,actor_id in ipairs(enter_ids) do
        local enter_actor = self:get_actor(actor_id)
        if enter_actor ~= nil then
            if msg ~= nil then
                enter_actor:pack_born_info(msg)
            end
        end
        enter_actor:on_actor_enter_sight(actor)
    end

    if msg ~= nil then
        actor:send_msg_to_client(MSG_ID_ACTOR_BORN, msg)
    end
end

function clsGameScene:_on_leave_sight(actor, leave_ids, is_actor_leave)
    if TableUtil.isEmpty(leave_ids) then return end
    local msg = nil
    if not is_actor_leave and actor.is_player() then
        msg = {actor_ids = {}}
    end

    for _,actor_id in ipairs(leave_ids) do
        local leave_actor = self:get_actor(actor_id)
        if leave_actor ~= nil then
            if msg ~= nil then
                table.insert(msg.actor_ids, leave_actor.actor_id)
            end
            leave_actor:on_actor_leave_sight(actor)
        end
    end

    if msg ~= nil then
        actor:send_msg_to_client(MSG_ID_ACTOR_DISSOLVE, msg)
    end
end

function clsGameScene:after_actor_enter(actor_id, enter_ids)
    print("after_actor_enter", self.scene_id, actor_id, type(enter_ids), StrUtil.tableToStr(enter_ids or {}))
    local actor = self:get_actor(actor_id)
    if actor == nil then return end
    self:_on_enter_sight(actor, enter_ids)
end

function clsGameScene:after_actor_leave(actor_id, leave_ids)
    local actor = self:get_actor(actor_id)
    if actor == nil then return end
    self:_on_leave_sight(actor, leave_ids, true)
end

function clsGameScene:after_actor_move(actor_id, enter_ids, leave_ids)
    local actor = self:get_actor(actor_id)
    if actor == nil then return end
    self:_on_enter_sight(actor, enter_ids)
    self:_on_leave_sight(actor, leave_ids, false)
end

function clsGameScene:destroy()
    self.service:remove_scene(self.scene_uid)
end

function clsGameScene:on_destroy()
    self.service:rpcCall(LOCAL_SERVICE_SCENE_CTRL, "UnRegScene", -1, {scene_uid=self.scene_uid})
    self.service = nil
end