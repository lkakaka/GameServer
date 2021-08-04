
require("io")
require("util.file_util")
require("util.crypt")
require("util.logger")

local script_path = "../script/lua"

Hotfix = {}

Hotfix.file_md5 = {}

function Hotfix.file_to_mod(file_name)
    local mod_name = string.sub(file_name, string.len(script_path) + 2, -5)
    return string.gsub(mod_name, "/", ".")
end

function Hotfix.mod_to_file(mod_name)
    local file_name = string.gsub(mod_name, ".", "/")
    return script_path .. "/" .. file_name .. ".lua"
end

function Hotfix.hotfix()
    local function cb(file_name)
        if not string.match(file_name, "%.lua$") then return end
        local md5_str = FileUtil.calc_file_md5(file_name)
        local mod_name = Hotfix.file_to_mod(file_name)
        if Hotfix.file_md5[mod_name] ~= md5_str then
            Hotfix.hotfix_mod(mod_name, md5_str)
        end
    end

    FileUtil.for_each_file(script_path, cb)
end

function Hotfix.hotfix_mod(mod_name, md5)
    package.loaded[mod_name] = nil
    require(mod_name)
    if md5 == nil then
        local file_name = Hotfix.mod_to_file(mod_name)
        md5 = FileUtil.calc_file_md5(file_name)
    end
    Hotfix.file_md5[mod_name] = md5
    logger.logInfo("hotfix %s", mod_name)
end

function Hotfix.init()
    local function cb(file_name)
        if not string.match(file_name, "%.lua$") then return end
        local mod_name = Hotfix.file_to_mod(file_name)
        local md5_str = FileUtil.calc_file_md5(file_name)
        Hotfix.file_md5[mod_name] = md5_str
    end

    FileUtil.for_each_file(script_path, cb)
end

Hotfix.init()