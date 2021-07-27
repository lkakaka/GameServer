require("service.service_base")
require("util.timer")
require("util.logger")
require("game.scene.game_scene")
require("base.service_type")
require("game.gm.gm_handler")

clsSceneService = clsServiceBase:Inherit("clsSceneService")

function clsSceneService:__init__()
    Super(clsSceneService).__init__(self)
    self._scenes = {}
    self._player_to_scene = {}
    self:initRpcHandler()
    self:initServiceMsgHandler()
    self.gm_handler = clsGMHandler:New(self)
    timer.addTimer(3, 1, function(timerId) self:initScene() end)
    logger.logInfo("clsSceneService:__init__")
end

function clsSceneService:initScene()
    local scene_ids = StrUtil.parseToIntArray(Config:getConfigStr("scene_ids"))
    for _,scene_id in ipairs(scene_ids) do
        self:createScene(scene_id)
    end
    logger.logInfo("initScene, %s", StrUtil.tableToStr(scene_ids))
end

function clsSceneService:createScene(scene_id)
    local gameScene = clsGameScene:New(self, scene_id)
    self._scenes[gameScene.scene_uid] = gameScene
    local scene_info = { scene_id = gameScene.scene_id, scene_uid = gameScene.scene_uid }
    local future = self:callRpc(LOCAL_SERVICE_SCENE_CTRL, "RegScene", 10, scene_info)
    future:regCallback(function(result) self:onRegSceneResp(result, scene_info) end)
end

function clsSceneService:initRpcHandler()
    self:regRpcHandler("Scene_EnterScene", self.rpcEnterScene)
end

function clsSceneService:initServiceMsgHandler()
    -- self:regServiceMsgHandler(MSG_ID_CLIENT_DISCONNECT, self.onClientDisconnect)
end

function clsSceneService:onRegSceneResp(result, scene_info)
    if result.errCode == ErrorCode.OK then
        logger.logInfo("reg scene sucess, scene_id:%d, scene_uid:%d", scene_info.scene_id, scene_info.scene_uid)
    else
        logger.logError("reg scene failed, errCode:%d, scene_id:%d, scene_uid:%d", result.errCode, scene_info.scene_id, scene_info.scene_uid)
    end
end

function clsSceneService:remove_scene(scene_uid)
    scene = self._scenes[scene_uid]
    if scene ~= nil then
        scene.on_destroy()
    end
end

function clsSceneService:get_player_scene(conn_id)
    local scene_uid = self._player_to_scene[conn_id]
    if scene_uid == nil then return end
    return self._scenes[scene_uid]
end

function clsSceneService:on_remove_player(conn_id)
    self._player_to_scene[conn_id] = nil
end

function clsSceneService:on_recv_client_msg(conn_id, msg_id, msg)
    logger.logInfo("recv client msg, conn_id:%d, msg_id:%d", conn_id, msg_id)
    -- # func = SceneService._c_cmd.get_cmd_func(msg_id, need_exist=False)
    -- # if func is not None:
    -- #     msg = Message.create_msg_by_id(msg_id)
    -- #     msg.ParseFromString(msg_data)
    -- #     func(self, conn_id, msg_id, msg)
    -- #     return

    local game_scene = self:get_player_scene(conn_id)
    if game_scene == nil then
        logger.logInfo("on_recv_client_msg error, player game_scene not found, conn_id:%d, msg_id:%d", conn_id, msg_id)
        return
    end
    local pbMsg = decodeMsg(msg_id, msg)
    game_scene:on_recv_client_msg(conn_id, msg_id, pbMsg)
end

function clsSceneService:rpcEnterScene(sender, param)
    local conn_id = param.conn_id
    local role_id = param.role_id
    local scene_uid = param.scene_uid
    logger.logInfo("recv rpc enter scene, conn_id:%d, role_id:%d, scene_uid:%d", conn_id, role_id, scene_uid)
    local scene = self._scenes[scene_uid]
    self._player_to_scene[conn_id] = scene_uid
    scene:prepare_enter_scene(conn_id, role_id)
end

-- function clsSceneService:onClientDisconnect(sender, msgId, msg)
--     local game_scene = self:get_player_scene(msg.conn_id)
--     if game_scene == nil then
--         logger.logError("_on_recv_disconnect error, not found scene, conn_id:%d", msg.conn_id)
--         return
--     end
--     local player = game_scene:get_player_by_conn_id(msg.conn_id)
--     if player == nil then
--         logger.logError("_on_recv_disconnect error, player not in scene, conn_id:%d", msg.conn_id)
--         return
--     end
--     game_scene:remove_player(player.role_id, msg.reason)
--     player:on_leave_game()
-- end