
require("util.str_util")
require("util.table_util")
require("base.hotfix")

cmdLine = {}

function cmdLine.on_cmd(args)
    print("on_cmd", args)
    local cmd_arr = StrUtil.split(args, " ")
    if TableUtil.isEmpty(cmd_arr) then return end
    local cmd = cmd_arr[1]
    if cmd == "hotfix" then
        Hotfix.hotfix()
    else
        print("unknown cmd:", cmd)
    end
end
