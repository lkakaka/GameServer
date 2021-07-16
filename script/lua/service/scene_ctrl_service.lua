require("service.service_base")
require("util.logger")
require("util.const")
require("game.scene_ctrl.scene_info")
require("util.multi_index_container")

clsSceneCtrlService = clsServiceBase:Inherit("clsSceneCtrlService")

function clsSceneCtrlService:__init__()
    Super(clsSceneCtrlService).__init__(self)
    self:initRpcHandlder()
    -- print("clsSceneCtrlService:__init__")
    self.sceneContainer = clsMultIndexContainer:New({clsSceneInfo.index_sceneUid, clsSceneInfo.index_sceneId})
end

function clsSceneCtrlService:initRpcHandlder()
    self:regRpcHandler("RegScene", function(sender, param) self:rpc_RegScene(sender, param) end)
    self:regRpcHandler("UnRegScene", function(sender, param) self:rpc_UnRegScene(sender, param) end)
end

function clsSceneCtrlService:rpc_RegScene(sender, param)
    if self.sceneContainer:getOneElem(clsSceneInfo.index_sceneUid, param.sceneUid) ~= nil then
        logger.logError("rpc_RegScene error, scene exist!!  %s", StrUtil.tableToStr(param))
        return ErrorCode.ILLEGAL_PARAM
    end
    logger.logInfo("rpc_RegScene, %s", StrUtil.tableToStr(param))
    local sceneInfo = clsSceneInfo:New(param.sceneUid, param.sceneId, sender)
    self.sceneContainer:addElem(sceneInfo)
    return ErrorCode.OK
end

function clsSceneCtrlService:rpc_UnRegScene(sender, param)
    logger.logInfo("rpc_UnRegScene, %s", StrUtil.tableToStr(param))
    local sceneInfo = self.sceneContainer:getOneElem(clsSceneInfo.index_sceneUid, param.sceneUid)
    if sceneInfo == nil then
        logger.logError("rpc_UnRegScene error, not found scene, %s", StrUtil.tableToStr(param))
        return ErrorCode.ILLEGAL_PARAM
    end
    self.sceneContainer:removeElem(sceneInfo)
    return ErrorCode.OK
end
