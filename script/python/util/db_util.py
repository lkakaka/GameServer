# -*- coding:utf-8 -*-
# import util.logger
import copy


class ColType(object):
    INT = 0
    BIGINT = 1
    DOUBLE = 2
    VARCHAR = 3
    TEXT = 4


class TbCol(object):
    old_name = ""
    name = ""
    type = 0
    key = False     # 是否是主键
    length = 0
    default = None
    is_del = False
    auto_incr = False   # 主键自增

    def __init__(self, **kwargs):
        for k, v in kwargs.items():
            self.__dict__[k] = v

    @property
    def default_val(self):
        if self.default is not None:
            return self.default
        if self.type in (ColType.INT, ColType.BIGINT):
            return 0
        if self.type == ColType.DOUBLE:
            return 0.0
        return ""


class TbIndex(object):
    is_unique = False
    cols = ()

    def __init__(self, **kwargs):
        for k, v in kwargs.items():
            self.__dict__[k] = v


class TbBase(object):

    tb_name = ""

    def __init__(self):
        columns = getattr(self, "_columns", None)
        for tb_col in columns:
            self.__dict__[tb_col.name] = None

    def __getitem__(self, item):
        if item in self._column_names:
            return self.__dict__[item]
        raise Exception("table {} not found column {}".format(self.tb_name, item))

    def __setitem__(self, item, val):
        if item in self._column_names:
            tb_col = self._column_dict.get(item)
            if tb_col.type in (ColType.INT, ColType.BIGINT):
                self.__dict__[item] = int(val)
            elif tb_col.type == ColType.DOUBLE:
                self.__dict__[item] = float(val)
            else:
                self.__dict__[item] = val
            return
        raise Exception("table {} not found column {}".format(self.tb_name, item))

    @property
    def primary_val(self):
        return self[self._primary_col.name]

    @property
    def primary_col(self):
        return self._primary_col

    @property
    def all_index(self):
        return self._indexs

    @property
    def all_column_names(self):
        return self._column_names

    def init_with_default(self):
        for tb_col in self._columns:
            if self[tb_col.name] is not None:
                continue
            self.__dict__[tb_col.name] = tb_col.default_val

    def get_indexs_by_col(self, col_name):
        return self._column_indexs.get(col_name, ())

    def match_index(self):
        valid_cols = []
        for tb_col in self._columns:
            if self.__dict__[tb_col.name] is None:
                continue
            valid_cols.append(tb_col.name)

        max_match_count = 0
        tgt_index = None
        for tb_index in self._indexs:
            match_count = 0
            for col_name in tb_index.cols:
                if col_name not in valid_cols:
                    continue
                match_count += 1
            if max_match_count < match_count:
                tgt_index = tb_index
                max_match_count = match_count
        return tgt_index

    def fiter(self, tbl_val):
        for tb_col in self._columns:
            val = self[tb_col.name]
            if val is None:
                continue
            if tbl_val[tb_col.name] != val:
                return False
        return True

    def clone(self, col_names=None):
        new_obj = copy.copy(self)
        if col_names is not None:
           for col_name in self._column_names:
               if col_name not in col_names:
                   new_obj.__dict__[col_name] = None
        return new_obj

    def make_redis_pri_key(self):
        return "{}:{}".format(self.tb_name, self.primary_val)

    def make_redis_index_key(self, tbl_index):
        redis_key = self.tb_name + ":"
        col_count = len(tbl_index.cols)
        for i in range(col_count):
            col_name = tbl_index.cols[i]
            redis_key += col_name + ":" + self[col_name]
            if i != col_count - 1:
                redis_key += ":"
        return redis_key

    def dump_cols(self):
        for col_name in self._column_names:
            print("{}: type({}), val:{}".format(col_name, type(self[col_name]), self[col_name]))


def init_columns(cls):
    _column_names = []
    _column_dict = {}
    _column_indexs = {}
    cls._primary_col = None
    for tb_col in cls._columns:
        _column_names.append(tb_col.name)
        _column_dict[tb_col.name] = tb_col
        if tb_col.key:
            if cls._primary_col is None:
                cls._primary_col = tb_col
            else:
                assert False, "table {} not set primary key".format(cls.tb_name)
        for tb_index in cls._indexs:
            if tb_col.name in tb_index.cols:
                if tb_col.name not in _column_indexs:
                    _column_indexs[tb_col.name] = []
                _column_indexs[tb_col.name].append(tb_index)

    cls._column_names = tuple(_column_names)
    cls._column_dict = _column_dict
    cls._column_indexs = _column_indexs


def create_tbl_obj(tb_name):
    import game.db.tbl
    return game.db.tbl.__dict__["tbl_" + tb_name].__dict__["Tbl" + tb_name.capitalize()]()


def get_tbl_name_from_key(key):
    pos = key.find(":")
    return key[0:pos]

# def tb_col(**kwargs):
#     def wrap_func(col_func):
#         def func(*args):
#             print("call func", kwargs)
#             col_func(*args)
#         return func
#
#     print("add col ", kwargs)
#     TbBase.add_col(**kwargs)
#     return wrap_func

