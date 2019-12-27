
{% for file_name in proto_files %}
import proto.{{ file_name }}_pb2
{% endfor %}

class message:
{% for item in msg_def %}
    {{ item[0] }} = {{ item[1] }}
{% endfor %}

    _MsgId2Msg = {
{% for item in msg_def %}
        {{ item[0] }}: proto.{{ item[4] }}_pb2.{{ item[2] }},
{% endfor %}
    }
    
    @staticmethod
    def create_msg_by_id(msg_id):
        msg_cls = message._MsgId2Msg.get(msg_id, None)
        if msg_cls is None:
            return
        return msg_cls()

