require("base.object")
require("util.logger")
require("util.multi_index_elem")

clsSceneEntity = clsMultiIndexElem:Inherit("clsSceneEntity")

function clsSceneEntity:__init__(game_scene, eid, index_attr_names)
    Super(clsSceneEntity).__init__(self, index_attr_names)
    self.entity_id = eid
    self.game_scene = game_scene
end

function clsSceneEntity:is_player()
    return false
end

function clsSceneEntity:is_npc()
    return false
end

function clsSceneEntity:check_can_see(entity)
    return true
end
