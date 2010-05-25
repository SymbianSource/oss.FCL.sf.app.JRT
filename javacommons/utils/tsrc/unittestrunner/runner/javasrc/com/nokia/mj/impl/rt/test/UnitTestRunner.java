/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.rt.test;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.rt.support.JvmInternal;
import com.nokia.mj.impl.rt.support.ThreadEventListener;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.utils.Tokenizer;


public class UnitTestRunner
{
    /**
     * UnitTestRunner main program.
     *
     * @param args command line arguments
     */
    public static void main(String[] args)
    {
        int exitCode = 0;

        System.out.println("java.version: " +
                           System.getProperty("java.version"));
        System.out.println("java.fullversion: " +
                           System.getProperty("java.fullversion"));

        if (args.length == 0)
        {
            Logger.ELOG(Logger.EJavaRuntime, "No args provided.");
            exitCode = -2;
        }
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
                    Logger.ELOG(Logger.EJavaRuntime, "Unhandled exception in " +
                                threadName, throwable);
                    System.exit(-1);
                }
            });
            test(args);
        }
        catch (Throwable t)
        {
            Logger.ELOG(Logger.EJavaRuntime, "Unhandled exception in main", t);
            exitCode = -1;
        }
        System.exit(exitCode);
    }

   /**
     * Main program for executing tests.
     */
    private static void test(String[] args)
    {
        String mainClass = getMainClass(args[0]);
        if (mainClass == null)
        {
            Logger.ELOG(Logger.EJavaRuntime, "Mainclass must be specified.");
            return;
        }

        try
        {
            // Instantiate mainclass and call its main() method.
            Class clazz = Class.forName(mainClass);
            InstallerMain instMain = (InstallerMain)clazz.newInstance();
            instMain.installerMain(args);
        }
        catch (Exception ex)
        {
            Logger.ELOG(Logger.EJavaRuntime, "Running mainclass failed", ex);
        }
    }

   /**
     * Main program for executing tests.
     */
    private static String getMainClass(String arg)
    {
        String[] mainClassArg = Tokenizer.split(arg, "=");
        if (mainClassArg.length != 2)
        {
            return null;
        }
        if (!mainClassArg[0].equals("-mainclass"))
        {
            return null;
        }
    return mainClassArg[1];
        
    }

    /**
     * Logs arguments given to TCK Runner
     */
    private static void logArgs(String[] args)
    {
        StringBuffer buf = new StringBuffer("UnitTestRunner args:");
        for (int i = 0; i < args.length; i++)
        {
            buf.append(" ");
            buf.append(args[i]);
        }
        Logger.ILOG(Logger.EJavaRuntime, buf.toString());
    }

}
