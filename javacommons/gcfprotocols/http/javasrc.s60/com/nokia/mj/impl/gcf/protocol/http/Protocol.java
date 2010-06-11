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

package com.nokia.mj.impl.gcf.protocol.http;

import java.io.IOException;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.http.HttpConnectionNative;
import com.nokia.mj.impl.gcf.utils.URI;
import com.nokia.mj.impl.connectionmanager.ConnectionManager;
import com.nokia.mj.impl.connectionmanager.AccessPoint;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.utils.Uid;

public class Protocol
{
    int iNativeHttpSessionHandle = 0;
    private String iFullUrl = null;

    public Protocol()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+Http::Protocol");
    }

    public javax.microedition.io.Connection openConnection(String aUri,
            int aMode, boolean aTimeouts) throws IOException
    {
        String apnstring = null;
        String urls = null;
        int apnid = -1;
        int index = -1;
        int responsetimeout = 180;  // default timeout of 3 minutes
        ConnectionManager cminstance = null;
        AccessPoint apn = null;
        iFullUrl = "http:" + aUri;
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "http +openConnection - aURi = " + iFullUrl);

        // security related stuff
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        HttpPermissionImpl per = new HttpPermissionImpl("http://*");
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
                                                       + iFullUrl);
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
                                                       + iFullUrl);
                }
            }
            else
            {
                throw new IllegalArgumentException("Invalid url: " + iFullUrl);
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
                                                           + iFullUrl);
                    }
                }
                else
                {
                    throw new IllegalArgumentException("Invalid url: "
                                                       + iFullUrl);
                }
                urls = aUri.substring(0, index);
            }
            else
            {
                urls = aUri;
            }
        }


        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "http +openConnection -after removing nokia_apnid : " + urls);
        // final int handle = _createHttpSession(0, 0, 0);
        // Logger.LOG(Logger.ESOCKET, Logger.EInfo,"Http Protocol - after
        // createSession");

        cminstance = ConnectionManager.getInstance();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "33");
        // get application suite uid
        try
        {
            Uid appSuite = ApplicationInfo.getInstance().getSuiteUid();
            try
            {
                apn = cminstance.getNetworkAccessPoint(appSuite, aUri);
            }
            catch (Exception e)
            {
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                           "+http Protocol getNetworkAccessPoint threw Exception");
            }
            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "apn.getType = "
                       + apn.getType() + " apn.getNapId= " + apn.getNapId());
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "getSuiteUid exception ");
        }

        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "appending http to uri");
        String uri = "http:" + urls;
        return new HttpConnectionNative(0, uri, aMode, aTimeouts, cminstance,
                                        apn, responsetimeout);
    }

    public void temp(int a)
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "temp()");
    }

    private void ensureValidURI(URI aUri) throws IOException
    {

        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+ensureValidURI " + aUri);

        // host analyse:
        String host = aUri.getHost();

        if (host == null || host.length() == 0)
        {
            // Invalid host
            throw new IllegalArgumentException("Invalid url: " + iFullUrl);
        }

        if (host.indexOf(' ') > -1)
        {
            // Invalid host
            throw new IllegalArgumentException("Invalid url: " + iFullUrl);
        }

        // port analyse
        String port = aUri.getPort();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "*ensureValidURI " + port);
        // getPort() returns empty string ("") if URI doesn't contain port

        if (port != null && port.length() != 0)
        {
            // are there digits only?
            // (if there is invalid char then Long.parseLong() method gives
            // "FormatNumberException" automatically)
            for (int i = 0; i < port.length(); i++)
            {
                if (!Character.isDigit(port.charAt(i)))
                {
                    throw new IllegalArgumentException("Invalid url: "
                                                       + iFullUrl);
                }
            }

            long portL = Long.parseLong(port);
            // TCP port is 16-bit number
            if (portL > 65535)
            {
                throw new IllegalArgumentException("Invalid url: " + iFullUrl);
            }
        }
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "- ensureValidURI");
    }
}
