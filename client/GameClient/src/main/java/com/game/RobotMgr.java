package com.game;

import java.util.HashMap;
import java.util.Map;

public class RobotMgr {
    private static final String m_serverIP = "127.0.0.1";
    private static final int m_serverPort = 20000;

    private static final RobotMgr m_robotMgr = new RobotMgr();
    public Map<String, GameRobot> robotMap = new HashMap<>();

    public void addRobot(String account) {
        if (robotMap.containsKey(account)) {
            return;
        }

        GameRobot gameRobot = new GameRobot();
        gameRobot.setServerIP(RobotMgr.m_serverIP);
        gameRobot.setServerPort(RobotMgr.m_serverPort);
        gameRobot.setAccount(account);
        gameRobot.init();
        gameRobot.login();
        robotMap.put(account, gameRobot);
        System.out.println("add robot, account:" + account);
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

    public GameRobot getRobot(String passport) {
        return robotMap.getOrDefault(passport, null);
    }

    public static RobotMgr getInstance() {
        return RobotMgr.m_robotMgr;
    }
}
