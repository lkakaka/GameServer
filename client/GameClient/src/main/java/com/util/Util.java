package com.util;

import java.security.MessageDigest;
import java.util.zip.Inflater;

public class Util {
    private static final char[] s_arrHexChar = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    public static byte[] intToByteArray(int num) {
        byte[] arr = new byte[4];
        for (int i = 0; i < 4; i++) {
            arr[3 - i] = (byte) (num >>> (i * 8));
        }
        return arr;
    }

    public static void intToByteArray(int num, byte[] arr, int offset) {
        for (int i = 0; i < 4; i++) {
            arr[offset + i] = (byte) (num >> (i * 8));
        }
    }

    public static void copyByteArray(byte[] src, byte[] dst, int dst_offset) {
        for (int i = 0; i < src.length; i++) {
            dst[dst_offset + i] = src[i];
        }
    }

    public static String toHexString(byte[] arrBytes) {
        StringBuilder strbld = new StringBuilder(arrBytes.length * 2);
        for (byte bt : arrBytes) {
            strbld.append(s_arrHexChar[(bt >>> 4) & 0xf]);
            strbld.append(s_arrHexChar[bt & 0xf]);
        }
        return strbld.toString();
    }

    private static String digest(String strType, byte[] strTemp) {
        try {
            MessageDigest mdTemp = MessageDigest.getInstance(strType);
            mdTemp.update(strTemp);
            return toHexString(mdTemp.digest());
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    public static String MD5(String str) {
        try {
            return digest("MD5", str.getBytes("UTF-8"));
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "";
    }

    public static void logInfo(String fmt, Object... args){
        System.out.println(String.format(fmt, args));
    }
}
