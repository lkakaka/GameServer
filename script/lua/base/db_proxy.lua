require("base.object")
require("base.service_type")

clsDBProxy = clsObject:Inherit("clsDBProxy")

function clsDBProxy:__init__(service)
    self.service = service
end


-- args: table类型 条件参数, {role_id=1}
function clsDBProxy:load(server_id, table_name, args)
    local tbl = { table_name = table_name, row = args }
    local db_addr = LOCAL_SERVICE_DB
    if server_id > 0 then db_addr = make_db_addr(server_id) end
    return self.service:callRpc(db_addr, "RpcLoad", -1, tbl)
end

-- tbls = {
--     {table_name = "", row = {}},
--     {table_name = "", row = {}},
-- }
function clsDBProxy:load_multi(server_id, tbls)
    local db_addr = LOCAL_SERVICE_DB
    if server_id > 0 then db_addr = make_db_addr(server_id) end
    return self.service:callRpc(db_addr, "RpcLoadMulti", -1, tbls)
end


function clsDBProxy:insert_one(server_id, table_name, row)
    local tbl = { table_name = table_name, rows = {row} }
    local db_addr = LOCAL_SERVICE_DB
    if server_id > 0 then db_addr = make_db_addr(server_id) end
    return self.service:callRpc(db_addr, "RpcInsert", -1, tbl)
end

-- tbl = {
--     table_name = "",
--     rows = { row, row }
-- }
function clsDBProxy:insert(server_id, table_name, rows)
    local tbl = { table_name = table_name, rows = rows }
    local db_addr = LOCAL_SERVICE_DB
    if server_id > 0 then db_addr = make_db_addr(server_id) end
    return self.service:callRpc(db_addr, "RpcInsert", -1, tbl)
end

-- tbl = {
--     table_name = "",
--     rows = { row, row }
-- }
function clsDBProxy:update_one(server_id, table_name, row)
    local tbl = { table_name = table_name, rows = {row} }
    local db_addr = LOCAL_SERVICE_DB
    if server_id > 0 then db_addr = make_db_addr(server_id) end
    return self.service:callRpc(db_addr, "RpcUpdate", -1, tbl)
end


function clsDBProxy:update(server_id, table_name, rows)
    local tbl = { table_name = table_name, rows = rows }
    local db_addr = LOCAL_SERVICE_DB
    if server_id > 0 then db_addr = make_db_addr(server_id) end
    return self.service:callRpc(db_addr, "RpcUpdate", -1, tbl)
end

-- tbl = {
--     table_name = "",
--     rows = { row, row }
-- }
function clsDBProxy:delete_one(server_id, table_name, row)
    local tbl = { table_name = table_name, rows = {row} }
    local db_addr = LOCAL_SERVICE_DB
    if server_id > 0 then db_addr = make_db_addr(server_id) end
    return self.service:callRpc(db_addr, "RpcDelete", -1, tbl)
end

function clsDBProxy:delete(server_id, table_name, rows)
    local tbl = { table_name = table_name, rows = rows }
    local db_addr = LOCAL_SERVICE_DB
    if server_id > 0 then db_addr = make_db_addr(server_id) end
    return self.service:callRpc(db_addr, "RpcDelete", -1, tbl)
end

