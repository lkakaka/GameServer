import re
import game.db.tbl.tbl_player
import game.db.tbl.tbl_item
import util.db_util

# def match_func(matched):
#     value = matched.group('value')
#     print(value)
#     if value.isupper():
#         return "_" + value
#     return value
#
# proto_name = "LoginReq"
# print(re.sub('(?P<value>\w)', match_func, proto_name))


def test_kw(x, a=1):
    print("test kw--", a)

d = {'a': 10}
test_kw(0, **d)

tbl_player = game.db.tbl.tbl_player.TblPlayer()
tbl_item = game.db.tbl.tbl_item.TblItem()
print(tbl_player.__dict__)
print(tbl_player.role_id, tbl_player.item_id)
