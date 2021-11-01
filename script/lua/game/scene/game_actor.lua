require("base.object")
require("util.logger")
require("util.multi_index_elem")

clsGameActor = clsMultiIndexElem:Inherit("clsGameActor")

function clsGameActor:__init__(game_scene, actor_id, index_attr_names)
    Super(clsGameActor).__init__(self, index_attr_names)
    self.actor_id = actor_id
    self.game_scene = game_scene
end

function clsGameActor:is_player()
    return false
end

function clsGameActor:is_npc()
    return false
end

function clsGameActor:check_can_see(actor)
    return true
end
