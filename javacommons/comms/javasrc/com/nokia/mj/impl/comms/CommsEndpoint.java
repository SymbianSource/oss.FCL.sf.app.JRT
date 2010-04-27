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

package com.nokia.mj.impl.comms;

import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.comms.exception.CommsException;

/**
 * This is a CommsEndpoint class, it provides implementation for
 * registering listeners and for synchronous message sending.
 * <p>
 * Comms endpoint provides interprocess communication functionality. Comms communication
 * is point-to-point meaning that client endpoint can be connected to a single server at a time.
 * Communication is message based, where users wrap their payload to CommsMessage.
 * Comms endpoints support synchronous and asynchronous message sending.
 * <p>
 * Messages can be sent asynchronously or synchronously. Synchronous message sending blocks until
 * a reply is received or when specified timeout occurs.
 * <p>
 * CommsListener provides a way to receive messages asynchronously. Listeners are registered
 * to endpoint. When message belonging to listener is received, will Comms endpoint make a
 * callback for listener. Application can register as many listeners as need.
 * <p>
 * Example to use CommsEndpoint class:
 * <pre>
 * <code>
 *       CommsEndpoint comms = new CommsEndpoint();
 *       comms.connect(CommsEndpoint.JAVA_CAPTAIN);
 *       comms.registerListener(MODULE_ID, listener);
 *
 *       CommsMessage message = new CommsMessage();
 *
 *       message.setMessageId(MESSAGE_ID);
 *       message.setModuleId(MODULE_ID);
 *
 *       message.write("hello world from java");
 *
 *       comms.send(message);
 *       comms.destroy();
 * </code>
 * </pre>
 */

public final class CommsEndpoint extends CommsEndpointBase
{
    /**
     * 'well-known' server endpoints
     */
    public static final int JAVA_CAPTAIN = 10000;
    public static final int TCK_RUNNER = 10300;
    public static final int JAVA_PREINSTALLER = 10400;
    public static final int JAVA_APPCONVERTER = 10500;
    public static final int JAVA_DEBUGAPI = 10700;

    /**
     * This constant can be used when synchronous sendReceive should not return because of timeout.
     * SendReceive method will return when reply is received or when connection is closed.
     * @see #sendReceive
     */
    public static final int WAIT_FOR_EVER = -1;

    private boolean iReleaseHandle;

    static
    {
        Jvm.loadSystemLibrary("javacomms");
    }

    public CommsEndpoint()
    {
        iHandle = _create();
        iReleaseHandle = true;
    }

    private CommsEndpoint(int aHandle)
    {
        iHandle = aHandle;
        iReleaseHandle = false;
    }

    public static CommsEndpoint find(String aName)
    {
        int handle = _find(aName);
        CommsEndpoint client = null;
        if (handle != 0)
        {
            client = new CommsEndpoint(handle);
        }
        return client;
    }

    public void connect(int aName) throws CommsException
    {
        checkHandle();
        int rc = _connect(iHandle, aName);
        if (rc!=0)
        {
            throw new CommsException("connect failed, address=" + aName);
        }
    }

    public void disconnect() throws CommsException
    {
        checkHandle();
        int rc = _disconnect(iHandle);
        if (rc!=0)
        {
            throw new CommsException("disconnect failed");
        }
    }

    /**
     * Releases native resources.
     * This method must be called when endpoint is no longer needed.
     * After calling this method it's not possible to call other methods.
     */
    public void destroy()
    {
        if (iHandle != 0)
        {
            if (iReleaseHandle)
            {
                _release(iHandle);
            }
            else
            {
                _detach(iHandle);
            }
        }
        iHandle = 0;
    }


    // native methods
    private native int _create();
    private native int _connect(int handle, int name);
    private native int _disconnect(int handle);
    private native static int _find(String name);
}
