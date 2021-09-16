package com.network;

//import com.sun.xml.internal.messaging.saaj.util.ByteOutputStream;

import com.util.Util;

import java.io.ByteArrayOutputStream;

import java.io.DataOutputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.util.Arrays;
import java.util.Iterator;
import java.util.Set;
import java.util.concurrent.*;

public class Network {

    public static final byte UDP_MSG_TYPE_VERIFY = 1;    // UDP验证
    public static final byte UDP_MSG_TYPE_KEEP_ALIVE = 2;    // UDP保活(心跳包)
    public static final byte UDP_MSG_TYPE_KCP = 3;    // KCP控制消息

    private SocketChannel m_sc = null;
    private String m_serverIP;
    private int m_serverPort;
    private int m_serverUdpPort;
    private Thread m_workThread = null;
    private boolean m_stop = false;
    private NetworkEventHandler m_eventHandler = null;
    private ByteBuffer m_buffer = ByteBuffer.allocate(1024);

    private int total_len = 0;

    private KCP m_kcp;
    private int m_udpPort;
    private DatagramSocket m_ds;

    private byte[] m_udpBuffer;
    private byte[] m_kcpBuffer;

    public Network(String serverIP, int serverPort, int serverUdpPort) {
        m_serverIP = serverIP;
        m_serverPort = serverPort;
        m_serverUdpPort = serverUdpPort;
    }

    public void connect() {
        try {
            m_sc = SocketChannel.open();
            InetSocketAddress addr = new InetSocketAddress(m_serverIP, m_serverPort);
            m_sc.connect(addr);
            m_sc.configureBlocking(false);
            System.out.println("xx1");
            startWork();
            if (m_eventHandler != null) {
                m_eventHandler.onConnected();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void startWork() throws Exception {
        final Selector selector = Selector.open();
        m_sc.register(selector, SelectionKey.OP_READ);

        m_workThread = new Thread(new Runnable() {
            @Override
            public void run() {
                while (true) {
                    if (m_stop) {
                        break;
                    }
                    try {
                        tryRead();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }

                onClosed();
            }

            private void tryRead() throws Exception {
                int count = selector.select();
                if (count <= 0)
                    return;
                Set<SelectionKey> selectKeys = selector.selectedKeys();
                Iterator iter = selectKeys.iterator();
                while (iter.hasNext()) {
                    SelectionKey key = (SelectionKey) iter.next();
                    if (key.isReadable()) {
                        if (!onRead()) {
                            key.cancel();
                            if (m_eventHandler != null) {
                                m_eventHandler.onDisConnected();
                            }
                            m_stop = true;
                        }
                    } else if (key.isWritable()) {
                        System.out.println("can writable");
                    } else if (key.isConnectable()) {
                        System.out.println("can connect");
                    }
                    iter.remove();
                }
            }
        });
        m_workThread.start();
//        m_workThread.join();
    }

    public int sendData(byte[] bytes) {
        ByteBuffer buffer = ByteBuffer.allocate(bytes.length);
        buffer.put(bytes);
        try {
            buffer.flip();
            return m_sc.write(buffer);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return 0;
    }

    private void sendKCPData(byte[] bytes, int len) {
        if (m_serverUdpPort <= 0) {
            throw new RuntimeException("not set server udp port");
        }
        try {
            sendUdpData(UDP_MSG_TYPE_KCP, bytes, len);

//            DatagramSocket ds = new DatagramSocket();
//            InetSocketAddress address = new InetSocketAddress(m_serverIP, m_serverUdpPort);
//            DatagramPacket dp = new DatagramPacket(bytes, len, address);
//            ds.send(dp);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private boolean onRead() {
        try {
            ByteArrayOutputStream byteOutputStream = new ByteArrayOutputStream();
            int len = m_sc.read(m_buffer);
            while (len > 0) {
                total_len += len;
                m_buffer.flip();
                byteOutputStream.write(m_buffer.array(), 0, len);
                m_buffer.clear();
                len = m_sc.read(m_buffer);
            }
            if (m_eventHandler != null) {
                m_eventHandler.onRecv(byteOutputStream.toByteArray(), byteOutputStream.size());
            } else {
                System.out.println("event hander is null");
            }
            System.out.println(String.format("read len: %d, total_len:%d", len, total_len));
        } catch (IOException e) {
            System.out.println("read error, e: " + e.getMessage());
            return false;
        }
        return true;
    }

    public void close() {
        m_stop = true;
//        onClosed();
        m_workThread.interrupt();
    }

    private void onClosed() {
        try {
            if (m_sc != null && m_sc.isConnected()) {
                m_sc.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public boolean startKcp(int connId, String token) {
        if (!startUdp(connId, token)) return false;

        m_kcp = new KCP(connId) {
            @Override
            protected void output(byte[] bytes, int size) {
                System.out.println("kcp output, len:" + size);
                sendKCPData(bytes, size);
            }
        };
        ScheduledThreadPoolExecutor tpe = new ScheduledThreadPoolExecutor(2);
        tpe.scheduleAtFixedRate(() -> m_kcp.Update(System.currentTimeMillis()), 0, 10, TimeUnit.MILLISECONDS);
        tpe.scheduleAtFixedRate(() -> sendUdpHeart(), 0, 60, TimeUnit.SECONDS);
        return true;
    }

    private boolean startUdp(int connId, String token) {
        int iUdpPort = 10000;
        while (iUdpPort < 65535) {
            try {
                m_ds = new DatagramSocket(iUdpPort);
                break;
            } catch (Exception e) {
                iUdpPort++;
            }
        }
        if (m_ds == null) return false;
        System.out.println(String.format("start udp, connId:%d, token:%s", connId, token));
        // 必须先发一个UDP包给服务端，然后才能收到数据(和NAT有关, 建立NAT映射关系)
        try {
            byte[] buff = new byte[token.length() + 5];
            buff[0] = UDP_MSG_TYPE_VERIFY;
            Util.writeInt(buff, 1, connId, Util.LITTLE_ENDIAN);
            byte[] tokens = token.getBytes();
            System.arraycopy(tokens, 0, buff, 5, tokens.length);
            sendUdpData(buff);
            m_udpPort = iUdpPort;
            Thread t = new Thread(() -> _startUdp());
            t.start();
            return true;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

    private void _startUdp() {
        int buffSize = 2048;
        m_udpBuffer = new byte[buffSize];
        m_kcpBuffer = new byte[buffSize];
        while (true) {
            tryRecvUdpData();
        }
    }

    private void tryRecvUdpData() {
        try {
            DatagramPacket dp = new DatagramPacket(m_udpBuffer, m_udpBuffer.length);
            m_ds.receive(dp);
//            String s = new String(m_udpBuffer, 0, dp.getLength());
            System.out.println("recv udp data:" + dp.getLength());
            byte[] data = Arrays.copyOf(m_udpBuffer, dp.getLength());
            m_kcp.Input(data);
            int recvLen = m_kcp.Recv(m_kcpBuffer);
            System.out.println("kcp recv data:" + recvLen);
            if (recvLen < 0) return;
            if (m_eventHandler != null) {
                m_eventHandler.onKCPRecv(m_kcpBuffer, recvLen);
            } else {
                System.out.println("event hander is null");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public int getUdpPort() {
        return m_udpPort;
    }

    public void registerEventHandler(NetworkEventHandler eventHandler) {
        m_eventHandler = eventHandler;
    }

    public void sendUdpData(byte[] msg) {
        try {
            InetSocketAddress address = new InetSocketAddress(m_serverIP, m_serverUdpPort);
            DatagramPacket dp = new DatagramPacket(msg, msg.length, address);
            m_ds.send(dp);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void sendUdpData(byte msgType, byte[] msg, int len) {
        byte[] data = new byte[len + 1];
        data[0] = msgType;
        if (len > 0) System.arraycopy(msg, 0, data, 1, len);
        sendUdpData(data);
    }

    private void sendUdpHeart() {
        sendUdpData(UDP_MSG_TYPE_KEEP_ALIVE, new byte[0], 0);
        System.out.println("send udp heart");
    }
}

