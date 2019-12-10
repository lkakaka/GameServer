package com.command;

import com.game.GameRobot;
import com.game.RobotMgr;
import com.proto.ProtoBufferMsg;
import com.proto.TestOuterClass;

public class CommandCmd extends CmdDispatch {
    public String cmd;
    public String[] params;

    CommandCmd(String cmdStr) {
        super("inputCmd");
        parseCmdStr(cmdStr);
    }

    public boolean parseCmdStr(String strCmd) {
        strCmd = strCmd.trim();
        String[] cmdInfo = strCmd.split(" ");
        if (cmdInfo.length < 1) {
            System.out.println("input cmd fromat error:" + strCmd);
            return false;
        }

        cmd = cmdInfo[0].trim();
        if (cmdInfo.length > 1) {
            params = cmdInfo[1].split(",");
        } else {
            params = null;
        }
        return true;
    }

    public void extcute() {
        if (cmd == null) {
            System.out.println("input cmd extcute error, cmd is null");
            return;
        }

        dispatchCmd(cmd);
    }

    @CmdAnnotation(inputCmd = "add_robot")
    private void addRobot() {
        if (params == null || params.length < 1) {
            System.out.println("addRobot error, params: " + params);
            return;
        }

        String account = params[0];
        RobotMgr.getInstance().addRobot(account);
        System.out.println("addRobot successful, account:" + account);
    }

    @CmdAnnotation(inputCmd = "remove_robot")
    private void removeRobot() {
        if (params == null || params.length < 1) {
            System.out.println("removeRobot error, params: " + params);
            return;
        }

        String account = params[0];
        RobotMgr.getInstance().removeRobot(account);
        System.out.println("removeRobot successful, account:" + account);
    }

    @CmdAnnotation(inputCmd = "test")
    private void test() {
        TestOuterClass.Test.Builder builder = TestOuterClass.Test.newBuilder();
        builder.setId(10);
        builder.setMsg("hello");
        GameRobot robot = RobotMgr.getInstance().getRobot("test");
        robot.sendProto(ProtoBufferMsg.MSG_ID_TEST, builder.build());
    }
}
