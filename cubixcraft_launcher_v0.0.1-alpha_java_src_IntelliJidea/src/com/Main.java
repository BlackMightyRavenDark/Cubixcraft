package com;

import org.json.simple.parser.JSONParser;
import org.json.simple.*;
import org.json.simple.parser.ParseException;
import sun.awt.OSInfo;

import javax.imageio.ImageIO;
import javax.swing.*;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.net.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CompletableFuture;

public class Main extends JFrame implements ActionListener {

    private String directoryHomeSelf;
    private String directoryHomeGame;
    public String directorySelf;
    private String libraryPath;
    public VersionStruct[] versions;
    private Logger logger;

    private JLabel label1;
    public JComboBox comboBoxVersions;
    private JButton btnDownload;
    private JTextArea textArea;
    private JScrollPane scroll;
    private Font font;

    private PrintStream myPrintStream;
    private boolean firstShown = true;

    private ConfigurationManager config;
    private static Main mainObj;

    public static Main getMain() {
        return mainObj;
    }

    public Main() {

        font = new Font("Tahoma", Font.PLAIN, 16);
        label1 = new JLabel();
        label1.setBounds(4, 2, 70, 20);
        label1.setFont(font);
        label1.setText("Версия:");
        add(label1);

        comboBoxVersions = new JComboBox();
        comboBoxVersions.setBounds(78, 2, 340, 20);
        comboBoxVersions.setFont(new Font("Tahoma", Font.PLAIN, 14));
        comboBoxVersions.setEnabled(false);
        add(comboBoxVersions);

        btnDownload = new JButton();
        btnDownload.setBounds(4, 25, 400, 40);
        btnDownload.setFont(font);
        btnDownload.setText("Start the game!");
        btnDownload.setEnabled(false);
        btnDownload.addActionListener(this);
        add(btnDownload);

        textArea = new JTextArea(1, 1);
        textArea.setBackground(Color.BLACK);
        textArea.setForeground(Color.GREEN);
        textArea.setLineWrap(true);
        textArea.setEditable(false);

        scroll = new JScrollPane(textArea,
                JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
                JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
        scroll.setBounds(4,70, 80, 90);
        add(scroll, BorderLayout.CENTER);

        myPrintStream = new PrintStream(new MyOutputStream(textArea));
        logger = new Logger(new PrintStream(myPrintStream));

        try {
            URL url = getClass().getResource("/textures/tree.png");
            setIconImage(ImageIO.read(url));
        } catch (IOException | IllegalArgumentException e) {
            logger.addLogWithNewLine("Cannot load image: /textures/tree.png");
            e.printStackTrace();
            e.printStackTrace(myPrintStream);
        }

        if (OSInfo.getOSType() == OSInfo.OSType.WINDOWS){
            directoryHomeSelf = System.getenv("appdata") + File.separator + "cubixcraft_launcher";
            directoryHomeGame = System.getenv("appdata") + File.separator + "cubixcraft";
        }
        else {
            directoryHomeSelf = System.getProperty("user.home") + File.separator + ".cubixcraft_launcher";
            directoryHomeGame = System.getProperty("user.home") + File.separator + ".cubixcraft";
        }

        addComponentListener(new ComponentAdapter() {
            @Override
            public void componentShown(ComponentEvent e){
                super.componentShown(e);
                if (firstShown){
                    getVersionsList();
                    if (!config.lastUserVersion.isEmpty()){
                        int i;
                        boolean found = false;
                        for (i = 0; i < versions.length; i++){
                            if (versions[i].title.equals(config.lastUserVersion)){
                                found = true;
                                break;
                            }
                        }
                        if (comboBoxVersions.getItemCount() > 0) {
                            comboBoxVersions.setSelectedIndex(found ? i : 0);
                        }
                    }
                    firstShown = false;
                }
            }

            @Override
            public void componentResized(ComponentEvent e) {
                super.componentResized(e);
                comboBoxVersions.setSize(getWidth() - 90, 20);
                scroll.setSize(getWidth() - 20, getHeight() - 110);
                textArea.setSize(getWidth() - 20, getHeight() - 110);
                btnDownload.setSize(getWidth() - 20, btnDownload.getHeight());
            }
        });

        addWindowListener(new WindowListener() {
            @Override
            public void windowOpened(WindowEvent e) {}
            @Override
            public void windowClosing(WindowEvent e) {
                config.saveConfig();
            }
            @Override
            public void windowClosed(WindowEvent e) {}
            @Override
            public void windowIconified(WindowEvent e) {}
            @Override
            public void windowDeiconified(WindowEvent e) {}
            @Override
            public void windowActivated(WindowEvent e) {}
            @Override
            public void windowDeactivated(WindowEvent e) {}
        });

        mainObj = this;
        directorySelf = System.getProperty("user.dir");
        config = new ConfigurationManager(directoryHomeSelf);
        config.loadConfig();
        logger.addLogWithNewLine("[Launcher] > Launcher home: " + directoryHomeSelf);
        logger.addLogWithNewLine("[Launcher] > Cubixcraft home: " + directoryHomeGame);
        logger.addLogWithNewLine("[Launcher] > Running directory: " + directorySelf);
    }

    private int getVersionsList() {
        try {
            String jsonString = "";
            for (int i = 0; i < config.motherHosts.size(); i++) {
                String tmpHost = config.motherHosts.get(i).endsWith("/") ?
                        config.motherHosts.get(i) : config.motherHosts.get(i).concat("/");
                String urlString = tmpHost.concat("files/cubixcraft/java/cubixcraft.json");
                jsonString = httpGet(urlString);
                if (!jsonString.isEmpty()){
                    if (i > 0){
                        config.motherHosts.add(0, config.motherHosts.get(i));
                        config.motherHosts.remove(i + 1);
                    }
                    break;
                }
            }
            File versionsListLocal = new File(directoryHomeSelf.concat(File.separator).concat("versions.json"));
            if (!jsonString.isEmpty()) {
                if (versionsListLocal.exists()) {
                    versionsListLocal.delete();
                }
                FileWriter fileWriter = new FileWriter(versionsListLocal);
                fileWriter.write(jsonString);
                fileWriter.close();
            }
            else {
                if (versionsListLocal.exists()) {
                    String s = versionsListLocal.getAbsolutePath();
                    logger.addLogWithNewLine("[Launcher] > Can not retrieve versions list from server! " +
                            "Using local list " + s);
                    jsonString = new String(Files.readAllBytes(Paths.get(s)));
                }
            }
            if (!jsonString.isEmpty()) {
                JSONParser jsonParser = new JSONParser();
                JSONObject jsonObject = (JSONObject) jsonParser.parse(jsonString);
                JSONArray ja = (JSONArray) ((JSONObject) (jsonObject.get("cubixcraft"))).get("versions");
                if (ja.size() > 0) {
                    versions = new VersionStruct[ja.size()];
                    for (int i = 0; i < ja.size(); i++) {
                        versions[i] = new VersionStruct();
                        versions[i].dependencyJarUrls = new ArrayList<String>();
                        versions[i].dependencyJarLocals = new ArrayList<String>();
                        versions[i].nativeUrls = new ArrayList<>();
                        versions[i].nativeLocals = new ArrayList<>();
                        versions[i].title = ((JSONObject) ja.get(i)).get("title").toString();
                        versions[i].displayTitle = ((JSONObject) ja.get(i)).get("display_title").toString();
                        versions[i].dirName = ((JSONObject) ja.get(i)).get("title").toString();
                        versions[i].rootLocal = directoryHomeGame + File.separator + "versions" + File.separator + versions[i].dirName;
                        versions[i].root = ((JSONObject) ja.get(i)).get("root_dir").toString();
                        JSONArray ja2 = (JSONArray) (((JSONObject) ja.get(i)).get("files"));
                        versions[i].dep = ((JSONObject) ja.get(i)).get("dependency").toString();
                        versions[i].executableJar = ((JSONObject) ja.get(i)).get("executable_jar").toString();
                        versions[i].mainClass = ((JSONObject) ja.get(i)).get("main_class").toString();
                        versions[i].fileRelativePaths = new String[ja2.size()];
                        versions[i].fileSizes = new int[ja2.size()];
                        for (int i2 = 0; i2 < ja2.size(); i2++) {
                            versions[i].fileRelativePaths[i2] = ((JSONObject) (ja2.get(i2))).get("relative_path").toString();
                            versions[i].fileSizes[i2] = (int) (long) ((JSONObject) (ja2.get(i2))).get("size");
                        }

                        JSONObject jDeps = (JSONObject) (((JSONObject) (jsonObject.get("cubixcraft"))).get("libraries"));

                        String depsRoot = jDeps.get("root_dir").toString();
                        JSONArray jaDeps = (JSONArray) (jDeps.get("list"));
                        int i2;
                        for (i2 = 0; i2 < jaDeps.size(); i2++) {
                            String t = ((JSONObject) (jaDeps.get(i2))).get("title").toString();
                            if (t.equalsIgnoreCase(versions[i].dep)) {
                                break;
                            }
                        }
                        JSONObject jsonLib = (JSONObject) (jaDeps.get(i2));
                        JSONArray jaJars = (JSONArray) (((JSONObject) (jsonLib.get("files"))).get("jars"));
                        versions[i].dependencyJarSizes = new int[jaJars.size()];
                        for (int i3 = 0; i3 < jaJars.size(); i3++) {
                            versions[i].dependencyJarSizes[i3] = (int) (long) (((JSONObject) (jaJars.get(i3))).get("size"));
                            String depPath = ((JSONObject) (jaJars.get(i3))).get("relative_path").toString();
                            versions[i].dependencyJarUrls.add(depsRoot + depPath);
                            versions[i].dependencyJarLocals.add(directoryHomeGame + File.separator +
                                    "libraries" + depPath.replace("/", File.separator));
                        }
                        JSONArray jaNatives = (JSONArray) (((JSONObject) (jsonLib.get("files"))).get(
                                OSInfo.getOSType() == OSInfo.OSType.WINDOWS ? "natives-windows" : "natives-linux"));
                        if (jaNatives == null) {
                            logger.addLogWithNewLine("Getting natives failed!");
                            return 0;
                        }
                        versions[i].dependencyNativeSizes = new int[jaNatives.size()];
                        for (int i3 = 0; i3 < jaNatives.size(); i3++) {
                            versions[i].dependencyNativeSizes[i3] = (int) (long) (((JSONObject) (jaNatives.get(i3))).get("size"));
                            String depPath = ((JSONObject) (jaNatives.get(i3))).get("relative_path").toString();
                            versions[i].nativeUrls.add(depsRoot + depPath);
                            if (OSInfo.getOSType() == OSInfo.OSType.WINDOWS) {
                                versions[i].nativeLocals.add(directoryHomeGame + File.separator +
                                        "libraries" + depPath.replace("/", File.separator));
                            } else {
                                versions[i].nativeLocals.add(directoryHomeGame + File.separator +
                                        "libraries" + depPath);
                            }
                        }
                    }

                    logger.addLogWithNewLine("[Launcher] > Available versions:");
                    for (int i = 0; i < versions.length; i++) {
                        comboBoxVersions.addItem(versions[i].displayTitle);
                        logger.addLogWithNewLine("[Launcher] > -= " + versions[i].displayTitle + " =-");
                    }
                    comboBoxVersions.setEnabled(true);
                    btnDownload.setEnabled(true);
                    return ja.size();
                }
            }
            else
            {
                logger.addLogWithNewLine("Unable to get versions list!");
                btnDownload.setEnabled(false);
                comboBoxVersions.setEnabled(false);
                return 0;
            }
        } catch (ParseException | IOException e) {
            logger.addLogWithNewLine("JSON parsing error!");
            e.printStackTrace();
            e.printStackTrace(myPrintStream);
            btnDownload.setEnabled(false);
            return 0;
        }
        return 0;
    }

    private void downloadVersion(VersionStruct ver){
        for (int i = 0; i < ver.fileRelativePaths.length; i++) {
            for (int iHosts = 0; iHosts < config.motherHosts.size(); iHosts++) {
                String pth = extractUrlFilePath(ver.fileRelativePaths[i]);
                String fileDir = directoryHomeGame + File.separator + "versions" + File.separator +
                        ver.dirName + (pth.isEmpty() ? "" : File.separator + pth);
                String fileName = extractUrlFileName(ver.fileRelativePaths[i]);
                if (OSInfo.getOSType() == OSInfo.OSType.WINDOWS) {
                    fileDir = fileDir.replace("/", File.separator);
                }
                String fullFileName = fileDir + (fileName.startsWith("/") ? fileName : File.separator + fileName);
                File f = new File(fullFileName);
                if (!f.exists() || f.length() != ver.fileSizes[i]) {
                    File f2 = new File(fileDir);
                    if (!f2.exists()) {
                        f2.mkdirs();
                    }
                    if (f.exists()) {
                        f.delete();
                    }

                    String tmp = (config.motherHosts.get(iHosts).endsWith("/") ?
                            config.motherHosts.get(iHosts) : config.motherHosts.get(iHosts).concat("/")) +
                            ver.root + ver.fileRelativePaths[i];
                    if (downloadFile(tmp, fullFileName) == ver.fileSizes[i]){
                        if (iHosts > 0){
                            config.motherHosts.add(0, config.motherHosts.get(iHosts));
                            config.motherHosts.remove(iHosts + 1);
                        }
                        break;
                    }
                }
            }
        }
        for (int i = 0; i < ver.dependencyJarUrls.size(); i++) {
            for (int iHosts = 0; iHosts < config.motherHosts.size(); iHosts++) {
                File f = new File(ver.dependencyJarLocals.get(i));
                if (!f.exists() || f.length() != ver.dependencyJarSizes[i]) {
                    File f2 = new File(extractFilePath(ver.dependencyJarLocals.get(i)));
                    if (!f2.exists()) {
                        f2.mkdirs();
                    }

                    String tmpHost = config.motherHosts.get(iHosts).endsWith("/") ?
                            config.motherHosts.get(iHosts) : config.motherHosts.get(iHosts).concat("/");
                    if (downloadFile(tmpHost + ver.dependencyJarUrls.get(i),
                            ver.dependencyJarLocals.get(i)) == ver.dependencyJarSizes[i]){
                        if (iHosts > 0){
                            config.motherHosts.add(0, config.motherHosts.get(iHosts));
                            config.motherHosts.remove(iHosts + 1);
                        }
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < ver.nativeUrls.size(); i++) {
            for (int iHosts = 0; iHosts < config.motherHosts.size(); iHosts++) {
                libraryPath = extractFilePath(ver.nativeLocals.get(0));
                File f = new File(ver.nativeLocals.get(i));
                if (!f.exists() || f.length() != ver.dependencyNativeSizes[i]) {
                    String dir = extractFilePath(ver.nativeLocals.get(i));
                    File f2 = new File(dir);
                    if (!f2.exists()) {
                        f2.mkdirs();
                    }
                    String tmpHost = config.motherHosts.get(iHosts).endsWith("/") ?
                            config.motherHosts.get(iHosts) : config.motherHosts.get(iHosts).concat("/");
                    if (downloadFile(tmpHost + ver.nativeUrls.get(i),
                            ver.nativeLocals.get(i)) == ver.dependencyNativeSizes[i]){
                        if (iHosts > 0){
                            config.motherHosts.add(0, config.motherHosts.get(iHosts));
                            config.motherHosts.remove(iHosts + 1);
                        }
                        break;
                    }
                }
            }
        }
    }

    private String extractUrlFilePath(String t){
        int n = t.lastIndexOf("/");
        return n >= 0 ? t.substring(0, n) : "";
    }

    private String extractUrlFileName(String urlString){
        int n = urlString.lastIndexOf("/");
        return n >= 0 ? urlString.substring(n + 1) : urlString;
    }

    public static String extractFilePath(String s){
        int n = s.lastIndexOf(File.separator);
        return n >= 0 ? s.substring(0, n + 1) : "";
    }

    private int downloadFile(String urlString, String fn) {
        logger.addLog("[Launcher] > Downloading " + extractUrlFileName(urlString) + "...");
        File f = new File(fn);
        if (f.exists()){
            f.delete();
        }
        URL url;
        try {
            url = new URL(urlString);
        } catch (MalformedURLException e) {
            logger.addLogWithNewLine("Failed!");
            e.printStackTrace();
            e.printStackTrace(myPrintStream);
            return 0;
        }
        BufferedInputStream bufferedInputStream;
        try {
            bufferedInputStream = new BufferedInputStream(url.openStream());
        } catch (IOException e) {
            logger.addLogWithNewLine("Failed!");
            e.printStackTrace(myPrintStream);
            e.printStackTrace();
            return 0;
        }
        FileOutputStream stream;
        try {
            stream = new FileOutputStream(fn);
        } catch (FileNotFoundException e) {
            logger.addLogWithNewLine("Failed!");
            e.printStackTrace();
            e.printStackTrace(myPrintStream);
            return 0;
        }

        int count;
        byte[] b1 = new byte[1024 * 32];

        do {
            try {
                count = bufferedInputStream.read(b1);
            } catch (IOException e) {
                logger.addLogWithNewLine("Failed!");
                e.printStackTrace();
                e.printStackTrace(logger.getPrintStream());
                return (int)f.length();
            }
            if (count > 0) {
                try {
                    stream.write(b1, 0, count);
                } catch (IOException e) {
                    logger.addLogWithNewLine("Failed!");
                    e.printStackTrace();
                    e.printStackTrace(logger.getPrintStream());
                    return (int) f.length();
                }
            }
        } while (count > 0);

        try {
            bufferedInputStream.close();
            stream.close();
        } catch (IOException e) {
            logger.addLogWithNewLine(" Unable to close a stream!");
            e.printStackTrace();
            e.printStackTrace(logger.getPrintStream());
            return (int)f.length();
        }

        logger.addLogWithNewLine("OK");
        return (int)f.length();
    }

    public String httpGet(String urlString) {
        logger.addLog("[Launcher] > Resolving versions list... ");
        try {
            StringBuilder result = new StringBuilder();
            URL url = new URL(urlString);
            HttpURLConnection conn = (HttpURLConnection) url.openConnection();
            conn.setRequestMethod("GET");
            int code = conn.getResponseCode();
            if (code == 200 || code == 0) {
                logger.addLogWithNewLine("OK");
            }
            else
            {
                logger.addLogWithNewLine("error! " + code);
                return "";
            }
            BufferedReader rd = new BufferedReader(new InputStreamReader(conn.getInputStream()));
            String line;
            while ((line = rd.readLine()) != null) {
                result.append(line);
            }
            rd.close();
            return result.toString();
        } catch (Exception e) {
            logger.addLogWithNewLine("error!");
            e.printStackTrace();
            e.printStackTrace(logger.getPrintStream());
        }
        return "";
    }

    public Logger getLogger(){
        return logger;
    }

    public static void main(String[] args) {
        System.out.println("Starting Cubixcraft launcher version 0.0.1-alpha");
        Main mainWnd = new Main();
        mainWnd.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        mainWnd.setLayout(new LayoutNone());
        mainWnd.setSize(500, 500);
        mainWnd.setTitle("Cubixcraft launcher 0.0.1-alpha");
        mainWnd.setVisible(true);
    }

    private static CompletableFuture<Boolean> redirectToLogger(final InputStream inputStream, Logger log) {
        return CompletableFuture.supplyAsync(() -> {
            try (
                    InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
                    BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
            ) {
                String line;
                while ((line = bufferedReader.readLine()) != null) {
                    log.addLogWithNewLine("[Cubixcraft] > " + line);
                }
                return true;
            } catch (IOException e) {
                return false;
            }
        });
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        if (e.getSource() == btnDownload){
            btnDownload.setEnabled(false);
            VersionStruct ver = versions[comboBoxVersions.getSelectedIndex()];
            logger.addLogWithNewLine("[Launcher] > Preparing to launch " + ver.displayTitle + "...");
            downloadVersion(ver);
            File file = new File(ver.rootLocal + File.separator + ver.executableJar);
            if (file.exists()){
                try {
                    List<String> commandLine = generateCommandLine(ver);
                    logger.addLogWithNewLine("[Launcher] > Executing version " + ver.displayTitle + "...");
                    logger.addLog("[Launcher] > Full command: ");
                    for (int i = 0; i < commandLine.size() - 1; i++) {
                       logger.addLog(commandLine.get(i) + " ");
                    }
                    logger.addLogWithNewLine(commandLine.get(commandLine.size() - 1));
                    setVisible(false);
                    ProcessBuilder processBuilder = new ProcessBuilder();
                    processBuilder.command(commandLine);
                    processBuilder.directory(new File(ver.rootLocal));

                    try {
                        Process process = processBuilder.start();
                        redirectToLogger(process.getInputStream(), logger);
                        redirectToLogger(process.getErrorStream(), logger);
                        int errorCode = process.waitFor();
                        logger.addLogWithNewLine("[Launcher] > Exiting cubixcraft with code " + errorCode);
                        setVisible(true);
                    } catch (InterruptedException e1){
                        setVisible(true);
                        e1.printStackTrace();
                        e1.printStackTrace(myPrintStream);
                    }
                } catch (IOException ex){
                    ex.printStackTrace();
                    ex.printStackTrace(myPrintStream);
                }
            }
            else
            {
                logger.addLogWithNewLine("[Launcher] > Executable JAR does not exists!");
            }
            btnDownload.setEnabled(true);
        }
    }

    private List<String> generateCommandLine(VersionStruct ver){
        List<String> res = new ArrayList<>();
        res.add("java");
        res.add("-Xmx1g");
        res.add("-Djava.library.path=" + libraryPath);
        res.add("-cp");
        String sep = OSInfo.getOSType() == OSInfo.OSType.WINDOWS ? ";" : ":";
        String t = "";
        for (int i = 0; i < ver.dependencyJarLocals.size(); i++){
            t = t.concat(ver.dependencyJarLocals.get(i)).concat(sep);
        }
        t = t.concat(ver.rootLocal).concat(File.separator).concat(ver.executableJar);
        res.add(t);
        res.add(ver.mainClass);
        return res;
    }
}
