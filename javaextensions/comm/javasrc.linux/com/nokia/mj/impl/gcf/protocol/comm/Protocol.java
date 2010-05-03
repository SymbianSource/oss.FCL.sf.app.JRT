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


package com.nokia.mj.impl.gcf.protocol.comm;

import java.io.IOException;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.comm.CommConnectionImpl;
import com.nokia.mj.impl.gcf.utils.ProtocolBase;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.utils.Logger;

public class Protocol implements ProtocolBase
{

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javacomm");
        }
        catch (Exception e)
        {

        }
    }

    public Protocol()
    {

    }

    public javax.microedition.io.Connection openConnection(String aUri,
            int aMode, boolean aTimeouts) throws IOException
    {
        /* security related stuff */

        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        CommPermissionImpl per = new CommPermissionImpl("comm:*");
        Logger.LOG(Logger.ESOCKET, Logger.EInfo,"calling check permissions from protocol");
        appUtils.checkPermission(per);
        /* security related stuff */

        return new CommConnectionImpl(aUri, aMode, aTimeouts);
    }

    public javax.microedition.io.Connection openConn(String name, int mode,
            boolean timeouts) throws IOException
    {
        return openConnection(name, mode, timeouts);
    }

}
