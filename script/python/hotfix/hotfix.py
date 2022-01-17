import inspect
import sys
import types
import hashlib
import game.util.logger

MOD_MD5 = {}


def update_function(old_func, new_func):
    if not both_instance_of(old_func, new_func, types.FunctionType):
        return
    # print("update function", old_func.__name__)
    if len(old_func.__code__.co_freevars) != len(new_func.__code__.co_freevars):
        print("update_function fail, co_freevars len mismatch", old_func.__name__)
        return
    old_func.__code__ = new_func.__code__
    old_func.__defaults__ = new_func.__defaults__
    old_func.__doc__ = new_func.__doc__
    old_func.__dict__ = new_func.__dict__
    if not old_func.__closure__ or not new_func.__closure__:
        return
    for old_cell, new_cell in zip(old_func.__closure__, new_func.__closure__):
        if not both_instance_of(old_cell.cell_contents, new_cell.cell_contents, types.FunctionType):
            continue
        update_function(old_cell.cell_contents, new_cell.cell_contents)


def both_instance_of(first, second, klass):
    return isinstance(first, klass) and isinstance(second, klass)


def update_class(old_cls, new_cls):
    for name, new_attr in new_cls.__dict__.items():
        old_attr = old_cls.__dict__.get(name, None)
        if not old_attr:
            setattr(old_cls, name, new_attr)
            continue

        if both_instance_of(new_attr, old_attr, types.FunctionType):
            update_function(old_attr, new_attr)
        elif both_instance_of(old_attr, new_attr, staticmethod):
            update_function(old_attr.__func__, new_attr.__func__)
        elif both_instance_of(old_attr, new_attr, classmethod):
            update_function(old_attr.__func__, new_attr.__func__)
        elif both_instance_of(old_attr, new_attr, property):
            update_function(old_attr.fdel, new_attr.fdel)
            update_function(old_attr.fget, new_attr.fget)
            update_function(old_attr.fset, new_attr.fset)
        elif both_instance_of(old_attr, new_attr, type):
            update_class(old_attr, new_attr)


def _update_module(old_module, new_module):
    for name, new_val in new_module.__dict__.items():
        if name not in old_module.__dict__:
            setattr(old_module, name, new_val)
        else:
            old_val = old_module.__dict__[name]
            if both_instance_of(old_val, new_val, types.FunctionType):
                update_function(old_val, new_val)
            elif both_instance_of(old_val, new_val, type):
                update_class(old_val, new_val)


def update_module(mod_name):
    old_mod = sys.modules.pop(mod_name)  # Not reload(hotfix)
    __import__(mod_name)  # Not hotfix = __import__('hotfix')
    new_mod = sys.modules.get(mod_name)
    # print(old_mod, new_mod)
    _update_module(old_mod, new_mod)
    sys.modules[mod_name] = old_mod
    game.util.logger.log_info("hotfix mod: {}", mod_name)


def start_hotfix():
    game.util.logger.log_info("start hotfix!!!")

    for dir_path, file_name, mod_name in for_each_file():
        # game.util.logger.log_info("{}-{}".format(file_name, mod_name))
        if sys.modules.get(mod_name) is None:
            continue
        # game.util.logger.log_info("--{}".format(file_name))
        with open(dir_path + "/" + file_name, 'rb') as fp:
            data = fp.read()
            file_md5 = hashlib.md5(data).hexdigest()
            if MOD_MD5[mod_name] != file_md5:
                update_module(mod_name)
                MOD_MD5[mod_name] = file_md5

    game.util.logger.log_info("hotfix finish!!!")


def for_each_file():
    import os
    list_dir = os.walk(os.path.dirname(__file__) + "/..")
    for (dir_path, _, files) in list_dir:
        if dir_path.find("google") > 0 or dir_path.find("__pycache__") > 0:
            continue
        # print(dir_path)
        pos = dir_path.find("..")
        if pos < 0:
            print("file path error")
        pkg_path = dir_path[pos + 3:].replace("\\", ".")
        pkg_path = pkg_path.replace("/", ".")
        if pkg_path != "":
            pkg_path += "."
        # print(pkg_path)
        for file_name in files:
            if not file_name.endswith(".py") or file_name == "__init__.py":
                continue
            mod_name = pkg_path + file_name[0:-3]
            yield dir_path, file_name, mod_name


def on_server_start():
    for dir_path, file_name, mod_name in for_each_file():
        with open(dir_path + "/" + file_name, 'rb') as fp:
            data = fp.read()
            file_md5 = hashlib.md5(data).hexdigest()
            MOD_MD5[mod_name] = file_md5
    # print(MOD_MD5)
