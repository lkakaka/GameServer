package com.network;

public interface NetworkEventHandler {
    void onConnected();
    void onDisConnected();
    void onRecv(byte[] data, int count);
    void onSend();
    void onClose();
}
