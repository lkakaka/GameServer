TableUtil = {}

function TableUtil.isEmpty(tab)
    if next(tab) == nil then
        return true
    end
    return false
end

function TableUtil.size(tab)
    local count = 0
    for _,v in pairs(tab) do
        count = count + 1
    end
    return count
end

function TableUtil.isInArray(arr, val)
    for _,v in ipairs(arr) do
        if val == v then
            return true
        end
    end
    return false
end
