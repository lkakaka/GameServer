#-*- ecoding:utf-8 -*-


class FieldType:
    INT = 0
    BIGINT = 1
    VARCHAR = 2

class TbField:
    name = ""
    type = 0
    length = 0
    val = None

class TbBase:

    fields = []

    def __init__(self):
        self.fields = []

    def set_field(self, filed, val):
        setattr(self, field, val)

    def get_field(self, field, default_val=None):
        return getattr(self, field, default_val)

    def add_field(self, name, **kwargs):
        if self.fields.get(name, None) is not None:
            return
        tb_field = TbField()
        tb_field.name = name
        for k,v in kwargs.iteritems():
            tb_field[k] = v


    @staticmethod
    def tb_col(**kwargs):
        def func(col_func):
            print("call func", kwargs)
            col_func()
        
        return func
