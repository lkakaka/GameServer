
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

function TblUtil.filter(dst_row, src_row)
    for k, v in pairs(src_row) do
        if dst_row[k] ~= v then
            return false
        end
    end

    return true
end

function TblUtil.init_with_default(tbName, row)
    local tbl = TblDefs[tbName]
    for _,col in ipairs(tbl.columns) do
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

function TblUtil.primary_column(tbName)
    local tbl = TblDefs[tbName]
    for _,col in ipairs(tbl.columns) do
        if col.key then return col end
    end
end

-- todo: 是否需要完全匹配?
function TblUtil.matchIndex(tbl)
    local tbName = tbl.table_name
    local tblDef = TblDefs[tbName]
    for _, index in ipairs(tblDef.indexs) do
        local isMatched = true
        for _, colName in ipairs(index.cols) do
            if tbl.row[colName] == nil then
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
        for _,indexColName in ipairs(index.cols) do
            if colName == indexColName then
                table.insert(indexs, index)
                break
            end
        end
    end
    return indexs
end

function TblUtil.transRow(table_name, row)
    local tblDef = TblDefs[table_name]
    for _,col in ipairs(tblDef.columns) do
        if row[col.name] ~= nil then
            if col.type == ColType.INT or col.type == ColType.BIGINT or col.type == ColType.DOUBLE then
                row[col.name] = tonumber(row[col.name])
            end
        end
    end
end


TblDefs = {

    test = {
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

    player = {
        tb_name = "player",
        columns = {
            {name = "role_id", type=ColType.BIGINT, key = true, auto_incr = true},
            {name = "role_name", type=ColType.VARCHAR, length=128, default=""},
            {name = "account", type=ColType.VARCHAR, length=128},
        },

        indexs = {
            { cols = { "role_name" }, is_unique = true},
            { cols = { "account" }, is_unique = false},
        },
    },

    item = {
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
