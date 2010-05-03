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

package com.nokia.mj.impl.gcf.protocol.http;

import java.io.IOException;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.gcf.utils.ProtocolBase;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.http.HttpClientConnection;

public class Protocol implements ProtocolBase
{

    static
    {
        try
        {
            Logger.ILOG(Logger.ESOCKET,
                        "Loading HTTP protocol implementation");
            Jvm.loadSystemLibrary("javasocket");
            Logger.ILOG(Logger.ESOCKET,
                        "HTTP protocol implementation loaded");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.ESOCKET, e.toString());
        }
    }

    public Protocol()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+Protocol");
    }

    public javax.microedition.io.Connection openConnection(String aUri,
            int aMode, boolean aTimeouts) throws IOException
    {

        String apnstring = null;
        String urls = null;
        int apnid = -1;
        int index = -1;
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "http +openConnection - aURi = " + aUri);

        /* security related stuff */
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        HttpPermissionImpl per = new HttpPermissionImpl("http://*");
        appUtils.checkPermission(per);

        /* security related stuff */
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
                    throw new IllegalArgumentException("url is invalid");
                }
            }
            else
            {
                throw new IllegalArgumentException("url is invalid");
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
                        throw new IllegalArgumentException("url is invalid");
                    }
                }
                else
                {
                    throw new IllegalArgumentException("url is invalid");
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
        HttpClientConnection httpclient = new HttpClientConnection(urls);
        return httpclient;

    }

    public javax.microedition.io.Connection openConn(String name, int mode,
            boolean timeouts) throws IOException
    {
        return openConnection(name, mode, timeouts);
    }

}
