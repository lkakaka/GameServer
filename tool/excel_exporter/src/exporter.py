
import os
from excel_parser import ExcelParser


class Exporter:

    @staticmethod
    def run():
        Exporter.walk_dir("../doc", lambda path, f_name: Exporter.handle_excel(path, f_name))

    @staticmethod
    def handle_excel(path, f_name):
        full_name = "{0}/{1}".format(path, f_name)
        print("read excel:{0}".format(full_name))
        ExcelParser(full_name).parse()

    @staticmethod
    def walk_dir(dir_path, callback=None):
        if not callback:
            return
        list_dir = os.walk(dir_path)
        for (dirPath, _, files) in list_dir:
            for f_name in files:
                callback(dirPath, f_name)
