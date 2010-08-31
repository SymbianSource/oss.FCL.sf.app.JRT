package com.nokia.mj.impl.uitestutils;

public class RuntimeUtils {

    public static final int S60 = 1;
    public static final int X11 = 2;
    
    public static int getPlatform() {
        return X11;
    }

    // We don't support InstallerMain-based test running for X11 target,
    // so only return 0 here.
    public synchronized static final int startUI(
            final Runnable callback) {
        return 0;
    }
}
