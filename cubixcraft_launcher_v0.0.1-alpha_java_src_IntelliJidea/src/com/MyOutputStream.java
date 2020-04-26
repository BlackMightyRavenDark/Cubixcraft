package com;

import javax.swing.*;
import java.io.IOException;
import java.io.OutputStream;

public class MyOutputStream extends OutputStream {

    private JTextArea textArea;
    private StringBuilder stringBuilder;

    public MyOutputStream(JTextArea textArea){
        this.textArea = textArea;
        stringBuilder = new StringBuilder();
    }

    @Override
    public void write(int b) throws IOException {
        if (b == '\r')
            return;

        if (b == '\n') {
            String text = stringBuilder.toString() + "\n";
            textArea.append(text);
            stringBuilder.setLength(0);
            return;
        }

        stringBuilder.append((char) b);
    }
}
