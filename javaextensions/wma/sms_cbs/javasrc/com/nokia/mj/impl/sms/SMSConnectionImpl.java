/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.sms;

import java.io.IOException;
import java.io.InterruptedIOException;

import javax.wireless.messaging.Message;
import javax.wireless.messaging.TextMessage;
import javax.wireless.messaging.BinaryMessage;
import javax.wireless.messaging.MessageListener;
import javax.wireless.messaging.MessageConnection;

import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.smscbs.utils.WmaUrl;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
/**
 * The SMSConnectionImpl used for sending and receiving Binary/Text Messages. It
 * offers methods for creating Binary/Text Messages and for sending them. It
 * implements the receiving functionality.
 */
public class SMSConnectionImpl implements MessageConnection
{
    private static final String SERVER_MSG =
        "Message Connection must be a Server connection";

    private static final int INITIAL = 0;

    private static final int OPEN = 1;

    private static final int CLOSED = 2;

    private static final int ERROR = 3;

    private static final int MAXIMUM_MESAGE_SEGMENTS_SUPPORTED = 10;

    //current Connection state
    private int iState;

    // any error while receiving message
    private int iRecvError;

    //status of send
    private int iSendResult;

    // waiting for message
    private int iMessageWaiting;

    // number of received messages
    private int iMessagesOnQueue;

    // number of notifications to be done
    private int iMessageNotificationsToMake;

    // handle to the native side
    private  int iNativeHandle;

    // any message notification missed
    private boolean iMissedMessage;

    // Is Server Connection
    private boolean iServerConnection;

    // synchronization for reading message
    private final Object iReadLock;

    // synchronization for sending message
    private final Object iWriteLock;

    // synchronization for closing
    private final Object iCloseLock;

    // synchronization for receiving
    private final Object iMessageLock;

    // synchronization for sending
    private final Object iSendLock;

    // the listener used for messages receiving notifications
    private MessageListener iMessageListener;

    private WmaUrl iUri;
    
    private Finalizer iFinalizer;

    public SMSConnectionImpl(WmaUrl aMsgUri, boolean aServerMode)
    throws IOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ SMSConnectionImpl::SMSConnectionImpl");
        iUri = aMsgUri;
        iState = INITIAL;
        iServerConnection = aServerMode;
        //create the native side peer
        iNativeHandle = _createConnection(iUri.getAbsoluteUri(),
                                          iServerConnection);
        iReadLock = new Object();
        iWriteLock = new Object();
        iCloseLock = new Object();
        iMessageLock = new Object();
        iSendLock = new Object();
        //register for Finalization
        iFinalizer = registerForFinalization();
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "- SMSConnectionImpl::SMSConnectionImpl");
    }

    /*
     * This function registers for Finalization.
     */


    public Finalizer registerForFinalization()
    {
        return new Finalizer()
        {
            public void finalizeImpl()
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
        };
    }

    /**
     * open() is only called when a MIDlet calls Connector.open() a MIDlet will
     * not be able to access the Connection until open() has completed and a
     * reference has been returned. It should then only be called once, by only
     * one thread. particularly, a Connection cannot be closed by a MIDlet until
     * it has been fully opened.
     * @exceptions Throws IOException if any error while Opening
     */
    public void open() throws IOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,"+ SMSConnectionImpl::open");
        if (iState == INITIAL)
        {
            iState = OPEN;
            //in case of server connection a notifier thread is created which
            //waits in the native side and informs the application when there
            //is an incoming message.
            if (iServerConnection)
            {
                synchronized (iCloseLock)
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
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,"- SMSConnectionImpl::open");
    }

    /**
     * close() is only called when a MIDlet calls close() on a connection that
     * is opened by calling Connector.open() A MIDlet will not be able to access
     * the Connection after calling close() on that connection.
     * @exceptions Throws IOException if any error while closing.
     */
    public void close() throws IOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,"+ SMSConnectionImpl::close");
        synchronized (iCloseLock)
        {
            if (iState != CLOSED)
            {
                iState = CLOSED;
                _closeConnection(iNativeHandle);

                //notify the notifier thread if it is waiting for a receive() or
                //setMessageListener() to be called
                synchronized (iMessageLock)
                {
                    iMessageLock.notify();
                }
                //notify the receive operation about connection closed
                synchronized (iReadLock)
                {
                    iReadLock.notify();
                }
                _dispose(iNativeHandle);
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,"- SMSConnectionImpl::close");
    }

    /**
     * Sets the message listener for this connection
     *
     * @param aListener
     *            listener interested for incoming message.
     * @exceptions throws IOException if the connection is already closed
     *             or this method is called on a client connection.
     */
    public void setMessageListener(MessageListener aListener)
    throws IOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ SMSConnectionImpl::setMessageListener");
        if (!iServerConnection)
        {
            throw new IOException(SERVER_MSG);
        }

        if (iState == CLOSED)
        {
            throw new IOException(
                "Setting message listener failed: connection is already closed");
        }
        checkReceivePermission();
        synchronized (iReadLock)
        {
            iMessageListener = aListener;
            if (aListener != null)
            {
                synchronized (iMessageLock)
                {
                    iMessageLock.notify();
                }
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ SMSConnectionImpl::setMessageListener");
    }

    /**
     * checks for any error while receiving a message
     * @exception throws IOException in case of any error
     */
    private final void checkForReceiveError() throws IOException
    {
        if (iRecvError !=0)
        {
            int err = iRecvError;
            iRecvError = 0;
            if (err < 0)
            {
                _checkError(err, false);
            }
        }
    }

    /**
     * Checks for security permission for receive operation.
     * @exception throws SecurityException if no valid permission exists.
     */
    protected void checkReceivePermission()
    {
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        SMSPermissionImpl permission = new SMSPermissionImpl("sms://*",
                "receive");
        appUtils.checkPermission(permission);
    }
    /**
     * Receive is a 3 step process: 1) Wait for some incoming message 2)
     * Retrieve the the message 3) Construct a Binary/Text Message
     *
     * @exceptions  throws IOException if the connection is already closed.
     * @exceptions  throws InterruptedIOException if the connection is closed
     *             while receiving
     * @returns the constructed message
     */
    public Message receive() throws IOException, InterruptedIOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,"+ SMSConnectionImpl::receive");
        if (!iServerConnection)
        {
            //throw IOException if receive is called on client connection
            throw new IOException(SERVER_MSG);
        }
        Message message = null;
        synchronized (iReadLock)
        {
            //check for connection
            if (iState == CLOSED)
            {
                throw new IOException(
                    "Receiving message failed: connection is already closed");
            }
            if (iUri.isRestrictedPort() == true)
            {
                throw new SecurityException(
                    "No permission to receive on port: " + iUri.getPort());
            }
            checkReceivePermission();
            //if no messages were there to receive wait till some message
            //arrives
            if (iMessagesOnQueue == 0)
            {
                iMessageWaiting++;
                //notify the notifier thread
                synchronized (iMessageLock)
                {
                    iMessageLock.notify();
                }
                //wait for incoming message notification
                try
                {
                    Logger.LOG(Logger.EWMA, Logger.EInfo,
                               "WMA : Waiting for message");
                    iReadLock.wait();
                }
                catch (InterruptedException ex)
                {
                    Logger.ELOG(Logger.EWMA, ex.toString(), ex);
                }
            }
            //check for any error while receiving
            checkForReceiveError();
            synchronized (iCloseLock)
            {
                String remoteAddress;
                //if the connection was closed throw InterruptedIOException
                if (iState == CLOSED)
                {
                    throw new InterruptedIOException(
                        "Connection closed while receiving message");
                }
                //get the message type ( Text/Binary)
                final int messageType = _getMessageType(iNativeHandle);
                if (messageType != SMSMessageImpl.MESSAGE_TEXT &&
                        messageType != SMSMessageImpl.MESSAGE_BINARY)
                {
                    if (messageType < 0)
                    {
                        _checkError(messageType, false);
                    }
                }
                //get the remote host address
                remoteAddress = _getHostAddress(iNativeHandle);
                //get the time stamp of the message
                final long timestamp = _getTimestamp(iNativeHandle);
                //construct the message object based on the message type
                if (messageType == SMSMessageImpl.MESSAGE_TEXT)
                {
                    //get the text message data.
                    final String textData = (String)_getReceivedMessageData(
                                                iNativeHandle, messageType);
                    //construct the text message object
                    TextMessage textMessage = new SMSTextMessageImpl(
                        remoteAddress, timestamp);
                    textMessage.setPayloadText(textData);
                    message = textMessage;
                }
                else
                {
                    //get the Binary message data
                    final byte[] binaryData = (byte[]) _getReceivedMessageData(
                                                  iNativeHandle, messageType);
                    //construct the binary message object
                    BinaryMessage binaryMessage = new SMSBinaryMessageImpl(
                        remoteAddress, timestamp);
                    binaryMessage.setPayloadData(binaryData);
                    message = binaryMessage;
                }
            }
            iMessagesOnQueue--;
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,"- SMSConnectionImpl::receive");
        return message;
    }

    /**
     * Method for creating a Binary/Text Message object. If it is a client
     * connection The destination address of the newly created messages will be
     * set to the destination part of the URI provided when this connection was
     * created.
     *
     * @param aType
     *            The type of the message to be created. It can only be
     *            "Binary/Text"
     */
    public Message newMessage(String aType)
    {
        String address = null;
        if (!iServerConnection)
        {
            address = iUri.getAbsoluteUri();
        }

        return newMessage(aType, address);
    }

    /**
     * Method for creating a Binary/Text Message object
     *
     * @param aType
     *            The type of the message to be created. It can only be
     *            "Binary/Text"
     * @param aAddress
     *            the destination address of the Message
     */
    public Message newMessage(String aType, String aAddress)
    {
        Message msg;
        if (aType.equals(TEXT_MESSAGE))
        {
            msg = new SMSTextMessageImpl(aAddress, 0);
        }
        else if (aType.equals(BINARY_MESSAGE))
        {
            msg = new SMSBinaryMessageImpl(aAddress, 0);
        }
        else
        {
            throw new IllegalArgumentException(
                "Message type is invalid: " + aType);
        }

        return msg;
    }

    /**
     * Returns the number of segments required for sending a
     *  Binary/Text Message.
     * @param aMsg The Message for which the segments to be found.
     * @exceptions  throws IllegalArguementException if no address is specified
     *               or if the message is not a valid message
     */
    public int numberOfSegments(Message aMsg)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ SMSConnectionImpl::numberOfSegments");
        int messageSegments = 0;
        if (aMsg == null)
        {
            throw new NullPointerException("Message parameter is null");
        }
        String address = aMsg.getAddress();
        // if it is a serverConnection and address field of the message is null
        // then it calculates numberOfSegments assuming Port number present.
        // Because that will be the maximum numberOfSegments required to send
        // the message.
        final WmaUrl uri;
        if (iServerConnection && address != null)
        {
            uri = new WmaUrl(address);
            uri.validateUri();
        }
        else
        {
            uri = iUri;
        }

        final SMSMessageImpl messageImpl;
        try
        {
            messageImpl = (SMSMessageImpl) aMsg;
        }
        catch (ClassCastException ex)
        {
            throw new IllegalArgumentException("NumberOfSegments can only"
                                               + "be calculated for Text/Binary Message");
        }
        synchronized (iCloseLock)
        {
            if (iState != CLOSED)
            {
                if (messageImpl.getType() == messageImpl.MESSAGE_TEXT)
                {

                    messageSegments = _smsTextDataSegments(iNativeHandle,
                                                           messageImpl.getType(), uri.getHost(), uri.getPort(),
                                                           messageImpl.getPayloadTextData());
                }
                else
                {

                    messageSegments = _smsBinaryDataSegments(iNativeHandle,
                                      messageImpl.getType(), uri.getHost(), uri.getPort(),
                                      messageImpl.getPayloadBinaryData());
                }
            }
        }
        if (messageSegments > MAXIMUM_MESAGE_SEGMENTS_SUPPORTED)
        {
            messageSegments = 0;
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ SMSConnectionImpl::numberOfSegments");
        return messageSegments;
    }

    /**
     * Method for sending a certain message. Only Binary/Text Messages can be
     * sent through this connection. The destination addresses of the Message to
     * be sent have to contain the destination address .
     *
     * @param aMsg
     *            The message to be sent
     * @exceptions throws IOException if there is any n/w problem or if
     *             connection is already closed
     * @exceptions throws InterruptedIOException if a timeout occurs or if
     *               the connection is closed while sending.
     * @exceptions throws IllegalArguementException if No address or host is
     *               specified
     */
    public void send(Message aMsg) throws IOException, InterruptedIOException
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo, "+ SMSConnectionImpl::send");
        int messageSegments = 0;
        if (aMsg == null)
        {
            throw new NullPointerException("Message parameter is null");
        }
        String sendAddress = aMsg.getAddress();
        //check whether destination address is specified in the message
        if (sendAddress == null)
        {
            throw new IllegalArgumentException(
                "Message destination address not specified");
        }
        final WmaUrl uri = new WmaUrl(sendAddress);
        // validate the address
        uri.validateUri();
        // check whether destination host address is specified in the message
        if ("".equals(uri.getHost()))
        {
            throw new IllegalArgumentException(
                "Message destination address not specified");
        }
        final SMSMessageImpl messageImpl;
        try
        {
            messageImpl = (SMSMessageImpl) aMsg;
        }
        catch (ClassCastException ex)
        {
            throw new IllegalArgumentException("The message to be sent with"
                                               + "this connection can only be Text/Binary Message");
        }
        // validates the port
        if (uri.isRestrictedPort() == true)
        {
            throw new SecurityException(
                "No permission to send on port: " + uri.getPort());
        }
        //to synchronize send functionality
        synchronized (iWriteLock)
        {
            synchronized (iSendLock)
            {
                // send operation should not be carried out while close
                // is in progress
                synchronized (iCloseLock)
                {
                    //verify whether connection is closed.
                    if (iState == CLOSED)
                    {
                        throw new IOException(
                            "Sending message failed: connection is already closed");
                    }
                    //calculate the number of message segments required to send this
                    //message
                    if (messageImpl.getType() == messageImpl.MESSAGE_TEXT)
                    {

                        messageSegments = _smsTextDataSegments(iNativeHandle,
                                                               messageImpl.getType(), uri.getHost(), uri.getPort(),
                                                               messageImpl.getPayloadTextData());
                    }
                    else
                    {

                        messageSegments = _smsBinaryDataSegments(iNativeHandle,
                                          messageImpl.getType(), uri.getHost(), uri.getPort(),
                                          messageImpl.getPayloadBinaryData());
                    }
                    //check whether message is too big for sending
                    if (messageSegments > MAXIMUM_MESAGE_SEGMENTS_SUPPORTED)
                    {
                        throw new IllegalArgumentException(
                            "Message exceeds the maximum supported segments: " +
                            MAXIMUM_MESAGE_SEGMENTS_SUPPORTED);
                    }
                    ApplicationUtils appUtils = ApplicationUtils.getInstance();
                    // remove the protocol
                    SMSPermissionImpl permission = new SMSPermissionImpl("sms://*",
                            "send",messageSegments,uri.getHost()
                            + ((uri.getPort() != null
                                && uri.getPort().length() > 0)
                               ? (":" + uri.getPort()) : ""));

                    appUtils.checkPermission(permission);
                    //to allow close operation to succeed while sending, send
                    //operation is carried out in a new thread
                    new Thread(new Runnable()
                    {
                        public void run()
                        {
                            doSend();
                        }
                    }).start();
                    //wait till the message was sent successfully
                    try
                    {
                        iSendLock.wait();
                    }
                    catch (InterruptedException ex)
                    {
                        Logger.ELOG(Logger.EWMA, "Sending a message failed", ex);
                    }
                    if (iSendResult < 0)
                    {
                        _checkError(iSendResult, true);
                    }
                }
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo, "- SMSConnectionImpl::send");
    }


    /**
     * This method runs in seperate thread and unblocks the send with the
     * send status
     */
    private void doSend()
    {
        iSendResult = _sendMessage(iNativeHandle);
        synchronized (iSendLock)
        {
            iSendLock.notify();
        }
    }

    /**
     * Callback through which the Native notifies about the receiving of a
     * Message or any error while receiving a message.
     * @param1  aReceivedMesgs - number of messages to be received
     * @param2  aError - any error while receiving a message
     *
     */
    protected int messageReceiveCallback(int aReceivedMsgs, int aError)
    {
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ SMSConnectionImpl::messageReceiveCallback");
        synchronized (iMessageLock)
        {
            //check either receive was called or a listener was set on this
            //connection
            if (0 == iMessageWaiting && (null == iMessageListener))
            {
                //if neither receive nor setMessageListener was called on this
                //wait for either of them to be called
                try
                {
                    iMessageLock.wait();
                }
                catch (InterruptedException ex)
                {
                    Logger.ELOG(Logger.EWMA, ex.toString(), ex);
                }
            }
        }
        synchronized (iReadLock)
        {
            //calculate number of new messages arrived and to be notified
            if (aReceivedMsgs > 0)
            {
                iMessagesOnQueue += aReceivedMsgs;
                iMessageNotificationsToMake += aReceivedMsgs;
            }
            //incase of any error store that error
            else
            {
                iRecvError = aError;
            }
            if (0 != iMessageWaiting)
            {
                iMessageWaiting--;
                //if a receive operation was called notify the receive about
                //the incoming message
                iReadLock.notify();
            }
            else if (null != iMessageListener)
            {
                //if a listener was set notify the listener about the incoming
                //messages
                while (iMessageNotificationsToMake > 0)
                {
                    iMessageListener.notifyIncomingMessage(this);
                    iMessageNotificationsToMake--;
                }
                //incase of an error notify
                if (iRecvError != 0)
                {
                    iMessageListener.notifyIncomingMessage(this);
                }
            }
        }
        Logger.LOG(Logger.EWMA, Logger.EInfo,
                   "+ SMSConnectionImpl::messageReceiveCallback");
        return ((iState == CLOSED)? -1 : 0);
    }

    private native  int _createConnection(String aUri,
                                          boolean aServerConnection);

    private native void _openConnection(int aNativeHandle);

    private native void _closeConnection(int aNativeHandle);

    private native int _smsTextDataSegments(int aNativePeerHandle,
                                            int aMsgType, String aHostAddress, String aHostPort, String aData);

    private native int _smsBinaryDataSegments(int aNativePeerHandle,
            int aMsgType, String aHostAddress, String aHostPort, byte[] aData);

    private native int _sendMessage(int aNativePeerHandle);

    private native int _getMessageType(int aNativePeerHandle);

    private native String _getHostAddress(int aNativePeerHandle);

    private native long _getTimestamp(int aNativePeerHandle);

    private native Object _getReceivedMessageData(int aNativePeerHandle,
            int aMsgType);
    private native void _dispose(int aNativeHandle);

    private native void _checkError(int aError,boolean aIsSendOperation);
}

