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
 * This class implements Comms server endpoint. Server endpoint waits for incoming
 * connections and it forwards received messages to listeners.
 * <p>
 * Comms endpoint provides interprocess communication functionality. Comms communication
 * is point-to-point meaning that client endpoint can be connected to a single server at a time.
 * Communication is message based, where users wrap their payload to CommsMessage.
 * Comms endpoints support synchronous and asynchronous message sending.
 * <p>
 * CommsListener provides a way to receive messages asynchronously. Listeners are registered
 * to endpoint. When message belonging to listener is received, will Comms endpoint make a
 * callback for listener. Application can register as many listeners as need.
 * <p>
 * Application can register also a default listener, which gets called if there is no registered
 * listener for received message.
 * <p>
 * CommsServerEndpoint is thread safe.
 * <p>
 * Usage:
 * It is recomended that at least default listener is registered before CommsServerEndpoint
 * is started. This ensures that no messages are lost.
 * <p>
 * Example to use CommsServerEndpoint class:
 * <pre>
 * <code>
 *       CommsServerEndpoint server = new CommsServerEndpoint();
 *       server.start(CommsEndpoint.JAVA_CAPTAIN);
 *       server.registerListener(MODULE_ID, listener);
 *
 *       server.stop();
 *       server.destroy();
 * </code>
 * </pre>
 */

public final class CommsServerEndpoint extends CommsEndpointBase
{

    private boolean iReleaseHandle;

    static
    {
        Jvm.loadSystemLibrary("javacomms");
    }

    /**
     * Creates a Comms endpoint.
     */
    public CommsServerEndpoint()
    {
        iHandle = _create();
        iReleaseHandle = true;
    }

    private CommsServerEndpoint(int aHandle)
    {
        iHandle = aHandle;
        iReleaseHandle = false;
    }

    public static CommsServerEndpoint find(String aName)
    {
        int handle = _find(aName);
        CommsServerEndpoint server = null;
        if (handle != 0)
        {
            server = new CommsServerEndpoint(handle);
        }
        return server;
    }

    public void start(int aName) throws CommsException
    {
        checkHandle();
        int rc = _start(iHandle, aName);
        if (rc!=0)
        {
            throw new CommsException("start failed, address=" + aName);
        }
    }

    public void stop() throws CommsException
    {
        checkHandle();
        int rc = _stop(iHandle);
        if (rc!=0)
        {
            throw new CommsException("stop failed");
        }
    }

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
    private native int _start(int handle, int name);
    private native int _stop(int handle);
    private native static int _find(String name);
}
