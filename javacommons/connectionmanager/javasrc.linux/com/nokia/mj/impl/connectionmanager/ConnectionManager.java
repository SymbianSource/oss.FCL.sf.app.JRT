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


package com.nokia.mj.impl.connectionmanager;

import com.nokia.mj.impl.connectionmanager.*;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.*;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.storage.*;



/*
JavaImplementation of ConnectionManager class
*/

public class ConnectionManager
{

    // Constants for URL paramater names
    private static String NOKIANETID = ";nokia_netid=";
    private static String NOKIAAPNID = ";nokia_apnid=";

    // Constants for application setting types
    private static char NONE = 'N'; // Supported by app mgr
    private static char ASKALWAYS = 'A'; // this is needed since device default supports it
    private static char DEFAULTCONN = 'F'; // Supported by app mgr
    private static char ASKONCE = 'O'; // supported by App mgr
    private static char SNAPID = 'D';
    private static char IAPID = 'I';

    private boolean iIsReseted = false;
    private int iNativeHandle;
    private static ConnectionManager iCmInstance;
    public AccessPoint iSessionNetworkAccessPoint;

    static
    {
        try
        {
            if (Logger.Activated[Logger.ESOCKET])
            {
                Logger.ILOG(Logger.ESOCKET,"Loading connection manager...");
            }
            Jvm.loadSystemLibrary("javaconnectionmanager");
            if (Logger.Activated[Logger.ESOCKET])
            {
                Logger.ILOG(Logger.ESOCKET, "Loading connection manager ok");
            }
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.ESOCKET, e.toString());
        }
    }



    public ConnectionManager()
    {
    }

    public synchronized static ConnectionManager getInstance()
    {
        return null;
    }

    public AccessPoint getApplicationDefault(Uid aAppSuiteUid)
    {
        return null;
    }


    public synchronized AccessPoint getNetworkAccessPoint(Uid aAppSuiteUid, String aUri) throws Exception
    {

        String apn = _askNetworkAccessPoint();
        apn = _getDeviceDefaultAccessPoint();
        return null;
    }




    public synchronized void reset()
    {
        //iSessionNetworkAccessPoint = new AccessPoint();
        //iIsReseted = true;
    }
    private native String _getDeviceDefaultAccessPoint();
    private native String _askNetworkAccessPoint();

}




