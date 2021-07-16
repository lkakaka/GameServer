
StrUtil = {}

function StrUtil.toStringEx(value)
    if type(value)=='table' then
       return StrUtil.tableToStr(value)
    elseif type(value)=='string' then
        return "\'"..value.."\'"
    else
       return tostring(value)
    end
end

function StrUtil.tableToStr(t)
    if t == nil then return "" end
    local retstr= "{"

    local i = 1
    for key,value in pairs(t) do
        local signal = ","
        if i==1 then
          signal = ""
        end

        if key == i then
            retstr = retstr..signal..StrUtil.toStringEx(value)
        else
            if type(key)=='number' or type(key) == 'string' then
                retstr = retstr..signal..'['..StrUtil.toStringEx(key).."]="..StrUtil.toStringEx(value)
            else
                if type(key)=='userdata' then
                    retstr = retstr..signal.."*s"..StrUtil.tableToStr(getmetatable(key)).."*e".."="..StrUtil.toStringEx(value)
                else
                    retstr = retstr..signal..key.."="..StrUtil.toStringEx(value)
                end
            end
        end

        i = i+1
    end

    retstr = retstr.."}"
    return retstr
end

function StrUtil.strToTable(str)
    if str == nil or type(str) ~= "string" then
        return
    end
    
    return load("return " .. str)()
end

function StrUtil.split(str, reps)
    local lst = {}
    string.gsub(str,'[^'..reps..']+',function ( w )
        table.insert(lst, w)
    end)
    return lst
end


-- local tb = {
--     1,
--     2,
--     key = "ss",
--     [5] = 10
-- }

-- local str = StrUtil.tableToStr(tb)
-- print(str)

-- local tbl = StrUtil.strToTable(str)
-- for k, v in pairs(tbl) do
--     print(k, v)
-- end

