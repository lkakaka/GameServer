package com.game;

import com.command.CmdAnnotation;
import com.command.CmdDispatch;

import java.util.Iterator;
import java.util.Map;

public class ServerCmd extends CmdDispatch {
    private GameRobot m_robot;

    ServerCmd(GameRobot robot) {
        super("serverCmd");
        m_robot = robot;
    }

    public void onRecvServerCmd(int iCmd, Object cmdParam) {
        dispatchCmd(iCmd, cmdParam);
    }

    @CmdAnnotation(serverCmd = 99991)
    private void onRecvLoginResp(Object param) {

    }

    @CmdAnnotation(serverCmd = 90005)
    private void onRecvTbNotify(Object param) {

    }
}
