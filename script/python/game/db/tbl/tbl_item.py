
import util.db_util
from util.db_util import TbCol
from util.db_util import TbIndex
from util.db_util import ColType


class TblItem(util.db_util.TbBase):

    tb_name = "item"
    _columns = (
        TbCol(name="item_uid", type=ColType.BIGINT, key=True),
        TbCol(name="name", type=ColType.VARCHAR, length=128),
        TbCol(name="role_id", type=ColType.BIGINT),
        TbCol(name="item_id", type=ColType.INT),
        TbCol(name="count", type=ColType.INT),
    )

    Index_RoleId = ("role_id",)
    _indexs = (
        TbIndex(cols=Index_RoleId, is_unique=False),
    )

    def __init__(self):
        util.db_util.TbBase.__init__(self)

