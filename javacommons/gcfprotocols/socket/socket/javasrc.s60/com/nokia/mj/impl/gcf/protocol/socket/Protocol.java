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


package com.nokia.mj.impl.gcf.protocol.socket;

import java.io.IOException;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.gcf.utils.*;
import com.nokia.mj.impl.socket.SocketConnectionImpl;
import com.nokia.mj.impl.socket.ServerSocketConnectionImpl;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.gcf.utils.UrlParser;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.connectionmanager.ConnectionManager;
import com.nokia.mj.impl.connectionmanager.AccessPoint;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.utils.Uid;

/*
 * Implementation of Connector class of GCF framework The protocol class is the
 * entry point for connection types When connector.open is called, the VM finds
 * the com.nokia.mj.impl.gcf.protocol."scheme".Protocol class which implements
 * openConnection method which is called by the Vm.
 */

public class Protocol
{

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javasocket");
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
        String fullurl = "socket:" + aUri;
        ConnectionManager cminstance = null;
        AccessPoint apn = null;
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
                    throw new IllegalArgumentException("Invalid URL: "
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
                   "socket +openConnection -after removing nokia_apnid : " + urls);

        UrlParser SocURL = new UrlParser("socket", urls);
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "socket - protocol class : port = " + SocURL.port);
        if (SocURL.getPort() == 8080 || SocURL.getPort() == 443
                || SocURL.getPort() == 80)
        {
            if (ApplicationInfo.getInstance().getRuntimeType().equals(
                        ApplicationInfo.RUNTIME_MIDP) == true)
            {
                if ((ApplicationInfo.getInstance().getProtectionDomain()
                        .equals(ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN)) == true)
                {
                    Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                               "Throwing security exception for restricted port");
                    throw new SecurityException("Unable to open socket connection: port "+SocURL.getPort()+" is restricted");
                }
            }
        }
        if (SocURL.host == null)
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "socket - protocol class : server connection");
            ApplicationUtils appUtils = ApplicationUtils.getInstance();
            SocketPermissionImpl per = new SocketPermissionImpl("socket://");
            appUtils.checkPermission(per);
            /* security related stuff */
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "socket - protocol class : after security check");
            return new ServerSocketConnectionImpl(SocURL, aMode);
        }
        else
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "socket - protocol class : client connection");
            if (SocURL.port == -1)
            {
                /* client connecction should have a port number */
                throw new IllegalArgumentException("Invalid URL: " + fullurl);
            }
            ApplicationUtils appUtils = ApplicationUtils.getInstance();
            SocketPermissionImpl per = new SocketPermissionImpl("socket://*");
            appUtils.checkPermission(per);
            /* security related stuff */
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "socket - protocol class : after security check");

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
                    Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                               "+secureconnection Protocol getNetworkAccessPoint threw Exception");
                }
                Logger.LOG(Logger.ESOCKET, Logger.EInfo, "apn.getType = "
                           + apn.getType() + " apn.getNapId= " + apn.getNapId());
            }
            catch (Exception e)
            {
                Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                           "getSuiteUid exception ");
            }

            return new SocketConnectionImpl(SocURL, aMode, -1, cminstance, apn);
        }
    }
}
