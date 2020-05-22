
import util.db_util
ADD_COL = util.db_util.TbBase.add_col
COL_TYPE = util.db_util.ColType


class TblItem(util.db_util.TbBase):

    tb_name = "item"
    # ADD_COL("role_id", type=COL_TYPE.BIGINT, key=True)
    # ADD_COL("name", type=COL_TYPE.VARCHAR, length=128)

    @util.db_util.tb_col(name="item_id", type=COL_TYPE.BIGINT, key=True)
    def _role_id(self):
        pass

    def __init__(self):
        util.db_util.TbBase.__init__(self)
        for name in TblItem.col_names:
            self.__dict__[name] = None