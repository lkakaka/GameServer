require("base.object")
require("base.service_type")

clsDBProxy = clsObject:Inherit("clsDBProxy")

function clsDBProxy:__init__(service)
    self.service = service
end


-- args: table类型 条件参数, {role_id=1}
function clsDBProxy:load(table_name, args)
    local tbl = { table_name = table_name, row = args }
    return self.service:callRpc(LOCAL_SERVICE_DB, "RpcLoad", -1, tbl)
end

-- tbls = {
--     {table_name = "", row = {}},
--     {table_name = "", row = {}},
-- }
function clsDBProxy:loadMulti(tbls)
    return self.service:callRpc(LOCAL_SERVICE_DB, "RpcLoadMulti", -1, tbls)
end


function clsDBProxy:insertOne(table_name, row)
    local tbl = { table_name = table_name, rows = {row} }
    return self.service:callRpc(LOCAL_SERVICE_DB, "RpcInsert", -1, tbl)
end

-- tbl = {
--     table_name = "",
--     rows = { row, row }
-- }
function clsDBProxy:insert(table_name, rows)
    local tbl = { table_name = table_name, rows = rows }
    return self.service:callRpc(LOCAL_SERVICE_DB, "RpcInsert", -1, tbl)
end

-- tbl = {
--     table_name = "",
--     rows = { row, row }
-- }
function clsDBProxy:updateOne(table_name, row)
    local tbl = { table_name = table_name, rows = {row} }
    return self.service:callRpc(LOCAL_SERVICE_DB, "RpcUpdate", -1, tbl)
end


function clsDBProxy:update(table_name, rows)
    local tbl = { table_name = table_name, rows = rows }
    return self.service:callRpc(LOCAL_SERVICE_DB, "RpcUpdate", -1, tbl)
end

-- tbl = {
--     table_name = "",
--     rows = { row, row }
-- }
function clsDBProxy:deleteOne(table_name, row)
    local tbl = { table_name = table_name, rows = {row} }
    return self.service:callRpc(LOCAL_SERVICE_DB, "RpcDelete", -1, tbl)
end

function clsDBProxy:delete(table_name, rows)
    local tbl = { table_name = table_name, rows = rows }
    return self.service:callRpc(LOCAL_SERVICE_DB, "RpcDelete", -1, tbl)
end

