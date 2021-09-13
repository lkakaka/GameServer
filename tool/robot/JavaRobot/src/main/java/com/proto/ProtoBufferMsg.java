// Generated by tool.  DO NOT EDIT!

package com.proto;

public class ProtoBufferMsg {
	public static final int MSG_ID_ACTOR_BORN = 1;
	public static final int MSG_ID_ACTOR_DISSOLVE = 2;
	public static final int MSG_ID_CREATE_ROLE_REQ = 4;
	public static final int MSG_ID_CREATE_ROLE_RSP = 5;
	public static final int MSG_ID_DISCONNECT_REQ = 6;
	public static final int MSG_ID_DISCONNECT_RSP = 7;
	public static final int MSG_ID_ENTER_GAME = 8;
	public static final int MSG_ID_ENTER_GAME_RSP = 9;
	public static final int MSG_ID_ENTER_SCENE_REQ = 10;
	public static final int MSG_ID_ENTER_SCENE_RSP = 11;
	public static final int MSG_ID_GM_CMD = 12;
	public static final int MSG_ID_GM_CMD_RSP = 13;
	public static final int MSG_ID_LOAD_ROLE_LIST_RSP = 14;
	public static final int MSG_ID_LOGIN_REQ = 15;
	public static final int MSG_ID_LOGIN_RSP = 16;
	public static final int MSG_ID_MOVE_TO = 17;
	public static final int MSG_ID_SEND_UDP_PORT = 20;
	public static final int MSG_ID_START_KCP = 21;
	public static final int MSG_ID_SYNC_POS = 23;
	public static final int MSG_ID_TEST_REQ = 24;

	public static Object createMsgById(int msgId, byte[] dat) {
		try {
			switch (msgId)
			{
				case MSG_ID_ACTOR_BORN:
					return Scene.ActorBorn.parseFrom(dat);
				case MSG_ID_ACTOR_DISSOLVE:
					return Scene.ActorDissolve.parseFrom(dat);
				case MSG_ID_CREATE_ROLE_REQ:
					return Login.CreateRoleReq.parseFrom(dat);
				case MSG_ID_CREATE_ROLE_RSP:
					return Login.CreateRoleRsp.parseFrom(dat);
				case MSG_ID_DISCONNECT_REQ:
					return Login.DisconnectReq.parseFrom(dat);
				case MSG_ID_DISCONNECT_RSP:
					return Login.DisconnectRsp.parseFrom(dat);
				case MSG_ID_ENTER_GAME:
					return Login.EnterGame.parseFrom(dat);
				case MSG_ID_ENTER_GAME_RSP:
					return Login.EnterGameRsp.parseFrom(dat);
				case MSG_ID_ENTER_SCENE_REQ:
					return Scene.EnterSceneReq.parseFrom(dat);
				case MSG_ID_ENTER_SCENE_RSP:
					return Scene.EnterSceneRsp.parseFrom(dat);
				case MSG_ID_GM_CMD:
					return Role.GmCmd.parseFrom(dat);
				case MSG_ID_GM_CMD_RSP:
					return Role.GmCmdRsp.parseFrom(dat);
				case MSG_ID_LOAD_ROLE_LIST_RSP:
					return Login.LoadRoleListRsp.parseFrom(dat);
				case MSG_ID_LOGIN_REQ:
					return Login.LoginReq.parseFrom(dat);
				case MSG_ID_LOGIN_RSP:
					return Login.LoginRsp.parseFrom(dat);
				case MSG_ID_MOVE_TO:
					return Role.MoveTo.parseFrom(dat);
				case MSG_ID_SEND_UDP_PORT:
					return Login.SendUdpPort.parseFrom(dat);
				case MSG_ID_START_KCP:
					return Login.StartKcp.parseFrom(dat);
				case MSG_ID_SYNC_POS:
					return Scene.SyncPos.parseFrom(dat);
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
			case MSG_ID_ACTOR_BORN:
				return Scene.ActorBorn.newBuilder();
			case MSG_ID_ACTOR_DISSOLVE:
				return Scene.ActorDissolve.newBuilder();
			case MSG_ID_CREATE_ROLE_REQ:
				return Login.CreateRoleReq.newBuilder();
			case MSG_ID_CREATE_ROLE_RSP:
				return Login.CreateRoleRsp.newBuilder();
			case MSG_ID_DISCONNECT_REQ:
				return Login.DisconnectReq.newBuilder();
			case MSG_ID_DISCONNECT_RSP:
				return Login.DisconnectRsp.newBuilder();
			case MSG_ID_ENTER_GAME:
				return Login.EnterGame.newBuilder();
			case MSG_ID_ENTER_GAME_RSP:
				return Login.EnterGameRsp.newBuilder();
			case MSG_ID_ENTER_SCENE_REQ:
				return Scene.EnterSceneReq.newBuilder();
			case MSG_ID_ENTER_SCENE_RSP:
				return Scene.EnterSceneRsp.newBuilder();
			case MSG_ID_GM_CMD:
				return Role.GmCmd.newBuilder();
			case MSG_ID_GM_CMD_RSP:
				return Role.GmCmdRsp.newBuilder();
			case MSG_ID_LOAD_ROLE_LIST_RSP:
				return Login.LoadRoleListRsp.newBuilder();
			case MSG_ID_LOGIN_REQ:
				return Login.LoginReq.newBuilder();
			case MSG_ID_LOGIN_RSP:
				return Login.LoginRsp.newBuilder();
			case MSG_ID_MOVE_TO:
				return Role.MoveTo.newBuilder();
			case MSG_ID_SEND_UDP_PORT:
				return Login.SendUdpPort.newBuilder();
			case MSG_ID_START_KCP:
				return Login.StartKcp.newBuilder();
			case MSG_ID_SYNC_POS:
				return Scene.SyncPos.newBuilder();
			case MSG_ID_TEST_REQ:
				return Test.TestReq.newBuilder();
		}
		return null;
	}
	
	public static Scene.ActorBorn.Builder createActorBornBuilder() {
		return Scene.ActorBorn.newBuilder();
	}
	
	public static Scene.ActorDissolve.Builder createActorDissolveBuilder() {
		return Scene.ActorDissolve.newBuilder();
	}
	
	public static Login.CreateRoleReq.Builder createCreateRoleReqBuilder() {
		return Login.CreateRoleReq.newBuilder();
	}
	
	public static Login.CreateRoleRsp.Builder createCreateRoleRspBuilder() {
		return Login.CreateRoleRsp.newBuilder();
	}
	
	public static Login.DisconnectReq.Builder createDisconnectReqBuilder() {
		return Login.DisconnectReq.newBuilder();
	}
	
	public static Login.DisconnectRsp.Builder createDisconnectRspBuilder() {
		return Login.DisconnectRsp.newBuilder();
	}
	
	public static Login.EnterGame.Builder createEnterGameBuilder() {
		return Login.EnterGame.newBuilder();
	}
	
	public static Login.EnterGameRsp.Builder createEnterGameRspBuilder() {
		return Login.EnterGameRsp.newBuilder();
	}
	
	public static Scene.EnterSceneReq.Builder createEnterSceneReqBuilder() {
		return Scene.EnterSceneReq.newBuilder();
	}
	
	public static Scene.EnterSceneRsp.Builder createEnterSceneRspBuilder() {
		return Scene.EnterSceneRsp.newBuilder();
	}
	
	public static Role.GmCmd.Builder createGmCmdBuilder() {
		return Role.GmCmd.newBuilder();
	}
	
	public static Role.GmCmdRsp.Builder createGmCmdRspBuilder() {
		return Role.GmCmdRsp.newBuilder();
	}
	
	public static Login.LoadRoleListRsp.Builder createLoadRoleListRspBuilder() {
		return Login.LoadRoleListRsp.newBuilder();
	}
	
	public static Login.LoginReq.Builder createLoginReqBuilder() {
		return Login.LoginReq.newBuilder();
	}
	
	public static Login.LoginRsp.Builder createLoginRspBuilder() {
		return Login.LoginRsp.newBuilder();
	}
	
	public static Role.MoveTo.Builder createMoveToBuilder() {
		return Role.MoveTo.newBuilder();
	}
	
	public static Login.SendUdpPort.Builder createSendUdpPortBuilder() {
		return Login.SendUdpPort.newBuilder();
	}
	
	public static Login.StartKcp.Builder createStartKcpBuilder() {
		return Login.StartKcp.newBuilder();
	}
	
	public static Scene.SyncPos.Builder createSyncPosBuilder() {
		return Scene.SyncPos.newBuilder();
	}
	
	public static Test.TestReq.Builder createTestReqBuilder() {
		return Test.TestReq.newBuilder();
	}
	
}

