package com.game;

import java.awt.*;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;
import java.util.function.Consumer;

public class RobotMgr {
    private static final String m_localServerIP = "127.0.0.1";
    private static final String m_remoteServerIP = "111.229.80.201";
    private static final int m_serverPort = 30001;
    private static final int m_serverUdpPort = 7777;

    private Map<String, GameRobot> m_robots = new HashMap<>();
    private GameRobot m_curRobot;   // 当前使用的robot

    private static final RobotMgr m_robotMgr;

    static {
        m_robotMgr = new RobotMgr();
    }

    public void addRobot(String account, boolean bRemoteServer) {
        if (m_robots.containsKey(account)) {
            System.out.println("robot exist, account:" + account);
            return;
        }

        String serverIP = bRemoteServer ? m_remoteServerIP : m_localServerIP;

        GameRobot gameRobot = new GameRobot();
        gameRobot.setServerIP(serverIP);
        gameRobot.setServerPort(RobotMgr.m_serverPort, RobotMgr.m_serverUdpPort);
        gameRobot.setAccount(account);
        gameRobot.init();
        gameRobot.login();
        m_robots.put(account, gameRobot);
        System.out.println(String.format("add robot, serverIP:%s, account:%s", serverIP, account));
    }

    public void removeRobot(String account) {
        GameRobot robot = m_robots.get(account);
        if (robot == null) return;

        m_robots.remove(account);
        robot.disconnect();

        if (robot == m_curRobot) m_curRobot = null;

        System.out.println("remove robot, account:" + account);
    }

    public GameRobot setUseRobot(String account) {
        m_curRobot = m_robots.get(account);
        return m_curRobot;
    }

    public GameRobot getRobot(String account) {
        return m_robots.get(account);
    }

    public GameRobot getOneRobot() {
        int count = m_robots.size();
        if (count == 0) return null;

        Random random = new Random();
        int r = random.nextInt(count);
        return (GameRobot) m_robots.values().toArray()[r];
    }

    public GameRobot getCurRobot() {
        return m_curRobot;
    }

    public GameRobot getCurOrRandRobot() {
        if (m_curRobot != null) return m_curRobot;
        return getOneRobot();
    }

    public void forEach(Consumer<GameRobot> consumer) {
        m_robots.values().forEach((robot) -> consumer.accept(robot));
    }

    public static RobotMgr getInstance() {
        return RobotMgr.m_robotMgr;
    }
}
