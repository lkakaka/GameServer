import os
import re
from jinja2 import Environment
from jinja2 import FileSystemLoader

PROTO_PATH = "./proto"
OUTPUT_PATH = "../../engine/ProtoBuffer"
JAVA_OUTPUT_PATH = "../robot/JavaRobot/src/main/java"
PY_OUTPUT_PATH = "../../script/python"
COCOS_PATH = "../../../CocosGameDemo/Classes/proto"
LUA_OUTPUT_PATH = "../../script/lua"


class Proto(object):

    def __init__(self, proto_name, full_file_name):
        self.proto_name = proto_name
        self.full_file_name = full_file_name
        self.msg_id = 0
        self.is_server_only = full_file_name.find("server_only") >= 0
        self._gen_org_file_name()
        self._gen_msg_id_var()

    def _gen_org_file_name(self):
        # match_obj = re.search("/?(\w+).proto$", self.full_file_name)
        # self.org_file_name = match_obj.group(1)
        self.org_file_name = self.full_file_name.replace(PROTO_PATH + "\\", "").replace(PROTO_PATH + "/", "").replace(".proto", "")
        # print("org file name----", self.full_file_name, self.org_file_name)
        self.py_file_name = self.org_file_name.replace("/", ".")
        self.java_file_name = self.org_file_name.capitalize()    # 首字母转换成大写

    def _gen_msg_id_var(self):
        def match_func(matched):
            value = matched.group('value')
            if value.isupper():
                return "_" + value
            return value

        msg_id_var = re.sub('(?P<value>\w)', match_func, self.proto_name)
        self.msg_id_var = "MSG_ID{}".format(msg_id_var.upper())


class RenderObj(object):

    def __init__(self):
        self.proto_list = []
        self.proto_files = []
        self.java_proto_files = []
        self.py_proto_files = []

    def add_proto_file(self, proto_file):
        if proto_file in self.proto_files:
            return
        self.proto_files.append(proto_file)

    def add_py_proto_file(self, proto_file):
        if proto_file in self.py_proto_files:
            return
        self.py_proto_files.append(proto_file)

    def add_java_proto_file(self, proto_file):
        if proto_file in self.java_proto_files:
            return
        self.java_proto_files.append(proto_file)


class ProtoBuilder(object):
    env = Environment(loader=FileSystemLoader('templates', 'utf-8'), trim_blocks=True)
    _msg_id = 0
    proto_list = []

    @staticmethod
    def alloc_msg_id():
        ProtoBuilder._msg_id += 1
        return ProtoBuilder._msg_id

    @staticmethod
    def make_proto():
        for path, dir_names, file_list in os.walk(PROTO_PATH):
            print(path, dir_names, file_list)
            for file_name in file_list:
                ProtoBuilder.make_proto_file(path, file_name)
                # ProtoBuilder.make_proto_file("{}/{}".format(path, file_name))
                # ProtoBuilder.make_proto_file(path, file_name)

        ProtoBuilder.gen_proto_file()

    @staticmethod    
    def make_proto_file(path, file_name):
        print(path, file_name)
        if not file_name.endswith(".proto"):
            return

        file_full_name = "{}/{}".format(path, file_name)
        # file_name = PROTO_PATH + "/" + file_name
        print("make proto file: " + file_full_name)
        os.system("protoc --cpp_out={} --proto_path=./ --proto_path={} {}".format(OUTPUT_PATH, PROTO_PATH, file_full_name))
        os.system("protoc --python_out={} --proto_path=./ --proto_path={} {}".format(PY_OUTPUT_PATH, PROTO_PATH, file_full_name))
        os.system("protoc -o {}/{}.pb {}".format(LUA_OUTPUT_PATH, file_full_name[0:-6], file_full_name))
        if file_full_name.find("server_only") < 0:
            os.system("protoc --java_out={} --proto_path=./ --proto_path={} {}".format(JAVA_OUTPUT_PATH, PROTO_PATH, file_full_name))
            if os.path.exists(COCOS_PATH):
                os.system("protoc --cpp_out={} --proto_path=./ --proto_path={} {}".format(COCOS_PATH, PROTO_PATH, file_full_name))

        with open(file_full_name) as f:
            for line in f.readlines():
                match_obj = re.search(".*message\s+([\w\d]+).*", line)
                if match_obj:
                    proto_name = match_obj.group(1)
                    if proto_name.startswith("_"):
                        continue
                    # if proto_name in ProtoBuilder.proto_list:
                    #     raise Exception("proto {} duplicate define!!!!!".format(proto_name))
                    ProtoBuilder.proto_list.append(Proto(proto_name, file_full_name))

    @staticmethod
    def gen_proto_file():
        def cmp_key_func(proto_obj):
            return proto_obj.proto_name

        ProtoBuilder.proto_list.sort(key=cmp_key_func)
        # print(ProtoBuilder.proto_list)
        template_render_obj = RenderObj()
        c_template_render_obj = RenderObj()
        for proto_obj in ProtoBuilder.proto_list:
            msg_id = ProtoBuilder.alloc_msg_id()
            proto_obj.msg_id = msg_id

            template_render_obj.proto_list.append(proto_obj)
            template_render_obj.add_proto_file(proto_obj.org_file_name)
            template_render_obj.add_py_proto_file(proto_obj.py_file_name)
            template_render_obj.add_java_proto_file(proto_obj.java_file_name)

            if not proto_obj.is_server_only:
                c_template_render_obj.proto_list.append(proto_obj)
                c_template_render_obj.add_proto_file(proto_obj.org_file_name)
                c_template_render_obj.add_py_proto_file(proto_obj.py_file_name)
                c_template_render_obj.add_java_proto_file(proto_obj.java_file_name)

        ProtoBuilder.render_file("proto_template.h", OUTPUT_PATH + "/proto.h", template_render_obj)
        ProtoBuilder.render_file("proto_template.cpp", OUTPUT_PATH + "/proto.cpp", template_render_obj)
        ProtoBuilder.render_file("proto_template.java", JAVA_OUTPUT_PATH + "/com/proto/ProtoBufferMsg.java", c_template_render_obj)
        ProtoBuilder.render_file("proto_template.py", PY_OUTPUT_PATH + "/proto/pb_message.py", template_render_obj)
        ProtoBuilder.render_file("proto_template.lua", LUA_OUTPUT_PATH + "/proto/pb_message.lua", template_render_obj)
        if os.path.exists(COCOS_PATH):
            ProtoBuilder.render_file("proto_template.h", COCOS_PATH + "/proto.h", c_template_render_obj)
            ProtoBuilder.render_file("proto_template.cpp", COCOS_PATH + "/proto.cpp", c_template_render_obj)

    @staticmethod
    def render_file(template_file, save_file, render_obj):
        template = ProtoBuilder.env.get_template(template_file)
        result = template.render(render_obj=render_obj)
        with open(save_file, "w", encoding="utf-8") as f:
            f.write(result)


if __name__ == "__main__":
    ProtoBuilder.make_proto()
