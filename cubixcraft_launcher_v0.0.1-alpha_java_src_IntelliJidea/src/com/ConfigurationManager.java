package com;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;

public class ConfigurationManager {

    private Main mainClass;
    private String homeDir;
    private String configFileName;

    public ArrayList<String> motherHosts;
    public String lastUserVersion;

    public ConfigurationManager(String homeDir){
        mainClass = Main.getMain();
        this.homeDir = homeDir;
        configFileName = homeDir + File.separator + "launcher.json";
        motherHosts = new ArrayList<>();
    }

    private String getValueAsString(Object obj, String defValue){
        return obj == null ? defValue : obj.toString();
    }

    public void loadConfig(){
        File cfgFile = new File(configFileName);
        if (cfgFile.exists()) {
            mainClass.getLogger().addLogWithNewLine("[Launcher] > Loading configuration...");
            JSONParser jsonParser = new JSONParser();
            String jsonString;
            try {
                jsonString = new String(Files.readAllBytes(Paths.get(configFileName)));
            } catch (IOException e) {
                mainClass.getLogger().addLogWithNewLine("[Launcher] > Load configuration failed!");
                e.printStackTrace();
                e.printStackTrace(mainClass.getLogger().getPrintStream());
                return;
            }
            try {
                JSONObject jsonConfig = (JSONObject) jsonParser.parse(jsonString);
                JSONArray jaHosts = (JSONArray)(jsonConfig.get("mother_hosts"));
                if (jaHosts != null && jaHosts.size() > 0) {
                    for (int i = 0; i < jaHosts.size(); i++) {
                        motherHosts.add(jaHosts.get(i).toString());
                    }
                }
                else
                {
                    motherHosts.add("http://46.175.35.158");
                    motherHosts.add("http://46.175.39.161");
                }
                lastUserVersion = getValueAsString(jsonConfig.get("last_used_version"), "");
            } catch (ParseException e) {
                e.printStackTrace();
                e.printStackTrace(mainClass.getLogger().getPrintStream());
            }
        }
        else
        {
            //set all settings to default values
            mainClass.getLogger().addLogWithNewLine("[Launcher] > Configuration file does not exists! Loading defaults...");
            motherHosts.add("http://46.175.35.158");
            motherHosts.add("http://46.175.39.161");
            lastUserVersion = "";
            saveConfig();
        }
    }

    public void saveConfig() {
        mainClass.getLogger().addLogWithNewLine("[Launcher] > Saving config...");
        JSONObject j = new JSONObject();
        j.put("mother_hosts", motherHosts);
        if (mainClass.comboBoxVersions.getItemCount() > 0) {
            int n = mainClass.comboBoxVersions.getSelectedIndex();
            if (n >= 0) {
                j.put("last_used_version", mainClass.versions[n].title);
            }
        }

        try {
            File pth = new File(homeDir);
            if (!pth.isFile() && !pth.exists()){
                pth.mkdirs();
            }
            FileWriter fileWriter = new FileWriter(configFileName);
            fileWriter.write(j.toJSONString());
            fileWriter.flush();
        } catch (IOException e) {
            e.printStackTrace();
            e.printStackTrace(mainClass.getLogger().getPrintStream());
        }
    }
}
