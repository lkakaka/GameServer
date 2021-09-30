
require("math")
require("util.timer")

clsGameNpc = clsGameActor:Inherit("clsGameNpc")

function clsGameNpc:__init__(npc_id, game_scene, engine_obj)
    Super(clsGameNpc).__init__(self, game_scene, engine_obj:getActorId())
    self.npc_id = npc_id
    self.engine_obj = engine_obj
    self.rand_move_timer_id = -1
end

function clsGameNpc:is_npc()
    return true
end

function clsGameNpc:pack_born_info(msg)
    msg.npc_list = msg.npc_list or {}
    local npc_info = {}
    npc_info.actor_id = self.actor_id
    npc_info.npc_id = self.npc_id
    table.insert(msg.npc_list, npc_info)
end


function clsGameNpc:on_actor_enter_sight(actor)
end

function clsGameNpc:on_actor_leave_sight(actor)
end

function clsGameNpc:on_leave_scene()
    if self.rand_move_timer_id > 0 then
        timer.remove_timer(self.rand_move_timer_id)
    end
end

function clsGameNpc:move_to(x, y)
    self.engine_obj:moveTo(x, y)
end

function clsGameNpc:rand_move()
    if self.engine_obj.move_speed <= 0 then self.engine_obj.move_speed = 5 end
    local cur_x, cur_y = self.engine_obj.x, self.engine_obj.y
    local x = math.random(20)
    local y = math.random(20)
    self:move_to(x, y)
    local move_time = math.sqrt((x - cur_x) * (x - cur_x) + (y - cur_y) * (y - cur_y)) / self.engine_obj.move_speed
    self.rand_move_timer_id = timer.add_timer(move_time, 1, function(timer_id) self:rand_move() end)
    print("move time", move_time)
end

