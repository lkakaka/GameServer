from ruler.ruler_factory import RulerFactory
from ruler.ruler_factory import RulerName

SIDE_ALL = 0
SIDE_SERVER = 1
SIDE_CLIENT = 2


class ColumnHeader:
    def __init__(self):
        self.col_name = ''
        self.col_type = ''
        self.def_val = None
        self.side = SIDE_ALL
        self.desc = ""
        self.ruler_list = []
        self.rulers = {}
        self.in_indexes = []

    def parse_ruler(self, ruler_dict):
        for ruler_name, v in ruler_dict.items():
            if ruler_name == RulerName.RULER_PRIMARY or ruler_name == RulerName.RULER_INDEX:
                continue
            self.add_ruler(RulerFactory.create_ruler(ruler_name, v))
        self.sort_ruler()

    def add_ruler(self, ruler):
        self.ruler_list.append(ruler)
        if ruler.ruler_name in self.rulers:
            raise Exception("ruler repeated!! ruler_name:{0}".format(ruler.ruler_name))
        self.rulers[ruler.ruler_name] = ruler

    def sort_ruler(self):
        self.ruler_list.sort(key=lambda ruler: ruler.ruler_pri)

    def fix_col_val(self, val):
        for rule in self.ruler_list:
            new_val = rule.fix_val(val)
            if new_val is not None:
                val = new_val
        return val

    def check_col_val(self, val):
        for rule in self.ruler_list:
            rule.check_val(val)
