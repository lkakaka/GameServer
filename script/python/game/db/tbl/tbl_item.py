import game.util.db_util
from game.util.db_util import TbCol
from game.util.db_util import TbIndex
from game.util.db_util import ColType


class TblItem(game.util.db_util.TbBase):

    tb_name = "item"
    _columns = (
        TbCol(name="item_uid", type=ColType.BIGINT, key=True),
        TbCol(name="role_id", type=ColType.BIGINT),
        TbCol(name="item_id", type=ColType.INT),
        TbCol(name="count", type=ColType.INT),
    )

    Index_RoleId = ("role_id",)
    _indexs = (
        TbIndex(cols=Index_RoleId, is_unique=False),
    )

    def __init__(self):
        game.util.db_util.TbBase.__init__(self)

game.util.db_util.init_columns(TblItem)
