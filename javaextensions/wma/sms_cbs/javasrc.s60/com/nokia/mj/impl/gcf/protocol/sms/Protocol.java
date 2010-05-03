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


package com.nokia.mj.impl.gcf.protocol.sms;

import java.io.IOException;

import javax.microedition.io.Connector;
import javax.wireless.messaging.MessageConnection;

import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.smscbs.utils.WmaUrl;
import com.nokia.mj.impl.sms.SMSConnectionImpl;
import com.nokia.mj.impl.sms.SMSPermissionImpl;
import com.nokia.mj.impl.rt.support.ApplicationUtils;

/**
 * This class is part of gcf framework which creates the SmsConnection
 * object based on the Uri.It creates Client or Server connection based
 * on Uri passed and returns the connection object to the application.
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
            Logger.LOG(Logger.EWMA, Logger.EInfo, e.toString());
        }
    }

    public Protocol()
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo, "Sms Protocol");
    }


    /**
     * This function is called by openPrim of Protocol class and
     * createConnection method of Connection class.This method based on uri
     * creates either server or client connection of SmsConnection object.
     *
     * @param uri
     *            URL passed to Connector.open to create a Sms connection
     * @param mode
     *            the mode in which the connection has to be opened
     * @param timeouts
     *            third parameter in Connector.open - ignored
     * @return an object of SmsConnection type
     * @throws IOException
     *             in case creation of connection fails.
     * @see
     */

    public javax.microedition.io.Connection openConnection(String aUri,
            int aMode, boolean aTimeouts) throws IOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo, "opening Sms Connection");
        SMSConnectionImpl msgConnection = null;
        WmaUrl smsuri = new WmaUrl(aUri, "sms:");
        smsuri.validateUri();

        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        SMSPermissionImpl permission = new SMSPermissionImpl("sms://*", "open");

        if (smsuri.isServerModeConnection())
        {
            appUtils.checkPermission(permission);
            Logger.LOG(Logger.EWMA, Logger.EInfo,
                       " creating Sms server connection");
            msgConnection = new SMSConnectionImpl(smsuri,
                                                  smsuri.SERVER_CONNECTION);
        }
        else
        {
            if (aMode == Connector.READ)
            {
                throw new IllegalArgumentException("Invalid Open Mode "
                                                   + "for SmsConnection");
            }
            appUtils.checkPermission(permission);
            Logger.LOG(Logger.EWMA, Logger.EInfo,
                       "creating Sms client connection");
            msgConnection = new SMSConnectionImpl(smsuri,
                                                  smsuri.CLIENT_CONNECTION);
        }
        msgConnection.open();
        return msgConnection;
    }

}