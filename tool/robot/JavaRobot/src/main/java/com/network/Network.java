package com.network;

//import com.sun.xml.internal.messaging.saaj.util.ByteOutputStream;
import java.io.ByteArrayOutputStream;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.util.Iterator;
import java.util.Set;

public class Network {

    private SocketChannel m_sc = null;
    private String m_serverIP;
    private int m_serverPort;
    private Thread m_workThread = null;
    private boolean m_stop = false;
    private NetworkEventHandler m_eventHandler = null;
    ByteBuffer m_buffer = ByteBuffer.allocate(1024);

    private int total_len = 0;

    public Network(String serverIP, int serverPort) {
        m_serverIP = serverIP;
        m_serverPort = serverPort;
    }

    public void connect() {
        try {
            m_sc = SocketChannel.open();
            InetSocketAddress addr = new InetSocketAddress(m_serverIP, m_serverPort);
            m_sc.connect(addr);
            m_sc.configureBlocking(false);
            startWork();
            if (m_eventHandler != null) {
                m_eventHandler.onConnected();
            }
        }catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void startWork() throws Exception{
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
                        if(!onRead()) {
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

    public int sendData(byte[] bytes){
        ByteBuffer buffer = ByteBuffer.allocate(bytes.length);
        buffer.put(bytes);
        try {
            buffer.flip();
            int len = m_sc.write(buffer);
            return len;
        }catch (IOException e){
            e.printStackTrace();
        }
        return 0;
    }

    private boolean onRead(){
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
            System.out.println("read len=" + total_len);
        } catch (IOException e){
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

    public void registerEventHandler(NetworkEventHandler eventHandler) {
        m_eventHandler = eventHandler;
    }
}

