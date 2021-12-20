
import weakref
from game.service.service_addr import LOCAL_DB_SERVICE_ADDR
from game.service.service_addr import ServiceAddr


class DBProxy(object):

    def __init__(self, service_obj):
        self._service_obj = weakref.ref(service_obj)

    def gen_db_addr(self, server_id):
        db_addr = LOCAL_DB_SERVICE_ADDR
        if server_id > 0:
            db_addr = ServiceAddr.make_db_addr(server_id)
        return db_addr

    def load(self, server_id, tb_name, **kwargs):
        return self._service_obj().rpc_call(self.gen_db_addr(server_id), "RpcLoadDB", tb_name=tb_name, **kwargs)

    def load_multi(self, server_id, tbls):
        if type(tbls) not in (list, tuple):
            tbls = (tbls,)
        tbl_lst = []
        for tbl in tbls:
            tbl_lst.append(tbl.to_dict(cols=None))
        return self._service_obj().rpc_call(self.gen_db_addr(server_id), "RpcLoadDBMulti", tbl_list=tuple(tbl_lst))

    def insert(self, server_id, tbls):
        if type(tbls) not in (list, tuple):
            tbls = (tbls,)
        tbl_lst = []
        for tbl in tbls:
            tbl_lst.append(tbl.to_dict(cols=None))
        return self._service_obj().rpc_call(self.gen_db_addr(server_id), "RpcInsertDB", tbl_list=tuple(tbl_lst))

    def update(self, server_id, tbls, cols=None):
        if type(tbls) not in (list, tuple):
            tbls = (tbls,)
        tbl_lst = []
        for tbl in tbls:
            tbl_lst.append(tbl.to_dict(cols=cols))
        return self._service_obj().rpc_call(self.gen_db_addr(server_id), "RpcUpdateDB", tbl_list=tuple(tbl_lst))

    # def update_sync(self, tb_name, tbls, cols=None):
    #     if type(tbls) not in (list, tuple):
    #         tbls = (tbls,)
    #     tbl_lst = []
    #     for tbl in tbls:
    #         tbl_lst.append(tbl.to_dict(cols=cols))
    #
    #     future = self._service_obj().rpc_call("db", "RpcUpdateDB", tb_name=tb_name, dat_list=tuple(tbl_lst))
    #     co = yield future
    #
    #     def on_finish(err_code, co=co):
    #         print("on update sync return---", err_code)
    #         co.send(err_code)
    #
    #     future.on_fin += on_finish
    #     future.on_timeout += on_finish
    #
    #     co.send
    #
    #     result = yield
    #     return result
    #
    #     # co = self._co_update_sync(tb_name, tbl_lst)
    #     #
    #     # # def on_finish(err_code, co=co):
    #     # #     print("on update sync return---", err_code)
    #     # #     co.send(err_code)
    #     #
    #     # future = co.send(None)
    #     # co.send(co)
    #     # # future.on_fin += on_finish
    #     # # future.on_timeout += on_finish

    # def create_co(self, func):
    #     co = func()
    #     co.send(None)
    #     co.send(co)
    #     return co
    #
    # def _co_update_sync(self, tb_name, tbl_lst):
    #     future = self._service_obj().rpc_call("db", "RpcUpdateDB", tb_name=tb_name, dat_list=tuple(tbl_lst))
    #     co = yield future
    #
    #     def on_finish(err_code, co=co):
    #         print("on update sync return---", err_code)
    #         co.send(err_code)
    #
    #     future.on_fin += on_finish
    #     future.on_timeout += on_finish
    #
    #     result = yield
    #     return result

    def delete(self, server_id, tbls, cols=None):
        if type(tbls) not in (list, tuple):
            tbls = (tbls,)
        tbl_lst = []
        for tbl in tbls:
            tbl_lst.append(tbl.to_dict(cols=cols))
        return self._service_obj().rpc_call(self.gen_db_addr(server_id), "RpcDeleteDB", tbl_list=tuple(tbl_lst))
