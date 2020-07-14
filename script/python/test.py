import re

import game.db.tbl
import game.db.tbl.tbl_player
import game.db.tbl.tbl_item


# import game.db.tbl.tbl_test
from game import util


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

def p_f(func):
    for x in range(1, 10):
        if func(x):
            print(x)

p_f(lambda x: x % 2 == 1)

print("xxxx", game.util.db_util.get_tbl_name_from_key("player:1"))
s = "tbl_player"
pos = s.find("_")
print("dfdas", s[pos+1:])

print(game.db.tbl.__dict__)
print(game.db.tbl.__dict__["tbl_player"].__name__)
tbl_player = game.db.tbl.__dict__["tbl_player"].__dict__["TblPlayer"]()
print("primary_col:", tbl_player.primary_val)
# tbl_player = game.db.tbl.tbl_player.TblPlayer()
tbl_item = game.db.tbl.tbl_item.TblItem()
tbl_item_clone = tbl_item.clone()
game.util.db_util.create_tbl_obj(tbl_item.tb_name)
print(game.db.tbl.tbl_player.TblPlayer.__dict__)
print(tbl_player.__dict__)
print(game.db.tbl.tbl_item.TblItem.__dict__)
print(tbl_player.role_id)


def to_dash_name(param_name):
    ts = re.sub(r"([A-Z])", r"_\1", param_name)
    print("111", ts)
    mt = re.findall(r"((_[^_]){2,})_?", ts)
    if mt:
        for m in mt:
            print(m[0], re.sub(r"_([^_])", r"\1", m[0]))
            s = '_' + re.sub(r"_([^_])", r"\1", m[0])
            ts = ts.replace(m[0], s)
            print(ts)
    ts = ts.replace('__', '_').lower()
    # ts = .lstrip('_')
    return ts

print(to_dash_name("grade_reward_a"))

lst = []
x = {"a":1, "b":2 ,"c":None}
lst.append(x)
t = tuple(lst)
print("x=", repr(t), eval(repr(t)))

# def co_func():
#     print("co 1")
#     x = yield 10
#     print("co 2", x)
#     y = yield 20
#     print("co 3", y)
#
# co = co_func()
# print("send none")
# print(co.send(None))
# # co.send(1)
# # print("send 1")
# # print(co.send(1))
# # co.send(2)


# async def compute(x, y):
#     print("Compute %s + %s ..." % (x, y))
#     await asyncio.sleep(1.0)
#     return x + y
#
#
# async def print_sum(x, y):
#     result = await compute(x, y)
#     print("%s + %s = %s" % (x, y, result))
#
# def co_loop(co_func, *p):
#     loop = asyncio.get_event_loop()
#     print("start")
#     # loop.run_until_complete(print_sum(1, 2))
#     loop.run_until_complete(co_func(*p))
#     print("end")
#     loop.close()
#
# co_loop(print_sum, 1, 2)


# future = asyncio.Future()
#
#
# async def coro1():
#     print("wait 1 second")
#     # await asyncio.sleep(1)
#     print("set_result")
#     future.set_result('data')
#
#
# async def coro2():
#     print("coro2")
#     result = await future
#     print(result)
#
#
# def co_loop():
#     loop = asyncio.get_event_loop()
#     loop.run_until_complete(asyncio.wait([
#         coro1(),
#         coro2()
#     ]))
#     loop.close()
#
# co_loop()

print("end co")
# coro1()

d = {"1" : [1, 2, 3]}
lst = d.get("1")
for i in lst:
    lst.remove(i)
    print("i=", i)

print(d)



def init_tables(mod):
    import os
    import sys
    tbls = []
    list_dir = os.walk(mod.__path__[0])
    for (dirPath, _, files) in list_dir:
        for fname in files:
           if not fname.endswith(".py") or not fname.startswith("tbl_"):
               continue
           fname = fname[0:-3]
           print("fname=", fname, mod.__name__)
           mod_name = mod.__name__ + "." + fname
           tb_mod = __import__(mod_name)

           print(sys.modules[mod_name].__dict__)
           # print(tb_mod.__dict__)
           pos = fname.find("_")
           cls_name = "Tbl" + fname[pos+1:].capitalize()
           mod_dict = sys.modules[mod_name].__dict__
           if cls_name not in mod_dict:
               raise RuntimeError("class {} not file {}".format(cls_name, fname))
           tbls.append(mod_dict[cls_name])

    # import os
    # dir = "game/db/tbl"
    # list_dir = os.walk("game/db/tbl")
    # for (dirPath, _, files) in list_dir:
    #     for fname in files:
    #        if fname.endswith(".py") and fname.startswith("tbl"):
    #            fname = fname[0:-3]
    #            print("fname=" + fname)
    #            __import__(dir.replace("/", ".") + "." + fname)
    #
    # tbls = []
    # # print("mod.__dict__=", mod.__dict__)
    # for k, v in mod.__dict__.items():
    #     if not k.startswith("tbl_"):
    #         continue
    #     # __import__(v.__name__)
    #     print(k)
    #     pos = k.find("_")
    #     cls_name = "Tbl" + k[pos+1:].capitalize()
    #     if cls_name not in mod.__dict__[k].__dict__:
    #         raise RuntimeError("class {} not file {}".format(cls_name, k))
    #     tbls.append(mod.__dict__[k].__dict__[cls_name])

    print(tbls)

# init_tables(game.db.tbl)
