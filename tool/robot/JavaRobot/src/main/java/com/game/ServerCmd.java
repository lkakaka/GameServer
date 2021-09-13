package com.game;

import com.command.CmdAnnotation;
import com.command.CmdDispatch;
import com.proto.*;
import com.util.Util;

public class ServerCmd extends CmdDispatch {
    private GameRobot m_robot;

    ServerCmd(GameRobot robot) {
        super("serverCmd");
        m_robot = robot;
    }

    public void onRecvServerCmd(int iCmd, Object cmdParam) {
        dispatchCmd(iCmd, cmdParam);
    }

    @CmdAnnotation(serverCmd = ProtoBufferMsg.MSG_ID_LOGIN_RSP)
    private void onRecvLoginResp(Object param) {
        Login.LoginRsp loginRsp = (Login.LoginRsp) param;
        Util.logInfo("recv login rsp proto, err_code:%s", loginRsp.getErrCode());
    }

    @CmdAnnotation(serverCmd = ProtoBufferMsg.MSG_ID_TEST_REQ)
    private void onRecvTest(Object param) {
        Test.TestReq test = (Test.TestReq) param;
        Util.logInfo("recv test proto, id:%d, :%s", test.getId(), test.getMsg());
    }

    @CmdAnnotation(serverCmd = ProtoBufferMsg.MSG_ID_LOAD_ROLE_LIST_RSP)
    private void onRecvRoleList(Object param) {
        Login.LoadRoleListRsp rsp = (Login.LoadRoleListRsp) param;
        int errCode = rsp.getErrCode();
        Util.logInfo("recv role list, err_code:%s", errCode);
        if (errCode == 0) {
            if (rsp.getRoleListCount() == 0) {
                Login.CreateRoleReq req = ProtoBufferMsg.createCreateRoleReqBuilder().setAccount(m_robot.getAccount())
                        .setRoleName(m_robot.getAccount()).build();
                m_robot.sendProto(ProtoBufferMsg.MSG_ID_CREATE_ROLE_REQ, req);
                Util.logInfo("create role, role_name:%s", m_robot.getAccount());
            } else {
                Login._RoleInfo roleInfo = rsp.getRoleList(0);
                Login.EnterGame enterGame = ProtoBufferMsg.createEnterGameBuilder().setAccount(m_robot.getAccount())
                        .setRoleId(roleInfo.getRoleId()).build();
                m_robot.sendProto(ProtoBufferMsg.MSG_ID_ENTER_GAME, enterGame);
                Util.logInfo("send enter game, role_id:%d", roleInfo.getRoleId());
            }
        }
    }

    @CmdAnnotation(serverCmd = ProtoBufferMsg.MSG_ID_CREATE_ROLE_RSP)
    private void onRecvCreateRoleRsp(Object param) {
        Login.CreateRoleRsp rsp = (Login.CreateRoleRsp) param;
        Util.logInfo("create role rsp, err_code:%d", rsp.getErrCode());
        if (rsp.getErrCode() != 0) {
            return;
        }
        Login.EnterGame enterGame = ProtoBufferMsg.createEnterGameBuilder().setAccount(m_robot.getAccount())
                .setRoleId(rsp.getRoleInfo().getRoleId()).build();
        m_robot.sendProto(ProtoBufferMsg.MSG_ID_ENTER_GAME, enterGame);
    }

    @CmdAnnotation(serverCmd = ProtoBufferMsg.MSG_ID_ENTER_GAME_RSP)
    private void onRecvEnterGameRsp(Object param) {
        Login.EnterGameRsp rsp = (Login.EnterGameRsp) param;
        int errCode = rsp.getErrCode();
        Util.logInfo("recv enter game rsp, err_code:%s", errCode);
    }

    @CmdAnnotation(serverCmd = ProtoBufferMsg.MSG_ID_START_KCP)
    private void onRecvStartKCP(Object param) {
        Login.StartKcp rsp = (Login.StartKcp) param;
        Util.logInfo("recv start kcp, %d", rsp.getKcpId());

        if(m_robot.startKCP(rsp.getKcpId())) {
            int udpPort = m_robot.getUdpPort();
            Login.SendUdpPort msg = ProtoBufferMsg.createSendUdpPortBuilder().setUdpPort(udpPort).build();
            m_robot.sendProto(ProtoBufferMsg.MSG_ID_SEND_UDP_PORT, msg);
            Util.logInfo("send udp port: %d", udpPort);
        } else {
            Util.logInfo("start kcp failed, %d", rsp.getKcpId());
        }
    }

    @CmdAnnotation(serverCmd = ProtoBufferMsg.MSG_ID_ACTOR_BORN)
    private void onRecvActorBorn(Object param) {
        Scene.ActorBorn rsp = (Scene.ActorBorn) param;
        for (Scene._NpcInfo npcInfo : rsp.getNpcListList()) {
            Util.logInfo("[%s] npc born: actor_id:%d, npc_id:%d", m_robot.getAccount(), npcInfo.getActorId(), npcInfo.getNpcId());
        }

        for (Scene._PlayerInfo playerInfo : rsp.getPlayerListList()) {
            Util.logInfo("[%s] player born: actor_id:%d, name:%s", m_robot.getAccount(), playerInfo.getActorId(), playerInfo.getName());
        }
    }

    @CmdAnnotation(serverCmd = ProtoBufferMsg.MSG_ID_ACTOR_DISSOLVE)
    private void onRecvActorDissolve(Object param) {
        Scene.ActorDissolve rsp = (Scene.ActorDissolve) param;
        Util.logInfo("actor dissolve: actor_ids:%s", rsp.getActorIdsList());
    }

    @CmdAnnotation(serverCmd = ProtoBufferMsg.MSG_ID_SYNC_POS)
    private void onRecvActorSyncPos(Object param) {
        Scene.SyncPos rsp = (Scene.SyncPos) param;
        Util.logInfo("actor move, actor_id:%d, x:%d, y:%d", rsp.getActorId(), rsp.getPosX(), rsp.getPosY());
    }

    @CmdAnnotation(serverCmd = ProtoBufferMsg.MSG_ID_GM_CMD_RSP)
    private void onRecvGmCmdRsp(Object param) {
        Role.GmCmdRsp rsp = (Role.GmCmdRsp) param;
        Util.logInfo("recv GM cmd rsp, cmd:%s, msg:%s", rsp.getCmd(), rsp.getMsg());
    }
}