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


public class WmaTestBase extends TestCase
{

    /** Maximum content size for a MultipartMessage. */
    public static final int MAX_CONTENT_SIZE = WmaTestUtils.MAX_CONTENT_SIZE;

    private static final String[] MSISDN_ADDRESS =
    {
        "+5550000",
        "+5550001",
        "+5550002",
        "+5550003",
        "+5550004",
        "+5550005",
        "+5550006",
        "+5550007",
        "+5550008",
        "+5550009",
        "+5550010",
        "+5550011"
    };

    private static final String[] EMAIL_ADDRESS=
    {
        "user@domain.com",
        "user1@domain.com",
        "user2@domain.com",
        "user3@domain.com",
        "user4@domain.com"
    };

    private static final int RECEIVE_TIMEOUT = 400;

    private static final int WAIT_TIME = 60;
    /** Test case name. */
    protected String testCaseName = null;

    /** Test case id. */
    protected String testCaseId = null;

    public WmaTestBase()
    {
    }

    public WmaTestBase(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public Test suite()
    {
        return null;
        // This method needs to be overriden in each subclass
        // which has the actual test cases.
    }

    /**
     * Opens a connection to given URL.
     */
    protected MessageConnection getConnection(String url)
    throws WmaException
    {
        return WmaTestUtils.getConnection(url);
    }

    /**
     * Opens a connection to given URL and closes it.
     */
    protected MessageConnection getClosedConnection(String url)
    throws WmaException
    {
        // Open connection.
        MessageConnection conn = getConnection(url);
        try
        {
            // Close connection.
            conn.close();
        }
        catch (Exception e)
        {
            throw new WmaException("Connection closing failed (url \"" + url + "\"): " + e);
        }
        return conn;
    }

    /**
     * Returns a string that is too large for MultipartMessage contents.
     */
    protected String getTooLargeContent()
    {
        return WmaTestUtils.getTooLargeContent();
    }

    /**
     * Returns a multipart message that is too large to be sent.
     */
    protected MultipartMessage getTooLargeMessage(
        MessageConnection conn, String subject)
    throws SizeExceededException, UnsupportedEncodingException
    {
        return WmaTestUtils.getTooLargeMessage(conn,subject);
    }

    /**
     * Creates a dummy MultipartMessage.
     */
    protected MultipartMessage getDummyMultipartMessage(
        MessageConnection conn, String address, String subject)
    throws SizeExceededException, UnsupportedEncodingException
    {
        return WmaTestUtils.getDummyMultipartMessage(conn, address, subject);
    }

    /**
     * Returns MessagePart of given size. MIME type is text/plain,
     * encoding is iso-8859-1.
     */
    protected MessagePart getDummyMessagePart(int size, String contentId)
    throws SizeExceededException, UnsupportedEncodingException
    {
        return WmaTestUtils.getDummyMessagePart(size, contentId, null, "iso-8859-1");
    }

    /**
     * Returns MessagePart of given size. MIME type is text/plain,
     * encoding is iso-8859-1.
     */
    protected MessagePart getDummyMessagePart(
        int size, String contentId, String contentLocation, String enc)
    throws SizeExceededException, UnsupportedEncodingException
    {
        return WmaTestUtils.getDummyMessagePart(size, contentId, contentLocation, enc);
    }

    /**
     * Checks if given messages have the same content.
     */
    protected boolean messagesEqual(
        MultipartMessage sentMsg, MultipartMessage receivedMsg,
        String from, String to)
    {
        return WmaTestUtils.messagesEqual(sentMsg, receivedMsg, from, to);
    }

    /**
     * Returns information from the given message as a string.
     */
    protected String msgToString(MultipartMessage msg)
    {
        return msgToString(msg, true);
    }

    /**
     * Returns information from the given message as a string.
     */
    protected String msgToString(MultipartMessage msg, boolean allHeaders)
    {
        return WmaTestUtils.msgToString(msg, allHeaders);
    }

    /**
     * Returns requested addresses from the given message as a string.
     */
    protected String addressesToString(MultipartMessage msg, String addrType)
    {
        return WmaTestUtils.addressesToString(msg, addrType);
    }

    /**
     * Returns information from the given MessagePart as a string.
     */
    protected String msgPartToString(MessagePart msgPart)
    {
        return WmaTestUtils.msgPartToString(msgPart);
    }

    /**
     * Returns an InputStream for reading given string.
     */
    protected InputStream getInputStream(String str)
    {
        if (str == null)
        {
            return null;
        }
        return new ByteArrayInputStream(str.getBytes());
    }

    /**
     * Returns addresses as a single string.
     */
    protected String getAddressString(String type, String addrs[])
    {
        return WmaTestUtils.getAddressString(type, addrs);
    }

    /**
     * Returns a string of given size.
     */
    public String getContentString(int size)
    {
        return WmaTestUtils.getContentString(size);
    }

    /**
     * Puts the thread into sleep for specified time (in seconds).
     */
    public void wait(int delay)
    {
        WmaTestUtils.wait(delay);
    }

    /**
     * Returns application property with given name.
     */
    private String getAppMsisdnProp(int n) throws WmaException
    {
        String result = null;
        try
        {
            //result = SpedeRunner.getInstance().getAppProperty(name);
            result = MSISDN_ADDRESS[n];
        }
        catch (Exception e)
        {
            throw new WmaException("Could not get app property " + n
                                   + ": " + e.toString());
        }
        return result;
    }

    /**
     * Returns application property with given name as an int.
     */
    private String getAppEmailProp(int n) throws WmaException
    {
        return EMAIL_ADDRESS[n];
    }


    /**
     * Returns nth email address to be used in module tests.
     */
    public String getEmailAddress(int n, boolean prefix)
    {
        String addr = null;
        String propName = "Wma20-Email-Address-" + n;
        try
        {
            addr = getAppEmailProp(n);
        }
        catch (Exception e)
        {
            wmaDebug("Exception while getting app property " + propName
                     + ": " + e.toString());
        }

        if (addr == null)
        {
            addr = "user" + n + "@domain.com";
            wmaDebug("Could not get email address " + n
                     + ", using default " + addr);
        }

        if (prefix)
        {
            addr = "mms://" + addr;
        }
        return addr;
    }

    /**
     * Returns nth msisdn address to be used in module tests.
     */
    public String getMsisdnAddress(int n, boolean prefix)
    {
        String addr = null;
        String propName = "Wma20-Msisdn-Address-" + n;
        try
        {
            addr = getAppMsisdnProp(n);
        }
        catch (Exception e)
        {
            wmaDebug("Exception while getting app property " + propName
                     + ": " + e.toString());
        }

        if (addr == null)
        {
            addr = "+" + (5550000 + n);
            wmaDebug("Could not get msisdn address " + n
                     + ", using default " + addr);
        }

        if (prefix)
        {
            addr = "mms://" + addr;
        }
        return addr;
    }

    /**
     * Returns receive timeout to be used in module tests.
     */
    public int getReceiveTimeout() throws WmaException
    {
        return RECEIVE_TIMEOUT;
    }

    /**
     * Returns notification wait time to be used in module tests.
     */
    public int getNotificationWaitTime() throws WmaException
    {
        return WAIT_TIME;
    }

    // Helper for logging.
    protected void wmaDebug(String msg)
    {
        WmaTestUtils.wmaDebug(msg, testCaseName);
    }

    /**
     * Receives a single message from given connection in a separate thread.
     */
    public MultipartMessage receiveMsg(MessageConnection conn)
    throws WmaException
    {
        return WmaTestUtils.receiveMsg(conn, getReceiveTimeout() * 1000);
    }
}
