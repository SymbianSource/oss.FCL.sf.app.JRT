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
import com.nokia.mj.impl.comms.CommsMessage;
import com.nokia.mj.impl.comms.CommsListener;
import com.nokia.mj.impl.comms.CommsEndpoint;

import com.nokia.mj.impl.tckrunner.Launcher;
import com.nokia.mj.impl.tckrunner.Installer;
import com.nokia.mj.impl.tckrunner.TckRunnerException;


public class TckRunner implements CommsListener
{
    /**
     *  How many times install operation is tried before giving up
     */
    private static final int NUMBER_OF_RETRIES = 30;

    /**
     *  Delay between retry operartions (in milliseconds)
     */
    private static final int RETRY_DELAY = 2000; // 2secs

    /**
     *  Error codes returned from TckRunner
     */
    private static final int ERR_NONE       = 0;
    private static final int ERR_TCK_RUNNER = -1;
    private static final int ERR_GENERAL    = -2;

    /**
     *  Comms address for TCK Runner
     */
    public static final int TCK_RUNNER_ADDRESS = 10300;

    /**
     *  Used comms message IDs
     */
    public static final int MSG_ID_APPLICATION_EXIT     = 104;
    public static final int MSG_ID_GET_NEXT_OPERATION   = 500;
    public static final int MSG_ID_OPERATION            = 501;
    public static final int MSG_ID_OPERATION_RESULT     = 502;

    /**
     *  Installer operations
     */
    public static final int OPERATION_INSTALL   = 0;
    public static final int OPERATION_UNINSTALL = 1;
    public static final int OPERATION_EXIT      = 2;

    private Installer iInstaller;
    private Launcher iLauncher;

    private String iUrl     = null;

    private int iRetryCount = 0;
    private int iExitCode   = 0;
    private int iState      = TckRunnerStateListener.STATE_NOT_RUNNING;
    private TckRunnerStateListener iListener = null;

    private Object iSync    = new Object();

    public TckRunner()
    {
    }

    /**
     * Starts TCK Runner.
     * Method blocks until TCK Runner is done or stop method is called.
     */
    public int start(String aUrl, String[] aInstallerArgs)
    {
        int rc = 0;
        try
        {
            iInstaller = new Installer(this);
            iLauncher = new Launcher(this);

            iRetryCount = 0;
            iUrl = aUrl;
            changeState(TckRunnerStateListener.STATE_INSTALL_APPLICATION);
            iInstaller.start(aInstallerArgs);
            synchronized (iSync)
            {
                iSync.wait();
            }
            rc = iExitCode;
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.ETckRunner, "Starting TCK Runner failed", e);
            rc = ERR_TCK_RUNNER;
        }
        finally
        {
            iInstaller.close();
            iLauncher.close();
        }
        return rc;
    }

    /**
     * Starts TCK Runner in unit test mode
     * In unit test mode JavaInstaller/JavaCaptain is replaced by unit test harness
     * and TCK Runner does not try to launch Java Installer process
     * Method blocks until TCK Runner is done or stop method is called.
     */
    public int startInTestMode(String aUrl)
    {
        int rc = 0;
        try
        {
            iInstaller = new Installer(this);
            iLauncher = new Launcher(this);

            iRetryCount = 0;
            iUrl = aUrl;
            changeState(TckRunnerStateListener.STATE_INSTALL_APPLICATION);

            synchronized (iSync)
            {
                iSync.wait();
            }
            rc = iExitCode;
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.ETckRunner, "Starting TCK Runner in test mode failed", e);
            rc = ERR_TCK_RUNNER;
        }
        finally
        {
            iInstaller.close();
            iLauncher.close();
        }
        return rc;
    }

    /**
     * Stops TCK Runner.
     */
    public void stop()
    {
        CommsEndpoint comms = new CommsEndpoint();
        try
        {
            iInstaller.stop();
            comms.connect(TCK_RUNNER_ADDRESS);

            CommsMessage msg = new CommsMessage();
            msg.setMessageId(TckRunner.MSG_ID_OPERATION_RESULT);
            msg.write(TckRunner.OPERATION_EXIT);
            msg.write(ERR_NONE);

            Logger.ILOG(Logger.ETckRunner, "Stopping TCK Runner");
            comms.send(msg);
            comms.disconnect();
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.ETckRunner, "Stopping TCK Runner failed", e);
            exit(ERR_TCK_RUNNER);
        }
        finally
        {
            comms.destroy();
        }
    }

    /**
     * Sets TCK Runner state listener.
     */
    public void setListener(TckRunnerStateListener aListener)
    {
        iListener = aListener;
    }

    /**
     * Message handler for Comms messages. This same listener is used as default listener
     * for two Comms connections. Hence, method is synchronized.
     *
     * Messages between TckRunner and Java Installer
     *
     * Message Name        Id      Contents
     *
     * Get Next Operation  500     None
     *
     * Operation           501     int operation   (install = 0, uninstall = 1, exit = 2)
     *                             String url      (present if operation = install)
     *                             String uid      (present if operation = uninstall)
     *
     * Operation Result    502     int operation   (install = 0, uninstall = 1, exit = 2)
     *                             int result      (ok = 0, error codes are negative)
     *                             int uidCount    (present if operation = install, value = uids.length)
     *                             String[] uids   (present if operation = install)
     *
     */
    public synchronized void processMessage(CommsMessage aMsg)
    {
        try
        {
            switch (aMsg.getMessageId())
            {
            case MSG_ID_GET_NEXT_OPERATION:
                iInstaller.saveRequest(aMsg);
                switch (iState)
                {
                case TckRunnerStateListener.STATE_INSTALL_APPLICATION:
                    iInstaller.install(iUrl);
                    break;
                case TckRunnerStateListener.STATE_UNINSTALL_APPLICATION:
                    String uid = iLauncher.getUidForUninstallation();
                    iInstaller.uninstall(uid);
                    break;
                case TckRunnerStateListener.STATE_RUN_APPLICATION:
                    // don't send next operation until application is ran
                    break;
                case TckRunnerStateListener.STATE_NOT_RUNNING:
                default:
                    iInstaller.stop();
                    break;
                }
                break;

            case MSG_ID_OPERATION_RESULT:
                int oper = aMsg.readInt();
                int result = aMsg.readInt();
                switch (oper)
                {
                case OPERATION_INSTALL:
                    if (result == 0)
                    {
                        int uidCount = aMsg.readInt();
                        String[] uids = new String[uidCount];
                        for (int i = 0; i < uids.length; i++)
                        {
                            String uid = aMsg.readString();
                            uids[i] = uid;
                        }
                        changeState(TckRunnerStateListener.STATE_RUN_APPLICATION);
                        iLauncher.setApplications(uids);
                        iLauncher.runNext();
                        iRetryCount = 0;
                    }
                    else
                    {
                        Logger.ELOG(Logger.ETckRunner, "Install operation failed: " + result);
                        retry();
                    }
                    break;

                case OPERATION_UNINSTALL:
                    if (result == 0)
                    {
                        changeState(TckRunnerStateListener.STATE_INSTALL_APPLICATION);
                    }
                    else
                    {
                        Logger.ELOG(Logger.ETckRunner, "Uninstall operation failed: " + result);
                        exit(ERR_TCK_RUNNER);
                    }
                    break;

                case OPERATION_EXIT:
                    exit(result);
                    break;

                default:
                    Logger.ELOG(Logger.ETckRunner, "Unknown operation: " + oper);
                    break;
                }
                break;

            case MSG_ID_APPLICATION_EXIT:
                String uid = aMsg.readString();
                int status = aMsg.readInt();
                Logger.ILOG(Logger.ETckRunner,
                            "ApplicationExitNotification: uid=" + uid + " status=" + status);

                boolean allDone = iLauncher.runNext();
                if (allDone)
                {
                    changeState(TckRunnerStateListener.STATE_UNINSTALL_APPLICATION);
                    if (iInstaller.isReady())
                    {
                        uid = iLauncher.getUidForUninstallation();
                        iInstaller.uninstall(uid);
                    }
                }
                break;

            default:
                Logger.WLOG(Logger.ETckRunner, "Unknow message received:");
                Logger.WLOG(Logger.ETckRunner, "    messageId = " + aMsg.getMessageId());
                Logger.WLOG(Logger.ETckRunner, "    moduleId  = " + aMsg.getModuleId());
                break;
            }
        }
        catch (TckRunnerException tre)
        {
            Logger.ELOG(Logger.ETckRunner, "TckRunner error", tre);
            exit(ERR_TCK_RUNNER);
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.ETckRunner, "Unexpected error", e);
            exit(ERR_GENERAL);
        }
    }

    /**
     * Changes TCK Runner state and notifies state listener.
     */
    private void changeState(int aState)
    {
        String state;
        switch (aState)
        {
        case TckRunnerStateListener.STATE_INSTALL_APPLICATION:
            state = "Install application";
            break;
        case TckRunnerStateListener.STATE_UNINSTALL_APPLICATION:
            state = "Remove application";
            break;
        case TckRunnerStateListener.STATE_RUN_APPLICATION:
            state = "Run application";
            break;
        case TckRunnerStateListener.STATE_NOT_RUNNING:
            state = "Not running";
            break;
        default:
            state = "Invalid state";
            break;
        }
        iState = aState;
        Logger.ILOG(Logger.ETckRunner, "TckRunner state change: " + state);
        if (iListener != null)
        {
            iListener.stateChanged(iState);
        }
    }

    /**
     * Increases retry counter and notifies listener about failed operation
     */
    private void retry()
    {
        try
        {
            iRetryCount++;
            if (NUMBER_OF_RETRIES < iRetryCount)
            {
                throw new TckRunnerException("Maximum number of retries reached");
            }

            long millisecondsToSleep = 2000; // 2 seconds
            Thread.sleep(millisecondsToSleep);
            if (iListener != null)
            {
                iListener.operationFailed(iState, iRetryCount);
            }
        }
        catch (InterruptedException e)
        {
        }
    }

    /**
     * Exits TCK Runner
     */
    private void exit(int aExitCode)
    {
        iExitCode = aExitCode;
        iInstaller.stop();
        Logger.ILOG(Logger.ETckRunner, "Exiting " + iExitCode);
        changeState(TckRunnerStateListener.STATE_NOT_RUNNING);

        synchronized (iSync)
        {
            iSync.notifyAll();
        }
    }

}
