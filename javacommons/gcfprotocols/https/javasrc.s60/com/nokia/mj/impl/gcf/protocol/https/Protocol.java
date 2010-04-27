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

package com.nokia.mj.impl.gcf.protocol.https;

import java.io.IOException;

import javax.microedition.io.Connection;

import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.gcf.utils.URI;
import com.nokia.mj.impl.https.HttpsConnectionNative;
import com.nokia.mj.impl.connectionmanager.ConnectionManager;
import com.nokia.mj.impl.connectionmanager.AccessPoint;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.gcf.protocol.https.HttpsPermissionImpl;
import com.nokia.mj.impl.utils.Uid;

public class Protocol
{
    public Protocol()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ https  Protocol(): ");
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- https  Protocol()");
    }

    public javax.microedition.io.Connection openConnection(String aUri,
            int aMode, boolean aTimeouts) throws IOException
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+https openconnection");
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ https  openConnection "
                   + aUri);
        String apnstring = null;
        String urls = null;
        String fullurl = "https:" + aUri;
        int apnid = -1;
        int index = -1;
        ConnectionManager cminstance = null;
        AccessPoint apn = null;
        int responsetimeout = -1;
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "https +openConnection - aURi = " + aUri);

        // security related stuff
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        HttpsPermissionImpl per = new HttpsPermissionImpl("https://*");
        appUtils.checkPermission(per);
        // security related stuff

        // Setting of response timeout using ";nokia_timeout=" parameter at the end of the url (even after IAP/SNAP id)
        // specific scenario identified only for java intaller
        // format : http://www.google.co.in/index.html;nokia_apnid=11;nokia_timeout=10
        if (ApplicationInfo.getInstance().getRuntimeType() == ApplicationInfo.RUNTIME_INSTALLER)
        {
            index = aUri.indexOf(";nokia_timeout=");
            if (index != -1)
            {
                String timeoutString = aUri.substring(index);
                int equalindex = timeoutString.indexOf('=');
                String temp = timeoutString.substring(equalindex + 1);
                try
                {
                    responsetimeout = Integer.parseInt(temp);
                }
                catch (NumberFormatException e)
                {
                    throw new IllegalArgumentException("Invalid url: "
                                                       + fullurl);
                }
                urls = aUri.substring(0, index);
                aUri = urls;
            }
        }

        index = aUri.indexOf(";nokia_netid=");
        if (index != -1)
        {
            apnstring = aUri.substring(index);
            int equalindex = apnstring.indexOf('=');
            if (equalindex != -1)
            {
                String temp = apnstring.substring(equalindex + 1);
                try
                {
                    apnid = Integer.parseInt(temp);
                }
                catch (NumberFormatException e)
                {
                    throw new IllegalArgumentException("Invalid url: "
                                                       + fullurl);
                }
            }
            else
            {
                throw new IllegalArgumentException("Invalid url: " + fullurl);
            }
            urls = aUri.substring(0, index);
        }
        else
        {
            index = aUri.indexOf(";nokia_apnid=");
            if (index != -1)
            {
                apnstring = aUri.substring(index);
                int equalindex = apnstring.indexOf('=');
                if (equalindex != -1)
                {
                    String temp = apnstring.substring(equalindex + 1);
                    try
                    {
                        apnid = Integer.parseInt(temp);
                    }
                    catch (NumberFormatException e)
                    {
                        throw new IllegalArgumentException("Invalid url: "
                                                           + fullurl);
                    }
                }
                else
                {
                    throw new IllegalArgumentException("Invalid url: "
                                                       + fullurl);
                }
                urls = aUri.substring(0, index);
            }
            else
            {
                urls = aUri;
            }
        }
        cminstance = ConnectionManager.getInstance();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "33");
        try
        {
            // get application suite uid
            Uid appSuite = ApplicationInfo.getInstance().getSuiteUid();
            try
            {
                apn = cminstance.getNetworkAccessPoint(appSuite, aUri);
                Logger.LOG(Logger.ESOCKET, Logger.EInfo, "apn.getType = "
                           + apn.getType() + " apn.getNapId= " + apn.getNapId());
            }
            catch (Exception e)
            {
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                           "+http Protocol getNetworkAccessPoint threw Exception");
            }
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "getSuiteUid exception ");
        }

        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "appending https to uri");
        String uri = "https:" + urls;
        return new HttpsConnectionNative(0, uri, aMode, aTimeouts, cminstance,
                                         apn, responsetimeout);
    }

}
