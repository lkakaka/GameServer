require("service.service_base")
require("util.logger")
require("util.const")
require("game.scene_ctrl.scene_mgr")
require("game.scene_ctrl.player_mgr")
require("util.multi_index_container")

clsSceneCtrlService = clsServiceBase:Inherit("clsSceneCtrlService")

function clsSceneCtrlService:__init__()
    Super(clsSceneCtrlService).__init__(self)

    self._scene_mgr = clsSceneMgr:New(self)
    self._player_mgr = clsSceneCtrlPlayerMgr:New()
    self:initRpcHandlder()
    logger.logInfo("clsSceneCtrlService __init__")
end

function clsSceneCtrlService:initRpcHandlder()
    self:regRpcHandler("RegScene", self.rpc_RegScene)
    self:regRpcHandler("UnRegScene", self.rpc_UnRegScene)
    self:regRpcHandler("Player_EnterGame", self.rpcPlayerEnterGame)
    self:regRpcHandler("Player_LeaveGame", self.rpcPlayerLeaveGame)
    self:regRpcHandler("Player_SwitchScene", self.rpcPlayerSwitchScene)
    self:regRpcHandler("Player_EnterScene", self.rpcPlayerEnterScene)
    self:regRpcHandler("Player_SwitchSceneReq", self.rpcPlayerSwitchSceneReq)
end

function clsSceneCtrlService:rpc_RegScene(sender, param)
    self._scene_mgr:reg_scene(param.scene_id, param.scene_uid, sender)
    logger.logInfo("rpc_RegScene, %s", StrUtil.tableToStr(param))
    return ErrorCode.OK
end

function clsSceneCtrlService:rpc_UnRegScene(sender, param)
    self._scene_mgr:unreg_scene(param.scene_uid)
    logger.logInfo("rpc_UnRegScene, %s", StrUtil.tableToStr(param))
    return ErrorCode.OK
end

function clsSceneCtrlService:rpcPlayerEnterGame(sender, param)
    logger.logInfo("player enter game, %s", StrUtil.tableToStr(param))
    local conn_id = param.conn_id
    local role_id = param.role_id
    local scene_id = param.scene_id or 1
    local scene = self._scene_mgr:get_min_player_scene(scene_id)
    if scene == nil then
        logger.logInfo("player enter game failed, not found scene, scene_id:%d, role_id:%d", scene_id, role_id)
        return ErrorCode.NOT_FOUND_SCENE
    end
    local player_info = self._player_mgr:add_player(role_id, conn_id)
    player_info.state = PlayerState.LOGINING
    logger.logInfo("player enter game, role_id:%d, scene_id:%d, scene_uid:%d", role_id, scene_id, scene.scene_uid)
    self:callRpc(scene.service_addr, "Scene_EnterScene", -1, {conn_id=conn_id, role_id=role_id, scene_uid=scene.scene_uid})
    return ErrorCode.OK
end

function clsSceneCtrlService:rpcPlayerLeaveGame(sender, param)
    self._player_mgr:remove_player(param.role_id)
    logger.logInfo("player leave game, role_id:%d", param.role_id)
end

function clsSceneCtrlService:rpcPlayerSwitchSceneReq(sender, param)
    local role_id = param.role_id
    local scene_id = param.scene_id
    local scene = self._scene_mgr:get_min_player_scene(scene_id)
    if scene == nil then
        logger.logInfo("player switch scene req failed, not found scene, scene_id:%d, role_id:%d", scene_id, role_id)
        return ErrorCode.NOT_FOUND_SCENE, -1
    end
    logger.logInfo("player switch scene req, role_id:%d, scene_id:%d, scene_uid:%d", role_id, scene_id, scene.scene_uid)
    return ErrorCode.OK, scene.scene_uid
end

function clsSceneCtrlService:rpcPlayerSwitchScene(sender, param)
    local conn_id = param.conn_id
    local role_id = param.role_id
    local scene_uid = param.scene_uid
    local scene = self._scene_mgr:get_scene_by_uid(scene_uid)
    if scene == nil then
        logger.logInfo("player switch scene failed, not found scene, scene_uid:%d, role_id:%d", scene_uid,
                        role_id)
        return ErrorCode.NOT_FOUND_SCENE
    end
    self._player_mgr:switch_state(role_id, PlayerState.SWITCHING, {scene_uid=0, scene_id=0})
    logger.logInfo("player switch scene, role_id:%d, scene_uid:%d", role_id, scene_uid)
    self:callRpc(scene.service_addr, "Scene_EnterScene", -1, {conn_id=conn_id, role_id=role_id, scene_uid=scene_uid})
    return ErrorCode.OK
end

function clsSceneCtrlService:rpcPlayerEnterScene(sender, param)
    local role_id = param.role_id
    local scene_uid = param.scene_uid
    local scene_id = param.scene_id
    self._player_mgr:switch_state(role_id, PlayerState.IN_SCENE, {scene_uid=scene_uid, scene_id=scene_id})
end
