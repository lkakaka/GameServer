# import util.logger


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
    key = False
    length = 0
    default = None
    is_del = False

    def __init__(self, **kwargs):
        for k, v in kwargs.items():
            self.__dict__[k] = v


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
        if columns is not None:
            for tb_col in columns:
                self.__dict__[tb_col.name] = None

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

