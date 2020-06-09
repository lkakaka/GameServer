
import util.db_util
from util.db_util import TbCol
from util.db_util import TbIndex
from util.db_util import ColType


class TblTest(util.db_util.TbBase):

    tb_name = "test"
    _columns = (
        TbCol(name="role_id", type=ColType.BIGINT, key=True, auto_incr=True),
        TbCol(name="role_name", type=ColType.VARCHAR, length=128, default=""),
        TbCol(name="account", type=ColType.VARCHAR, length=128),
    )

    INDEX_NAME = ("role_name",)
    INDEX_ACCOUNT = ("account",)
    _indexs = (
        TbIndex(cols=INDEX_NAME, is_unique=True),
        TbIndex(cols=INDEX_ACCOUNT, is_unique=False),
    )

    def __init__(self):
        util.db_util.TbBase.__init__(self)

util.db_util.init_columns(TblTest)
