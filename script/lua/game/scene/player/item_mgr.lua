require("base.object")
require("util.logger")
require("game.scene.player.item")
require("util.str_util")
require("util.table_util")
require("base.id_mgr")

local MAX_OVERLAP_COUNT = 10
local MAX_GRID_COUNT = 20

clsItemMgr = clsObject:Inherit("clsItemMgr")

function clsItemMgr:__init__(player)
    self.player = player
    self._items = {}
    self._items_by_id = {}
end

function clsItemMgr:on_load_item(tbl)
    if tbl == nil or TableUtil.isEmpty(tbl.rows) then return end
    local role_id = self.player.role_id
    for _,row in pairs(tbl.rows) do
        if row.role_id == role_id then
            local item = self:_create_item(row.item_uid, row.item_id, row.count)
            self:_add_item(item)
        else
            logger.logError("load item role id mismatch, item's role_id:%d, player's role_id:%d", row.role_id, role_id)
        end
    end
end

function clsItemMgr:_add_item(item)
    self._items[item.item_uid] = item
    if self._items_by_id[item.item_id] == nil then
        self._items_by_id[item.item_id] = {}
    end
    self._items_by_id[item.item_id][item.item_uid] = true
end

function clsItemMgr:_remove_item(item)
    self._items[item.item_uid] = nil
    if self._items_by_id[item.item_id] == nil then return end
    self._items_by_id[item.item_id][item.item_uid] = nil
end

function clsItemMgr:_remove_item_by_uid(item_uid)
    local item = self._items[item_uid]
    if item == nil then return end
    self:_remove_item(item)
end

function clsItemMgr:_create_item(item_uid, item_id, item_count)
    local role_id = self.player.role_id
    item_uid = item_uid or IDMgr.alloc_item_uid(1)
    local item = clsItem:New(role_id, item_uid, item_id, item_count)
    return item
end

function clsItemMgr:check_add_item(item_list)
    local new_gird_count = 0
    for i=1,TableUtil.size(item_list),1 do
        local item_id = item_list[i][0]
        local item_count = item_list[i][1]
        local item_uids = self._items_by_id[item_id]
        if item_uids ~= nil then
            for item_uid,_ in pairs(tem_uids) do
                local item = self._items[item_uid]
                if item.count < MAX_OVERLAP_COUNT then
                    local add_count = MAX_OVERLAP_COUNT - item.count
                    if add_count >= item_count then
                        item_count = 0
                    else
                        item_count = item_count - add_count
                    end
                end
            end
        end
        if item_count > 0 then
            new_gird_count = new_gird_count + math.ceil(item_count / MAX_OVERLAP_COUNT)
        end
    end

    if new_gird_count + TableUtil.size(self._items) > MAX_GRID_COUNT then
        return ErrorCode.BAG_SPACE_NOT_ENOUGH
    end

    return ErrorCode.OK
end

-- # item_list: {{item_id, item_count}, {item_id, item_count},}
function clsItemMgr:add_item(item_list)
    local err_code = self.check_add_item(item_list)
    if err_code ~= ErrorCode.OK then
        return err_code
    end

    local chg_items = {}
    local new_items = {}
    for _, item in ipairs(item_list) do
        local item_id = item[1]
        local item_count = item[2]
        local item_uids = self._items_by_id[item_id]
        if item_uids ~= nil then
            for item_uid,_ in pairs(item_uids) do
                local itemObj = self._items[item_uid]
                if itemObj.count < MAX_OVERLAP_COUNT then
                    add_count = MAX_OVERLAP_COUNT - itemObj.count
                    if add_count >= item_count then
                        itemObj.count = itemObj.count + item_count
                        item_count = 0
                    else
                        itemObj.count = MAX_OVERLAP_COUNT
                        item_count = item_count - add_count
                    end
                    local tbl_item = {}
                    tbl_item.item_uid = item.item_uid
                    tbl_item.count = item.count
                    table.insert(chg_items, tbl_item)
                end
            end
        end

        if item_count > 0 then
            local new_item_count = math.ceil(item_count / MAX_OVERLAP_COUNT)
            local item_uid = IDMgr.alloc_item_uid(new_item_count) - new_item_count + 1
            while item_count > 0 do
                local add_count = math.min(item_count, MAX_OVERLAP_COUNT)
                local new_item = self:_create_item(item_uid, item_id, add_count)
                self:_add_item(new_item)
                item_count = item_count - add_count
                local tbl_item = new_item.to_tbl_item()
                table.insert(new_items, tbl_item)
                item_uid = item_uid + 1
            end
        end
    end

    print(chg_items, new_items)
    if not TableUtil.isEmpty(chg_items) then
        self.service_obj.db_proxy:update(chg_items)
    end

    if not TableUtil.isEmpty(new_items) then
        self.service_obj.db_proxy:insert(new_items)
    end

    logger.logInfo("add item:%s", StrUtil.tableToStr(item_list))
    return ErrorCode.OK
end

function clsItemMgr:check_use_item(item_id, item_count)
    if item_count < 0 then
        return ErrorCode.ILLEGAL_PARAM
    end
    local item_uids = self._items_by_id[item_id]
    if item_uids == nil then
        return ErrorCode.ITEM_NOT_EXIST
    end
    for item_uid,_ in pairs(item_uids) do
        local item = self._items[item_uid]
        item_count = item_count -item.count
        if item_count <= 0 then break end
    end
    if item_count > 0 then
        return ErrorCode.ITEM_NOT_ENOUGH
    end
    return ErrorCode.OK
end

function clsItemMgr:use_item(item_id, item_count)
    local err_code = self:check_use_item(item_id, item_count)
    if err_code ~= ErrorCode.OK then
        return err_code
    end

    local del_items = {}
    local chg_items = {}
    local item_uids = self._items_by_id[item_id]
    for item_uid,_ in pairs(item_uids) do
        local item = self._items[item_uid]
        local tbl_item = {}
        tbl_item.item_uid = item.item_uid
        if item_count >= item.count then
            item_count = item_count - item.count
            item.count = 0
            table.insert(del_items, tbl_item)
        else
            item.count = item.count - item_count
            item_count = 0
            tbl_item.count = item.count
            table.insert(chg_items, tbl_item)
        end

        if item_count == 0 then break end
    end

    if not TableUtil.isEmpty(del_items) then
        for tbl_item,_ in ipairs(del_items) do
            self:_remove_item_by_uid(tbl_item.item_uid)
        end
        self.service_obj.db_proxy:delete(del_items)
    end

    if not TableUtil.isEmpty(chg_items) then
        self.service_obj.db_proxy:update(chg_items)
    end

    logger.logInfo("use item, item_id:%d, item_count:%d", item_id, item_count)
    return ErrorCode.OK
end

function clsItemMgr:dump_items()
    local item_str = ""
    for _, item in pairs(self._items) do
        item_str = item_str .. item.to_string() .. "\n"
    end
    item_str = item_str .. "item_by_ids:\n" .. StrUtil.tableToStr(self._items_by_id)
    return item_str
end
