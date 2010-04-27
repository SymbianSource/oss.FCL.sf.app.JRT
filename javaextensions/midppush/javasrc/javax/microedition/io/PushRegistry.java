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


package javax.microedition.io;

import java.lang.ClassNotFoundException;
import javax.microedition.io.ConnectionNotFoundException;
import java.io.IOException;

import com.nokia.mj.impl.push.PushRegistryImpl;

public class PushRegistry
{
    private PushRegistry() {}

    public static String getFilter(String aURI)
    {
        return PushRegistryImpl.getFilter(aURI);
    }

    public static String getMIDlet(String aURI)
    {
        return PushRegistryImpl.getMIDlet(aURI);
    }

    public static String[] listConnections(boolean aAvailable)
    {
        return PushRegistryImpl.listConnections(aAvailable);
    }

    public static long registerAlarm(String aMIDletClassName, long aTime)
    throws ClassNotFoundException,ConnectionNotFoundException
    {
        return PushRegistryImpl.registerAlarm(aMIDletClassName,aTime);
    }

    public static void registerConnection(String aConnection, String aMIDletClassName, String aFilter)
    throws ClassNotFoundException,java.io.IOException
    {
        PushRegistryImpl.registerConnection(aConnection,aMIDletClassName,aFilter);
    }

    public static boolean unregisterConnection(String aConnection)
    {
        return PushRegistryImpl.unregisterConnection(aConnection);
    }
}

