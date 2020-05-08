
{% for file_name in render_obj.py_proto_files %}
import proto.{{ file_name }}_pb2
{% endfor %}


class Message:
{% for proto_obj in render_obj.proto_list %}
    {{ proto_obj.msg_id_var }} = {{ proto_obj.msg_id }}
{% endfor %}

    _MsgId2Msg = {
{% for proto_obj in render_obj.proto_list %}
        {{ proto_obj.msg_id_var }}: proto.{{ proto_obj.py_file_name }}_pb2.{{ proto_obj.proto_name }},
{% endfor %}
    }
    
    @staticmethod
    def create_msg_by_id(msg_id):
        msg_cls = Message._MsgId2Msg.get(msg_id, None)
        if msg_cls is None:
            return
        return msg_cls()

