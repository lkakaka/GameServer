#-*- ecoding:utf-8 -*-
from db.TbBase import TbBase


class TbPlayer(TbBase):

    id = 0
    name = ""
    age = 0
    
    def __init__(self):
        TbBase.__init__(self)
        self.id = 0
        self.name = ""
        self.age = 22
    

