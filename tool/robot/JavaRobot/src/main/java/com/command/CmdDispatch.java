package com.command;

import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Map;

public class CmdDispatch {

    private String m_cmdName;
    private Map<Object, Method> m_cmdMethods = new HashMap<>();

    public CmdDispatch(String cmdName) {
        initCmdMethods(cmdName);
    }

    public void initCmdMethods(String cmdName) {
        m_cmdName = cmdName;
        Method annoMethod;
        try {
            annoMethod = CmdAnnotation.class.getDeclaredMethod(m_cmdName);
        }catch (Exception e) {
            e.printStackTrace();
            System.out.println("not found cmd Annotation:" + m_cmdName);
            return;
        }

        Method[] methods = getClass().getDeclaredMethods();
        for (Method method : methods) {
            CmdAnnotation cmdAnnotation = method.getAnnotation(CmdAnnotation.class);
            if (cmdAnnotation == null) {
                continue;
            }
            try {
                Object cmd = annoMethod.invoke(cmdAnnotation);
                if (m_cmdMethods.containsKey(cmd)) {
                    System.out.println("cmd method exist, m_cmdName:" + m_cmdName + ",cmd" + cmd);
                    continue;
                }
                m_cmdMethods.put(cmd, method);
            }catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public boolean dispatchCmd(Object cmd, Object... args) {
        Method method = m_cmdMethods.getOrDefault(cmd, null);
        if (method == null) {
            System.out.println("not found cmd func, cmdName:" + m_cmdName + ",cmd:" + cmd);
            return false;
        }

        try {
            method.setAccessible(true);
            method.invoke(this, args);
            return true;
        }catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }
}
