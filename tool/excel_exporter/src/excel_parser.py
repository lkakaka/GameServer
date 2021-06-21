
import xlrd
from sheet_parser import SheetParser


class ExcelParser:

    def __init__(self, excel_file_name):
        self._excel_file_name = excel_file_name

    def parse(self):
        with xlrd.open_workbook(filename=self._excel_file_name) as wb:
            names = wb.sheet_names()
            for name in names:
                sheet = wb.sheet_by_name(name)
                SheetParser(sheet).parse_sheet()



