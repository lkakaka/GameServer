require("service.login_service")

-- obj = clsLoginService:New()

function test(a, b)
    print(a, b)
end

a = {
    ["test"] = test,
}

a.test(100, 100)


arr = {}
arr[#arr+1] = 3
for k,v in ipairs(arr) do
    print(k, v)
end


function log(fmt, ...)
    local s = string.format(fmt, ...)
    print(s)
    -- local arg={...}
    -- for k, v in ipairs(arg) do
    --     string.format(fmt, unpack(arg) )
    -- end
end

log("hello,%d,%s", 1, "test")

-- a = {
--     b = 1,
--     c = 2,
-- }

-- a.b = 2
-- a[1] = 3
-- for k, v in pairs(a) do
--     print(k, v)
-- end
