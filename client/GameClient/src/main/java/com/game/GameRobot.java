package com.game;

import com.network.Network;
import com.network.NetworkEventHandler;
import com.proto.Login;
import com.proto.ProtoBufferMsg;
import com.proto.Test;
import com.util.Util;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;

public class GameRobot {
    private Network m_network;
    private String serverIP;
    private int serverPort;
    private int userId;
    private String account;
    private ByteArrayOutputStream m_buffer = new ByteArrayOutputStream();
    private ServerCmd m_robotCmd;

    GameRobot() {
        m_robotCmd = new ServerCmd(this);
    }

    public void init() {
        m_network = new Network(serverIP, serverPort);
        m_network.connect();
        m_network.registerEventHandler(new NetworkEventHandler() {
            @Override
            public void onConnected() {

            }

            @Override
            public void onDisConnected() {
                RobotMgr.getInstance().removeRobot(account);
            }

            @Override
            public void onRecv(byte[] data, int count) {
                try {
                    m_buffer.write(data, 0, count);
                }catch (Exception e) {
                    e.printStackTrace();
                }
                while (true) {
                    if(!parseData())
                        break;
                }
            }

            @Override
            public void onSend() {

            }

            @Override
            public void onClose() {

            }
        });
    }

    public void login() {
        Login.LoginReq.Builder builder = ProtoBufferMsg.createLoginReqBuilder();
        builder.setAccount(getAccount());
        builder.setPwd("");
        Login.LoginReq login = builder.build();
        sendProto(ProtoBufferMsg.MSG_ID_LOGIN_REQ, login);
    }

    public void disconnect() {
        m_network.close();
    }

    private boolean parseData() {
        if (m_buffer.size() < 8) {
            return false;
        }

        byte[] allBytes = m_buffer.toByteArray();
        ByteArrayInputStream buffer = new ByteArrayInputStream(allBytes);
        DataInputStream dataInputStream = new DataInputStream(buffer);
        try {
            int iPacketLen = dataInputStream.readInt();
            if (m_buffer.size() < iPacketLen) {
                return false;
            }
            int iMsgId = dataInputStream.readInt();
            int iMsgLen = iPacketLen - 8;
            byte[] dat = new byte[iMsgLen];
            dataInputStream.read(dat,0, iMsgLen);
            onMsg(iMsgId, dat);
            m_buffer.reset();
            m_buffer.write(allBytes, iPacketLen, allBytes.length - iPacketLen);
            return true;
        }catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

    private void onMsg(int iMsgId, byte[] dat) {
        System.out.println("recv msg: " + iMsgId);
        Object param = ProtoBufferMsg.createMsgById(iMsgId, dat);
        m_robotCmd.onRecvServerCmd(iMsgId, param);
    }

    public void sendData(byte[] data) {
        int len = m_network.sendData(data);
        System.out.println("send data len:" + len);
    }

    public void sendProto(int iMsgId, com.google.protobuf.GeneratedMessageV3 msg) {
//        int size = msg.getSerializedSize();
//        System.out.println(size);
        try {
            byte[] msgData = msg.toByteArray();
            int iMsgLen = msgData.length;
            ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
            DataOutputStream dataOutputStream = new DataOutputStream(outputStream);
            dataOutputStream.writeInt(iMsgLen + 8);
            dataOutputStream.writeInt(iMsgId);
            dataOutputStream.write(msgData, 0, iMsgLen);

//        byte[] data = new byte[size+8];
//        Util.intToByteArray(iMsgId, data, 0);
//        Util.intToByteArray(size, data, 4);

//        Util.copyByteArray(msgData, data, 8);
            sendData(outputStream.toByteArray());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void setServerIP(String m_serverIP) {
        this.serverIP = m_serverIP;
    }

    public void setServerPort(int m_serverPort) {
        this.serverPort = m_serverPort;
    }

    public int getUserId() {
        return userId;
    }

    public void setUserId(int userId) {
        this.userId = userId;
    }

    public String getAccount() {
        return account;
    }

    public void setAccount(String account) {
        this.account = account;
    }
}
