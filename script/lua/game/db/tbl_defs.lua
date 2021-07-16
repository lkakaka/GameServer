
local ColType = {
    INT = 0,
    BIGINT = 1,
    DOUBLE = 2,
    VARCHAR = 3,
    TEXT = 4,
}

TblUtil = {}

function TblUtil.getPriColName(tbName)
    local tbl = TblDefs[tbName]
    for i, v in ipairs(tbl.columns) do
        if v.key then
            return v.name
        end
    end
end

function TblUtil.makeRedisPriKey(tbName, priVal)
    return string.format("%s:%s", tbName, priVal)
end

function TblUtil.make_redis_index_key(tb_name, row, tbl_index)
    local redis_key = tb_name .. ":"
    for i, colName in ipairs(tbl_index.cols) do
        redis_key = redis_key .. string.format("%s:%s", colName, row[colName])
        if i ~= #tbl_index.cols then
            redis_key = redis_key .. ":"
        end
    end
    return redis_key
end

function TblUtil.fiter(tbl, tbl_val)
    for k, v in paris(tbl) do
        if tbl_val[k] ~= v then
            return false
        end
    end

    return true
end

function TblUtil.init_with_default(tbName, row)
    local tbl = TblDefs[tbName]
    for _,col in ipiars(tbl.columns) do
        if col.default ~= nil and row[col.name] == nil then
            row[col.name] = col.default
        end
    end
end

function TblUtil.all_index(tbName)
    local tbl = TblDefs[tbName]
    return tbl.indexs
end

function TblUtil.all_column(tbName)
    local tbl = TblDefs[tbName]
    return tbl.columns
end

-- todo: 是否需要完全匹配?
function TblUtil.matchIndex(tbl)
    local tbName = tbl.table_name
    local tbl = TblDefs[tbName]
    for _, index in ipairs(tbl.indexs) do
        local isMatched = true
        for _, colName in ipairs(index.cols) do
            if tbl[colName] == nil then
                isMatched = false
                break
            end
        end
        if isMatched then return index end
    end
end

function TblUtil.get_indexs_by_col(tbName, colName)
    local tbl = TblDefs[tbName]
    local indexs = {}
    for _, index in ipairs(tbl.indexs) do
        for _,indexColName in index.cols do
            if colName == indexColName then
                table.insert(indexs, index)
                break
            end
        end
    end
    return indexs
end

TblDefs = {

    tblTest = {
        tb_name = "test",
        columns = { 
            {name="role_id", type=ColType.BIGINT, key=true, auto_incr=true},
            {name="role_name", type=ColType.VARCHAR, length=128, default=""},
            {name="account", type=ColType.VARCHAR, length=128},
            {name="data1", type=ColType.BIGINT},
        },
        indexs = {
            { cols = { "role_name" }, is_unique = true},
            { cols = { "account" }, is_unique = false},
        }
    },

    tblPlayer = {
        tb_name = "player",
        columns = {
            {name = "item_uid", type=ColType.BIGINT, key = true, auto_incr = true},
            {name = "role_name", type=ColType.VARCHAR, length=128, default=""},
            {name = "account", type=ColType.VARCHAR, length=128},
        },

        indexs = {
            { cols = { "role_name" }, is_unique = true},
            { cols = { "account" }, is_unique = false},
        },
    },

    tblItem = {
        tb_name = "item",
        columns = {
            {name="item_uid", type=ColType.BIGINT, key=true},
            {name="role_id", type=ColType.BIGINT},
            {name="item_id", type=ColType.INT},
            {name="count", type=ColType.INT},
        },
        indexs = {
            { cols = { "role_id" }, is_unique = false},
        },
    },

}
