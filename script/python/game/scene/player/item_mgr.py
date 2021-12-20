import weakref
import math

from game.util.const import ErrorCode
from game.util import logger
from game.util.id_mgr import IDMgr
import game.util.db_util

MAX_OVERLAP_COUNT = 10
MAX_GRID_COUNT = 20


class Item(object):

    def __init__(self, role_id, item_uid, item_id, count):
        self.item_uid = item_uid
        self.role_id = role_id
        self.item_id = item_id
        self.count = count

    def to_tbl_item(self):
        tbl_item = game.db.tbl.tbl_item.TblItem .db_util.create_tbl_obj("item")
        tbl_item.item_uid = self.item_uid
        tbl_item.role_id = self.role_id
        tbl_item.item_id = self.item_id
        tbl_item.count = self.count
        return tbl_item

    def to_string(self):
        return "item_uid:{},item_id:{},count:{}".format(self.item_uid, self.item_id, self.count)


class ItemMgr(object):

    def __init__(self, player):
        self._weak_player = weakref.ref(player)
        self._items = {}
        self._items_by_id = {}
        self.server_id = player.server_id

    @property
    def service_obj(self):
        return self._weak_player().game_scene.service

    def on_load_item(self, tbls):
        if not tbls:
            return
        role_id = self._weak_player().role_id
        for tbl_item in tbls:
            if tbl_item.role_id != role_id:
                game.util.logger.log_error("load item role id mismatch, item's role_id:{}, player's role_id:{}", tbl_item.role_id, role_id)
                continue
            item = Item(role_id, tbl_item.item_uid, tbl_item.item_id, tbl_item.count)
            self._add_item(item)

    def _add_item(self, item):
        self._items[item.item_uid] = item
        if item.item_id not in self._items_by_id:
            self._items_by_id[item.item_id] = [item.item_uid,]
        else:
            self._items_by_id[item.item_id].append(item.item_uid)

    def _remove_item(self, item):
        self._items.pop(item.item_uid)
        if item.item_id not in self._items_by_id:
            return
        self._items_by_id[item.item_id].remove(item.item_uid)

    def _remove_item_by_uid(self, item_uid):
        item = self._items.get(item_uid, None)
        if item is None:
            return
        self._remove_item(item)

    def _create_item(self, item_id, item_count, item_uid=None):
        role_id = self._weak_player().role_id
        item_uid = item_uid if item_uid is not None else IDMgr.alloc_item_uid(count=1)
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
                    tbl_item = game.util.db_util.create_tbl_obj("item")
                    tbl_item.item_uid = item.item_uid
                    tbl_item.count = item.count
                    chg_items.append(tbl_item)

            if item_count > 0:
                new_item_count = math.ceil(float(item_count) / MAX_OVERLAP_COUNT)
                item_uid = IDMgr.alloc_item_uid(count=new_item_count) - new_item_count + 1
                while item_count > 0:
                    add_count = min(item_count, MAX_OVERLAP_COUNT)
                    new_item = self._create_item(item_id, add_count, item_uid=item_uid)
                    self._add_item(new_item)
                    item_count -= add_count
                    tbl_item = new_item.to_tbl_item()
                    new_items.append(tbl_item)
                    item_uid += 1

        print(chg_items, new_items)
        if len(chg_items) > 0:
            self.service_obj.db_proxy.update(self.server_id, chg_items)

        if len(new_items) > 0:
            self.service_obj.db_proxy.insert(self.server_id, new_items)

        game.util.logger.log_info("add item:{}", repr(item_list))
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
            tbl_item = game.util.db_util.create_tbl_obj("item")
            tbl_item.item_uid = item.item_uid
            if item_count >= item.count:
                item_count -= item.count
                item.count = 0
                del_items.append(tbl_item)
            else:
                item.count -= item_count
                item_count = 0
                tbl_item.count = item.count
                chg_items.append(tbl_item)

            if item_count == 0:
                break

        if len(del_items) > 0:
            for tbl_item in del_items:
                self._remove_item_by_uid(tbl_item.item_uid)
            self.service_obj.db_proxy.delete(self.server_id, del_items)

        if len(chg_items) > 0:
            self.service_obj.db_proxy.update(self.server_id, chg_items)

        game.util.logger.log_info("use item:{}", repr((item_id, item_count)))
        return ErrorCode.OK

    def dump_items(self):
        item_str = ""
        for _, item in self._items.items():
            item_str += item.to_string() + "\n"
        item_str += "item_by_ids:\n" + repr(self._items_by_id)
        return item_str
