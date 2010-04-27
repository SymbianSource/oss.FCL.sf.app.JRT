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

import java.io.IOException;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.comms.exception.CommsException;

/**

 * This is an abstract base class for Comms endpoints. It provides implementation for
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
 * Application can register also a default listener, which gets called if there is no registered
 * listener for received message.
 */

public abstract class CommsEndpointBase
{

    protected int iHandle;

    /**
     * Creates a Comms endpoint and connects to a given server endpoint.
     * @param aName Server Endpoint name
     * @throws IOException  if unable to connect to server
     */
    public CommsEndpointBase()
    {
    }

    /**
     * Registers message handler for given module id.
     * CommsListener provides a way to receive messages asynchronously.
     * Module id identifies to which component message belongs. Every message will contain module id
     * information that is used to route message to correct listener.
     * <p>
     * There can be one registered listener for each module id and same listener can be registered
     * for multiple module ids.
     * @param aModuleId
     *            module id, identifies group of messages that should be forwarded to this handler
     * @param aListener
     *            callback interface, which will be called when message with matching module id is received
     * @throws CommsException
     *            if listener exists with same module id
     * @see #unregisterListener
     * @see CommsListener
     */
    public void registerListener(int aModuleId, CommsListener aListener) throws CommsException
    {
        if (aListener == null)
        {
            throw new IllegalArgumentException("aListener is null");
        }
        checkHandle();
        int rc = _registerlistener(iHandle, aModuleId, aListener);
        if (rc!=0)
        {
            throw new CommsException("Listener already exists, moduleId=" + aModuleId);
        }
    }

    /**
     * Unregisters message handler for given module id.
     * @param aModuleId
     *            module id, identifies which handler should be removed
     * @throws CommsException
     *            if listener for module id is not found
     * @see #registerListener
     */
    public void unregisterListener(int aModuleId) throws CommsException
    {
        checkHandle();
        int rc = _unregisterlistener(iHandle, aModuleId);
        if (rc!=0)
        {
            throw new CommsException("Listener not found, moduleId=" + aModuleId);
        }
    }

    /**
     * Registers default listener.
     * There can be only one default listener. Default listener is called if there is no registered listener for received message.
     * If there is no default listener then received messages without listener will be discarded.
     * @param aListener
     *            callback interface, which will be called when message with matching module id is received
     * @throws CommsException
     *            if default listener already exists
     * @see #unregisterDefaultListener
     * @see CommsListener
     */
    public void registerDefaultListener(CommsListener aListener) throws CommsException
    {
        if (aListener == null)
        {
            throw new IllegalArgumentException("aListener is null");
        }
        checkHandle();
        int rc = _registerdefaultlistener(iHandle, aListener);
        if (rc!=0)
        {
            throw new CommsException("default listener already exists");
        }
    }

    /**
     * Unregisters default listener.
     * @throws CommsException
     *            if default listener does not exist
     * @see #registerDefaultListener
     */
    public void unregisterDefaultListener() throws CommsException
    {
        checkHandle();
        int rc = _unregisterdefaultlistener(iHandle);
        if (rc!=0)
        {
            throw new CommsException("default listener not found");
        }
    }

    /**
     * Sends a message.
     * It is important that message header is properly filled because header
     * information is used in message routing.
     * @param aMessage A message to be sent
     * @throws CommsException
     *          if error occurs in message sending
     * @see #sendReceive
     * @see CommsMessage
     */
    public void send(CommsMessage aMessage) throws CommsException
    {
        if (aMessage == null)
        {
            throw new IllegalArgumentException("aMessage is null");
        }
        checkHandle();
        int rc = _send(iHandle, aMessage.toByteArray());
        if (rc != 0)
        {
            throw new CommsException("send failed, error code=" + rc);
        }
    }

    /**
     * Sends and receives message syncronously.
     * Method blocks until response message is received or timeout expires.
     * It is important that message header is properly filled because header
     * information is used in message routing.
     * @param aMessage
     *          A message to be sent
     * @param timeoutInSecs
     *          Timeout in seconds
     * @return message
     *          Received message
     * @throws CommsException
     *          if error occurs in message sending
     *          if timeout occurs
     * @see #send
     * @see CommsMessage
     */
    public CommsMessage sendReceive(CommsMessage aMessage, int timeoutInSecs) throws CommsException
    {
        if (aMessage == null)
        {
            throw new IllegalArgumentException("aMessage is null");
        }
        checkHandle();
        try
        {
            byte[] msg = _sendreceive(iHandle, aMessage.toByteArray(), timeoutInSecs);
            return new CommsMessage(msg);
        }
        catch (IOException io)
        {
            throw new CommsException("sendReceive failed", io);
        }
        catch (InterruptedException ie)
        {
            throw new CommsException("sendReceive timeout", ie);
        }
    }

    /**
     * Releases native resources.
     * This method must be called when endpoint is no longer needed.
     * After calling this method it's not possible to call other methods.
     */
    abstract public void destroy();

    /**
     * Checks that native peer exists
     * @throws CommsException
     *          if no native peer
     */
    protected void checkHandle() throws CommsException
    {
        if (iHandle == 0)
        {
            throw new CommsException("No native peer");
        }
    }

    // native methods
    private native int _registerlistener(int handle, int moduleId, CommsListener listener);
    private native int _unregisterlistener(int handle, int moduleId);

    private native int _registerdefaultlistener(int handle, CommsListener listener);
    private native int _unregisterdefaultlistener(int handle);

    private native int _send(int handle, byte message[]);
    private native byte[] _sendreceive(int handle, byte message[], int timeout) throws IOException, InterruptedException;

    protected native void _release(int handle);
    protected native void _detach(int handle);
}
