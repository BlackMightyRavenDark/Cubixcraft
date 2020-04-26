package com;

import java.io.PrintStream;

public class Logger {

    private PrintStream printStream;

    public Logger(PrintStream printStream){
        this.printStream = printStream;
    }

    public void addLogWithNewLine(String s){
        printStream.println(s);
        System.out.println(s);
    }

    public void addLog(String s){
        printStream.print(s);
        System.out.print(s);
    }

    public PrintStream getPrintStream(){
        return printStream;
    }
}
