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
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.comms.CommsEndpoint;
import com.nokia.mj.impl.comms.CommsMessage;
import com.nokia.mj.impl.comms.CommsListener;
import com.nokia.mj.impl.comms.exception.CommsException;

import com.nokia.mj.impl.tckrunner.TckRunner;
import com.nokia.mj.impl.tckrunner.TckRunnerException;

/**
 * This class takes care of launching applications.
 * Applications are launched via Java Captain
 */
public class Launcher
{
    private static final int PLUGIN_ID_RTC = 1;
    private static final int RTC_MSG_ID_LAUNCH_APPLICATION_REQ      = 101;
    private static final int RTC_LAUNCH_TYPE_NORMAL_C               = 0;
    private static final int RTC_LAUNCH_OPTIONS_TERMINATE_IND_REQ_C = 2;

    private CommsEndpoint iComms = null;

    private int iUidIndex = 0;
    private String[] iUids = null;

    /**
     * Constructor. Creates Comms connection to Java Captain
     */
    public Launcher(CommsListener aListener)
    {
        try
        {
            iComms = new CommsEndpoint();
            iComms.registerDefaultListener(aListener);
            iComms.connect(CommsEndpoint.JAVA_CAPTAIN);
        }
        catch (Exception e)
        {
            throw new TckRunnerException(
                "Launcher constructor failed. Unable to connect to JavaCaptain", e);
        }
    }

    /**
     *  Closes Comms connection
     */
    public void close()
    {
        try
        {
            iComms.unregisterDefaultListener();
            iComms.disconnect();
            iComms.destroy();
        }
        catch (Exception e)
        {
            Logger.WLOG(Logger.ETckRunner, "Launcher close failed", e);
        }
    }

    /**
     *  Launches next application
     */
    public boolean runNext()
    {
        if (iUidIndex >= iUids.length)
        {
            return true; // we are done
        }

        try
        {
            CommsMessage msg = new CommsMessage();
            msg.setMessageId(RTC_MSG_ID_LAUNCH_APPLICATION_REQ);
            msg.setModuleId(PLUGIN_ID_RTC);

            msg.write(iUids[iUidIndex]);
            msg.write(RTC_LAUNCH_TYPE_NORMAL_C);
            msg.write(RTC_LAUNCH_OPTIONS_TERMINATE_IND_REQ_C);
            msg.write("midp");
            msg.write(""); // application parameters
            msg.write(""); // runtime parameters

            Logger.ILOG(Logger.ETckRunner,
                        "LaunchApplicationReq: uid=" + iUids[iUidIndex]);
            iComms.send(msg);
            iUidIndex++;

            return false;
        }
        catch (CommsException ce)
        {
            throw new TckRunnerException("Message sending to Java Captain failed", ce);
        }
    }

    /**
     *  Set a list of applications, which need to be launched
     */
    public void setApplications(String[] aUids)
    {
        iUids = aUids;
        iUidIndex = 0;
    }

    /**
     *  Return a UID that can be used to remove ran applications
     */
    public String getUidForUninstallation()
    {
        return iUids[0];
    }

}
