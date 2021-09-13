package com.network;

import java.net.DatagramPacket;
import java.net.DatagramSocket;

public class Udp {
    private static final int m_port = 8888;

    public static void start() {
        Thread t = new Thread(() -> Udp._start());
        t.start();
    }

    private static void _start() {
        try {
            DatagramSocket ds = new DatagramSocket(m_port);
            int buffSize = 2048;
            byte[] buffer = new byte[buffSize];
            while (true) {
                DatagramPacket dp = new DatagramPacket(buffer, buffSize);
                ds.receive(dp);
                String s = new String(buffer, 0, dp.getLength());
                System.out.println("recv udp data:" + dp.getLength() + " " + s);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
