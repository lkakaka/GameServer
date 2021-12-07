require("math")

RandUtil = {}

function RandUtil.rand_in_range(min, max)
    math.random(min, max)
end

function RandUtil.get_random_one(tbl)
    local keys = {}
    for k, _ in pairs(tbl) do
        table.insert(keys, k)
    end
    local r = math.random(1, #keys)
    return tbl[keys[r]]
end
