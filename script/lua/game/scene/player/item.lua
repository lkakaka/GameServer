require("base.object")
require("util.logger")

clsItem = clsObject:Inherit("clsItem")

function clsItem:__init__(role_id, item_uid, item_id, item_count)
    self.role_id = role_id
    self.item_uid = item_uid
    self.item_id = item_id
    self.item_count = item_count
end

