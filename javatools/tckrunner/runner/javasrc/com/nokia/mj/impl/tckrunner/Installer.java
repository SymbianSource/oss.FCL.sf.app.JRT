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

import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.comms.CommsServerEndpoint;
import com.nokia.mj.impl.comms.CommsMessage;
import com.nokia.mj.impl.comms.CommsListener;
import com.nokia.mj.impl.comms.exception.CommsException;

import com.nokia.mj.impl.tckrunner.TckRunner;
import com.nokia.mj.impl.tckrunner.TckRunnerException;

/**
 * This call takes care of downloading, installing and uninstalling applications with
 * the help of Java Installer.
 */
public class Installer
{
    private CommsServerEndpoint iComms;
    private CommsMessage iRequest = null;

    static
    {
        try
        {
            Jvm.loadSystemLibrary("tckrunner");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaComms, "Unable to load tckrunner library");
        }
    }

    /**
     * Constructor. Starts Comms server to which Java Installer connects
     */
    public Installer(CommsListener aListener)
    {
        try
        {
            iComms = new CommsServerEndpoint();
            iComms.registerDefaultListener(aListener);
            iComms.start(TckRunner.TCK_RUNNER_ADDRESS);
        }
        catch (Exception e)
        {
            throw new TckRunnerException("Installer contructor failed", e);
        }
    }

    /**
     *  Closes Comms server
     */
    public void close()
    {
        try
        {
            iComms.unregisterDefaultListener();
            iComms.stop();
            iComms.destroy();
        }
        catch (Exception e)
        {
            Logger.WLOG(Logger.ETckRunner, "Installer close failed", e);
        }
    }

    /**
     *  Launches Java Installer process
     */
    public void start(String aArgs[])
    {
        int rc = _launchJavaInstaller(aArgs);
        if (rc != 0)
        {
            throw new TckRunnerException("Starting Java Installer process failed");
        }
    }

    /**
     *  Stops Java Installer process
     */
    public void stop()
    {
        try
        {
            if (isReady())
            {
                CommsMessage msg = new CommsMessage();
                msg.replyTo(iRequest);
                msg.setMessageId(TckRunner.MSG_ID_OPERATION);
                msg.write(TckRunner.OPERATION_EXIT);

                Logger.ILOG(Logger.ETckRunner, "Stopping Java Installer");
                iComms.send(msg);
                iRequest = null;
            }
        }
        catch (CommsException ce)
        {
            Logger.WLOG(Logger.ETckRunner,
                        "Sending exit request to Java Installer failed", ce);
        }
    }

    /**
     *  Stores Java Installer's get next operation request.
     */
    public void saveRequest(CommsMessage aMsg)
    {
        iRequest = aMsg;
    }

    /**
     *  Is Java Installer ready to receive next operation
     */
    public boolean isReady()
    {
        return iRequest != null;
    }

    /**
     *  Tell Java Installer to download and install application from given URL
     */
    public void install(String aUrl)
    {
        try
        {
            CommsMessage msg = new CommsMessage();
            msg.replyTo(iRequest);
            msg.setMessageId(TckRunner.MSG_ID_OPERATION);

            msg.write(TckRunner.OPERATION_INSTALL);
            msg.write(aUrl);

            Logger.ILOG(Logger.ETckRunner, "Install Operation: url=" + aUrl);
            iComms.send(msg);
            iRequest = null;
        }
        catch (CommsException ce)
        {
            throw new TckRunnerException("Message sending to Java Installer failed", ce);
        }
    }

    /**
     *  Tell Java Installer to remove given application
     */
    public void uninstall(String aUid)
    {
        try
        {
            CommsMessage msg = new CommsMessage();
            msg.replyTo(iRequest);
            msg.setMessageId(TckRunner.MSG_ID_OPERATION);

            msg.write(TckRunner.OPERATION_UNINSTALL);
            msg.write(aUid);

            Logger.ILOG(Logger.ETckRunner, "Uninstall Operation: uid=" + aUid);
            iComms.send(msg);
            iRequest = null;
        }
        catch (CommsException ce)
        {
            throw new TckRunnerException("Message sending to Java Installer failed", ce);
        }
    }

    private native int _launchJavaInstaller(String[] aArgs);
}
