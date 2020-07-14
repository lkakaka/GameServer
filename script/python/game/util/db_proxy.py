
import weakref


class DBProxy(object):

    def __init__(self, service_obj):
        self._service_obj = weakref.ref(service_obj)

    def load(self, tb_name, **kwargs):
        return self._service_obj().rpc_call("db", "RpcLoadDB", tb_name=tb_name, **kwargs)

    def load_multi(self, tbls):
        if type(tbls) not in (list, tuple):
            tbls = (tbls,)
        tbl_lst = []
        for tbl in tbls:
            tbl_lst.append(tbl.to_dict(cols=None))
        return self._service_obj().rpc_call("db", "RpcLoadDBMulti", tbl_list=tuple(tbl_lst))

    def insert(self, tbls):
        if type(tbls) not in (list, tuple):
            tbls = (tbls,)
        tbl_lst = []
        for tbl in tbls:
            tbl_lst.append(tbl.to_dict(cols=None))
        return self._service_obj().rpc_call("db", "RpcInsertDB", tbl_list=tuple(tbl_lst))

    def update(self, tbls, cols=None):
        if type(tbls) not in (list, tuple):
            tbls = (tbls,)
        tbl_lst = []
        for tbl in tbls:
            tbl_lst.append(tbl.to_dict(cols=cols))
        return self._service_obj().rpc_call("db", "RpcUpdateDB", tbl_list=tuple(tbl_lst))

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

    def delete(self, tbls, cols=None):
        if type(tbls) not in (list, tuple):
            tbls = (tbls,)
        tbl_lst = []
        for tbl in tbls:
            tbl_lst.append(tbl.to_dict(cols=cols))
        return self._service_obj().rpc_call("db", "RpcDeleteDB", tbl_list=tuple(tbl_lst))
