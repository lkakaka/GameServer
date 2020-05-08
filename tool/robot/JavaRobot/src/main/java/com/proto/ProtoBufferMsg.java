package com.proto;

public class ProtoBufferMsg {
	public static final int MSG_ID_DISCONNECT = 1;
	public static final int MSG_ID_LOAD_ROLE_RSP = 3;
	public static final int MSG_ID_LOGIN_REQ = 4;
	public static final int MSG_ID_LOGIN_RSP = 5;
	public static final int MSG_ID_TEST_REQ = 8;

	public static Object createMsgById(int msgId, byte[] dat) {
		try {
			switch (msgId)
			{
				case MSG_ID_DISCONNECT:
					return Login.Disconnect.parseFrom(dat);
				case MSG_ID_LOAD_ROLE_RSP:
					return Login.LoadRoleRsp.parseFrom(dat);
				case MSG_ID_LOGIN_REQ:
					return Login.LoginReq.parseFrom(dat);
				case MSG_ID_LOGIN_RSP:
					return Login.LoginRsp.parseFrom(dat);
				case MSG_ID_TEST_REQ:
					return Test.TestReq.parseFrom(dat);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return null;
	}

	public static Object createBuilderById(int msgId) {
		switch (msgId) {
			case MSG_ID_DISCONNECT:
				return Login.Disconnect.newBuilder();
			case MSG_ID_LOAD_ROLE_RSP:
				return Login.LoadRoleRsp.newBuilder();
			case MSG_ID_LOGIN_REQ:
				return Login.LoginReq.newBuilder();
			case MSG_ID_LOGIN_RSP:
				return Login.LoginRsp.newBuilder();
			case MSG_ID_TEST_REQ:
				return Test.TestReq.newBuilder();
		}
		return null;
	}
	
	public static Login.Disconnect.Builder createDisconnectBuilder() {
		return Login.Disconnect.newBuilder();
	}
	
	public static Login.LoadRoleRsp.Builder createLoadRoleRspBuilder() {
		return Login.LoadRoleRsp.newBuilder();
	}
	
	public static Login.LoginReq.Builder createLoginReqBuilder() {
		return Login.LoginReq.newBuilder();
	}
	
	public static Login.LoginRsp.Builder createLoginRspBuilder() {
		return Login.LoginRsp.newBuilder();
	}
	
	public static Test.TestReq.Builder createTestReqBuilder() {
		return Test.TestReq.newBuilder();
	}
	
}

