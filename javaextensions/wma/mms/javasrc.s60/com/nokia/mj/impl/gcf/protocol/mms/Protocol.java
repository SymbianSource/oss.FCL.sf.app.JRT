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


package com.nokia.mj.impl.gcf.protocol.mms;

import java.io.IOException;

import javax.microedition.io.Connector;

import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.mms.MMSAddress;
import com.nokia.mj.impl.mms.MMSConnectionImpl;
import com.nokia.mj.impl.mms.MMSPermissionImpl;
import com.nokia.mj.impl.rt.support.ApplicationUtils;

/**
 * This class is part of gcf framework which creates the MmsConnection
 * object and returns it.
 */
public class Protocol
{
    static
    {
        try
        {
            Jvm.loadSystemLibrary("javawmamms");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EWMA, "Loading WMA protocol failed", e);
        }
    }

    private final String MMS_PROTOCOL = "mms:";
    /**
     * This function is called by the gcf to create MmssConnection object
     * @param name the type of connection to be opened
     * @param mode the mode in which the connection has to be opened
     * @param timeouts third parameter in Connector.open - ignored
     * @return an object of Mmsconnection type
     * @throws IOException in case creation of connection fails.
     * @see
     */
    public Protocol()
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo, "Mms Protocol");
    }


    /**
    * Creates a MmsConnection object and returs it.
    * @param aUri URL passed to Connector.open to create a mms connection
    * @param aMode Mode in which connection can be opened.
    * @param aTimeouts third parameter in Connector.open - ignored
    * @return an object of MmsConnection type
    * @throws IOException in case creation of connection fails.
    */
    public javax.microedition.io.Connection openConnection(String aUri,
            int aMode, boolean aTimeouts) throws IOException
    {

        Logger.LOG(Logger.EWMA, Logger.EInfo, "opening MMS Connection");
        String uri = MMS_PROTOCOL + aUri;

        MMSConnectionImpl conn = null;

        MMSAddress.validateUrl(uri);

        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        MMSPermissionImpl permission = new MMSPermissionImpl("mms://*","open");

        if (MMSAddress.getConnectionMode(aUri) == MMSAddress.SERVER)
        {
            appUtils.checkPermission(permission);
            Logger.LOG(Logger.EWMA, Logger.EInfo,
                       "Creating MMS Server Connection");
            conn = new MMSConnectionImpl(uri, MMSAddress.SERVER);
        }
        else
        {
            if (aMode == Connector.READ)
            {
                throw new IllegalArgumentException(
                    "MMS Client connection not allowed in READ Mode");
            }
            appUtils.checkPermission(permission);
            Logger.LOG(Logger.EWMA, Logger.EInfo,
                       "Creating MMS Client Connection");
            conn = new MMSConnectionImpl(uri, MMSAddress.CLIENT);
        }
        conn.open();
        return conn;
    }
}
