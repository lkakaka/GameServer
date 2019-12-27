package com.proto;

public class ProtoBufferMsg {
{% for item in msg_def %}
	public static final int {{ item[0] }} = {{ item[1] }};
{% endfor %}

	public static Object createMsgById(int msgId, byte[] dat) {
		try {
			switch (msgId)
			{
{% for item in msg_def %}
				case {{ item[0] }}:
					return {{ item[3] }}OuterClass.{{ item[2] }}.parseFrom(dat);
{% endfor %}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return null;
	}

	public static Object createBuilderById(int msgId) {
		switch (msgId) {
{% for item in msg_def %}
			case {{ item[0] }}:
				return {{ item[3] }}OuterClass.{{ item[2] }}.newBuilder();
{% endfor %}
		}
		return null;
	}
	
{% for item in msg_def %}
	public static {{ item[3] }}OuterClass.{{ item[2] }}.Builder create{{ item[2] }}Builder() {
		return {{ item[3] }}OuterClass.{{ item[2] }}.newBuilder();
	}
	
{% endfor %}
}


