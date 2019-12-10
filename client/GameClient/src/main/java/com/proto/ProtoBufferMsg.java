package com.proto;

public class ProtoBufferMsg {
	public static final int MSG_ID_LOGIN = 1;
	public static final int MSG_ID_TEST = 2;

	public static Object createMsgById(int msgId, byte[] dat) {
		try {
			switch (msgId)
			{
				case MSG_ID_LOGIN:
					return LoginOuterClass.Login.parseFrom(dat);
				case MSG_ID_TEST:
					return TestOuterClass.Test.parseFrom(dat);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return null;
	}

	public static Object createBuilderById(int msgId) {
		switch (msgId) {
			case MSG_ID_LOGIN:
				return LoginOuterClass.Login.newBuilder();
			case MSG_ID_TEST:
				return TestOuterClass.Test.newBuilder();
		}
		return null;
	}
	
	public static LoginOuterClass.Login.Builder createLoginBuilder() {
		return LoginOuterClass.Login.newBuilder();
	}
	
	public static TestOuterClass.Test.Builder createTestBuilder() {
		return TestOuterClass.Test.newBuilder();
	}
	
}

