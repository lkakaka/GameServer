package com.command;

import com.game.GameRobot;
import com.game.RobotMgr;
import com.proto.ProtoBufferMsg;
import com.proto.Role;
import com.proto.Test;
import com.util.RC4;

import javax.crypto.Cipher;
import java.nio.charset.Charset;

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

    @CmdAnnotation(inputCmd = "add")
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

    @CmdAnnotation(inputCmd = "gm")
    private void gmCmd() {
        if (params == null || params.length < 1) {
            System.out.println("gmCmd error, params: " + params);
            return;
        }

        String gmCmd = params[0];
        StringBuilder sb = new StringBuilder();
        for (int i = 1; i < params.length; i++) {
            sb.append(params[i]);
            if (i != params.length - 1)
                sb.append(",");
        }
        Role.GmCmd.Builder builder = Role.GmCmd.newBuilder();
        builder.setCmd(gmCmd).setArgs(sb.toString());

        GameRobot robot = RobotMgr.getInstance().getOneRobot();
        robot.sendProto(ProtoBufferMsg.MSG_ID_GM_CMD, builder.build());
        System.out.println("gmCmd successful, account:" + robot.getAccount());
    }

    @CmdAnnotation(inputCmd = "move")
    private void move() {
        if (params == null || params.length < 3) {
            System.out.println("move error, params: " + params);
            return;
        }

        String account = params[0];
        int x = Integer.parseInt(params[1]);
        int y = Integer.parseInt(params[2]);
        Role.MoveTo.Builder builder = Role.MoveTo.newBuilder();
        builder.setPosX(x).setPosY(y);
        GameRobot robot = RobotMgr.getInstance().getRobot(account);
        robot.sendProto(ProtoBufferMsg.MSG_ID_MOVE_TO, builder.build());
        System.out.println("move successful, account:" + account);
    }

    @CmdAnnotation(inputCmd = "test")
    private void test() {
        Test.TestReq.Builder builder = Test.TestReq.newBuilder();
        builder.setId(10).setMsg("hello");
        GameRobot robot = RobotMgr.getInstance().getOneRobot();
        robot.sendProto(ProtoBufferMsg.MSG_ID_TEST_REQ, builder.build());
    }

    @CmdAnnotation(inputCmd = "rc4")
    private void rc4() throws Exception {
        String key = "testrc4";
        RC4 rc4 = new RC4(key.getBytes());
        String plainText = "Hello RC4";
        byte[] cipherText = rc4.encryptWithRC4(plainText);
//        byte[] cipherText = rc4.encrypt(plainText.getBytes());
        String strCipherText = new String(cipherText, Charset.forName("ASCII"));
        System.out.println(strCipherText);

        byte[] decodeBytes = rc4.decryptWithRC4(strCipherText, cipherText);
//        byte[] decodeBytes = rc4.decrypt(cipherText);
        String decodeText = new String(decodeBytes);
        System.out.println(decodeText);
    }
}
