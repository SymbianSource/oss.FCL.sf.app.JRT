/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/**
 * Base class for WMA 2.0 (JSR-205) module test cases.
 */
package com.nokia.mj.test.wma;

import j2meunit.framework.*;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.Vector;

import javax.microedition.io.Connector;
import javax.wireless.messaging.MessageConnection;
import javax.wireless.messaging.MessagePart;
import javax.wireless.messaging.MultipartMessage;
import javax.wireless.messaging.SizeExceededException;

import java.io.OutputStream;
import javax.microedition.io.file.FileConnection;

import com.nokia.mj.impl.utils.Base64;
public class WmaTestUtils
{
    /** Maximum content size for a MultipartMessage. */
    public static final int MAX_CONTENT_SIZE = 1001 * 1024;
    public static Object lock = new Object();

    /**
     * Opens a connection to given URL.
     */
    public static MessageConnection getConnection(String url)
    throws WmaException
    {
        MessageConnection conn = null;
        try
        {
            // Open connection.
            conn = (MessageConnection)Connector.open(url, Connector.WRITE);
            if (conn == null)
            {
                throw new WmaException("Connection opening to url \"" + url + "\" failed");
            }
        }
        catch (WmaException wmae)
        {
            throw wmae;
        }
        catch (Exception e)
        {
            throw new WmaException("Connection opening to url \"" + url + "\" failed: " + e);
        }
        return conn;
    }

    /**
     * Returns a string that is too large for MultipartMessage contents.
     */
    public static String getTooLargeContent()
    {
        return getContentString(MAX_CONTENT_SIZE + 1);
    }

    /**
     * Returns a multipart message that is too large to be sent.
     */
    public static MultipartMessage getTooLargeMessage(
        MessageConnection conn, String subject)
    throws SizeExceededException, UnsupportedEncodingException
    {
        MultipartMessage msg = (MultipartMessage)conn.newMessage(
                                   MessageConnection.MULTIPART_MESSAGE);
        if (subject != null)
        {
            msg.setSubject(subject);
        }
        int oneContentSize = 1024;
        for (int i = 0; (i*oneContentSize) < (MAX_CONTENT_SIZE + 1); i++)
        {
            msg.addMessagePart(getDummyMessagePart(
                                   oneContentSize, Integer.toString(i)));
        }
        return msg;
    }

    /**
     * Creates a dummy MultipartMessage.
     */
    public static MultipartMessage getDummyMultipartMessage(
        MessageConnection conn, String address, String subject)
    throws SizeExceededException, UnsupportedEncodingException
    {
        MultipartMessage msg = (MultipartMessage)conn.newMessage(
                                   MessageConnection.MULTIPART_MESSAGE);
        if (address != null)
        {
            msg.setAddress(address);
        }
        if (subject != null)
        {
            msg.setSubject(subject);
        }
        msg.setHeader("X-Mms-Priority", "normal");
        msg.addMessagePart(getDummyMessagePart(1024, "1"));
        return msg;
    }

    /**
     * Returns MessagePart of given size. MIME type is text/plain,
     * encoding is iso-8859-1.
     */
    public static MessagePart getDummyMessagePart(
        int size, String contentId, String contentLocation, String enc)
    throws SizeExceededException, UnsupportedEncodingException
    {
        MessagePart mp = null;
        if (enc == null)
        {
            mp = new MessagePart(
                getContentString(size).getBytes(),
                "image/jpeg", contentId, contentLocation, enc);
        }
        else
        {
            mp = new MessagePart(
                getContentString(size).getBytes(enc),
                "text/plain", contentId, contentLocation, enc);
        }
        return mp;
    }

    /**
     * Returns MessagePart of given size. MIME type is text/plain,
     * encoding is iso-8859-1.
     */
    public static MessagePart getDummyMessagePart(int size, String contentId)
    throws SizeExceededException, UnsupportedEncodingException
    {
        return getDummyMessagePart(size, contentId, null, "iso-8859-1");
    }

    /**
     * Checks if given messages have the same content.
     */
    public static boolean messagesEqual(
        MultipartMessage sentMsg, MultipartMessage receivedMsg,
        String from, String to)
    {
        if (sentMsg == null || receivedMsg == null)
        {
            return false;
        }

        String msgStr1 = msgToString(sentMsg, false).toLowerCase();
        String msgStr2 = msgToString(receivedMsg, false).toLowerCase();
        if (!msgStr1.equals(msgStr2))
        {
            wmaDebug("messages not equal:\n" + msgStr1 + "\n" + msgStr2);
            return false;
        }

        // Check that sent message has correct address.
        if (!sentMsg.getAddress().equals(from))
        {
            wmaDebug("sent messages has incorrect address: "
                     + sentMsg.getAddress() + ", expected: " + from);
            return false;
        }

        // Check that received message has correct address.
        if ((to == null && receivedMsg.getAddress() != null) ||
                (receivedMsg.getAddress() != null
                 && !receivedMsg.getAddress().equals(to)))
        {
            wmaDebug("received message has incorrect address: "
                     + receivedMsg.getAddress() + ", expected: " + to);
            return false;
        }

        return true;
    }

    /**
     * Returns information from the given message as a string.
     */
    public static String msgToString(MultipartMessage msg, boolean allHeaders)
    {
        if (msg == null)
        {
            return null;
        }

        StringBuffer result = new StringBuffer();
        if (allHeaders)
        {
            result.append("getAddress: ").append(msg.getAddress()).append("\n");
            result.append(addressesToString(msg, "from"));
            result.append(addressesToString(msg, "to"));
        }
        result.append(addressesToString(msg, "cc"));
        result.append(addressesToString(msg, "bcc"));
        if (msg.getSubject() != null)
        {
            result.append("subject: ").append(msg.getSubject()).append("\n");
        }
        if (msg.getStartContentId() != null)
        {
            result.append("startContentId: ")
            .append(msg.getStartContentId()).append("\n");
        }
        if (allHeaders)
        {
            if (msg.getHeader("X-Mms-Delivery-Time") != null)
            {
                result.append("X-Mms-Delivery-Time: ")
                .append(msg.getHeader("X-Mms-Delivery-Time")).append("\n");
            }
        }
        if (msg.getHeader("X-Mms-Priority") != null)
        {
            result.append("X-Mms-Priority: ")
            .append(msg.getHeader("X-Mms-Priority")).append("\n");
        }
        MessagePart msgParts[] = msg.getMessageParts();
        if (msgParts != null && msgParts.length > 0)
        {
            result.append("MessageParts: ").append(msgParts.length).append("\n");
            for (int i = 0; i < msgParts.length; i++)
            {
                result.append("MessagePart["+i+"]:\n")
                .append(msgPartToString(msgParts[i]));
            }
        }
        return result.toString();
    }

    /**
     * Returns requested addresses from the given message as a string.
     */
    public static String addressesToString(MultipartMessage msg, String addrType)
    {
        StringBuffer result = new StringBuffer();
        String addrs[] = msg.getAddresses(addrType);
        if (addrs != null)
        {
            for (int i = 0; i < addrs.length; i++)
            {
                result.append(addrType).append(": ")
                .append(addrs[i]).append("\n");
            }
        }
        return result.toString();
    }

    /**
     * Returns information from the given MessagePart as a string.
     */
    public static String msgPartToString(MessagePart msgPart)
    {
        if (msgPart == null)
        {
            return null;
        }

        StringBuffer result = new StringBuffer();
        result.append("contentId: ").append(msgPart.getContentID()).append("\n");
        result.append("contentLocation: ").append(msgPart.getContentLocation()).append("\n");
        result.append("encoding: ").append(msgPart.getEncoding()).append("\n");
        result.append("mimeType: ").append(msgPart.getMIMEType()).append("\n");
        result.append("length: ").append(msgPart.getLength()).append("\n");
        if (msgPart.getContent() != null)
        {
            result.append("base64 encoded content: ").append(
                Base64.encode(msgPart.getContent())).append("\n");
        }
        return result.toString();
    }

    /**
     * Returns addresses as a single string.
     */
    public static String getAddressString(String type, String addrs[])
    {
        String result = type + ":";
        if (addrs != null)
        {
            for (int i = 0; i < addrs.length; i++)
            {
                if (i > 0)
                {
                    result += ",";
                }
                result += " " + addrs[i];
            }
        }
        return result;
    }

    /**
     * Returns a string of given size.
     */
    public static String getContentString(int size)
    {
        StringBuffer contents = new StringBuffer(size);
        for (int i = 0; i < size; i++)
        {
            contents.append(i%10);
        }
        return contents.toString();
    }

    /**
     * Returns application property with given name as an int.
     */
    public static int getAppPropInt(String value) throws WmaException
    {
        int result = 0;
        try
        {
            result = Integer.parseInt(value);
        }
        catch (Exception e)
        {
            throw new WmaException("Could not parse int from " + value +
                                   ": " + e.toString());
        }
        return result;
    }

    public static void wmaDebug(String msg)
    {
        wmaDebug(msg, null);
    }
    public static void wmaDebug(String msg, String testCaseName)
    {
        OutputStream out = null;
        if (out == null)
        {
            try
            {
                String path = System.getProperty("fileconn.dir.private");
                FileConnection filecon =
                    (FileConnection)Connector.open(path + "wma20mt.txt");
                // Create the file if it doesn't exist.
                if (!filecon.exists())
                {
                    filecon.create();
                }
                out = filecon.openOutputStream();
            }
            catch (Exception e)
            {
            }
        }
        if (out != null)
        {
            try
            {
                String outMsg = (new java.util.Date()) + (testCaseName != null ? (" [" + testCaseName + "] "):"") + msg + "\n";
                out.write(outMsg.getBytes());
                out.flush();
            }
            catch (Exception e)
            {
            }
        }
    }


    /**
     * Puts the thread into sleep for specified time (in seconds).
     */
    public static void wait(int delay)
    {
        try
        {
            Thread.sleep(delay * 1000);
        }
        catch (Exception ex)
        {
            wmaDebug("Exception while waiting: " + ex.toString());
        }
    }

    /**
     * Receives a single message from given connection in a separate thread.
     */
    public static MultipartMessage receiveMsg(MessageConnection conn, int timeout)
    throws WmaException
    {
        MessageReceiver receiver = new MessageReceiver(lock, conn);
        receiver.start();
        synchronized (lock)
        {
            try
            {
                // Bogdan: if notifications are done using "notify"
                // method, then the "Object.wait" method should be
                // used insted.
                // Thread.sleep() will not be notified by "notify",
                // it can only be interrupted, but not
                // notified.
                lock.wait((long)(timeout));
            }
            catch (InterruptedException e)
            {
            }
        }
        if (receiver.getError() != null)
        {
            throw new WmaException(receiver.getError()
                                   + (receiver.getTrace() != null?
                                      ", " + receiver.getTrace(): ""));
        }
        MultipartMessage msg = receiver.getMsg();
        if (msg == null)
        {
            throw new WmaException("Message was not received"
                                   + (receiver.getTrace() != null?
                                      ", " + receiver.getTrace(): ""));
        }
        return msg;
    }

    /**
     * Thread for receiving a message from given connection.
     */
    static class MessageReceiver extends Thread
    {
        private Object owner = null;
        private MessageConnection connection = null;
        private MultipartMessage msg = null;
        private String error = null;
        private String trace = null;

        public MessageReceiver(Object anOwner, MessageConnection aConnection)
        {
            owner = anOwner;
            connection = aConnection;
        }

        public void run()
        {
            if (connection == null)
            {
                error = "Receiving message failed: connection is null";
            }
            else
            {
                try
                {
                    trace = "Started to call receive...";
                    long start = System.currentTimeMillis();
                    msg = (MultipartMessage)connection.receive();
                    trace += " receive returned after " + ((System.currentTimeMillis()-start)/1000) + " seconds";
                }
                catch (Exception e)
                {
                    error = "Receiving message failed with exception: "
                            + e.toString();
                }
            }
            if (owner != null)
            {
                synchronized (owner)
                {
                    owner.notify();
                }
            }
        }

        public MultipartMessage getMsg()
        {
            return msg;
        }

        public String getError()
        {
            return error;
        }

        public String getTrace()
        {
            return trace;
        }
    }
}
