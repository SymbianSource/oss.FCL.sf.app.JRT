/*
* Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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



package com.nokia.mj.impl.rt.legacy;

public class MIDEventServer
{
    private static final int MAX_USER_NAME=64;

    private final int iHandle;
    /**
     *  Construct a named EPOC event server system.
     *  <p>
     *  The server and event threads manage EPOC event notification
     *  and propogation of those events back to Java.
     *
     *  The native server handle should be passed through to
     *  native event sources to bind to this event server.
     *  @see getHandle
     *  @param aName The event server system to use
     */
    public MIDEventServer(String aName)
    {
        super();
        iHandle = openEventServer(aName, new DefaultFactory());
    }

    protected MIDEventServer(String aName, MIDEventServerFactory aFactory)
    {
        super();
        iHandle = openEventServer(aName, aFactory);
    }

    private int openEventServer(String aName, MIDEventServerFactory aFactory)
    {
        //
        if (aName.length()>MAX_USER_NAME)
        {
            throw new IllegalArgumentException();
        }
        int server = aFactory.createServer(aName);
        NativeError.checkOOM(server);
        return server;
    }
    /**
     *  Get the server handle for passing through to native
     *  event sources.
     * @returns The handle of the native event server
     */
    public int getHandle()
    {
        return iHandle;
    }

    public void shutdown()
    {
        _shutdown(iHandle);
    }

    private class DefaultFactory implements MIDEventServerFactory
    {
        DefaultFactory()
        {
        }
        public int createServer(String aName)
        {
            return _createServer(aName);
        }
    }

    /**
     *  Native methods required to implement getServer()
     */
    private native void _shutdown(int aHandle);
    private native int _createServer(String aName);
};

