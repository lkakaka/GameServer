require("base.object")
require("util.logger")

clsGameActor = clsObject:Inherit("clsGameActor")

function clsGameActor:__init__(actor_id)
    self.actor_id = actor_id
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