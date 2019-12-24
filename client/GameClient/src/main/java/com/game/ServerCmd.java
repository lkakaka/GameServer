package com.game;

import com.command.CmdAnnotation;
import com.command.CmdDispatch;
import com.proto.LoginOuterClass;
import com.proto.ProtoBufferMsg;
import com.proto.TestOuterClass;
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

    @CmdAnnotation(serverCmd = ProtoBufferMsg.MSG_ID_LOGINRSP)
    private void onRecvLoginResp(Object param) {
        LoginOuterClass.LoginRsp loginRsp = (LoginOuterClass.LoginRsp) param;
        Util.logInfo("recv login rsp proto, account:%s, userId:%d", loginRsp.getAccount(), loginRsp.getUserId());
    }

    @CmdAnnotation(serverCmd = ProtoBufferMsg.MSG_ID_TEST)
    private void onRecvTest(Object param) {
        TestOuterClass.Test test = (TestOuterClass.Test) param;
        Util.logInfo("recv test proto, id:%d, :%s", test.getId(), test.getMsg());
    }
}