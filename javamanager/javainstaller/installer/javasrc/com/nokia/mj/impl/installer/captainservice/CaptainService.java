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


package com.nokia.mj.impl.installer.captainservice;

import com.nokia.mj.impl.comms.CommsEndpoint;
import com.nokia.mj.impl.comms.CommsMessage;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.Uid;

import java.io.IOException;
import java.util.Vector;

public class CaptainService
{
    private CommsEndpoint iComms = null;
    private int iOptions = 0; // options sent in the messages

    /**
     * Default constructor.
     */
    private CaptainService()
    {
        iComms = CommsEndpoint.find("InstallerJavaCaptain");
        if (iComms == null)
        {
            Log.logError("CaptainService: No comms connection to JavaCaptain");
        }
    }

    /**
     * Constructor.
     * @param aSilent indicates if requests through this instance
     * are to be made silently, i.e. without any user interaction
     */
    public CaptainService(boolean aSilent)
    {
        this();
        if (aSilent)
        {
            iOptions |= CaptainRequest.OPT_SILENT;
        }
    }

    /**
     * Request to stop specified applications.
     * @param aUids uids for applications to be stopped
     * @throws InstallerException if operation fails
     */
    public void stopApp(Uid[] aUids) throws InstallerException
    {
        if (!isCommsAvailable("stopApp"))
        {
            return;
        }
        try
        {
            CaptainRequest req = new CaptainRequest(
                CaptainRequest.OP_STOP, iOptions, aUids);
            Log.log("Sending " + req);
            CommsMessage msg = iComms.sendReceive(req, 10); // timeout in secs
            CaptainResponse res = new CaptainResponse(msg);
            if (res.statusOk())
            {
                Log.log("Received " + res);
            }
            else
            {
                Log.logError("Stopping application failed, received " + res);
                InstallerException.internalError("Stopping application failed");
            }
        }
        catch (InstallerException iex)
        {
            throw iex;
        }
        catch (Exception ex)
        {
            Log.logError(
                "Sending stop app request to JavaCaptain failed", ex);
            InstallerException.internalError(
                "Sending stop app request to JavaCaptain failed", ex);
        }
    }

    /**
     * Request to start specified applications.
     * @param aUids uids for applications to be started
     */
    public void startApp(Uid[] aUids)
    {
        if (!isCommsAvailable("startApp"))
        {
            return;
        }
        try
        {
            CaptainRequest req = new CaptainRequest(
                CaptainRequest.OP_START, iOptions, aUids);
            Log.log("Sending " + req);
            iComms.send(req);
        }
        catch (Exception ex)
        {
            Log.logError(
                "Sending start app request to JavaCaptain failed", ex);
        }
    }

    /**
     * Notifies JavaCaptain that specified applications have been
     * added, upgraded, or removed.
     * @param aBeforeUids uids valid before the operation
     * @param aAfterUids uids valid after the operation
     */
    public void appUpdated(Uid[] aBeforeUids, Uid[] aAfterUids)
    {
        if (!isCommsAvailable("appUpdated"))
        {
            return;
        }
        try
        {
            CaptainRequest req = new CaptainRequest(
                CaptainRequest.OP_UPDATE, iOptions, aBeforeUids, aAfterUids);
            Log.log("Sending " + req);
            iComms.send(req);
        }
        catch (Exception ex)
        {
            Log.logError(
                "Sending update app notification to JavaCaptain failed", ex);
        }
    }

    /**
     * Request to start specified application using RLC message.
     * @param aUids uid for application to be started
     */
    public void launchApp(Uid aUid)
    {
        if (!isCommsAvailable("launchApp"))
        {
            return;
        }
        try
        {
            CommsMessage req = new CommsMessage();
            req.setMessageId(101); // RTC_MSG_ID_LAUNCH_APPLICATION_REQ
            req.setModuleId(1); // PLUGIN_ID_RTC_C

            // Must be kept in sync with rtcmessages.h native encoder/decoder!
            req.write(aUid.getStringValue());
            req.write(0);       // RTC_LAUNCH_TYPE_NORMAL_C
            req.write(0);       // RTC_LAUNCH_OPTIONS_NONE_C
            req.write("midp");  // Runtime type
            req.write("");      // application parameters
            req.write("");      // runtime parameters
            iComms.send(req);
        }
        catch (Exception ex)
        {
            Log.logError(
                "Sending launch app request to JavaCaptain failed", ex);
        }
    }

    /**
     * Returns true if comms connection is availble, false otherwise.
     */
    private boolean isCommsAvailable(String aMsg)
    {
        if (iComms == null)
        {
            Log.logError(aMsg + ": No comms connection to JavaCaptain");
            return false;
        }
        return true;
    }
}
