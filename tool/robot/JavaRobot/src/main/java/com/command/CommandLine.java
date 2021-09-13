package com.command;

import org.jline.reader.LineReader;
import org.jline.reader.LineReaderBuilder;
import org.jline.reader.impl.history.DefaultHistory;
import org.jline.terminal.Terminal;
import org.jline.terminal.TerminalBuilder;

import java.util.Scanner;
import java.util.concurrent.atomic.AtomicInteger;

public class CommandLine {

    public static void startInput(){
//        Scanner scan = new Scanner(System.in);
        Terminal terminal;
        try {
            terminal = TerminalBuilder.builder().system(true).build();
        }catch (Exception e) {
            e.printStackTrace();
            return;
        }
//        LineReader lineReader = LineReaderBuilder.builder().terminal(terminal).history(new DefaultHistory()).build();
        LineReader lineReader = LineReaderBuilder.builder().terminal(terminal).build();
        String prompt = "command> ";

        Thread t = new Thread(new Runnable() {
            @Override
            public void run() {

                while (true) {
                    String line;
                    line = lineReader.readLine(prompt);
//                    System.out.println(line);
//                    System.out.println("please input:");
//                    String inputString = scan.nextLine(); //scan.next();
                    CommandCmd cmd = new CommandCmd(line);
                    cmd.extcute();
//                    AtomicInteger ai = new AtomicInteger();t
//                    ai.compareAndSet(1, 1);
                }
            }
        });
        t.start();
//        try {
//            t.join();
//        }catch (Exception e) {
//            e.printStackTrace();
//        }
    }
}
