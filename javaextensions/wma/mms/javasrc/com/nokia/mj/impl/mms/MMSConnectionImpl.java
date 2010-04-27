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


package com.nokia.mj.impl.mms;

import java.io.IOException;
import java.io.InterruptedIOException;
import javax.wireless.messaging.*;

import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.mms.MmsPropertyRetriever;
import com.nokia.mj.impl.rt.support.ShutdownListener;
import com.nokia.mj.impl.rt.support.ApplicationUtils;

public final class MMSConnectionImpl implements MessageConnection
{
    private static final int INITIAL = 0;

    private static final int OPEN = 1;

    private static final int CLOSED = 2;

    // current state of execution
    private int iState;

    // status of send
    private int iSendError = 0;

    // number of received messages
    private int iMessagesOnQueue;

    // number of notifications to be done
    private int iNotificationsToMake;

    // handle to the native side
    private int iNativeHandle;

    // waiting for message
    private boolean iMessageWaiting;

    // any message notification missed
    private boolean iMissedMessage;

    // Is Server Connection
    private boolean iServerConnection;

    // synchronization for reading message
    private final Object iReadLock;

    // synchronization for sending
    private final Object iSendLock;

    // synchronization for sending message
    private final Object iWriteLock;

    // synchronization for closing
    private final Object iCloseLock;

    // synchronization for receiving
    private final Object iMessageLock;

    private String iUri;

    // Application Id
    private String iAppID = null;

    // the listener used for messages receiving notifications
    private MessageListener iMessageListener;

    public MMSConnectionImpl(String aUri, boolean aServerConnection)
    throws IOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSConnectionImpl::MMSConnectionImpl()");
        iUri = aUri;
        iState = INITIAL;
        iServerConnection = aServerConnection;
        // create the native side peer
        iNativeHandle = _createPeer(iServerConnection, iUri);
        iReadLock = new Object();
        iWriteLock = new Object();
        iCloseLock = new Object();
        iMessageLock = new Object();
        iSendLock = new Object();
        // register for shutdown listening
        setShutdownListener();
        if (iServerConnection)
        {
            iAppID = aUri.substring("mms://:".length());
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSConnectionImpl::MMSConnectionImpl()");
    }

    /*
    * This function registers this object for shutDown.
    */
    private void setShutdownListener()
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSConnectionImpl::setShutdownListener()");
        // Get the insatnce of ApplicationUtils.
        ApplicationUtils appUtils = ApplicationUtils.getInstance();

        // Get the name of the application.
        appUtils.addShutdownListener(new ShutdownListener()
        {
            //The method that gets called when Application is shutting down
            public void shuttingDown()
            {
                try
                {
                    close();
                }
                catch (IOException e)
                {
                    //Nothing to do, just ignore
                    Logger.LOG(Logger.EWMA, Logger.EInfo, e.toString());
                }
            }
        });
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSConnectionImpl::setShutdownListener()");
    }

    /**
     * open() is only called when a MIDlet calls Connector.open() a MIDlet will
     * not be able to access the Connection until open() has completed and a
     * reference has been returned. It should then only be called once, by only
     * one thread. particularly, a Connection cannot be closed by a MIDlet until
     * it has been fully opened.
     */
    public void open()
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSConnectionImpl::open()");
        if (iState == INITIAL)
        {
            iState = OPEN;
            // in case of server connection a notifier thread is created which
            // waits in the native side and informs the application when there
            // is an incoming message.
            if (iServerConnection)
            {
                new Thread(new Runnable()
                {
                    public void run()
                    {
                        _openConnection(iNativeHandle);
                    }
                }).start();
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSConnectionImpl::open()");
    }

    /**
     * close() is only called when a MIDlet calls close() on a connection that
     * is opened by calling Connector.open(). A MIDlet will not be able to access
     * the Connection after calling close() on that connection.
     */
    public void close() throws IOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSConnectionImpl::close()");
        synchronized (iCloseLock)
        {
            if (iState != CLOSED)
            {
                iState = CLOSED;
                _closeConnection(iNativeHandle);
                // notify the notifier thread if it is waiting for a receive()
                // or setMessageListener() to be called
                synchronized (iMessageLock)
                {
                    iMessageLock.notify();
                }
                // notify the receive operation about connection closed
                synchronized (iReadLock)
                {
                    iReadLock.notify();
                }
                _dispose(iNativeHandle);
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSConnectionImpl::close()");
    }

    /**
     * Returns the number of segments required for sending this Message.
     */
    public int numberOfSegments(Message aMessage)
    {
        // For MMS numberOfSegments is always 1.
        return 1;
    }

    private int getMMSMaxSize()
    {
        return MmsPropertyRetriever.getMMSMaxSize();
    }

    /**
     * Method for creating a Multipart Message object. If it is a client
     * connection The destination address of the newly created messages will be
     * set to the destination part of the URI provided when this connection was
     * created.
     *
     * @param aType
     *            The type of the message to be created. It can only be
     *            "Multipart"
     */
    public Message newMessage(String aType)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSConnectionImpl::newMessage()");
        if (MessageConnection.MULTIPART_MESSAGE.equals(aType))
        {
            if (iServerConnection)
            {
                Logger.LOG(Logger.EWMA, Logger.EInfo,
                           "- MMSConnectionImpl::newMessage()");
                return new MultipartMessageImpl(null, 0, getMMSMaxSize());
            }
            else
            {
                Logger.LOG(Logger.EWMA, Logger.EInfo,
                           "- MMSConnectionImpl::newMessage()");
                return new MultipartMessageImpl(iUri, 0, getMMSMaxSize());
            }
        }
        // unrecognized type
        throw new IllegalArgumentException(
            "Unrecognized message type: " + aType);
    }

    /**
     * Method for creating a Multipart Message object
     *
     * @param aType
     *            The type of the message to be created. It can only be
     *            "Multipart"
     * @param aAddress
     *            the destination address of the Message
     */
    public Message newMessage(String aType, String aAddress)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSConnectionImpl::newMessage()");
        if (MessageConnection.MULTIPART_MESSAGE.equals(aType))
        {
            Logger.LOG(Logger.EWMA, Logger.EInfo,
                       "- MMSConnectionImpl::newMessage()");
            return new MultipartMessageImpl(aAddress, 0, getMMSMaxSize());
        }
        // unrecognized type
        throw new IllegalArgumentException(
            "Unrecognized message type " + aType);

    }

    /**
     * Method for sending a certain message. Only Multipart Messages can be sent
     * through this connection. The Message to sent is validated before sending.
     *
     * @param aMsg
     *            The message to be sent
     * @exceptions  throws IOException if there is any n/w problem or if
     *             connection is already closed
     * @exceptions  throws InterruptedIOException if a timeout occurs or if the
     *             connection is closed while sending.
     * @exceptions  throws IllegalArguementException if any of the header field
     *             is not specified
     */
    public void send(Message aMsg) throws IOException, InterruptedIOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSConnectionImpl::send()");
        // validate the message to be sent
        MultipartMessageImpl mMsg = validateMessage(aMsg);
        if (iServerConnection)
        {
            mMsg.setReplyToAppID(iAppID);
        }
        else
        {
            // validate the client-side message
            validateClientMessage(mMsg);
        }
        // Before sending the message serialize it
        byte[] bMsg = mMsg.serialize();
        // to synchronize send functionality
        synchronized (iWriteLock)
        {
            synchronized (iSendLock)
            {
                // send operation should not be carried out while close is in
                // progress
                synchronized (iCloseLock)
                {
                    if (iState == CLOSED)
                    {
                        throw new IOException("Sending message failed: " +
                                              ExceptionStrings.CONNECTION_CLOSED);
                    }
                    //checking for valid permissions
                    ApplicationUtils appUtils = ApplicationUtils.getInstance();
                    MMSPermissionImpl permission = new MMSPermissionImpl(
                        "mms://*", "send",
                        getRecipientsCount(mMsg));
                    appUtils.checkPermission(permission);
                    // send the message
                    int status = _send(bMsg, 0, bMsg.length, mMsg.getAddress(),
                                       iNativeHandle);
                    if (status < 0)
                    {
                        _checkError(status);
                    }
                    // wait till the message was sent successfully
                    try
                    {
                        iSendLock.wait();
                    }
                    catch (InterruptedException e)
                    {
                        Logger.ELOG(Logger.EWMA, "Message sending failed", e);
                    }
                    if (iSendError < 0)
                    {
                        _checkError(iSendError);
                    }
                }
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSConnectionImpl::send()");
    }

    /**
     * Callback through which the Native notifies about the sending of a Message
     * or any error while sending a message
     *
     * @param1 result  the status of send operation
     */
    protected void messageSentCallback(int aResult)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSConnectionImpl::messageSentCallback()");
        synchronized (iSendLock)
        {
            iSendError = aResult;
            iSendLock.notify();
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSConnectionImpl::messageSentCallback()");
    }

    /**
     * Receive is a 3 step process: 1) Wait for some incoming message 2)
     * Retrieve the the message 3) Construct a Multipart Message
     *
     * @exceptions throws IOException if the connection is already closed.
     * @exceptions throws InterruptedIOException if the connection is closed
     *             while receiving
     * @returns the constructed message
     */
    synchronized public Message receive() throws IOException,
                InterruptedIOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSConnectionImpl::receive()");
        if (!iServerConnection)
        {
            // Not Supported in Client MODE
            throw new IOException(ExceptionStrings.CLIENT_MODE);
        }
        Message rMsg;
        synchronized (iReadLock)
        {
            // check for connection
            if (iState == CLOSED)
            {
                throw new IOException("Receiving message failed: " +
                                      ExceptionStrings.CONNECTION_CLOSED);
            }
            // if no messages were there to receive wait till some message
            // arrives
            if (iMessagesOnQueue == 0)
            {
                iMessageWaiting =true;
                // notify the notifier thread
                synchronized (iMessageLock)
                {
                    iMessageLock.notify();
                }
                // wait for incoming message notification
                try
                {
                    iReadLock.wait();
                }
                catch (InterruptedException e)
                {
                    Logger.ELOG(Logger.EWMA, "Message receiving failed", e);
                    throw new InterruptedIOException("");
                }
            }
            synchronized (iCloseLock)
            {
                //if the connection was closed throw InterruptedIOException
                if (iState == CLOSED)
                {
                    throw new InterruptedIOException(
                        "Connection closed while receiving message");
                }
                //checking for valid permissions
                ApplicationUtils appUtils = ApplicationUtils.getInstance();
                MMSPermissionImpl permission = new MMSPermissionImpl("mms://*",
                        "receive");
                appUtils.checkPermission(permission);

                byte[] msg = _retrieveMessage(iNativeHandle);
                rMsg = MultipartMessageImpl.deserialize(msg, getMMSMaxSize());
                iMessagesOnQueue--;
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSConnectionImpl::receive()");
        return rMsg;
    }

    /**
     * Sets the message listener for this connection
     *
     * @param aListener
     *            listener interested for incoming message.
     * @exceptions throws IOException if the connection is already closed or
     *             this method is called on a client connection.
     */
    public void setMessageListener(MessageListener aListener) throws IOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSConnectionImpl::setMessageListener()");
        if (!iServerConnection)
        {
            throw new IOException(ExceptionStrings.CLIENT_MODE);
        }
        if (iState == CLOSED)
        {
            throw new IOException("Setting message listener failed: " +
                                  ExceptionStrings.CONNECTION_CLOSED);
        }
        iMessageListener = aListener;
        if (iMessageListener != null)
        {
            synchronized (iMessageLock)
            {
                iMessageLock.notify();
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSConnectionImpl::setMessageListener()");
    }

    /**
     * Method that validates the message attributes
     *
     * @exceptions Throws IOException if any of the attribute is not valid
     */
    private MultipartMessageImpl validateMessage(Message aMsg)
    throws IOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSConnectionImpl::validateMessage()");
        // check against null
        if (null == aMsg)
        {
            throw new NullPointerException(ExceptionStrings.NULL_MESSAGE);
        }

        // make sure it is a MultipartMessageImpl
        MultipartMessageImpl mMsg;
        try
        {
            mMsg = (MultipartMessageImpl) aMsg;
        }
        catch (ClassCastException ex)
        {
            throw new IllegalArgumentException(ExceptionStrings.
                                               INVALID_MESSAGE_TYPE);
        }

        // check basic validation of the message and the corresponding parts
        if (!mMsg.iValidationDone)
        {
            // Forces the validation of the message if it hasn't been done
            // validation is done so that we reconstruct the message
            // with the eyes of the application and report any exceptions
            // to the application
            MultipartMessageImpl cloneMsg = new MultipartMessageImpl(
                mMsg.getDestinationAddress(),
                mMsg.getTimestamp() == null ? 0 : mMsg.getTimestamp().getTime(),
                MmsPropertyRetriever.getMMSMaxSize());

            // first go through addresses and clone them for the new message
            cloneAddresses(MultipartMessageHeader.ADDRESS_TYPE_TO, cloneMsg,
                           mMsg);
            cloneAddresses(MultipartMessageHeader.ADDRESS_TYPE_BCC, cloneMsg,
                           mMsg);
            cloneAddresses(MultipartMessageHeader.ADDRESS_TYPE_CC, cloneMsg,
                           mMsg);

            // clone the headers
            cloneMsg.setHeader(MultipartMessageHeader.DELIVERY_TIME_HEADER,
                               mMsg.getHeader(MultipartMessageHeader.DELIVERY_TIME_HEADER));
            cloneMsg.setHeader(MultipartMessageHeader.PRIORITY_HEADER,
                               mMsg.getHeader(MultipartMessageHeader.PRIORITY_HEADER));

            // clone the message parts
            cloneMessageParts(cloneMsg, mMsg);

            // clone the startContentId
            cloneMsg.setStartContentId(mMsg.getStartContentId());

            // clone the subject
            cloneMsg.setSubject(mMsg.getSubject());
        }

        // make sure there are recipients
        if (isEmpty(mMsg.getAddresses(MultipartMessageHeader.ADDRESS_TYPE_TO))
                && isEmpty(mMsg.getAddresses(MultipartMessageHeader.ADDRESS_TYPE_BCC))
                && isEmpty(mMsg.getAddresses(MultipartMessageHeader.ADDRESS_TYPE_CC)))
        {
            throw new IllegalArgumentException(ExceptionStrings.NO_RECIPIENTS);
        }

        if (mMsg.iReply)
        {
            // reset the delivery time
            mMsg.setHeader(MultipartMessageHeader.DELIVERY_TIME_HEADER, null);
            // if this message does not have the replyToAppid, it means it came
            // from a client connection and we can not reply
            if (mMsg.getReplyToAppID() == null ||(mMsg.getReplyToAppID() != null
                                                  && (mMsg.getReplyToAppID().length() == 0 || mMsg.getReplyToAppID()
                                                      .equals(MultipartMessageHeader.DUMMY_REPLY_TO_APP_ID))))
            {
                throw new IllegalArgumentException(ExceptionStrings.
                                                   CLIENT_MESSAGE);
            }
            String dest = mMsg.getAddress();
            mMsg.removeAddresses();
            // replace applicationID with replyToAppID
            mMsg.setAddress(dest);
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSConnectionImpl::validateMessage()");
        return mMsg;
    }

    private void cloneAddresses(String aAddressType,
                                MultipartMessageImpl aDestMsg, MultipartMessageImpl aSourceMsg)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSConnectionImpl::cloneAddresses()");
        String[] addresses = aSourceMsg.getAddresses(aAddressType);
        if (addresses != null)
        {
            for (int i = 0; i < addresses.length; i++)
            {
                aDestMsg.addAddress(aAddressType, addresses[i]);
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSConnectionImpl::cloneAddresses()");
    }

    private void cloneMessageParts(MultipartMessageImpl aDestMsg,
                                   MultipartMessageImpl aSourceMsg) throws SizeExceededException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSConnectionImpl::cloneMessageParts()");
        MessagePart[] parts = aSourceMsg.getMessageParts();
        if (parts != null)
        {
            for (int i = 0; i < parts.length; i++)
            {
                MessagePart part = parts[i];
                aDestMsg.addMessagePart(new MessagePart(part.getContent(),
                                                        part.getMIMEType(), part.getContentID(),
                                                        part.getContentLocation(), part.getEncoding()));
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSConnectionImpl::cloneMessageParts()");
    }

    /**
     * Check that the address that has been provided when this connection was
     * created is still among the "to" recipients
     */
    private void validateClientMessage(MultipartMessageImpl aMsg)
    {
        // sets the replyToAppID to its own iAppID
        aMsg.setReplyToAppID(MultipartMessageHeader.DUMMY_REPLY_TO_APP_ID);
    }

    /**
     * Method used for checking if there are no recipients set for * a message
     * to be set
     */
    private boolean isEmpty(String[] aArray)
    {
        if (aArray == null || (aArray != null && aArray.length == 0))
        {
            return true;
        }
        return false;
    }

    /**
     * Method that ensures whether a connection is opened
     *
     * @exceptions Throws IOException if the connection is in closed state
     */
    private void ensureOpen() throws IOException
    {
        synchronized (iCloseLock)
        {
            if (iState == CLOSED)
            {
                throw new IOException(ExceptionStrings.CONNECTION_CLOSED);
            }
        }
    }

    /**
     * Callback through which the Native notifies about the receiving of a
     * Message or any error while receiving a message
     *
     * @param1  aReceivedMesgs - number of messages to be received
     *
     */
    protected int messageReceiveCallback(int aReceivedMesgs)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ MMSConnectionImpl::messageReceiveCallback()");
        synchronized (iMessageLock)
        {
            if ((!iMessageWaiting) && (null == iMessageListener))
            {
                try
                {
                    iMessageLock.wait();
                }
                catch (InterruptedException e)
                {
                    Logger.ELOG(Logger.EWMA, "Receiving a callback failed", e);
                }
            }
        }
        synchronized (iReadLock)
        {
            if (aReceivedMesgs >= 0)
            {
                iMessagesOnQueue += aReceivedMesgs;
                iNotificationsToMake += aReceivedMesgs;
            }
            if (iMessageWaiting)
            {
                iMessageWaiting = false;
                //if a receive operation was called notify the receive about
                //the incoming message
                iReadLock.notify();

            }
            else if (iMessageListener != null)
            {
                while (iNotificationsToMake > 0)
                {
                    iMessageListener.notifyIncomingMessage(this);
                    iNotificationsToMake--;
                }
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- MMSConnectionImpl::messageReceiveCallback()");
        return ((iState == CLOSED)? -1 : 0);
    }

    private int getRecipientsCount(MultipartMessageImpl mMsg)
    {
        int cnt = 0;
        String[] tmp = mMsg.getAddresses(
                           MultipartMessageHeader.ADDRESS_TYPE_TO);
        if (tmp != null)
        {
            cnt += tmp.length;
        }
        tmp = mMsg.getAddresses(
                  MultipartMessageHeader.ADDRESS_TYPE_CC);
        if (tmp != null)
        {
            cnt += tmp.length;
        }
        tmp = mMsg.getAddresses(
                  MultipartMessageHeader.ADDRESS_TYPE_BCC);
        if (tmp != null)
        {
            cnt += tmp.length;
        }
        return cnt;
    }

    private native int _createPeer(boolean aServerConnection, String aUri);

    private native void _openConnection(int aNativeHandle);

    private native int _send(byte[] aMsg, int aOffset, int aMsgLength,
                             String aAddress, int aNativeHandle);

    private native byte[] _retrieveMessage(int aNativeHandle);

    private native void _closeConnection(int aNativeHandle);

    private native void _dispose(int aNativeHandle);

    private native void _checkError(int aError);
}


