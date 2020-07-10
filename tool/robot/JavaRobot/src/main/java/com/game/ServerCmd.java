package com.game;

import com.command.CmdAnnotation;
import com.command.CmdDispatch;
import com.proto.Login;
import com.proto.ProtoBufferMsg;
import com.proto.Role;
import com.proto.Test;
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
            if  (rsp.getRoleListCount() == 0) {
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

    @CmdAnnotation(serverCmd = ProtoBufferMsg.MSG_ID_ENTER_GAME_RSP)
    private void onRecvEnterGameRsp(Object param) {
        Login.EnterGameRsp rsp = (Login.EnterGameRsp) param;
        int errCode = rsp.getErrCode();
        Util.logInfo("recv enter game rsp, err_code:%s", errCode);
    }

    @CmdAnnotation(serverCmd = ProtoBufferMsg.MSG_ID_GM_CMD_RSP)
    private void onRecvGmCmdRsp(Object param) {
        Role.GmCmdRsp rsp = (Role.GmCmdRsp) param;
        Util.logInfo("recv GM cmd rsp, cmd:%s, msg:%s", rsp.getCmd(), rsp.getMsg());
    }
}