/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.gcf.utils.ProtocolBase;
import com.nokia.mj.impl.https.HttpsConnectionImpl;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.utils.Logger;

public class Protocol implements ProtocolBase
{

    static
    {
        try
        {
            Logger.ILOG(Logger.ESOCKET, "Before Loading https native");
            Jvm.loadSystemLibrary("javahttps");
            Logger.ILOG(Logger.ESOCKET, "Loaded https native");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.ESOCKET, e.toString());
        }
        try
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,"Before Loading ssl  native");
            Jvm.loadSystemLibrary("javassl");
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,"Loaded ssl  native");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.ESOCKET, e.toString());
        }
        try
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,"Before Loading http  native");
            Jvm.loadSystemLibrary("javahttp");
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,"Loaded http  native");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.ESOCKET, e.toString());
        }
    }

    public Protocol()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,"+Protocol");
    }

    public javax.microedition.io.Connection openConnection(String aUri,
            int aMode, boolean aTimeouts) throws IOException
    {

        /* security related stuff */
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        HttpsPermissionImpl per = new HttpsPermissionImpl("https://*");
        appUtils.checkPermission(per);

        /* security related stuff */
        return new HttpsConnectionImpl(aUri, aMode);
    }

    public javax.microedition.io.Connection openConn(String name, int mode,
            boolean timeouts) throws IOException
    {
        return openConnection(name, mode, timeouts);
    }

}
