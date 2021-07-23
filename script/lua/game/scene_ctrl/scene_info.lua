
require("base.object")
require("util.logger")

clsSceneInfo = clsObject:Inherit("clsSceneInfo")

clsSceneInfo.index_sceneUid = "scene_uid"
clsSceneInfo.index_sceneId = "scene_id"

function clsSceneInfo:__init__(scene_uid, scene_id, service_addr)
    self.scene_uid = scene_uid
    self.scene_id = scene_id
    self.service_addr = service_addr
    self.player_list = {}
end
