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

clsGMHandler._gm_cmd = {
    ["mem_usage"] = clsGMHandler._gm_mem_usage,
    ["mem_gc"] = clsGMHandler._gm_mem_gc,
    ["add_item"] = clsGMHandler._gm_add_item,
    ["dump_item"] = clsGMHandler._gm_dump_item,
    ["hotfix"] = clsGMHandler._gm_hotfix,
    ["goto_scene"] = clsGMHandler._gm_goto_scene,
}
