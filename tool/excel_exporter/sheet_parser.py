# -*- coding: utf-8 -*-
import util

from jinja2 import Environment
from jinja2 import FileSystemLoader
from col_header import ColumnHeader
from ruler.ruler_factory import RulerName

ROW_IDX_DESC = 0
ROW_IDX_NAME = 1
ROW_IDX_RULER = 2
ROW_IDX_TYPE = 3
ROW_IDX_DAT = 4


class SheetParser(object):

    def __init__(self, sheet):
        self.sheet = sheet
        self.headers = []
        self.primary_index = [] # 主键包含的列
        self.indexes = {}   # 索引 {index_name:[列名1,...]}
        self.row_data = {}
        self.col_name_idx = {}  # {列名:列编号(下标)}
        self.index_data = {}    # {index_name: {[col_val,...] = 主键}}

    def parse_sheet(self):
        self.parse_header()
        self.parse_data()
        self.change_data_to_tuple()
        print(self.col_name_idx)
        self.render()

    def change_data_to_tuple(self):
        for index_name, index_data in self.index_data.items():
            for index_val, pri_val in index_data.items():
                index_data[index_val] = tuple(pri_val)

    def parse_header(self):
        desc_row = self.sheet.row_values(ROW_IDX_DESC)
        name_row = self.sheet.row_values(ROW_IDX_NAME)
        ruler_row = self.sheet.row_values(ROW_IDX_RULER)
        type_row = self.sheet.row_values(ROW_IDX_TYPE)
        if len(name_row) != len(type_row):
            raise Exception("name row length not equal type row!!!")
        idx = 0
        for col_type, col_name, ruler, desc in zip(type_row[0:], name_row[0:], ruler_row[0:], desc_row[0:]):
            self.col_name_idx[col_name] = idx
            idx += 1
            c_header = ColumnHeader()
            c_header.col_name = col_name
            c_header.col_type = col_type
            c_header.desc = desc
            ruler_dict = self.parse_ruler(ruler)
            ruler_dict[RulerName.RULER_TYPE] = c_header.col_type
            c_header.parse_ruler(ruler_dict)
            self.headers.append(c_header)
            self._parse_indexes(c_header, ruler_dict)

    def parse_ruler(self, ruler):
        if ruler.startswith("#"):
            return {}
        ruler_dict = util.parse_to_dict(ruler)
        return ruler_dict

    def _parse_indexes(self, c_header, ruler_dict):
        pri = ruler_dict.get(RulerName.RULER_PRIMARY)
        if pri:
            self.primary_index.append(c_header.col_name)

        index_names = ruler_dict.get(RulerName.RULER_INDEX)
        if index_names:
            arr_index = index_names.split("|")
            for index_name in arr_index:
                index_name = "Index_" + index_name
                self.add_index_col_name(index_name, c_header.col_name)
                c_header.in_indexes.append(index_name)

    def add_index_col_name(self, index_name, col_name):
        if index_name not in self.indexes:
            self.indexes[index_name] = []
        self.indexes[index_name].append(col_name)

    def parse_data(self):
        print("row count={0}".format(self.sheet.nrows))
        for idx in range(ROW_IDX_DAT, self.sheet.nrows):
            self.parse_row(idx)

    def parse_row(self, row_idx):
        row_data = []
        row = self.sheet.row(row_idx)
        for col_idx in range(0, len(row)):
            cell = row[col_idx]
            # print(self.headers, col_idx)
            c_header = self.headers[col_idx]
            val = c_header.fix_col_val(cell.value)
            c_header.check_col_val(val)
            row_data.append(val)
        row_data = tuple(row_data)
        # 获取主键值
        pri_val = self._get_primary_index_val(row_data, row_idx - ROW_IDX_DAT + 1)
        if pri_val in self.row_data:
            raise Exception("table[{0}], primary index[{1}], val repeated:{1}".format(self.sheet.name, self.primary_index, pri_val))
        self.row_data[pri_val] = row_data
        self.parse_index_data(row_data, pri_val)

    def _get_primary_index_val(self, row_data, row_idx):
        if not self.primary_index:
            return row_idx
        return self._get_index_val(self.primary_index, row_data)

    # 获取row_data索引值
    def _get_index_val(self, index_cols, row_data):
        index_val = []
        for col_name in index_cols:
            col_idx = self.col_name_idx[col_name]
            index_val.append(row_data[col_idx])
        if len(index_val) == 1:
            return index_val[0]
        return tuple(index_val)

    def parse_index_data(self, row_data, pri_val):
        for index_name, col_names in self.indexes.items():
            # 获取行的索引值
            index_val = self._get_index_val(col_names, row_data)
            self._add_index_data(index_name, index_val, pri_val)

    def _add_index_data(self, index_name, index_val, pri_val):
        if index_name not in self.index_data:
            self.index_data[index_name] = {}
        if index_val not in self.index_data[index_name]:
            self.index_data[index_name][index_val] = []
        self.index_data[index_name][index_val].append(pri_val)

    def render(self):
        render_ctx = {}
        render_ctx["table_name"] = self.sheet.name
        render_ctx["row_data"] = self.row_data
        render_ctx["index_data"] = self.index_data
        render_ctx["col_name_idx"] = self.col_name_idx
        env = Environment(loader=FileSystemLoader(
            'templates'), trim_blocks=True)
        template = env.get_template("template.py")
        result = template.render(**render_ctx)
        tb_name = "cfg_{0}".format(self.sheet.name.lower())
        with open("../../script/python/game/data/{0}.py".format(tb_name), "w", encoding="utf-8") as f:
            f.write(result)
