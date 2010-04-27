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


package com.nokia.mj.impl.gcf.protocol.sensor;

import java.io.IOException;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Jvm;

import com.nokia.mj.impl.sensor.SensorConnectionImpl;

public class Protocol
{
    static
    {
        try
        {
            Jvm.loadSystemLibrary("javasensor");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaFile, "Unable to load javasensor dll");
        }
    }

    public Protocol()
    {
    }


    public javax.microedition.io.Connection openConnection(String aUri,
            int aMode, boolean aTimeouts) throws IOException
    {
        String sensor = "sensor:"+aUri;
        return new SensorConnectionImpl(sensor);
    }
}

