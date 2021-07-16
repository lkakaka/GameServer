require("util.logger")
require("game.db.db_redis")
require("util.str_util")
require("game.db.tbl_defs")

local INDEX_ALL_CACHED = "ALL_CACHED"
local INDEX_NOT_ALL_CACHED_SCORE = "0"
local INDEX_ALL_CACHED_SCORE = "1"
local INDEX_PRI_KEY_SCORE = "10"

local REDIS_FLUSH_TIME = 5 * 6

clsDBHandler = clsObject:Inherit("clsDBHandler")

function clsDBHandler:__init__(dbName, redisIp, redisPort)
    self.db_inst = LuaDB.new(dbName)
    self.redis_inst = clsRedis:New(redisIp, redisPort)
    self.db_inst:initTable(TblDefs)
    self:test()
end

function clsDBHandler:test()
    local result = self.db_inst:executeSql("select * from player")
    print(StrUtil.tableToStr(result))
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

    local row = {
        table_name = "test",
        columns = {
            role_id=1,
        }
    }
    local result = self.db_inst:deleteRow(row)
    print(result)
end

function clsDBHandler:initTable(tblDefs)
    logger.logInfo("*********init table, %s", StrUtil.tableToStr(tbls))
    if not self.db_inst:initTable(tblDefs) then
        error("init tables error")
    end
    self:flush_add_clean_redis()
    self:_init_id_allocator(tbl_names)
    return tbl_names
end

function clsDBHandler:flush_add_clean_redis()
    self:flushRedis()
    self.redis_inst:execRedisCmd("FLUSHDB")
end

function clsDBHandler:getTblNameFromKey(tblKey)
    local pos = string.find(tblKey, ":")
    return string.sub( tblKey, 0, pos)
end

function clsDBHandler:flushRedis()
    logger.LogInfo("begin flush redis!!!!")
    local redis_cmd = "SMEMBERS _chg_keys"
    local redis_result = self.redis_inst:execRedisCmd(redis_cmd)
    if not redis_result then
        logger.logError("exec redis cmd error, %s", redis_cmd)
    end

    local flush_count = 0
    if len(redis_result) > 0 then
        local all_result = {}
        for k, redisKey in pairs(redis_result) do
            local table_name = self:getTblNameFromKey(redisKey)
            local result = self:_load_data_from_redis(table_name, key)
            if result then
                flush_count = flush_count + len(result)
                table.insert(all_result, result)
                if len(all_result) >= 1000 then
                    if not self:replace_rows(all_result) then
                        logger.logError("flush redis error, update mysql failed, count: %d", len(all_result))
                        return
                    end
                    all_result = {}
                end
            else
                logger.logError("not found redis key %s", key)
            end
            
        end

        if len(all_result) > 0 then
            if not self:replace_rows(all_result) then
                logger.logError("flush redis error, update mysql failed, count: %d", len(all_result))
                return
            end
        end

        self.redis_inst:execRedisCmd("DEL _chg_keys")
    end
    logger.logInfo("end flush redis!!!!, flush count: %d", flush_count)
end

function clsDBHandler:_load_data_from_redis(table_name, primary_key)
    local result = self.redis_inst:execRedisCmd("HGETALL {}", primary_key)
    if not result then return end
    -- res_tbl = game.util.db_util.create_tbl_obj(table_name)
    local row = {}
    for i=1,len(result),2 do
        row[result[i]] = result[i+1]
    end
    return {row} 
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
    local primary_val = row[priColName]
    if primary_val ~= nil then
        local redisKey = TblUtil.makeRedisPriKey(priColName, primary_val)
        return self:_load_data_by_primary_key(tbl, redisKey)
    end
    local tb_index = TblUtil.matchIndex(tbl)
    if tb_index ~= nil then
        print("match_index----", tb_index.cols)
        return self:_load_data_by_index(tbl, tb_index)
    end
    logger.log_warn("load data from db, may be create index on table %s", tbl.table_name)
    return self:_load_from_db_and_cache(tbl)
end

function clsDBHandler:_load_data_by_primary_key(tbl, primary_key)
    local result = self:_load_data_from_redis(tbl.table_name, primary_key)
    if result ~= nil then
        return result
    end
    -- local tbl_sql = tbl.clone(col_names=(tbl.primary_col.name,))
    return self:_load_from_db_and_cache(tbl)
end

function clsDBHandler:_load_data_by_index(tbl, tb_index)
    local result_lst = {}
    local key = TblUtil.make_redis_index_key(tbl.table_name, tbl.row, tb_index)
    local result = self.redis_inst:execRedisCmd("ZRANGE {} 0 -1 WITHSCORES", key)
    print("redis result----", result)
    if result ~= nil and result[2] == INDEX_ALL_CACHED_SCORE then    -- 已在redis中全缓存
        for i=1,len(result),2 do
            local pri_key = result[i]
            if pri_key ~= "" then
                local tup = self:_load_data_by_primary_key(tbl, pri_key)
                for _,res in ipairs(tup) do
                    if TblUtil.fiter(tbl, res) then
                        table.insert(result_lst, res)
                    end
                end
            end
        end
        return result_lst
    end
    -- tbl_sql = tbl.clone(col_names=tb_index.cols)
    -- print("tbl sql-===", tbl_sql)
    local res = self._load_from_db_and_cache(tbl)
    self:_cache_tbl_index_to_redis(res.table_name, res.rows, tb_index, true)
    return lst
end

function clsDBHandler:_load_from_db_and_cache(tbl)
    local rows = self:get_row(tbl)
    if rows == nil then return end
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
    for _,row in iparis(tbl.rows) do
        local primary_val = row[priColName]
        local pri_key = TblUtil.makeRedisPriKey(tbName, primary_val)
        local redis_cmd = "HSET " .. pri_key
        for _,col in ipiars(TblDefs[tbName]["columns"]) do
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
    if not rows then return end
    local key = TblUtil.make_redis_index_key(table_name, rows[0], tb_index)
    local redis_cmd = "EXISTS " .. key
    local redis_result = self.redis_inst:execRedisCmd(redis_cmd)
    if len(redis_result) == 1 then   -- 索引存在
        if is_all_cached then
            redis_cmd = string.format("ZADD %s %s %s", key, DBHandler.INDEX_ALL_CACHED_SCORE, DBHandler.INDEX_ALL_CACHED)
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
    for tbl in ipairs(tbls) do
        local priColName = TblUtil.getPriColName(tbl.table_name)
        local primary_val = tbl[priColName]
        redis_cmd = redis_cmd .. string.format(" %s %s", INDEX_PRI_KEY_SCORE,  StrUtil.makeRedisPriKey(tbl.table_name, primary_val))
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
    for row in ipairs(tbl.rows) do
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
    for row in ipairs(tbl.rows) do
        local primary_val = row[priColName]
        redis_cmd = redis_cmd .. " " .. TblUtil.makeRedisPriKey(tbl.table_name, primary_val)
    end
    redis_result = self.db_redis:execRedisCmd(redis_cmd)
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
            local res = self:_load_data_by_primary_key(row, pri_key)
            if res ~= nil then
                logger.logInfo("insert error, key %s has exist", pri_key)
                return false
            end
        end
        TblUtil.init_with_default(tbName, row)
        for _,tb_index in ipairs(indexs) do
            if index_dict[tb_index] == nil then index_dict[tb_index] = {} end
            table.insert(index_dict[tb_index], row)
        end
    end
    self._cache_to_redis(tbl)
    for tb_index, rows in pairs(index_dict) do
        self:_cache_tbl_index_to_redis(tbName, rows, tb_index, false)
    end
    self._add_changed_tbls(tbl)
    return true
end

function clsDBHandler:isInArray(arr, val)
    for _,v in ipairs(arr) do
        if val == v then
            return true
        end
    end
    return false
end

-- tbl = {
--     table_name = "",
--     rows = {}
-- }
function clsDBHandler:update_table(self, tbl)
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
        local res = self:_load_data_by_primary_key(tbl, pri_key) -- todo: 有问题
        local redis_cmd = "HSET " + pri_key
        local old_tbl = nil
        if res ~= nil then old_tbl = res[1] end
        
        local has_change = false
        for _,col in ipairs(allCols) do
            local col_name = col.name
            if row[col_name] ~= nil then
                if old_tbl == nil or row[col_name] ~= old_tbl[col_name] then
                    has_change = True
                    redis_cmd = redis_cmd .. string.format(" {} {}", col_name, row[col_name])
                    -- 更新索引
                    local indexs = TblUtil.get_indexs_by_col(tbName, col_name)
                    for _,tb_index in ipairs(indexs) do
                        if old_tbl ~= nil then
                            if old_index[tb_index] == nil then old_index[tb_index] = {} end
                            if not self:isInArray(old_index[tb_index], old_tbl) then
                                table.insert(old_index[tb_index], old_tbl)
                            end
                        end

                        if new_index[tb_index] == nil then new_index[tb_index] = {} end
                        if not self:isInArray(new_index[tb_index], tbl) then
                            table.insert(new_index[tb_index], tbl)
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

    for tb_index, tbls in pairs(new_index) do
        self:_cache_tbl_index_to_redis(tbls, tb_index, false)
    end

    for tb_index, tbls in pairs(old_index) do
        self:_remove_cache_tbl_index_from_redis(tbls, tb_index)
    end

    self:_add_changed_tbls(tbl)
    return true
end
