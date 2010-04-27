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
package com.nokia.mj.test.wma;

import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

import javax.microedition.lcdui.Form;
import javax.microedition.midlet.MIDlet;
import javax.wireless.messaging.MessageConnection;
import javax.wireless.messaging.MessagePart;
import javax.wireless.messaging.MultipartMessage;
import javax.wireless.messaging.SizeExceededException;

import com.nokia.mj.impl.utils.Base64;
/**
 * WMA 2.0 module test utilities for push testing.
 */
public class WmaPushUtils
{
    /** MSISDN for the receiver terminal. */
    static public String receiverMsisdn = "+5550000";

    /** MSISDN for the sender terminal. */
    static public String senderMsisdn = "+5551000";

    /** Push application id. */
    static public String appId = "test.push";

    /** Push application id for static midlet. */
    static public String appIdStatic = "test.push.static";

    /** Server connection url. */
    static public String serverConnectionUrl = "mms://:" + appId;

    /** Server connection url for static midlet. */
    static public String serverConnectionUrlStatic = "mms://:" + appIdStatic;

    /** Client connection url. */
    static public String clientConnectionUrl =
        "mms://" + receiverMsisdn + ":" + appId;

    /** Client connection url for static midlet. */
    static public String clientConnectionUrlStatic =
        "mms://" + receiverMsisdn + ":" + appIdStatic;

    /** Sender address upon message reception. */
    //static public String senderAddress = "mms://" + receiverMsisdn;

    /** Message used in push tests. */
    //static private MultipartMessage testMessage = null;

    /**
     * Initializes URLs basing on parameters in JAD file.
     */
    synchronized static void initUrls(MIDlet midlet, Form form)
    {
        try
        {
            senderMsisdn = getAppProp(midlet, "Wma20-Sender-Msisdn");
            receiverMsisdn = getAppProp(midlet, "Wma20-Receiver-Msisdn");
            serverConnectionUrl = "mms://:" + appId;
            serverConnectionUrlStatic = "mms://:" + appIdStatic;
            clientConnectionUrl = "mms://" + receiverMsisdn + ":" + appId;
            clientConnectionUrlStatic = "mms://" + receiverMsisdn + ":" + appIdStatic;
            //senderAddress = "mms://" + receiverMsisdn;
            wmaDebug("Sender MSISDN: " + senderMsisdn);
            wmaDebug("Receiver MSISDN: " + receiverMsisdn);
        }
        catch (Exception e)
        {
            form.append("Init failed: " + e.toString() + "\n");
            wmaDebug("Init failed: " + e.toString());
        }
    }

    /**
     * Returns application property with given name.
     */
    static String getAppProp(MIDlet midlet, String name) throws Exception
    {
        String result = null;
        try
        {
            result = midlet.getAppProperty(name);
        }
        catch (Exception e)
        {
            throw new Exception(
                "Could not get app property " + name + ": " + e.toString());
        }
        return result;
    }

    /**
     * Creates a MultipartMessage used for push testing.
     */
    static public MultipartMessage getTestMessage(
        MessageConnection connection, String address)
    {
        //if (testMessage == null) {
        MultipartMessage testMessage = null;
        try
        {
            testMessage = WmaPushUtils.getDummyMultipartMessage(
                              connection, 200, address, "Test MMS");
        }
        catch (Exception e)
        {
        }
        //}
        return testMessage;
    }

    /**
     * Checks if given messages have the same content.
     */
    static public boolean messagesEqual(
        MultipartMessage sentMsg, MultipartMessage receivedMsg,
        String from, String to)
    {
        String msgStr1 = msgToString(sentMsg, false);
        String msgStr2 = msgToString(receivedMsg, false);
        if (!msgStr1.equalsIgnoreCase(msgStr2))
        {
            wmaDebug("Received invalid message:\n" + msgStr2
                     + "\nexpected:\n" + msgStr1);
            return false;
        }
        if (!sentMsg.getAddress().equalsIgnoreCase(from))
        {
            wmaDebug("Address mismatch in sent message, got: " + sentMsg.getAddress()
                     + ", expected: " + from);
            return false;
        }
        if (!receivedMsg.getAddress().equalsIgnoreCase(to))
        {
            wmaDebug("Address mismatch in received message, got: " + receivedMsg.getAddress()
                     + ", expected: " + to);
            return false;
        }
        wmaDebug("Received valid message");
        return true;
    }

    /**
     * Returns information from the given message as a string.
     */
    static public String msgToString(MultipartMessage msg)
    {
        return msgToString(msg, true);
    }

    /**
     * Creates a dummy MultipartMessage.
     */
    static private MultipartMessage getDummyMultipartMessage(
        MessageConnection conn, int size, String address, String subject)
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
        msg.addMessagePart(getDummyMessagePart(size, "1"));
        msg.addMessagePart(getDummyMessagePart(size*2, "2"));
        msg.addMessagePart(getDummyMessagePart(size, "3"));
        return msg;
    }

    /**
     * Returns MessagePart of given size. MIME type is text/plain,
     * encoding is iso-8859-1.
     */
    static private MessagePart getDummyMessagePart(int size, String contentId)
    throws SizeExceededException, UnsupportedEncodingException
    {
        return getDummyMessagePart(size, contentId, null);
    }

    /**
     * Returns MessagePart of given size. MIME type is text/plain,
     * encoding is iso-8859-1.
     */
    static private MessagePart getDummyMessagePart(
        int size, String contentId, String contentLocation)
    throws SizeExceededException, UnsupportedEncodingException
    {
        return new MessagePart(
                   getContentString(size).toString().getBytes("iso-8859-1"),
                   "text/plain", contentId, contentLocation, "iso-8859-1");
    }

    /**
     * Returns information from the given message as a string.
     */
    static private String msgToString(MultipartMessage msg, boolean allHeaders)
    {
        if (msg == null)
        {
            return null;
        }

        StringBuffer result = new StringBuffer();
        if (allHeaders)
        {
            result.append("getAddress: " + msg.getAddress()).append("\n");
            result.append(addressesToString(msg, "from"));
            result.append(addressesToString(msg, "to"));
            result.append(addressesToString(msg, "cc"));
            result.append(addressesToString(msg, "bcc"));
        }
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
    static private String addressesToString(MultipartMessage msg, String addrType)
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
    static private String msgPartToString(MessagePart msgPart)
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
        result.append("base64 encoded content: ").append(
            Base64.encode(msgPart.getContent())).append("\n");
        return result.toString();
    }

    /**
     * Returns a string of given size.
     */
    static private String getContentString(int size)
    {
        StringBuffer contents = new StringBuffer(size);
        for (int i = 0; i < size; i++)
        {
            contents.append(i%10);
        }
        return contents.toString();
    }

    // Helper for logging.
    static private OutputStream out = null;
    synchronized static void wmaDebug(String msg)
    {
        if (out == null)
        {
            try
            {
                String path = System.getProperty("fileconn.dir.private");
                FileConnection filecon =
                    (FileConnection)Connector.open(path + "wma20mtpush.txt");
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
                String outMsg = (new java.util.Date()) + ": " + msg + "\n";
                out.write(outMsg.getBytes());
                out.flush();
            }
            catch (Exception e)
            {
            }
        }
    }
}
