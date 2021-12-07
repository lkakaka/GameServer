require("service.service_base")
require("util.logger")
require("util.const")
require("util.rand_util")
require("game.scene_ctrl.scene_mgr")
require("game.scene_ctrl.player_mgr")
require("util.multi_index_container")

clsSceneCtrlService = clsServiceBase:Inherit("clsSceneCtrlService")

function clsSceneCtrlService:__init__()
    Super(clsSceneCtrlService).__init__(self)

    self._scene_mgr = clsSceneMgr:New(self)
    self._player_mgr = clsSceneCtrlPlayerMgr:New()
    self._gateway_addresses = {}
    self._cross_server_roles = {}

    self._local_cross_player_info = {} -- 本地服记录跨服的玩家信息

    self:initRpcHandlder()
    logger.logInfo("clsSceneCtrlService __init__")
end

function clsSceneCtrlService:initRpcHandlder()
    self:reg_rpc_handler("RegGatewayAddress", self.rpc_reg_gateway_address)
    self:reg_rpc_handler("RegScene", self.rpc_reg_scene)
    self:reg_rpc_handler("UnRegScene", self.rpc_unreg_scene)
    self:reg_rpc_handler("Player_EnterGame", self.rpc_enter_game)
    self:reg_rpc_handler("Player_LeaveGame", self.rpc_leave_game)

    self:reg_rpc_handler("Player_SwitchSceneReq", self.rpc_switch_scene_req)
    self:reg_rpc_handler("Player_SwitchScene", self.rpc_switch_scene)
    self:reg_rpc_handler("Player_EnterScene", self.rpc_enter_scene)
    self:reg_rpc_handler("Player_SwitchToRemoteScene", self.rpc_switch_to_remote_scene)

    self:reg_rpc_handler("Player_RemoteSwitchSceneReq", self.rpc_remote_switch_scene_req)
    self:reg_rpc_handler("Player_RemoteSwitchScene", self.rpc_remote_switch_scene)
    self:reg_rpc_handler("Player_RemoteEnterGame", self.rpc_remote_enter_game)
    
end

function clsSceneCtrlService:rpc_reg_gateway_address(sender, param)
    self._gateway_addresses[sender.serviceId] = param
    logger.logInfo("rpc_RegGatewayAddress, %s, %s", StrUtil.tableToStr(param), StrUtil.tableToStr(self._gateway_addresses))
    return ErrorCode.OK
end

function clsSceneCtrlService:rpc_reg_scene(sender, param)
    self._scene_mgr:reg_scene(param.scene_id, param.scene_uid, sender)
    logger.logInfo("rpc_RegScene, %s", StrUtil.tableToStr(param))
    return ErrorCode.OK
end

function clsSceneCtrlService:rpc_unreg_scene(sender, param)
    self._scene_mgr:unreg_scene(param.scene_uid)
    logger.logInfo("rpc_UnRegScene, %s", StrUtil.tableToStr(param))
    return ErrorCode.OK
end

function clsSceneCtrlService:rpc_enter_game(sender, param)
    logger.logInfo("player enter game, %s", StrUtil.tableToStr(param))
    local conn_id = param.conn_id
    local role_id = param.role_id
    local scene_id = param.scene_id or 1

    local player_info = self._player_mgr:get_player_info_by_role_id(role_id)
    -- 玩家在场景中
    if player_info ~= nil then
        if player_info.state == PlayerState.IN_SCENE then
            logger.logInfo("player already entered game, role_id:%d, scene_id:%d, scene_uid:%d", role_id, scene_id, player_info.scene_uid)
            player_info:change_conn_id(conn_id)
            local scene = self._scene_mgr:get_scene_by_uid(player_info.scene_uid)
            self:callRpc(scene.service_addr, "Scene_EnterScene", -1, {conn_id=conn_id, role_id=role_id, scene_uid=player_info.scene_uid})
            return ErrorCode.OK
        end
        logger.logError("player status error, role_id:%d, state:%d", role_id, player_info.state)
        return ErrorCode.SYS_ERROR
    end

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

function clsSceneCtrlService:rpc_leave_game(sender, param)
    self._player_mgr:remove_player(param.role_id)
    logger.logInfo("player leave game, role_id:%d", param.role_id)
end

function clsSceneCtrlService:rpc_switch_scene_req(sender, param)
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

function clsSceneCtrlService:rpc_switch_scene(sender, param)
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

function clsSceneCtrlService:rpc_enter_scene(sender, param)
    local role_id = param.role_id
    local scene_uid = param.scene_uid
    local scene_id = param.scene_id
    self._player_mgr:switch_state(role_id, PlayerState.IN_SCENE, {scene_uid=scene_uid, scene_id=scene_id})
end

function clsSceneCtrlService:rpc_switch_to_remote_scene(sender, param)
    local remote_server_id = param.server_id
    local role_id = param.role_id
    local scene_uid = param.scene_uid
    self._local_cross_player_info[role_id] = {
        remote_server_id = remote_server_id,
        scene_uid = scene_uid,
    }
    -- self._player_mgr:switch_state(role_id, PlayerState.REMOTE_SWITCHING, {scene_uid=scene_uid, scene_id=scene_id})
    logger.logInfo("player switch to remote scene, role_id:%d, remote_server_id:%d scene_uid:%d", role_id, remote_server_id, scene_uid)
end

-- 跨服请求切换场景
function clsSceneCtrlService:rpc_remote_switch_scene_req(sender, param)
    local role_id = param.role_id
    local scene_id = param.scene_id
    local scene = self._scene_mgr:get_min_player_scene(scene_id)
    if scene == nil then
        logger.logInfo("player switch scene req failed, not found scene, scene_id:%d, role_id:%d", scene_id, role_id)
        return ErrorCode.NOT_FOUND_SCENE, -1
    end
    local rand_num = math.random(1, 10000)
    local str_token = TokenPrefix.CROSS_SERVER .. tostring(role_id) .. tostring(os.time()) .. tostring(rand_num)
    local token_valid_ts = os.time() + 30
    self._cross_server_roles[role_id] = { token = str_token, token_ts = token_valid_ts, scene_uid = scene.scene_uid}
    local gateway_addr = RandUtil.get_random_one(self._gateway_addresses)
    logger.logInfo("player switch scene req, role_id:%d, scene_id:%d, scene_uid:%d, gateway_addr:%s", 
                    role_id, scene_id, scene.scene_uid, StrUtil.tableToStr(gateway_addr))

    local future = self:callRpc(LOCAL_SERVICE_LOGIN, "regRemoteRole", -1, { role_id=role_id, token=str_token, token_ts = token_valid_ts})
    future:regCallback(function(err_code, result) 
        if err_code == ErrorCode.OK then return end
        logger.logError("reg remote role to login failed, role_id:%d", role_id)
    end)

    return ErrorCode.OK, {server_id=SERVER_GROUP_ID, scene_uid=scene.scene_uid, gateway_addr=gateway_addr, token=str_token}
end

function clsSceneCtrlService:rpc_remote_switch_scene(sender, param)
    local role_id = param.role_id
    local scene_uid = param.scene_uid
    local scene = self._scene_mgr:get_scene_by_uid(scene_uid)
    if scene == nil then
        logger.logInfo("player remote switch scene failed, not found scene, scene_uid:%d, role_id:%d", scene_uid,
                        role_id)
        return ErrorCode.NOT_FOUND_SCENE
    end
    self._player_mgr:switch_state(role_id, PlayerState.REMOTE_SWITCHING, {scene_uid=0, scene_id=0})
    logger.logInfo("player remote switch scene, role_id:%d, scene_uid:%d", role_id, scene_uid)
    -- self:callRpc(scene.service_addr, "Scene_EnterScene", -1, {conn_id=conn_id, role_id=role_id, scene_uid=scene_uid})
    return ErrorCode.OK
end

function clsSceneCtrlService:rpc_remote_enter_game(sender, param)
    logger.logInfo("player remote enter game, %s", StrUtil.tableToStr(param))
    local conn_id = param.conn_id
    local role_id = param.role_id
    local scene_id = param.scene_id or 1

    local player_info = self._player_mgr:get_player_info_by_role_id(role_id)
    -- 玩家在场景中
    if player_info ~= nil then
        if player_info.state == PlayerState.IN_SCENE then
            logger.logInfo("remote player already entered game, role_id:%d, scene_id:%d, scene_uid:%d", role_id, scene_id, player_info.scene_uid)
            player_info:change_conn_id(conn_id)
            local scene = self._scene_mgr:get_scene_by_uid(player_info.scene_uid)
            self:callRpc(scene.service_addr, "Scene_EnterScene", -1, {conn_id=conn_id, role_id=role_id, scene_uid=player_info.scene_uid})
            return ErrorCode.OK
        end
        logger.logError("remote player status error, role_id:%d, state:%d", role_id, player_info.state)
        return ErrorCode.SYS_ERROR
    end

    local cross_info = self._cross_server_roles[role_id]
    if cross_info == nil then
        logger.logInfo("player remote enter game failed, not found cross server token, scene_id:%d, role_id:%d", scene_id, role_id)
        return ErrorCode.NOT_FOUND_SCENE
    end

    self._cross_server_roles[role_id] = nil

    local scene = self._scene_mgr:get_scene_by_uid(cross_info.scene_uid)

    local player_info = self._player_mgr:add_player(role_id, conn_id)
    player_info.state = PlayerState.LOGINING
    logger.logInfo("player remote enter game, role_id:%d, scene_id:%d, scene_uid:%d", role_id, scene_id, scene.scene_uid)
    self:callRpc(scene.service_addr, "Scene_EnterScene", -1, {conn_id=conn_id, role_id=role_id, scene_uid=scene.scene_uid})
    return ErrorCode.OK
end
