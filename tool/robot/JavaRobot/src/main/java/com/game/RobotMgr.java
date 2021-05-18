package com.game;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;

public class RobotMgr {
    private static final String m_localServerIP = "127.0.0.1";
    private static final String m_remoteServerIP = "111.229.80.201";
    private static final int m_serverPort = 30000;

    private static final RobotMgr m_robotMgr = new RobotMgr();
    public Map<String, GameRobot> robotMap = new HashMap<>();

    public void addRobot(String account, boolean bRemoteServer) {
        if (robotMap.containsKey(account)) {
            return;
        }

        String serverIP = bRemoteServer ? m_remoteServerIP : m_localServerIP;

        GameRobot gameRobot = new GameRobot();
        gameRobot.setServerIP(serverIP);
        gameRobot.setServerPort(RobotMgr.m_serverPort);
        gameRobot.setAccount(account);
        gameRobot.init();
        gameRobot.login();
        robotMap.put(account, gameRobot);
        System.out.println(String.format("add robot, serverIP:%s, account:%s", serverIP, account));
    }

    public void removeRobot(String account) {
        GameRobot robot = robotMap.getOrDefault(account, null);
        if (robot == null) {
            return;
        }
        robotMap.remove(account);
        robot.disconnect();
        System.out.println("remove robot, account:" + account);
    }

    public GameRobot getRobot(String account) {
        return robotMap.getOrDefault(account, null);
    }

    public GameRobot getOneRobot() {
        int count = robotMap.size();
        if (count == 0) {
            return null;
        }
        Random random = new Random();
        int r = random.nextInt(count);
        return (GameRobot) robotMap.values().toArray()[r];
    }

    public static RobotMgr getInstance() {
        return RobotMgr.m_robotMgr;
    }
}
