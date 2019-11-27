#-*- ecoding:utf-8 -*-
from db.TbBase import TbBase


class TbPlayer(TbBase):

    id = 0
    name = ""
    age = 0
    
    def __init__(self):
        TbBase.__init__(self)
        # self.col_id = 0
        # self.col_idx = 1
        # self.col_name = ""
        # self.col_age = 1

    @staticmethod
    @TbBase.tb_col(type="int")
    def col_id():
        print("col_id func")

