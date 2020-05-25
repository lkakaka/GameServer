# import util.logger


class ColType(object):
    INT = 0
    BIGINT = 1
    DOUBLE = 2
    VARCHAR = 3
    TEXT = 4


class TbCol(object):
    name = ""
    type = 0
    key = False
    length = 0
    default = None


class TbIndex(object):
    is_unique = False
    cols = ()


class TbBase(object):

    col_schema = []
    col_names = []
    tb_index = []
    tb_name = ""

    def __init__(self):
        for name in self.col_names:
            self.__dict__[name] = None


def add_col(cls, **kwargs):
    name = kwargs.get("name")
    assert name, "not define col name {}".format(name)
    assert name not in cls.col_names, "repeated define col {}".format(name)
    tb_col = TbCol()
    for k, v in kwargs.items():
        tb_col.__dict__[k] = v
    cls.col_names.append(name)
    cls.col_schema.append(tb_col)


def add_index(cls, cols, is_unique):
    dict = {}
    for col_name in cols:
        if col_name in dict:
            print("index col {} repeated".format(col_name))
            return
        dict[col_name] = True
        assert col_name in cls.col_names, "index col {} not table col".format(col_name)
    index = TbIndex()
    index.cols = cols
    index.is_unique = is_unique
    cls.tb_index.append(index)


def tb_col(**kwargs):
    def wrap_func(col_func):
        def func(*args):
            print("call func", kwargs)
            col_func(*args)
        return func

    print("add col ", kwargs)
    TbBase.add_col(**kwargs)
    return wrap_func

