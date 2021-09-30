require("util.logger")
require("game.db.db_redis")
require("util.str_util")
require("game.db.tbl_defs")
require("util.table_util")
require("util.timer")

local INDEX_ALL_CACHED = "ALL_CACHED"
local INDEX_NOT_ALL_CACHED_SCORE = "0"
local INDEX_ALL_CACHED_SCORE = "1"
local INDEX_PRI_KEY_SCORE = "10"

local REDIS_FLUSH_TIME = 5 * 6

clsDBHandler = clsObject:Inherit("clsDBHandler")

function clsDBHandler:__init__(dbName, redisIp, redisPort)
    self.db_inst = LuaDB.new(dbName)
    self.redis_inst = LuaRedis.new(redisIp, redisPort)
    self:initTable(TblDefs)
    self._flush_timer_id = timer.add_timer(REDIS_FLUSH_TIME, -1, function() self:flushRedis() end)
    self:test_db_and_redis()
    self:test()
end

function clsDBHandler:initTable(tblDefs)
    logger.logInfo("*********init table, %s", StrUtil.tableToStr(tblDefs))
    if not self.db_inst:initTable(tblDefs) then
        error("init tables error")
    end
    self:flush_add_clean_redis()
    self:_init_id_allocator(tblDefs)
end

function clsDBHandler:_init_id_allocator(tblDefs)
    local redis_cmd = "HMSET _id_allocator"
    for _,tbl in pairs(tblDefs) do
        local tbl_name = tbl.tb_name
        local primary_col = TblUtil.primary_column(tbl_name)
        if primary_col ~= nil then
            --  sql = "select auto_increment from information_schema.TABLES where TABLE_NAME='{}'".format(tbl_name)
            local sql = string.format("select max(%s) as max_id from %s", primary_col.name, tbl_name)
            local res = self.db_inst:executeSql(sql)
            -- print(StrUtil.tableToStr(res))
            local max_id = res[1].max_id
            redis_cmd = redis_cmd .. string.format(" %s %d", tbl_name, max_id)
        end
    end
    if not self.redis_inst:execRedisCmd(redis_cmd) then
        error(string.format("init id allocator error, redis cmd:%s", redis_cmd))
    end
end

function clsDBHandler:flush_add_clean_redis()
    self:flushRedis()
    -- self.redis_inst:execRedisCmd("FLUSHDB")
end

function clsDBHandler:getTblNameFromKey(tblKey)
    local pos = string.find(tblKey, ":")
    return string.sub(tblKey, 0, pos-1)
end

function clsDBHandler:flushRedis()
    logger.logInfo("begin flush redis!!!!")
    local redis_cmd = "SMEMBERS _chg_keys"
    local redis_result = self.redis_inst:execRedisCmd(redis_cmd)
    if not redis_result then
        logger.logError("exec redis cmd error, %s", redis_cmd)
    end

    local flush_count = 0
    if not TableUtil.isEmpty(redis_result) then
        local all_result = {}
        local result_count = 0
        for _, redisKey in pairs(redis_result) do
            local table_name = self:getTblNameFromKey(redisKey)
            local result = self:_load_data_from_redis(table_name, redisKey)
            if result and not TableUtil.isEmpty(result.rows) then
                flush_count = flush_count + 1
                table.insert(all_result, {table_name = table_name, row = result.rows[1]})
                result_count = result_count + 1
                if result_count >= 1000 then
                    if not self:replace_rows(all_result) then
                        logger.logError("flush redis error, update mysql failed, count: %d", result_count)
                        return
                    end
                    all_result = {}
                    result_count = 0
                end
            else
                logger.logError("not found redis key %s", redisKey)
            end
        end

        if not TableUtil.isEmpty(all_result) then
            -- print("replace_rows---------", StrUtil.tableToStr(all_result))
            if not self:replace_rows(all_result) then
                logger.logError("flush redis error, update mysql failed, count: %d", result_count)
                return
            end
        end

        self.redis_inst:execRedisCmd("DEL _chg_keys")
    end
    logger.logInfo("end flush redis!!!!, flush count: %d", flush_count)
end

function clsDBHandler:_load_data_from_redis(table_name, primary_key)
    local result = self.redis_inst:execRedisCmd("HGETALL " .. primary_key)
    -- print(StrUtil.tableToStr(result))
    if not result or TableUtil.isEmpty(result) then return end
    local row = {}
    for i=1,#result,2 do
        row[result[i]] = result[i+1]
    end
    TblUtil.transRow(table_name, row)
    return {table_name = table_name, rows = {row}} 
end

function clsDBHandler:replace_rows(tbls)
    return self.db_inst:replaceRows(tbls)
end

-- tbl = {
--     table_name = "",
--     row = {
--         a = b,
--     }
-- }
function clsDBHandler:load(tbl)
    local priColName = TblUtil.getPriColName(tbl.table_name)
    local primary_val = tbl.row[priColName]
    if primary_val ~= nil then
        local redisKey = TblUtil.makeRedisPriKey(tbl.table_name, primary_val)
        return self:_load_data_by_primary_key(tbl, redisKey)
    end
    local tb_index = TblUtil.matchIndex(tbl)
    if tb_index ~= nil then
        return self:_load_data_by_index(tbl, tb_index)
    end
    logger.logWarn("load data from db, may be create index on table %s", tbl.table_name)
    return self:_load_from_db_and_cache(tbl)
end

function clsDBHandler:_load_data_by_primary_key(tbl, primary_key)
    local result = self:_load_data_from_redis(tbl.table_name, primary_key)
    -- print("_load_data_by_primary_key", result)
    if result ~= nil then
        return result
    end
    return self:_load_from_db_and_cache(tbl)
end

function clsDBHandler:_load_data_by_index(tbl, tb_index)
    local result = {table_name = tbl.table_name, rows={}}
    local key = TblUtil.make_redis_index_key(tbl.table_name, tbl.row, tb_index)
    local idx_result = self.redis_inst:execRedisCmd(string.format("ZRANGE %s 0 -1 WITHSCORES", key))
    -- print("redis result----", idx_result)
    if idx_result ~= nil and idx_result[2] == INDEX_ALL_CACHED_SCORE then    -- 已在redis中全缓存
        for i=1,#idx_result,2 do
            local pri_key = idx_result[i]
            if pri_key ~= "" then
                local res_tbl = self:_load_data_by_primary_key(tbl, pri_key)
                for _,row in ipairs(res_tbl.rows) do
                    if TblUtil.filter(row, tbl.row) then
                        table.insert(result.rows, row)
                    end
                end
            end
        end
        return result
    end
    -- print("tbl sql-===", tbl_sql)
    result = self:_load_from_db_and_cache(tbl)
    self:_cache_tbl_index_to_redis(result.table_name, result.rows, tb_index, true)
    return result
end

function clsDBHandler:_load_from_db_and_cache(tbl)
    local rows = self:get_row(tbl)
    if rows == nil then
        error("load from db error")
        return
    end
    -- print("_load_from_db_and_cache", StrUtil.tableToStr(rows))
    local result = {
        table_name = tbl.table_name,
        rows = rows,
    }
    self:_cache_to_redis(result)
    return result
end

function clsDBHandler:_cache_to_redis(tbl)
    local tbName = tbl.table_name
    local priColName = TblUtil.getPriColName(tbName)
    for _,row in ipairs(tbl.rows) do
        local primary_val = row[priColName]
        local pri_key = TblUtil.makeRedisPriKey(tbName, primary_val)
        local redis_cmd = "HSET " .. pri_key
        for _,col in ipairs(TblDefs[tbName]["columns"]) do
            local colName = col.name
            local val = row[colName]
            -- todo: 怎么存入空字符串?
            if val == "" then
                redis_cmd = redis_cmd .. string.format(' %s "%s"', colName, val)
            else
                redis_cmd = redis_cmd .. string.format(" %s %s", colName, val)
            end
        end
        local redis_result = self.redis_inst:execRedisCmd(redis_cmd)
        if redis_result == nil then
            logger.logError("exec redis cmd error, %s", redis_cmd)
        end
    end
end


function clsDBHandler:_cache_tbl_index_to_redis(table_name, rows, tb_index, is_all_cached)
    -- ZADD key score member [[score member] [score member] …]
    -- score=0,1：代表是否已经全部缓存在redis中
    -- score=10：代表redis键值(具体记录)
    if TableUtil.isEmpty(rows) then return end
    local key = TblUtil.make_redis_index_key(table_name, rows[1], tb_index)
    local redis_cmd = "EXISTS " .. key
    local hasKey = self.redis_inst:execRedisCmd(redis_cmd)
    if hasKey == 1 then   -- 索引存在
        if is_all_cached then
            redis_cmd = string.format("ZADD %s %s %s", key, INDEX_ALL_CACHED_SCORE, INDEX_ALL_CACHED)
        else
            redis_cmd = string.format("ZADD %s", key)
        end
    else
        local all_cached_score = nil
        if is_all_cached then
            all_cached_score = INDEX_ALL_CACHED_SCORE
        else
            all_cached_score = INDEX_NOT_ALL_CACHED_SCORE
        end
        redis_cmd = string.format("ZADD %s %s %s", key, all_cached_score, INDEX_ALL_CACHED)
    end
    for _,row in ipairs(rows) do
        local priColName = TblUtil.getPriColName(table_name)
        local primary_val = row[priColName]
        redis_cmd = redis_cmd .. string.format(" %s %s", INDEX_PRI_KEY_SCORE, TblUtil.makeRedisPriKey(table_name, primary_val))
    end
    local redis_result = self.redis_inst:execRedisCmd(redis_cmd)
    if redis_result == nil then
        logger.logError("exec redis cmd error, %s", redis_cmd)
    end
end

function clsDBHandler:get_row(tbl)
    return self.db_inst:getRow(tbl)
end

function clsDBHandler:_add_changed_tbls(tbl)
    local priColName = TblUtil.getPriColName(tbl.table_name)
    local redis_cmd = "SADD _chg_keys"
    for _,row in ipairs(tbl.rows) do
        local primary_val = row[priColName]
        redis_cmd = redis_cmd ..  " " ..  TblUtil.makeRedisPriKey(tbl.table_name, primary_val)
    end
    local redis_result = self.redis_inst:execRedisCmd(redis_cmd)
    if redis_result == nil then
        logger.logError("exec redis cmd error, %s", redis_cmd)
    end
end

function clsDBHandler:_remove_changed_tbls(tbl)
    local priColName = TblUtil.getPriColName(tbl.table_name)
    local redis_cmd = "SREM _chg_keys"
    for _,row in ipairs(tbl.rows) do
        local primary_val = row[priColName]
        redis_cmd = redis_cmd .. " " .. TblUtil.makeRedisPriKey(tbl.table_name, primary_val)
    end
    local redis_result = self.redis_inst:execRedisCmd(redis_cmd)
    if redis_result == nil then
        logger.logError("exec redis cmd error, %s", redis_cmd)
    end
end

-- tbl = {
--     table_name = "",
--     rows = {
--         {},
--     }
-- }
function clsDBHandler:insert_table(tbl)
    local tbName = tbl.table_name
    local priColName = TblUtil.getPriColName(tbName)
    local indexs = TblUtil.all_index(tbName)
    local index_dict = {}
    for _,row in ipairs(tbl.rows) do
        local primary_val = row[priColName]
        if primary_val ~= nil then
            local pri_key = TblUtil.makeRedisPriKey(tbName, primary_val)
            local _row = { table_name = tbName, row = row }
            local res = self:_load_data_by_primary_key(_row, pri_key)
            if res ~= nil and not TableUtil.isEmpty(res.rows) then
                logger.logError("insert error, key %s has exist", pri_key)
                return false
            end
        end
        TblUtil.init_with_default(tbName, row)
        for _,tb_index in ipairs(indexs) do
            if index_dict[tb_index] == nil then index_dict[tb_index] = {} end
            table.insert(index_dict[tb_index], row)
        end
    end
    self:_cache_to_redis(tbl)
    for tb_index, rows in pairs(index_dict) do
        self:_cache_tbl_index_to_redis(tbName, rows, tb_index, false)
    end
    self:_add_changed_tbls(tbl)
    return true
end

-- tbl = {
--     table_name = "",
--     rows = {}
-- }
function clsDBHandler:update_table(tbl)
    local old_index = {}
    local new_index = {}
    local tbName = tbl.table_name
    local priColName = TblUtil.getPriColName(tbName)
    local allCols = TblUtil.all_column(tbName)
    for _,row in ipairs(tbl.rows) do
        local primary_val = row[priColName]
        if primary_val == nil then
            logger.logError("update table failed, not set primary key val, table:%s", tbName)
            return false
        end
        local pri_key = TblUtil.makeRedisPriKey(tbName, primary_val)
        local _tbl = {table_name = tbl.table_name, row = row}
        local res = self:_load_data_by_primary_key(_tbl, pri_key)
        local redis_cmd = "HSET " .. pri_key
        local old_row = nil
        if res ~= nil and not TableUtil.isEmpty(res.rows) then
            old_row = res.rows[1]
        end

        local has_change = false
        for _,col in ipairs(allCols) do
            local col_name = col.name
            if row[col_name] ~= nil then
                if old_row == nil or row[col_name] ~= old_row[col_name] then
                    has_change = true
                    redis_cmd = redis_cmd .. string.format(" %s %s", col_name, row[col_name])
                    -- 更新索引
                    local indexs = TblUtil.get_indexs_by_col(tbName, col_name)
                    for _,tb_index in ipairs(indexs) do
                        if old_row ~= nil then
                            if old_index[tb_index] == nil then old_index[tb_index] = {} end
                            if not TableUtil.isInArray(old_index[tb_index], old_row) then
                                table.insert(old_index[tb_index], old_row)
                            end
                        end

                        if new_index[tb_index] == nil then new_index[tb_index] = {} end
                        if not TableUtil.isInArray(new_index[tb_index], row) then
                            table.insert(new_index[tb_index], row)
                        end
                    end
                end
            end
        end

        if has_change then
            local redis_result = self.redis_inst:execRedisCmd(redis_cmd)
            if redis_result == nil then
                logger.logError("exec redis cmd error, %s", redis_cmd)
            end
        end
    end

    for tb_index, rows in pairs(new_index) do
        self:_cache_tbl_index_to_redis(tbName, rows, tb_index, false)
    end

    for tb_index, rows in pairs(old_index) do
        self:_remove_cache_tbl_index_from_redis(tbName, rows, tb_index)
    end

    self:_add_changed_tbls(tbl)
    return true
end

-- tbl = {
--     table_name = "",
--     rows = {}
-- }
function clsDBHandler:delete_table(tbl)
    local tbName = tbl.table_name
    local priColName = TblUtil.getPriColName(tbName)

    for _,row in ipairs(tbl.rows) do
        local primary_val = row[priColName]
        if primary_val == nil then
            logger.logError("delete table must use primary key, table:%s", tbName)
            return false
        end
    end
    local keys = {}
    local index_dict = {}
    for _,row in ipairs(tbl.rows) do
        local primary_val = row[priColName]
        local pri_key = TblUtil.makeRedisPriKey(tbName, primary_val)
        table.insert(keys, pri_key)
        local res = self:_load_data_from_redis(tbName, pri_key)
        local old_row = nil
        if res ~= nil and not TableUtil.isEmpty(res.rows) then
            old_row = res.rows[1]
        end
        if old_row ~= nil then
            for _,tb_index in ipairs(TblUtil.all_index(tbName)) do
                if index_dict[tb_index] == nil then index_dict[tb_index] = {} end
                table.insert(index_dict[tb_index], old_row)
            end
        end
        local _tbl = {table_name = tbName, row = row}
        self:delete_row(_tbl)
    end

    local redis_cmd = "DEL " .. table.concat(keys, " ")
    local redis_result = self.redis_inst:execRedisCmd(redis_cmd)
    if redis_result == nil then
        logger.logError("exec redis cmd error, %s", redis_cmd)
        return false
    end
    for tb_index, rows in pairs(index_dict) do
        self:_remove_cache_tbl_index_from_redis(tbName, rows, tb_index)
    end
    self:_remove_changed_tbls(tbl)
    return true
end

function clsDBHandler:_remove_cache_tbl_index_from_redis(table_name, rows, tb_index)
    if TableUtil.isEmpty(rows) then return end
    local priColName = TblUtil.getPriColName(table_name)
    local key = TblUtil.make_redis_index_key(table_name, rows[1], tb_index)
    local redis_cmd = "ZREM " .. key
    for _,row in ipairs(rows) do
        local primary_val = row[priColName]
        redis_cmd = redis_cmd .. string.format(" %s", TblUtil.makeRedisPriKey(table_name, primary_val))
    end
    local redis_result = self.redis_inst:execRedisCmd(redis_cmd)
    if redis_result == nil then
        logger.logError("exec redis cmd error, %s", redis_cmd)
    end
end

function clsDBHandler:delete_row(tbl)
    return self.db_inst:deleteRow(tbl)
end

function clsDBHandler:execute_sql(sql)
    return self.db_inst:executeSql(sql)
end

function clsDBHandler:test_db_and_redis()
    local err_msg = "test db and redis failed!!!"

    local function _output(flag)
        local tbl_sql = {}
        tbl_sql.role_id = 3
        local res = self:load({table_name="test", row=tbl_sql})
        print(flag)
        for _,row in ipairs(res.rows) do
            print(StrUtil.tableToStr(row))
        end
        print("\n")
        return res.rows
    end

    print("--------test insert--------")
    local tbl_test = {}
    tbl_test.role_id = 3
    tbl_test.role_name = "test3"
    tbl_test.account = "test3"
    self:insert_table({table_name = "test", rows={tbl_test}})
    local res = _output("after insert:")
    if TableUtil.size(res) ~= 1 then
        error(err_msg)
    end
    local res_tbl = res[1]
    if res_tbl.role_id ~= 3 or res_tbl.role_name ~= "test3" or res_tbl.account ~= "test3" then
        error(err_msg)
    end

    print("--------test update--------")
    tbl_test = {}
    tbl_test.role_id = 3
    tbl_test.role_name = "test3-1"
    self:update_table({table_name="test", rows={tbl_test}})
    res = _output("after update:")
    if TableUtil.size(res) ~= 1 then
        error(err_msg)
    end
    res_tbl = res[1]
    if res_tbl.role_id ~= 3 or res_tbl.role_name ~= "test3-1" or res_tbl.account ~= "test3" then
        error(err_msg)
    end

    print("--------test delete--------")
    tbl_test = {}
    tbl_test.role_id = 3
    self:delete_table({table_name="test", rows = {tbl_test}})
    res = _output("after delete:")
    if not TableUtil.isEmpty(res) then
        error(err_msg)
    end

    logger.logInfo("db and redis test ok!!!")
end

function clsDBHandler:redis_benchmark()
    local loop_count = 10
    local tbl = {table_name="player", row = {role_id=1}}
    local res = nil
    local st = os.clock()
    for i=0, loop_count do
        res = self:load(tbl)
    end
    print("[redis cache] load time=", os.clock() - st)
    print(res)

    st = os.clock()
    for i=0, loop_count do
        res = self.redis_inst:execRedisCmd("HGETALL player:1")
    end
    print("[redis only] load time=", os.clock() - st)
    print(res)

    st = os.clock()
    for i=0, loop_count do
        res = self:get_row(tbl)
    end
    print("[sql] load time=", os.clock() - st)
    print(res)

    st = os.clock()
    for i=0, loop_count do
        res = self:execute_sql("select * from player where role_id=1")
    end
    print("[sql only] load time=", os.clock() - st)
    print(res)
end

function clsDBHandler:test()
    -- local result = self.db_inst:executeSql("select * from player")
    -- print(StrUtil.tableToStr(result))
    -- for k, v in pairs(result) do
    --     print(k, v)
    -- end

    -- local row = {
    --     table_name = "test",
    --     -- pri_key = 1,
    --     fields = {
    --         role_name = "test1",
    --         account = "test1",
    --     }
    -- }
    -- self.db_inst:insertRow(row)
    -- local result = self.db_inst:executeSql("select * from test")
    -- print(StrUtil.tableToStr(result))

    -- local row = {
    --     table_name = "test",
    --     columns = {
    --         account = "test1",
    --     }
    -- }
    -- local result = self.db_inst:getRow(row)
    -- print(StrUtil.tableToStr(result))

    -- local row = {
    --     table_name = "test",
    --     columns = {
    --         role_id=1,
    --         role_name = "test1_new",
    --     }
    -- }
    -- local result = self.db_inst:updateRow(row)
    -- print(result)

    -- local row = {
    --     table_name = "test",
    --     columns = {
    --         role_id=1,
    --     }
    -- }
    -- local result = self.db_inst:deleteRow(row)
    -- print(result) 
    
    -- local tbl = {
    --     table_name = "test",
    --     row = {
    --         role_id=3,
    --     }
    -- }
    -- local result = self:load(tbl)
    -- print(StrUtil.tableToStr(result))

    -- local tbl = {
    --     table_name = "test",
    --     rows = {
    --         {role_id=6, role_name="test6", account="test"}
    --     }
    -- }
    -- local result = self:insert_table(tbl)
    -- print(result)
end