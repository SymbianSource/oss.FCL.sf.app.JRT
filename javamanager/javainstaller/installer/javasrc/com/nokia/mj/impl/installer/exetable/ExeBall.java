/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.exetable;

import com.nokia.mj.impl.installer.ui.InstallerUiListener;
import com.nokia.mj.impl.installer.utils.Platform;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Tokenizer;

/**
 * Base class for objects which contain the information that is
 * passed from one execution step to another.
 */
public class ExeBall implements InstallerUiListener
{
    /** Flag for debugging purposes. */
    public static final int DBG_TIMES_OUT = 1;

    /** Exception which caused execution to fail. */
    private Exception iExecuteException = null;

    /** Exception which caused execution cancellation to fail. */
    private Exception iCancelException = null;

    /** Flag telling if execution happens silently,
        without any user interaction. */
    private boolean iSilent = false;

    /** Flag telling is user has cancelled the execution. */
    private boolean iCancelled = false;

    /** Flag telling if user has a possibility to cancel the execution. */
    private boolean iCanCancel = true;

    /** Last time when execution proceeded from one execution
        step to another. */
    private long iLastExeStepTime = 0;

    /** ForceExitThread instance. */
    private Thread iForceExitThread = null;

    /** Debug flags. */
    private int iDebugFlags = 0;

    /**
     * Message to be displayed together with execution
     * step times information.
     */
    private StringBuffer iTimesMsg = null;

    /**
     * This method is used to notify that execution has proceeded
     * from one execution step to another.
     */
    void exeStepChanged()
    {
        iLastExeStepTime = System.currentTimeMillis();
    }

    void setExecuteException(Exception aException)
    {
        iExecuteException = aException;
    }

    void setCancelException(Exception aException)
    {
        iCancelException = aException;
    }

    public Exception getExecuteException()
    {
        return iExecuteException;
    }

    public Exception getCancelException()
    {
        return iCancelException;
    }

    public boolean isSilent()
    {
        return iSilent;
    }

    public void setSilent(boolean aSilent)
    {
        iSilent = aSilent;
    }

    /**
     * Called just before InstallerUi is about to be opened.
     * This call can be used to hide the "preparing installation"
     * indicator.
     */
    public void uiIsReady()
    {
        // This method is overriden in InstallBall class.
    }

    /**
     * Called when user cancels the execution from the InstallerUi.
     * This method must return quickly.
     */
    public void cancel()
    {
        if (iCanCancel)
        {
            Log.log("User cancelled.");
            iCancelled = true;
            synchronized (this)
            {
                // Notify this object so that execution steps that are
                // waiting for it can proceed.
                this.notify();
            }
        }
        else
        {
            Log.log("User cancel ignored.");
        }
        if (System.getProperty("com.nokia.mj.impl.installer.disableforceexit") != null)
        {
            Log.log("ForceExitThread is disabled");
        }
        else
        {
            // Create a thread which will force the process to exit
            // if execution gets stuck.
            createForceExitThread();
        }
    }

    /**
     * Enables or disables the possibility to cancel the execution.
     */
    public void setCanCancel(boolean aCanCancel)
    {
        iCanCancel = aCanCancel;
        if (iCanCancel)
        {
            Log.log("User cancelling enabled.");
        }
        else
        {
            Log.log("User cancelling disabled.");
        }
    }

    /**
     * Returns true if user has cancelled the execution.
     */
    public boolean isCancelled()
    {
        return iCancelled;
    }

    /**
     * The inheriting class must override this method
     * to throw appropriate exception if user has cancelled
     * the execution.
     */
    public void checkForCancel() throws Exception
    {
        if (isCancelled())
        {
            Log.log("User cancel checked.");
        }
    }

    public boolean isDebugSet(int aDebugFlag)
    {
        return (iDebugFlags & aDebugFlag) != 0;
    }

    public void setDebug(int aDebugFlag)
    {
        iDebugFlags |= aDebugFlag;
    }

    public void setDebug(String aDebugAttribute)
    {
        if (aDebugAttribute == null)
        {
            return;
        }
        String[] tokens = Tokenizer.split(aDebugAttribute, ",");
        if (tokens != null)
        {
            for (int i = 0; i < tokens.length; i++)
            {
                if (tokens[i].equalsIgnoreCase("times"))
                {
                    setDebug(DBG_TIMES_OUT);
                }
            }
        }
    }

    /**
     * Add a message which will be displayed
     * together with execution step times
     * information in the end of execution.
     */
    public void addTimesMsg(String aMsg)
    {
        if (iTimesMsg == null)
        {
            iTimesMsg = new StringBuffer();
        }
        iTimesMsg.append(aMsg);
    }

    /**
     * Returns a message which will be displayed
     * together with execution step times
     * information in the end of execution.
     */
    public String getTimesMsg()
    {
        if (iTimesMsg != null)
        {
            return iTimesMsg.toString();
        }
        return null;
    }

    public void log(String aMsg)
    {
        if (isSilent())
        {
            Log.log(aMsg);
        }
        else
        {
            if (Platform.isLinux())
            {
                Log.logOut(aMsg);
            }
            else
            {
                Log.log(aMsg);
            }
        }
    }

    /**
     * Creates a new thread which will force the process
     * to exit with System.exit() after a while.
     */
    private void createForceExitThread()
    {
        if (iForceExitThread != null)
        {
            // ForceExitThread has already been created.
            return;
        }
        iForceExitThread = new Thread(new ForceExitThread(this),
                                      "ForceExitThread");
        Jvm.setThreadAsDaemon(iForceExitThread, true);
        iForceExitThread.start();
    }

    /**
     * ForceExitThread will force the process to exit with System.exit()
     * if the process still exists after 30 seconds has elapsed from
     * creation of this thread. This thread gets created when user
     * cancels the execution or presses the device red key.
     */
    private static class ForceExitThread implements Runnable
    {
        private static final int WAIT_MILLIS = 30000;
        private ExeBall iExeBall = null;

        private ForceExitThread()
        {
        }

        public ForceExitThread(ExeBall aExeBall)
        {
            iExeBall = aExeBall;
        }

        public void run()
        {
            Log.log("ForceExitThread starts");
            try
            {
                // Give installer a little time to make controlled
                // cancellation before forcing exit.
                Thread.sleep(WAIT_MILLIS);
            }
            catch (InterruptedException ie)
            {
            }
            // If the ForceExitThread still exists after WAIT_MILLIS time
            // has elapsed, make sure that JavaInstaller really exits by
            // calling System.exit().
            long timeSinceLastStep =
                System.currentTimeMillis() - iExeBall.iLastExeStepTime;
            Log.log("ForceExitThread: timeSinceLastStep=" + timeSinceLastStep +
                    ", timeSinceCancel=" + WAIT_MILLIS);
            Log.log("ForceExitThread System.exit(0)");
            System.exit(0);
        }
    }

}
