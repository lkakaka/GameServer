import os
import re
from jinja2 import Environment
from jinja2 import FileSystemLoader

PROTO_PATH = "./proto"
OUTPUT_PATH = "../ProtoBuffer"
JAVA_OUTPUT_PATH="./robot/JavaRobot/src/main/java"
PY_OUTPUT_PATH="../script/python/proto"
COCOS_PATH = "../../CocosGameDemo/Classes/proto"

class ProtoBuilder():
    env = Environment(loader=FileSystemLoader(
                'templates'), trim_blocks=True)
    _msg_id = 0
    proto_list = []
    render_obj = {}

    @staticmethod
    def alloc_msg_id():
        ProtoBuilder._msg_id += 1
        return ProtoBuilder._msg_id

    @staticmethod
    def make_proto():
        ProtoBuilder.render_obj["msg_def"] = []
        ProtoBuilder.render_obj["proto_files"] = []
        ProtoBuilder.render_obj["java_proto_files"] = []

        for path, dir_names,file_list in os.walk(PROTO_PATH):
            print(path, dir_names, file_list)
            for file_name in file_list:
                ProtoBuilder.make_proto_file("{}/{}".format(path, file_name))

        ProtoBuilder.gen_proto_file()

    @staticmethod    
    def make_proto_file(file_name):
        if not file_name.endswith(".proto"):
            return

        print("make proto file: " + file_name)
        os.system("protoc --cpp_out {} --proto_path {} {}".format(OUTPUT_PATH, PROTO_PATH, file_name))
        os.system("protoc --java_out {} --proto_path {} {}".format(JAVA_OUTPUT_PATH, PROTO_PATH, file_name))
        os.system("protoc --python_out {} --proto_path {} {}".format(PY_OUTPUT_PATH, PROTO_PATH, file_name))
        if os.path.exists(COCOS_PATH):
            os.system("protoc --cpp_out {} --proto_path {} {}".format(COCOS_PATH, PROTO_PATH, file_name))
        match_obj = re.search("/?(\w+).proto$", file_name)
        ProtoBuilder.render_obj["proto_files"].append(match_obj.group(1))
        org_file_name = match_obj.group(1)
        java_file_name = match_obj.group(1).capitalize()    # 首字母转换成大写
        ProtoBuilder.render_obj["java_proto_files"].append(java_file_name) 

        with open(file_name) as f:
            for line in f.readlines():
                match_obj = re.search(".*message\s+([\w\d]+).*", line)
                if match_obj:
                    proto_name = match_obj.group(1)
                    if proto_name in ProtoBuilder.proto_list:
                        print("proto {} duplicate define!!!!!".format(proto_name))
                        raise
                    ProtoBuilder.proto_list.append((proto_name, org_file_name))

    @staticmethod
    def gen_proto_file():
        # print(ProtoBuilder.proto_list)
        def cmp_key_func(elem):
            return elem[0]

        ProtoBuilder.proto_list.sort(key=cmp_key_func)
        # print(ProtoBuilder.proto_list)
        for proto_name, org_file_name in ProtoBuilder.proto_list:
            def match_func(matched):
                value = matched.group('value')
                if value.isupper():
                    return "_" + value
                return value
                    
            proto_id_name = re.sub('(?P<value>\w)', match_func, proto_name)
            msg_id = ProtoBuilder.alloc_msg_id()
            java_file_name = org_file_name.capitalize()    # 首字母转换成大写
            ProtoBuilder.render_obj["msg_def"].append(("MSG_ID{}".format(proto_id_name.upper()), msg_id, proto_name, java_file_name, org_file_name))
        
        ProtoBuilder.render_file("proto_template.h", OUTPUT_PATH + "/proto.h", ProtoBuilder.render_obj)
        ProtoBuilder.render_file("proto_template.cpp", OUTPUT_PATH + "/proto.cpp", ProtoBuilder.render_obj)
        ProtoBuilder.render_file("proto_template.java", JAVA_OUTPUT_PATH + "/com/proto/ProtoBufferMsg.java", ProtoBuilder.render_obj)
        ProtoBuilder.render_file("proto_template.py", PY_OUTPUT_PATH + "/message.py", ProtoBuilder.render_obj)
        if os.path.exists(COCOS_PATH):
            ProtoBuilder.render_file("proto_template.h", COCOS_PATH + "/proto.h", ProtoBuilder.render_obj)
            ProtoBuilder.render_file("proto_template.cpp", COCOS_PATH + "/proto.cpp", ProtoBuilder.render_obj)

    @staticmethod
    def render_file(template_file, save_file, render_obj):
        template = ProtoBuilder.env.get_template(template_file)
        result = template.render(**render_obj)
        with open(save_file, "w", encoding="utf-8") as f:
            f.write(result)

if __name__ == "__main__":
    ProtoBuilder.make_proto()
