package com;

import java.util.ArrayList;

public class VersionStruct {

    public String title;
    public String displayTitle;
    public String dirName;
    public String root;
    public String rootLocal;
    public String[] fileRelativePaths;
    public int[] fileSizes;
    public String dep;
    public ArrayList<String> dependencyJarUrls;
    public ArrayList<String> dependencyJarLocals;
    public ArrayList<String> nativeUrls;
    public ArrayList<String> nativeLocals;
    public int[] dependencyJarSizes;
    public int[] dependencyNativeSizes;
    public String executableJar;
    public String mainClass;
}
