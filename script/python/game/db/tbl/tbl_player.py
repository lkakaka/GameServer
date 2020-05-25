
import util.db_util
COL_TYPE = util.db_util.ColType


class TblPlayer(util.db_util.TbBase):

    tb_name = "player"
    col_schema = []
    col_names = []

    def __init__(self):
        util.db_util.TbBase.__init__(self)

util.db_util.add_col(TblPlayer, name="role_id", type=COL_TYPE.BIGINT, key=True)
util.db_util.add_col(TblPlayer, name="name", type=COL_TYPE.VARCHAR, length=128, default="")

INDEX_NAME = ("name",)
util.db_util.add_index(TblPlayer, INDEX_NAME, True)
