package com.proto;

public class ProtoBufferMsg {
	public static final int MSG_ID_LOGIN_REQ = 1;
	public static final int MSG_ID_LOGIN_RSP = 2;
	public static final int MSG_ID_RPC_MSG = 3;
	public static final int MSG_ID_RPC_MSG_RSP = 4;
	public static final int MSG_ID_TEST_REQ = 5;

	public static Object createMsgById(int msgId, byte[] dat) {
		try {
			switch (msgId)
			{
				case MSG_ID_LOGIN_REQ:
					return Login.LoginReq.parseFrom(dat);
				case MSG_ID_LOGIN_RSP:
					return Login.LoginRsp.parseFrom(dat);
				case MSG_ID_RPC_MSG:
					return Common.RpcMsg.parseFrom(dat);
				case MSG_ID_RPC_MSG_RSP:
					return Common.RpcMsgRsp.parseFrom(dat);
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
			case MSG_ID_LOGIN_REQ:
				return Login.LoginReq.newBuilder();
			case MSG_ID_LOGIN_RSP:
				return Login.LoginRsp.newBuilder();
			case MSG_ID_RPC_MSG:
				return Common.RpcMsg.newBuilder();
			case MSG_ID_RPC_MSG_RSP:
				return Common.RpcMsgRsp.newBuilder();
			case MSG_ID_TEST_REQ:
				return Test.TestReq.newBuilder();
		}
		return null;
	}
	
	public static Login.LoginReq.Builder createLoginReqBuilder() {
		return Login.LoginReq.newBuilder();
	}
	
	public static Login.LoginRsp.Builder createLoginRspBuilder() {
		return Login.LoginRsp.newBuilder();
	}
	
	public static Common.RpcMsg.Builder createRpcMsgBuilder() {
		return Common.RpcMsg.newBuilder();
	}
	
	public static Common.RpcMsgRsp.Builder createRpcMsgRspBuilder() {
		return Common.RpcMsgRsp.newBuilder();
	}
	
	public static Test.TestReq.Builder createTestReqBuilder() {
		return Test.TestReq.newBuilder();
	}
	
}

