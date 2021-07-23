
require("base.object")
require("util.logger")
require("util.multi_index_container")
require("game.scene_ctrl.player_info")

clsSceneCtrlPlayerMgr = clsObject:Inherit("clsSceneCtrlPlayerMgr")


function clsSceneCtrlPlayerMgr:__init__()
    self._mi_player_info = clsMultIndexContainer:New({clsPlayerInfo.index_roleId, clsPlayerInfo.index_connId})
end

function clsSceneCtrlPlayerMgr:add_player(role_id, conn_id)
    local player_info = clsPlayerInfo:New(role_id, conn_id)
    self._mi_player_info:addElem(player_info)
    return player_info
end

function clsSceneCtrlPlayerMgr:remove_player(role_id)
    local player_info = self:get_player_info_by_role_id(role_id)
    if player_info == nil then return end
    self._mi_player_info:removeElem(player_info)
end

function clsSceneCtrlPlayerMgr:get_player_info_by_role_id(role_id)
    return self._mi_player_info:getOneElem(clsPlayerInfo.index_roleId, role_id)
end

function clsSceneCtrlPlayerMgr:get_player_info_by_conn_id(conn_id)
    return self._mi_player_info:getOneElem(clsPlayerInfo.index_connId, conn_id)
end

function clsSceneCtrlPlayerMgr:switch_state(role_id, state, args)
    local player_info = self:get_player_info_by_role_id(role_id)
    if player_info == nil then
        logger.logError("not found player info, role_id:%d", role_id)
        return
    end
    player_info.state = state
    for _,attr_name in ipairs({"scene_uid", "scene_id"}) do
        local val = args[attr_name]
        if val ~= nil then
            player_info[attr_name] = val
        end
    end
end