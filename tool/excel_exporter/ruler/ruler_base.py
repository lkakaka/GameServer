# -*- coding: utf-8 -*-


# 规则优先级，数值越小，优先级越高
class RulerPriority(object):
    RULER_DEFAULT = 1   # 默认值规则优先级
    RULER_TYPE = 2      # 类型规则优先级

    RULER_DEFAULT_PRI = 100     # 默认优先级


class RulerBase(object):
    def __init__(self, ruler_name, ruler_val):
        self.ruler_name = ruler_name
        self.ruler_val = ruler_val  # string
        self.ruler_pri = RulerPriority.RULER_DEFAULT_PRI

    # 检查值
    def check_val(self, val):
        pass

    # 修正值
    def fix_val(self, val):
        return None
