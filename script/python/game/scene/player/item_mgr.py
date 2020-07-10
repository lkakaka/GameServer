
import weakref
import util.db_util
from util.const import ErrorCode
import math
import util.logger

MAX_OVERLAP_COUNT = 10
MAX_GRID_COUNT = 20


class Item(object):

    def __init__(self, role_id, item_uid, item_id, count):
        self.item_uid = item_uid
        self.role_id = role_id
        self.item_id = item_id
        self.count = count

    def to_tbl_item(self):
        tbl_item = util.db_util.create_tbl_obj("item")
        tbl_item.item_uid = self.item_uid
        tbl_item.role_id = self.role_id
        tbl_item.item_id = self.item_id
        tbl_item.count = self.count
        return tbl_item


class ItemMgr(object):

    def __init__(self, player):
        self._weak_player = weakref.ref(player)
        self._items = {}
        self._items_by_id = {}

    @property
    def service_obj(self):
        return self._weak_player().game_scene.service

    def on_load_item(self, tbls):
        if not tbls:
            return
        role_id = self._weak_player().role_id
        for tbl in tbls:
            tbl_item = util.db_util.create_tbl_with_data("item", tbl)
            if tbl_item.role_id != role_id:
                util.logger.log_error("load item role id mismatch, item's role_id:{}, player's role_id:{}", tbl_item.role_id, role_id)
                continue
            item = Item(role_id, tbl_item.item_uid, tbl_item.item_id, tbl_item.count)
            self._add_item(item)

    def _add_item(self, item):
        self._items[item.item_uid] = item
        if item.item_id not in self._items_by_id:
            self._items_by_id[item.item_id] = [item.item_uid,]
        else:
            self._items_by_id[item.item_id].append(item.item_uid)

    def _create_item(self, item_id, item_count):
        role_id = self._weak_player().role_id
        item_uid = 0  # todo: ·ÖÅäID
        item = Item(role_id, item_uid, item_id, item_count)
        return item

    def check_add_item(self, item_list):
        new_gird_count = 0
        for i in range(len(item_list)):
            item_id = item_list[i][0]
            item_count = item_list[i][1]
            item_uids = self._items_by_id.get(item_id)
            if item_uids:
                for item_uid in item_uids:
                    item = self._items[item_uid]
                    if item.count >= MAX_OVERLAP_COUNT:
                        continue
                    add_count = MAX_OVERLAP_COUNT - item.count
                    if add_count >= item_count:
                        item_count = 0
                    else:
                        item_count -= add_count
            if item_count > 0:
                new_gird_count += math.ceil(item_count / MAX_OVERLAP_COUNT)

        if new_gird_count + len(self._items) > MAX_GRID_COUNT:
            return ErrorCode.BAG_SPACE_NOT_ENOUGH

        return ErrorCode.OK

    # item_list: [(item_id, item_count), (item_id, item_count),]
    def add_item(self, item_list):
        err_code = self.check_add_item(item_list)
        if err_code != ErrorCode.OK:
            return err_code

        chg_items = []
        new_items = []
        for item_id, item_count in item_list:
            item_uids = self._items_by_id.get(item_id)
            if item_uids:
                for item_uid in item_uids:
                    item = self._items[item_uid]
                    if item.count >= MAX_OVERLAP_COUNT:
                        continue
                    add_count = MAX_OVERLAP_COUNT - item.count
                    if add_count >= item_count:
                        item.count += item_count
                        item_count = 0
                    else:
                        item.count = MAX_OVERLAP_COUNT
                        item_count -= add_count
                    chg_items.append(item)

            while item_count > 0:
                add_count = min(item_count, MAX_OVERLAP_COUNT)
                new_item = self._create_item(item_id, add_count)
                new_items.append(new_item)
                item_count -= add_count
        print(chg_items, new_items)
        if len(chg_items) > 0:
            chg_tbls = []
            for item in chg_items:
                tbl_item = util.db_util.create_tbl_obj("item")
                tbl_item.item_uid = item.item_uid
                tbl_item.count = item.count
                chg_tbls.append(tbl_item)
            self.service_obj.db_proxy.update(chg_tbls)

        if len(new_items) > 0:
            new_tbls = []
            for item in new_items:
                tbl_item = item.to_tbl_item()
                new_tbls.append(tbl_item)
            self.service_obj.db_proxy.insert(new_tbls)

        util.logger.log_info("add item:{}", repr(item_list))
        return ErrorCode.OK

    def check_use_item(self, item_id, item_count):
        if item_count < 0:
            return ErrorCode.ILLEGAL_PARAM
        item_uids = self._items_by_id.get(item_id)
        if item_uids is None:
            return ErrorCode.ITEM_NOT_EXIST
        for item_uid in item_uids:
            item = self._items[item_uid]
            item_count -= item.count
            if item_count <= 0:
                break
        if item_count > 0:
            return ErrorCode.ITEM_NOT_ENOUGH
        return ErrorCode.OK

    def use_item(self, item_id, item_count):
        err_code = self.check_use_item(item_id, item_count)
        if err_code != ErrorCode.OK:
            return err_code

        del_items = []
        chg_items = []
        item_uids = self._items_by_id.get(item_id)
        for item_uid in item_uids:
            item = self._items[item_uid]
            if item_count >= item.count:
                item_count -= item.count
                item.count = 0
                del_items.append(item)
            else:
                item.count -= item_count
                item_count = 0
                chg_items.append(item)

            if item_count == 0:
                break

        if len(del_items) > 0:
            del_tbls = []
            for item in del_items:
                tbl_item = util.db_util.create_tbl_obj("item")
                tbl_item.item_uid = item.item_uid
                del_tbls.append(tbl_item)
            self.service_obj.db_proxy.delete(del_tbls)

        if len(chg_items) > 0:
            chg_tbls = []
            for item in chg_items:
                tbl_item = util.db_util.create_tbl_obj("item")
                tbl_item.item_uid = item.item_uid
                tbl_item.count = item.count
                chg_tbls.append(tbl_item)
            self.service_obj.db_proxy.update(chg_tbls)

        util.logger.log_info("use item:{}", repr((item_id, item_count)))
        return ErrorCode.OK
