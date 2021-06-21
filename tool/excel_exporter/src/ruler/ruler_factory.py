from ruler.default_ruler import DefaultRuler
from ruler.type_ruler import TypeRuler


class RulerName:
    RULER_PRIMARY = "primary"
    RULER_INDEX = "index"

    RULER_DEFAULT = "default"
    RULER_TYPE = "type"

    RULER_LIST = (RULER_PRIMARY, RULER_INDEX, RULER_DEFAULT, RULER_TYPE)

    RULE_CLS = {
        RULER_DEFAULT: DefaultRuler,
        RULER_TYPE: TypeRuler,
    }

    @staticmethod
    def check_valid_ruler(ruler):
        if ruler not in RulerName.RULER_LIST:
            raise Exception("unkown ruler:" + ruler)

    @staticmethod
    def get_ruler_cls(ruler):
        return RulerName.RULE_CLS.get(ruler)


class RulerFactory(object):

    @staticmethod
    def create_ruler(ruler_name, params):
        RulerName.check_valid_ruler(ruler_name)
        cls = RulerName.get_ruler_cls(ruler_name)
        return cls(ruler_name, params)
