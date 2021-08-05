require("base.object")
require("util.logger")
require("util.str_util")
require("util.table_util")
require("base.hotfix")

clsGMHandler = clsObject:Inherit("clsGMHandler")

function clsGMHandler:__init__(service)
    self.service = service
end

-- params = {
--     role_id = 0,
--     player = nil,
--     args = nil,
-- }
function clsGMHandler:handle_gm_cmd(cmd, param)
    local func = clsGMHandler._gm_cmd[cmd]
    if func == nil then
        logger.logError("not found gm handler func, cmd:%s", cmd)
        return "not define cmd"
    end
    return func(self, param)
end

function clsGMHandler:_gm_add_item(param)
    local arg_list = StrUtil.split(param.args, ",")
    if TableUtil.size(arg_list) < 2 then
        return "arg num error"
    end
    local item_id = tonumber(arg_list[0])
    local item_count = tonumber(arg_list[1])
    param.player._item_mgr:add_item({{item_id, item_count},})
    return "ok"
end

function clsGMHandler:_gm_dump_item(param)
    return param.player._item_mgr:dump_items()
end

function clsGMHandler:_gm_hotfix(param)
    if param.args == nil or param.args == "" then
        Hotfix.hotfix()
        return "ok"
    end
    local arg_list = StrUtil.split(param.args, ",")
    local service_group = tonumber(arg_list[1])
    local service_type = tonumber(arg_list[2])
    local service_id = tonumber(arg_list[3])
    self.service:callRpc({serviceGroup=service_group, serviceType=service_type, serviceId=service_id}, "RpcHotfix", -1)
    return "ok"
end

function clsGMHandler:_gm_goto_scene(param)
    local scene_id = tonumber(param.args)
    if scene_id == nil then
        return "error arg"
    end
    param.player:try_switch_scene(scene_id)
    return "ok"
end

clsGMHandler._gm_cmd = {
    ["add_item"] = clsGMHandler._gm_add_item,
    ["dump_item"] = clsGMHandler._gm_dump_item,
    ["hotfix"] = clsGMHandler._gm_hotfix,
    ["goto_scene"] = clsGMHandler._gm_goto_scene,
}
