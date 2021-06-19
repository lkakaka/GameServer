# -*- coding: utf-8 -*-
from ruler.ruler_base import RulerBase
from ruler.ruler_base import RulerPriority
from col_type import ColType


# 类型规则
class TypeRuler(RulerBase):
    def __init__(self, ruler_name, ruler_val):
        RulerBase.__init__(self, ruler_name, ruler_val)
        self.ruler_pri = RulerPriority.RULER_TYPE

    def fix_val(self, val):
        if self.ruler_val == ColType.INT:
            return self.fix_int_val(val)
        if self.ruler_val == ColType.STRING:
            return self.fix_str_val(val)
        if self.ruler_val == ColType.FLOAT:
            return self.fix_float_val(val)

    def fix_int_val(self, val):
        # print("fix int val:{0},{1}".format(val, type(val)))
        if type(val) is int:
            return val
        return int(val)

    def fix_str_val(self, val):
        return val

    def fix_float_val(self, val):
        if type(val) is int or type(val) is float:
            return val
        return float(val)
