
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
    val = None


class TbBase:

    col_schema = []
    col_names = []
    tb_name = ""

    def __init__(self):
        self.fields = []

    def set_field(self, field, val):
        setattr(self, field, val)

    def get_field(self, field, default_val=None):
        return getattr(self, field, default_val)

    @staticmethod
    def add_col(**kwargs):
        name = kwargs.get("name")
        if name in TbBase.col_names:
            return
        tb_col = TbCol()
        for k, v in kwargs.iteritems():
            tb_col.__dict__[k] = v
        TbBase.col_names.append(name)
        TbBase.col_schema.append(tb_col)

    @staticmethod
    def tb_col(**kwargs):
        def wrap_func(col_func):
            def func(*args):
                print("call func", kwargs)
                col_func(*args)
            return func

        print("add col ", kwargs)
        TbBase.add_col(**kwargs)
        return wrap_func

tb_col = TbBase.tb_col
