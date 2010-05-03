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


package com.nokia.mj.impl.gcf.protocol.cbs;

import java.io.IOException;

import javax.microedition.io.Connector;
import javax.wireless.messaging.MessageConnection;

import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.smscbs.utils.WmaUrl;
import com.nokia.mj.impl.cbs.CBSConnectionImpl;
import com.nokia.mj.impl.cbs.CBSPermissionImpl;
import com.nokia.mj.impl.rt.support.ApplicationUtils;

/**
 * This class is part of gcf framework which creates the CbsConnection
 * object and returns it.
 */
public class Protocol
{
    static
    {
        try
        {
            Jvm.loadSystemLibrary("javawma");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EWMA, e.toString());
        }
    }

    public Protocol()
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo, "+Protocol");
    }

    /**
     * * This function is called by openPrim of Protocol class and
     * createConnection method of Connection class.
     * @param uri URL passed to Connector.open to create a Cbsconnection
     * @param mode the mode in which the connection has to be opened
     * @param timeouts third parameter in Connector.open - ignored
     * @return an object of Cbsconnection type
     * @throws IOException in case creation of connection fails.
     * @see
     */

    public javax.microedition.io.Connection openConnection(String aUri,
            int aMode, boolean aTimeouts) throws IOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo, "opening CBS Connection");

        CBSConnectionImpl msgConnection = null;
        WmaUrl cbsuri = new WmaUrl(aUri, "cbs:");

        cbsuri.validateUri();

        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        CBSPermissionImpl permission = new CBSPermissionImpl("cbs://*", "open");

        if (cbsuri.isServerModeConnection())
        {
            if (aMode == Connector.WRITE)
            {
                throw new IllegalArgumentException(
                    " Invalid mode CBS Connector.write");
            }
            appUtils.checkPermission(permission);
            Logger.LOG(Logger.EWMA, Logger.EInfo,
                       "creating CBS connection");
            msgConnection = new CBSConnectionImpl(cbsuri, cbsuri.SERVER_CONNECTION);
            msgConnection.open();
        }
        return msgConnection;
    }
}