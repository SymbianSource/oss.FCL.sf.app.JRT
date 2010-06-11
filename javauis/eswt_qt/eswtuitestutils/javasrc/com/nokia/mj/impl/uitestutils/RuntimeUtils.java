package com.nokia.mj.impl.uitestutils;

import org.eclipse.ercp.swt.midp.UIThreadSupport;

public class RuntimeUtils {

    public static final int S60 = 1;
    public static final int X11 = 2;
    
    public static int getPlatform() {
        return S60;
    }

    public synchronized static final int startUI(
            final Runnable callback) {
        UIThreadSupport.startInUIThread(new Runnable() {
            public void run() {
                callback.run();
            }
        });
        return 0;
    }
}
