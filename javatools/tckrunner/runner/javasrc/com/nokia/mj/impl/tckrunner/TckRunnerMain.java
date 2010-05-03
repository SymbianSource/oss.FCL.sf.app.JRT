/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.tckrunner;

import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.JvmInternal;
import com.nokia.mj.impl.rt.support.ThreadEventListener;
import com.nokia.mj.impl.tckrunner.TckRunnerUi;

public class TckRunnerMain
{

    /**
     * TckRunnerMain main program.
     *
     * @param aArgs command line arguments
     */
    public static void main(String[] aArgs)
    {
        Logger.ILOG(Logger.ETckRunner, "TckRunnerMain.main starts");
        int exitCode = 0;

        try
        {
            JvmInternal.setThreadEventListener(new ThreadEventListener()
            {
                public void threadStarting(Thread newThread, Thread parentThread) {}
                public void threadDied(Thread thread) {}
                public void uncaughtException(Thread thread, Throwable throwable)
                {
                    String threadName = null;
                    if (thread != null)
                    {
                        threadName = thread.getName();
                    }
                    Logger.ELOG(Logger.ETckRunner, "Unhandled exception in " +
                                threadName, throwable);
                }
            });
            exitCode = mainWithResult(aArgs);
        }
        catch (Throwable t)
        {
            Logger.ELOG(Logger.ETckRunner, "Unhandled exception in main", t);
            exitCode = -1;
        }

        Logger.ILOG(Logger.ETckRunner,  "TckRunnerMain.main exits with code " + exitCode);
        System.exit(exitCode);
    }

    /**
     * TckRunnerMain main program which returns error code indicating
     * operation result.
     *
     * @param aArgs command line arguments
     * @return error code indicating operation result
     */
    public static int mainWithResult(String[] aArgs)
    {
        int rc = 0;
        logArgs(aArgs);
        if (aArgs.length == 0)
        {
            // run in UI mode
            TckRunnerUi ui = new TckRunnerUi();
            ui.show();
        }
        else if (aArgs.length > 0 && aArgs[0].startsWith("-url="))
        {
            // run in console mode
            try
            {
                String url = aArgs[0].substring(5);
                String[] installerArgs = new String[aArgs.length-1];
                System.arraycopy(aArgs, 1, installerArgs, 0, installerArgs.length);
                TckRunner runner = new TckRunner();
                rc = runner.start(url, installerArgs);
            }
            catch (Exception e)
            {
                usage(aArgs);
                rc = -1;
            }
        }
        else if (aArgs.length == 2 && aArgs[0].equals("unittest"))
        {
            // Run in unit test mode
            // In unit test mode JavaInstaller/JavaCaptain is replaced by unit test harness
            TckRunner runner = new TckRunner();
            rc = runner.startInTestMode(aArgs[1]);
        }
        else
        {
            usage(aArgs);
        }
        return rc;
    }

    /**
     * Prints usage information to log and system output.
     */
    private static void usage(String[] aArgs)
    {
        String cmd = "tckrunner";
        String usageMsg = "\n\n" +
                          "Usage: "+cmd+" <command> [installer options]\n"+
                          "\n"+
                          "where command is one of:\n"+
                          "  -help                    Prints help on "+cmd+" usage.\n"+
                          "  -url=<url to JAD>        JavaTest Harness URL. For example:\n"+
                          "                           tckrunner -url=http://server:8080/test/getNextApp.jad\n"+
                          "  <no command>             tckrunner starts in UI mode\n"+
                          "where installer options is one of:\n"+
                          "  -iap=<iap_id>            Internet access point to be used in\n"+
                          "                           downloads during TCK run\n"+
                          "  -snap=<snap_id>          Service network access point to be used in\n"+
                          "                           downloads during TCK run\n"+
                          "  -skipjarcheck            Skip application package scanning.\n"+
                          "\n"+
                          "For a full list of available installer options use:\n"+
                          "  javainstaller help install\n";
        Logger.WLOG(Logger.ETckRunner, usageMsg);
    }

    /**
     * Logs arguments given to TCK Runner
     */
    private static void logArgs(String[] aArgs)
    {
        StringBuffer buf = new StringBuffer("tckrunner args:");
        for (int i = 0; i < aArgs.length; i++)
        {
            buf.append(" ");
            buf.append(aArgs[i]);
        }
        Logger.ILOG(Logger.ETckRunner, buf.toString());
    }

}
