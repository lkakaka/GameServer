require("base.object")
require("util.logger")
require("game.scene_ctrl.scene_info")
require("util.multi_index_container")

clsSceneMgr = clsObject:Inherit("clsSceneMgr")

function clsSceneMgr:__init__(service)
    self._service = service
    self._players = {}  -- {role_id: scene_uid}
    self.sceneContainer = clsMultIndexContainer:New({clsSceneInfo.index_sceneUid, clsSceneInfo.index_sceneId})
end

function clsSceneMgr:get_scene_by_uid(scene_uid)
    return self.sceneContainer:getOneElem(clsSceneInfo.index_sceneUid, scene_uid)
end

function clsSceneMgr:get_scenes_by_id(scene_id)
    return self.sceneContainer:getElems(clsSceneInfo.index_sceneId, scene_id)
end

function clsSceneMgr:reg_scene(scene_id, scene_uid, service_addr)
    local scene = self:get_scene_by_uid(scene_uid)
    if scene ~= nil then
        logger.logError("scene uid has exist, scene_id:%d, scene_uid:%d", scene_id, scene_uid)
        self.sceneContainer:removeElem(scene)
    end
    scene = clsSceneInfo:New(scene_uid, scene_id, service_addr)
    self.sceneContainer:addElem(scene)
    logger.logInfo("reg scene, scene_id:%d, scene_uid:%d", scene_id, scene_uid)
end

function clsSceneMgr:unreg_scene(scene_uid)
    local scene = self:get_scene_by_uid(scene_uid)
    if scene == nil then
        logger.logError("unreg scene not found, scene_uid:%d", scene_uid)
        return
    end
    self.sceneContainer:removeElem(scene)
    logger.logInfo("unreg scene, scene_id:%d, scene_uid:%d", scene.scene_id, scene_uid)
end

function clsSceneMgr:get_min_player_scene(scene_id)
    local scenes = self:get_scenes_by_id(scene_id)
    if scenes == nil then return end
    local min_count = 0x0FFFFFFF
    local tgt_scene = nil
    for _,scene in pairs(scenes) do
        local scene_player_count = TableUtil.size(scene.player_list)
        if min_count > scene_player_count then
            min_count = scene_player_count
            tgt_scene = scene
        end
    end
    return tgt_scene
end

function clsSceneMgr:reg_player_to_scene(role_id, scene_uid)
    local scene = self.get_scene_by_uid(scene_uid)
    if scene == nil then
        logger.logError("reg player to scene not found, role_id:%d, scene_uid:%d", role_id, scene_uid)
        return
    end
    scene.player_list[role_id] = true
    self._players[role_id] = scene_uid
end

function clsSceneMgr:unreg_player_to_scene(role_id, scene_uid)
    local scene = self.get_scene_by_uid(scene_uid)
    if scene == nil then
        logger.logError("unreg player to scene not found, role_id:%d, scene_uid:%d", role_id, scene_uid)
        return
    end
    scene.player_list[role_id] = nil
    self._players[role_id] = nil
end
