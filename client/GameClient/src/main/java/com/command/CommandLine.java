package com.command;

import java.util.Scanner;
import java.util.concurrent.atomic.AtomicInteger;

public class CommandLine {

    public static void startInput(){
        Scanner scan = new Scanner(System.in);
        Thread t = new Thread(new Runnable() {
            @Override
            public void run() {

                while (true) {
                    System.out.println("please input:");
                    String inputString = scan.nextLine(); //scan.next();
                    CommandCmd cmd = new CommandCmd(inputString);
                    cmd.extcute();
                    AtomicInteger ai = new AtomicInteger();
                    ai.compareAndSet(1, 1);
                }
            }
        });
        t.start();
        try {
            t.join();
        }catch (Exception e) {
            e.printStackTrace();
        }
    }
}
