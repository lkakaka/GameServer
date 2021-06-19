# -*- coding: utf-8 -*-
from ruler.ruler_base import RulerBase
from ruler.ruler_base import RulerPriority


# 默认值规则
class DefaultRuler(RulerBase):

    def __init__(self, ruler_name, ruler_val):
        RulerBase.__init__(self, ruler_name, ruler_val)
        self.ruler_pri = RulerPriority.RULER_DEFAULT

    def fix_val(self, val):
        if val is not None:
            return val
        return self.ruler_val
