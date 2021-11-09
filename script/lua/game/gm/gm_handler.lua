require("base.object")
require("util.logger")
require("util.str_util")
require("util.table_util")
require("base.hotfix")
require("base.mem")
require("util.const")

clsGMHandler = clsObject:Inherit("clsGMHandler")

function clsGMHandler:__init__(service)
    self.service = service
end

-- param = {
--     role_id = 0,
--     player = nil,
--     args = nil,
-- }
function clsGMHandler:handle_gm_cmd(cmd, param)
    local func = clsGMHandler._gm_cmd[cmd]
    if func == nil then
        logger.logError("not found gm handler func, cmd:%s", cmd)
        return ErrorCode.ILLEGAL_PARAM, "not define cmd"
    end
    logger.logInfo("recv gm cmd:%s", cmd)
    return func(self, param)
end

function clsGMHandler:_gm_add_item(param)
    local arg_list = StrUtil.split(param.args, ",")
    if TableUtil.size(arg_list) < 2 then
        return ErrorCode.ILLEGAL_PARAM, "arg num error"
    end
    local item_id = tonumber(arg_list[0])
    local item_count = tonumber(arg_list[1])
    param.player._item_mgr:add_item({{item_id, item_count},})
    return ErrorCode.OK, "ok"
end

function clsGMHandler:_gm_dump_item(param)
    return param.player._item_mgr:dump_items()
end

function clsGMHandler:_gm_hotfix(param)
    if param.args == nil or param.args == "" then
        Hotfix.hotfix()
        return ErrorCode.OK, "ok"
    end
    local arg_list = StrUtil.split(param.args, ",")
    local service_group = tonumber(arg_list[1])
    local service_type = tonumber(arg_list[2])
    local service_id = tonumber(arg_list[3])
    local params = {
        cmd = "hotfix",
    }
    self.service:callRpc({serviceGroup=service_group, serviceType=service_type, serviceId=service_id}, "RpcGMCmd", -1, params)
    return ErrorCode.OK, "ok"
end

function clsGMHandler:_gm_goto_scene(param)
    local scene_id = tonumber(param.args)
    if scene_id == nil then
        return ErrorCode.ILLEGAL_PARAM, "error arg"
    end
    param.player:try_switch_scene(scene_id)
    return ErrorCode.OK, "ok"
end

function clsGMHandler:_gm_mem_usage(param)
    if param.args == nil or param.args == "" then
        local msg = Mem.usage()
        return ErrorCode.OK, msg
    end
    local arg_list = StrUtil.split(param.args, ",")
    local service_group = tonumber(arg_list[1])
    local service_type = tonumber(arg_list[2])
    local service_id = tonumber(arg_list[3])
    local params = {
        cmd = "mem_usage",
    }
    local future = self.service:callRpc({serviceGroup=service_group, serviceType=service_type, serviceId=service_id}, "RpcGMCmd", -1, params)
    local function _callback(err_code, result)
        if err_code == ErrorCode.OK and param.player ~= nil then
            param.player:sendGMCmdRsp("mem_usage", result)
        end
    end
    future:regCallback(_callback)
    return ErrorCode.GM_CMD_RSP_DELAY, "ok"
end

function clsGMHandler:_gm_mem_gc(param)
    if param.args == nil or param.args == "" then
        local msg = Mem.gc()
        return ErrorCode.OK, msg
    end
    local arg_list = StrUtil.split(param.args, ",")
    local service_group = tonumber(arg_list[1])
    local service_type = tonumber(arg_list[2])
    local service_id = tonumber(arg_list[3])
    local params = {
        cmd = "mem_gc",
    }
    local future = self.service:callRpc({serviceGroup=service_group, serviceType=service_type, serviceId=service_id}, "RpcGMCmd", -1, params)
    local function _callback(err_code, result)
        if err_code == ErrorCode.OK and param.player ~= nil then
            param.player:sendGMCmdRsp("mem_gc", result)
        end
    end
    future:regCallback(_callback)
    return ErrorCode.GM_CMD_RSP_DELAY, "ok"
end

function clsGMHandler:_gm_create_npc(param)
    if param.args == nil or param.args == "" then
        return ErrorCode.ILLEGAL_PARAM, "error arg"
    end
    local arg_list = StrUtil.split(param.args, ",")
    local npc_id = tonumber(arg_list[1])
    local x, y = param.player.engineObj.x, param.player.engineObj.y
    if arg_list[2] ~= nil and arg_list[3] ~= nil then
        x = tonumber(arg_list[2])
        y = tonumber(arg_list[3])
    end

    if x == nil or y == nil then
        return ErrorCode.ILLEGAL_PARAM, "pos invalid"
    end

    for i=1,1000 do
        local npc = param.player.game_scene:create_npc(npc_id, x, y)
        npc:rand_move()
    end
    -- local npc_info = string.format( "entity_id:%d, npc_id:%d", npc.entity_id, npc_id)
    local npc_info = "ok"
    return ErrorCode.OK, npc_info
end

function clsGMHandler:_gm_remove_npc(param)
    if param.args == nil or param.args == "" then
        return ErrorCode.ILLEGAL_PARAM, "error arg"
    end

    local arg_list = StrUtil.split(param.args, ",")
    local npc_uid = tonumber(arg_list[1])
    param.player.game_scene:remove_npc(npc_uid)
    return ErrorCode.OK, "ok"
end

clsGMHandler._gm_cmd = {
    ["mem_usage"] = clsGMHandler._gm_mem_usage,
    ["mem_gc"] = clsGMHandler._gm_mem_gc,
    ["add_item"] = clsGMHandler._gm_add_item,
    ["dump_item"] = clsGMHandler._gm_dump_item,
    ["hotfix"] = clsGMHandler._gm_hotfix,
    ["goto_scene"] = clsGMHandler._gm_goto_scene,
    ["cnpc"] = clsGMHandler._gm_create_npc,
    ["rnpc"] = clsGMHandler._gm_remove_npc,
}
