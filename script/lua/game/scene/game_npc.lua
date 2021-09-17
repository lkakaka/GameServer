
clsGameNpc = clsGameActor:Inherit("clsGameNpc")

function clsGameNpc:__init__(game_scene, engine_obj)
    Super(clsGameNpc).__init__(self, game_scene, engine_obj:getActorId())
end

function clsGameNpc:is_npc()
    return true
end