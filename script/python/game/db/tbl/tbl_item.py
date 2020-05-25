
import util.db_util
COL_TYPE = util.db_util.ColType


class TblItem(util.db_util.TbBase):

    tb_name = "item"
    col_schema = []
    col_names = []
    tb_index = []

    def __init__(self):
        util.db_util.TbBase.__init__(self)

util.db_util.add_col(TblItem, name="item_id", type=COL_TYPE.BIGINT, key=True)
util.db_util.add_col(TblItem, name="name", type=COL_TYPE.VARCHAR, length=128)
