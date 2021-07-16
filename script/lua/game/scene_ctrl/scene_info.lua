
require("base.object")
require("util.logger")

clsSceneInfo = clsObject:Inherit("clsSceneInfo")

clsSceneInfo.index_sceneUid = "sceneUid"
clsSceneInfo.index_sceneId = "sceneId"

function clsSceneInfo:__init__(sceneUid, sceneId, serviceAddr)
    self.sceneUid = sceneUid
    self.sceneId = sceneId
    self.serviceAddr = serviceAddr
end
