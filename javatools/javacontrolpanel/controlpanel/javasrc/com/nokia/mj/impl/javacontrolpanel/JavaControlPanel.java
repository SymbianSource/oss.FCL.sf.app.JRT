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

package com.nokia.mj.impl.javacontrolpanel;

import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.JvmInternal;
import com.nokia.mj.impl.rt.support.ThreadEventListener;

class JavaControlPanel
{
    /**
     * JavaControlPanel main program.
     *
     * @param aArgs command line arguments
     */
    public static void main(String[] aArgs)
    {
        Logger.ILOG(Logger.EUtils, "JavaControlPanel.main starts");
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
                    Logger.ELOG(Logger.EUtils, "Unhandled exception in " +
                                threadName, throwable);
                }
            });
            exitCode = mainWithResult(aArgs);
        }
        catch (Throwable t)
        {
            Logger.ELOG(Logger.EUtils, "Unhandled exception in main", t);
            exitCode = -1;
        }

        Logger.ILOG(Logger.EUtils, "JavaControlPanel.main exits with code " + exitCode);
        System.exit(exitCode);
    }

    /**
     * JavaControlPanel main program which returns error code indicating
     * operation result.
     *
     * @param aArgs command line arguments
     * @return error code indicating operation result
     */
    public static int mainWithResult(String[] aArgs)
    {
        if (aArgs.length == 0)
        {
            JavaControlPanelUi ui = new JavaControlPanelUi();
            ui.show();
        }
        else
        {
            Logger.WLOG(Logger.EUtils, "JavaControlPanel making thread dump and exiting");
            new JavaCaptain().doThreadDump();
        }
        return 0;
    }

}
