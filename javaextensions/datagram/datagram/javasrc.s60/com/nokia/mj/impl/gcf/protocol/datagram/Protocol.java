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


package com.nokia.mj.impl.gcf.protocol.datagram;

import java.io.IOException;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.datagram.UDPDatagramConnectionImpl;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.utils.Logger;


public class Protocol
{

    static
    {
        try
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "Before Loading datagram native");
            Jvm.loadSystemLibrary("javadatagram");
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "Loaded datagram native");
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo, e.toString());
        }
    }

    public Protocol()
    {
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "+Protocol");
    }

    public javax.microedition.io.Connection openConnection(String aUri,
            int aMode, boolean aTimeouts) throws IOException
    {

        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "+openConnection-udpdatagramconnection - aUri = "+aUri);

        return new UDPDatagramConnectionImpl(aUri, aMode, aTimeouts);
    }
}
