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


package com.nokia.mj.impl.gcf.protocol.ssl;

import java.io.IOException;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.gcf.utils.UrlParser;
import com.nokia.mj.impl.ssl.SecureConnectionImpl;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.connectionmanager.ConnectionManager;
import com.nokia.mj.impl.connectionmanager.AccessPoint;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.utils.Uid;

public class Protocol
{

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javassl");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.ESOCKET, e.toString());
        }
    }

    public Protocol()
    {
    }

    public javax.microedition.io.Connection openConnection(String aUri,
            int aMode, boolean aTimeouts) throws IOException
    {
        String apnstring = null;
        String urls = null;
        int apnid = -1;
        int index = -1;
        ConnectionManager cminstance = null;
        AccessPoint apn = null;
        String fullurl = "ssl:" + aUri;

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
                    throw new IllegalArgumentException("Invalid URL:  "
                                                       + fullurl);
                }
            }
            else
            {
                throw new IllegalArgumentException("Invalid URL: " + fullurl);
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
                        throw new IllegalArgumentException("Invalid URL: "
                                                           + fullurl);
                    }
                }
                else
                {
                    throw new IllegalArgumentException("Invalid URL: "
                                                       + fullurl);
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

        /* check for restricted ports */
        UrlParser sslUrl = new UrlParser("ssl", urls);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "secureconnection - protocol class : port = " + sslUrl.port);
        if (sslUrl.getPort() == 443)
        {
            if (ApplicationInfo.getInstance().getRuntimeType().equals(
                        ApplicationInfo.RUNTIME_MIDP) == true)
            {
                if ((ApplicationInfo.getInstance().getProtectionDomain()
                        .equals(ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN)) == true)
                {
                    Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                               "Throwing security exception for restricted port");
                    throw new SecurityException("Unable to open secure connection: port "+sslUrl.getPort()+" is restricted");
                }
            }
        }

        /* security related stuff */
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        SSLPermissionImpl per = new SSLPermissionImpl("ssl://*");
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "calling check permissions from protocol");
        appUtils.checkPermission(per);
        /* security related stuff */

        // remove comments if open c patch available
        cminstance = ConnectionManager.getInstance();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "after getting ConnectionManager instance");
        // get application suite uid
        try
        {
            Uid appSuite = ApplicationInfo.getInstance().getSuiteUid();
            try
            {
                // remove comments if open c patch available
                apn = cminstance.getNetworkAccessPoint(appSuite, aUri);
            }
            catch (Exception e)
            {
                Logger
                .LOG(Logger.ESOCKET, Logger.EInfo,
                     "+secureconnection Protocol getNetworkAccessPoint threw Exception");
            }
            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "apn.getType = "
                       + apn.getType() + " apn.getNapId= " + apn.getNapId());
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "getSuiteUid exception ");
        }

        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "secure connection protocol.java");

        if (sslUrl.port == -1)
        {
            throw new IllegalArgumentException("Invalid URL: " + fullurl);
        }
        return new SecureConnectionImpl(sslUrl, aMode, -1, cminstance, apn);

    }

}
