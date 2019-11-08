#-*- ecoding:utf-8 -*-

class TbField:
    field_name = ""
    field_type = 0
    field_val = None

class TbBase:

    fields = []

    def set_field(self, filed, val):
        setattr(self, field, val)

    def get_field(self, field):
        return getattr(self, field)
