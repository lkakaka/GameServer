# -*- coding: utf-8 -*-

def parse_to_dict(str):
    if not str:
        return {}
    try:
        dict = {}
        entry_arr = str.split(",")
        for entry in entry_arr:
            key_val = entry.split("=")
            dict[key_val[0]] = key_val[1]
    except Exception as e:
        print("parse str to dict error!!, str={0}".format(str))
        raise e

    return dict
