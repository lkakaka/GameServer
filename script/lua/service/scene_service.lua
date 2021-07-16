require("service.service_base")
require("util.timer")
require("util.logger")
require("game.scene.game_scene")
require("base.service_type")

clsSceneService = clsServiceBase:Inherit("clsSceneService")

function clsSceneService:__init__()
    Super(clsSceneService).__init__(self)
    self._scene = {}
    timer.addTimer(3, 1, function(timerId) self:initScene() end)
    print("clsSceneService:__init__")
end

function clsSceneService:initScene()
    print("clsSceneService:initScene")
    local gameScene = clsGameScene:New(1)
    self._scene[gameScene.sceneUid] = gameScene
    local sceneInfo = { sceneId = gameScene.sceneId, sceneUid = gameScene.sceneUid }
    local future = self:callRpc(LOCAL_SERVICE_SCENE_CTRL, "RegScene", 10, sceneInfo)
    future:regCallback(function(result) self:onRegSceneResp(result, sceneInfo) end)
    logger.logInfo("initScene")
end

function clsSceneService:onRegSceneResp(result, sceneInfo)
    if result.errCode == ErrorCode.OK then
        logger.logInfo("reg scene sucess, scene_id:%d, sceneUid:%d", sceneInfo.sceneId, sceneInfo.sceneUid)
    else
        print(sceneInfo.scene_id, sceneInfo.scene_uid)
        logger.logError("reg scene failed, errCode:%d, sceneId:%d, sceneUid:%d", result.errCode, sceneInfo.sceneId, sceneInfo.sceneUid)
    end
end