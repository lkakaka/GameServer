package com.game;

import com.network.Network;
import com.network.NetworkEventHandler;
import com.proto.Login;
import com.proto.ProtoBufferMsg;
import com.proto.Role;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.util.concurrent.ScheduledThreadPoolExecutor;
import java.util.concurrent.ThreadLocalRandom;
import java.util.concurrent.TimeUnit;

public class GameRobot {
    private static final int TYPE_TCP = 0;
    private static final int TYPE_KCP = 1;

    private Network m_network;
    private String m_serverIP;
    private int m_serverPort;
    private int m_serverUdpPort;
    private long m_roleId;
    private String m_account;
    private ByteArrayOutputStream m_buffer = new ByteArrayOutputStream();
    private ByteArrayOutputStream m_kcpBuffer = new ByteArrayOutputStream();
    private ServerCmd m_robotCmd;
    private boolean isSwitchServer;

    ScheduledThreadPoolExecutor tpeMove = new ScheduledThreadPoolExecutor(1);

    GameRobot() {
        m_robotCmd = new ServerCmd(this);
    }

    public void init() {
        m_network = new Network(m_serverIP, m_serverPort, m_serverUdpPort);
        m_network.connect();
        m_network.registerEventHandler(new NetworkEventHandler() {
            @Override
            public void onConnected() {

            }

            @Override
            public void onDisConnected() {
                if (isSwitchServer) return;
                RobotMgr.getInstance().removeRobot(m_account);
            }

            @Override
            public void onRecv(byte[] data, int count) {
                try {
                    m_buffer.write(data, 0, count);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                while (true) {
                    if (!parseData(TYPE_TCP, m_buffer)) break;
                }
            }

            @Override
            public void onKCPRecv(byte[] data, int count) {
                try {
                    m_kcpBuffer.write(data, 0, count);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                while (true) {
                    if (!parseData(TYPE_KCP, m_kcpBuffer)) break;
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

    private boolean parseData(int type, ByteArrayOutputStream stream) {
        if (stream.size() < 8) {
            return false;
        }

        byte[] allBytes = stream.toByteArray();
        ByteArrayInputStream buffer = new ByteArrayInputStream(allBytes);
        DataInputStream dataInputStream = new DataInputStream(buffer);
        try {
            int iPacketLen = dataInputStream.readInt();
            if (stream.size() < iPacketLen) {
                return false;
            }
            int iMsgId = dataInputStream.readInt();
            int iMsgLen = iPacketLen - 8;
            byte[] dat = new byte[iMsgLen];
            dataInputStream.read(dat, 0, iMsgLen);
            onMsg(type, iMsgId, dat);
            stream.reset();
            stream.write(allBytes, iPacketLen, allBytes.length - iPacketLen);
            return true;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

    private void onMsg(int type, int iMsgId, byte[] dat) {
        String srcType = type == TYPE_KCP ? "KCP" : "TCP";
        System.out.println(String.format("recv msg[%s]: %d", srcType, iMsgId));
        Object param = ProtoBufferMsg.createMsgById(iMsgId, dat);
        m_robotCmd.onRecvServerCmd(iMsgId, param);
    }

    public void sendData(byte[] data) {
        int len = m_network.sendData(data);
        System.out.println("send data len:" + len);
    }

    public void sendUdpData(byte[] data) {
        m_network.sendUdpData(data);
        System.out.println("send UDP data len:" + data.length);
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

    public void setServerIP(String serverIP) {
        this.m_serverIP = serverIP;
    }

    public void setServerPort(int serverPort, int serverUdpPort) {
        this.m_serverPort = serverPort;
        this.m_serverUdpPort = serverUdpPort;
    }

    public long getRoleId() {
        return m_roleId;
    }

    public void setRoleId(long roleId) {
        this.m_roleId = roleId;
    }

    public String getAccount() {
        return m_account;
    }

    public void setAccount(String account) {
        this.m_account = account;
    }

    public boolean startKCP(int kcpId, String token) {
        return m_network.startKcp(kcpId, token);
    }

    public int getUdpPort() {
        return m_network.getUdpPort();
    }

    public void switchServer(String ip, int port, int udpPort, Runnable runnable) {
        isSwitchServer = true;
        disconnect();
        m_serverIP = ip;
        m_serverPort = port;
        m_serverUdpPort = udpPort;
        ScheduledThreadPoolExecutor tpe = new ScheduledThreadPoolExecutor(1);
        tpe.schedule(() -> {
            init();
            isSwitchServer = false;
            runnable.run();
        }, 1, TimeUnit.SECONDS);
    }

    public void startRandMove(int iDelayMillSec) {
        tpeMove.scheduleAtFixedRate(() -> randMove(), 0, iDelayMillSec, TimeUnit.MILLISECONDS);
    }

    public void stopRandMove() {
        tpeMove.shutdown();
    }

    public void randMove() {
        ThreadLocalRandom random = ThreadLocalRandom.current();
        int x = random.nextInt(1, 100);
        int y = random.nextInt(1, 100);
        Role.MoveTo.Builder builder = Role.MoveTo.newBuilder();
        builder.setPosX(x).setPosY(y);
        sendProto(ProtoBufferMsg.MSG_ID_MOVE_TO, builder.build());
    }
}
