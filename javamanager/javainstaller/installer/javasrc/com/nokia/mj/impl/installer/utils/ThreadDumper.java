/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


package com.nokia.mj.impl.installer.utils;

import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.DebugUtils;

public class ThreadDumper implements Runnable
{
    /** Time when timer was started. */
    private long iStartTime = -1;
    /** Time to wait before thread dump. */
    private long iWaitTime = -1;
    /** Flag telling if ThreadDumper is running. */
    private boolean iRunning = false;

    private ThreadDumper()
    {
    }

    public static ThreadDumper getInstance()
    {
        ThreadDumper td = new ThreadDumper();
        Thread t = new Thread(td, "InstallerThreadDumper");
        Jvm.setThreadAsDaemon(t, true);
        synchronized (td)
        {
            td.iRunning = true;
            t.start();
        }
        return td;
    }

    synchronized public void startTimer(long aMillis)
    {
        Log.log("ThreadDumper.startTimer " + aMillis);
        if (!iRunning)
        {
            throw new IllegalThreadStateException("ThreadDumper not running");
        }
        iStartTime = System.currentTimeMillis();
        iWaitTime = aMillis;
        this.notify();
    }

    synchronized public void stopTimer()
    {
        Log.log("ThreadDumper.stopTimer");
        if (!iRunning)
        {
            throw new IllegalThreadStateException("ThreadDumper not running");
        }
        iStartTime = -1;
        this.notify();
    }

    synchronized public void destroy()
    {
        Log.log("ThreadDumper.destroy");
        iStartTime = -1;
        iRunning = false;
        DebugUtils.closeThreadDumper();
        this.notify();
    }

    public void run()
    {
        Log.log("ThreadDumper.run starts");
        while (iRunning)
        {
            boolean dumpThreads = false;
            synchronized (this)
            {
                try
                {
                    if (iStartTime == -1)
                    {
                        // Wait till startTimer gets called.
                        wait();
                    }
                    else
                    {
                        // Timer has been set, wait till it expires.
                        wait(iWaitTime);
                        if (iStartTime != -1 &&
                                System.currentTimeMillis() - iStartTime >= iWaitTime)
                        {
                            dumpThreads = true;
                        }
                    }
                }
                catch (InterruptedException ie)
                {
                }
            }
            if (dumpThreads)
            {
                Log.log("ThreadDumper.run dumping threads");
                DebugUtils.doThreadDump();
                iStartTime = -1;
            }
        }
        Log.log("ThreadDumper.run ends");
    }

    public String toString()
    {
        StringBuffer buf = new StringBuffer();
        buf.append("ThreadDumper running=").append(iRunning);
        buf.append(", startTime=").append(iStartTime);
        buf.append(", waitTime=").append(iWaitTime);
        return buf.toString();
    }
}
