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
 * TEST CASE SPECIFICATION<br>
 *
 * Code complexity rate (of the tested module) if measured:<br>
 *
 * The purpose of the module test:<br>
 *
 * To test MessageConnection methods.<br>
 *
 * Test case descriptions and inputs:<br>
 *
 * 1 Closed connection tests<br>
 * 1.1 Calling newMessage(String) on closed connection => OK<br>
 * 1.2 Calling newMessage(String, String) on closed connection => OK<br>
 * 1.3 Calling receive on closed connection => IOException<br>
 * 1.4 Calling receive on client connection => IOException<br>
 * 1.5 Calling send on closed connection => IOException<br>
 * 1.6 Calling numberOfSegments on closed connection => OK<br>
 * 1.7 Calling setMessageListener on closed connection  => IOException<br>
 * 1.8 Calling setMessageListener on client connection  => IOException<br>
 * 1.9 Calling close on closed connection => OK<br>
 *
 * 2 Default newMessage tests<br>
 * 2.1 newMessage with null parameter => NullPointerException<br>
 * 2.2 newMessage with invalid message type => IllegalArgumentException<br>
 * 2.3 check default state for multipart using client without address<br>
 * 2.4 check default state for multipart using server without address<br>
 * 2.5 check default state for binary using client without address<br>
 * 2.6 check default state for binary using server without address<br>
 * 2.7 check default state for text using client without address<br>
 * 2.8 check default state for text using server without address<br>
 * 2.9 check default state for multipart using client with address<br>
 * 2.10 check default state for multipart using server with address<br>
 * 2.11 check default state for binary using client with address<br>
 * 2.12 check default state for binary using server with address<br>
 * 2.13 check default state for text using client with address<br>
 * 2.14 check default state for text using server with address<br>
 *
 * 3 MessageTests - TextMessage<br>
 * 3.1 Creating a text message<br>
 * 3.2 Creating a text message with address<br>
 * 3.3 getPayloadText with a message with no payload should return null<br>
 * 3.4 setPayloadText/getPayloadText with valid string (should be ok)<br>
 * 3.5 setPayloadText/getPayloadText with zero length string (should be ok)<br>
 * 3.6 setPayloadText/getPayloadText with null parameter (should be ok)<br>
 * 3.7 Calling through the base - Message methods from TextMessage<br>
 * 3.7.1 get/setAddress - null address<br>
 * 3.7.2 get/setAddress - valid address<br>
 * 3.7.3 get/setAddress - accept invalid address<br>
 * 3.7.4 getTimestamp<br>
 * 3.7.5 getTimestamp on sent message<br>
 *
 * 4 MessageTests - BinaryMessage<br>
 * 4.1 Creating a binary message<br>
 * 4.2 Creating a binary message with address<br>
 * 4.3 getPayloadData with a message with no payload should return null<br>
 * 4.4 setPayloadData/getPayloadData with valid parameter (should be ok)<br>
 * 4.5 setPayloadData/getPayloadData with zero length array (should be ok)<br>
 * 4.6 setPayloadData/getPayloadData with null parameter (should be ok)<br>
 * 4.7 setPayloadData and then change array contents - changes should affect payload<br>
 * 4.8 Calling through the base - Message methods from BinaryMessage<br>
 * 4.8.1 get/setAddress - null address<br>
 * 4.8.2 get/setAddress - valid address<br>
 * 4.8.3 get/setAddress - accept invalid address<br>
 * 4.8.4 getTimestamp<br>
 * 4.8.5 getTimestamp on sent message<br>
 *
 * 5 MessageTests - MultiPartMessage<br>
 * 5.1 addMessagePart with valid parameter => OK<br>
 * 5.2 addMessagePart with invalid parameter (conflicting contentId) => IllegalArgumentException<br>
 * 5.3 addMessagePart with invalid parameter (too large content) => SizeExceededException<br>
 * 5.4 addMessagePart with null parameter => NullPointerException<br>
 * 5.5 addAddress with valid parameters => OK<br>
 * 5.6 addAddress with null parameter => IllegalArgumentException<br>
 * 5.7 addAddress with invalid parameter => IllegalArgumentException<br>
 * 5.8 getAddresses when more than one address exist<br>
 * 5.9 getAddresses for unknown type => returns null<br>
 * 5.10 removeAddress with valid parameters => OK<br>
 * 5.11 removeAddress with invalid parameters => IllegalArgumentException<br>
 * 5.12 removeAddresses => OK<br>
 * 5.13 set/getHeader for delivery time and priority headers<br>
 * 5.14 set/getHeader for subject, to from, cc, bcc headers => IllegalArgumentException<br>
 * 5.15 getHeader for unknown header => IllegalArgumentException<br>
 * 5.16 set/getSubject with valid value => OK<br>
 * 5.17 set/getSubject with null => OK<br>
 * 5.18 getMessagePart with valid contentId => OK<br>
 * 5.19 getMessagePart with null parameter => NullPointerException<br>
 * 5.20 getStartContentId when startMessagePart is set => OK<br>
 * 5.21 getStartContentId when startMessagePart is not set => null<br>
 * 5.22 setStartContentId with valid contentId => OK<br>
 * 5.23 setStartContentId with not matching contentId => IllegalArgumentException<br>
 * 5.24 removeMessagePart with matching messagePart parameter => OK, true<br>
 * 5.25 removeMessagePart with non-matching messagePart parameter => OK, false<br>
 * 5.26 removeMessagePart with null parameter => NullPointerException<br>
 * 5.27 removeMessagePartId with matching contentId parameter => OK, true<br>
 * 5.28 removeMessagePartId with non-matching contentId parameter => OK, false<br>
 * 5.28 removeMessagePartId with null parameter => NullPointerException<br>
 * 5.30 removeMessagePartLocation with matching contentLocation parameter => OK, true<br>
 * 5.31 removeMessagePartLocation with non-matching contentLocation parameter => OK, false<br>
 * 5.32 removeMessagePartLocation with null parameter => NullPointerException<br>
 * 5.33 Calling through the base - Message methods from MultiPartMessage<br>
 * 5.33.1 get/setAddress - null address<br>
 * 5.33.2 get/setAddress - valid address<br>
 * 5.33.3 get/setAddress - invalid address => IllegalArgumentException<br>
 * 5.33.4 getTimestamp from not sent message => null<br>
 * 5.33.5 getTimestamp on sent message => Date<br>
 * 5.34 addAddress with invalid applicationId parameter => IllegalArgumentException<br>
 *
 * 6 Text message numberOfSegments tests<br>
 * 6.1 Segmentation tests for gsm7 (message to user)<br>
 * 6.2 Segmentation tests for ucs-2 (message to user)<br>
 * 6.3 Segmentation tests for binary (message to user)<br>
 * 6.4 Segmentation tests for gsm7, port (message to application)<br>
 * 6.5 Segmentation tests for ucs-2, port (message to application)<br>
 * 6.6 Segmentation tests for binary, port (message to application)<br>
 *
 * Used stubs and description:<br>
 *
 * Setting up the environment:<br>
 *
 * How to run the test:<br>
 *
 * Is it tested with WINSCW and/or HW:<br>
 *
 * Outputs and expected results:<br>
 *
 * Branch coverage percentage (explanation if it is under 70%):<br>
 *
 * In case some of the tests cannot be run, an explanation should be given:<br>
 *
 * Included test files:<br>
 *
 * Build number (in which the tested module is integrated):<br>
 *
 * Note whether the test case belongs to the MRT set:<br>
 *
 **/

package com.nokia.mj.test.wma;

import j2meunit.framework.*;

import java.io.IOException;
import java.util.Date;
import java.util.Vector;

import javax.microedition.io.Connector;
import javax.wireless.messaging.Message;
import javax.wireless.messaging.MessageConnection;
import javax.wireless.messaging.MessageListener;
import javax.wireless.messaging.MessagePart;
import javax.wireless.messaging.MultipartMessage;
import javax.wireless.messaging.SizeExceededException;

public class TestMessageConnection extends WmaTestBase
{


    public TestMessageConnection()
    {
    }

    public TestMessageConnection(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public Test suite()
    {
        TestSuite aSuite = new TestSuite();


        aSuite.addTest(new TestMessageConnection("newMsgOnClosedConnection", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).newMsgOnClosedConnection();
            }
        }));


        aSuite.addTest(new TestMessageConnection("newMsg2OnClosedConnection", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).newMsg2OnClosedConnection();
            }
        }));


        aSuite.addTest(new TestMessageConnection("receiveOnClosedConnection", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).receiveOnClosedConnection();
            }
        }));


        aSuite.addTest(new TestMessageConnection("receiveOnClientConnection", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).receiveOnClientConnection();
            }
        }));


        aSuite.addTest(new TestMessageConnection("sendOnClosedConnection", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).sendOnClosedConnection();
            }
        }));


        aSuite.addTest(new TestMessageConnection("nrOfSegsOnClosedConnection", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).nrOfSegsOnClosedConnection();
            }
        }));


        aSuite.addTest(new TestMessageConnection("setListenerOnClosedConnection", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).setListenerOnClosedConnection();
            }
        }));


        aSuite.addTest(new TestMessageConnection("setListenerOnClientConnection", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).setListenerOnClientConnection();
            }
        }));


        aSuite.addTest(new TestMessageConnection("closeOnClosedConnection", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).closeOnClosedConnection();
            }
        }));


        aSuite.addTest(new TestMessageConnection("newMessageInvalidParams", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).newMessageInvalidParams();
            }
        }));


        aSuite.addTest(new TestMessageConnection("defaultNewMessage", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).defaultNewMessage();
            }
        }));


        aSuite.addTest(new TestMessageConnection("addMessagePart", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).addMessagePart();
            }
        }));


        aSuite.addTest(new TestMessageConnection("addAddress", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).addAddress();
            }
        }));


        aSuite.addTest(new TestMessageConnection("removeAddress", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).removeAddress();
            }
        }));


        aSuite.addTest(new TestMessageConnection("getSetHeader", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).getSetHeader();
            }
        }));


        aSuite.addTest(new TestMessageConnection("getStartContentId", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).getStartContentId();
            }
        }));

        aSuite.addTest(new TestMessageConnection("removeMessagePart", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).removeMessagePart();
            }
        }));


        aSuite.addTest(new TestMessageConnection("removeMessagePartId", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).removeMessagePartId();
            }
        }));


        aSuite.addTest(new TestMessageConnection("removeMessagePartLocation", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).removeMessagePartLocation();
            }
        }));


        aSuite.addTest(new TestMessageConnection("getSetAddress", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).getSetAddress();
            }
        }));


        aSuite.addTest(new TestMessageConnection("getTimestamp", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).getTimestamp();
            }
        }));


        aSuite.addTest(new TestMessageConnection("addAddressWithInvalidAppId", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMessageConnection) tc).addAddressWithInvalidAppId();
            }
        }));

        return aSuite;
    }


    /**
     * 1.1 Calling newMessage(String) on closed connection => OK<br>
     */
    public void newMsgOnClosedConnection()
    {
        // Test preparations.
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection(getMsisdnAddress(0, true) + ":test.appid");
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create new message on closed connection.
        try
        {
            Message msg = conn.newMessage(MessageConnection.MULTIPART_MESSAGE);
            if (msg == null)
            {
                assertTrue("newMessage returned null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue(e.toString(),false);
        }
        assertTrue("",true);
    }

    /**
     * 1.2 Calling newMessage(String, String) on closed connection => OK<br>
     */
    public void newMsg2OnClosedConnection()
    {
        // Test preparations.
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection("mms://:test.appid");
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create new message on closed connection.
        try
        {
            Message msg = conn.newMessage(
                              MessageConnection.MULTIPART_MESSAGE, getMsisdnAddress(1, true));
            if (msg == null)
            {
                assertTrue("newMessage returned null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue(e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 1.3 Calling receive on closed connection => IOException<br>
     */
    public void receiveOnClosedConnection()
    {


        // Test preparations.
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection("mms://:test.appid");
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Call receive on closed connection.
        try
        {
            Message msg = conn.receive();
            assertTrue("receive on closed connection did not throw IOException",false);
        }
        catch (IOException ioe)
        {
            // OK
        }
        catch (Exception e)
        {
            assertTrue(e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 1.4 Calling receive on client connection => IOException<br>
     */
    public void receiveOnClientConnection()
    {


        // Test preparations.
        MessageConnection conn = null;
        try
        {
            conn = getConnection(getMsisdnAddress(0, true) + ":test.appid");
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Call receive on client connection.
        try
        {
            Message msg = conn.receive();
            assertTrue("receive on client connection did not throw IOException",false);
        }
        catch (IOException ioe)
        {
            // OK
        }
        catch (Exception e)
        {
            assertTrue(e.toString(),false);
        }

        // Cleanup.
        try
        {
            // Close the client connection.
            conn.close();
        }
        catch (Exception e)
        {
        }

        assertTrue("",true);
    }

    /**
     * 1.5 Calling send on closed connection => IOException<br>
     */
    public void sendOnClosedConnection()
    {


        // Test preparations.
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection(getMsisdnAddress(0, true) + ":test.appid");
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Call send on closed connection.
        try
        {
            Message msg = conn.newMessage(MessageConnection.MULTIPART_MESSAGE);
            // Try to send a message through a closed connection.
            conn.send(msg);
            assertTrue("send through closed connection did not throw IOException",false);
        }
        catch (IOException ioe)
        {
            // OK
        }
        catch (Exception e)
        {
            assertTrue(e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 1.6 Calling numberOfSegments on closed connection => OK<br>
     */
    public void nrOfSegsOnClosedConnection()
    {


        // Test preparations.
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection(getMsisdnAddress(0, true) + ":test.appid");
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create message.
        Message msg = null;
        try
        {
            msg = getDummyMultipartMessage(
                      conn, getEmailAddress(0, true), "Test message");
        }
        catch (Exception e)
        {
            assertTrue("Message creation failed: " + e.toString(),false);
        }

        // Call numberOfSegments on closed connection.
        try
        {
            int nrOfSegs = conn.numberOfSegments(msg);
            if (nrOfSegs == 1)
            {
                // Our implementation will always return 1 segment
                // for MultipartMessages.
                // OK
            }
            else
            {
                assertTrue("numberOfSegments returned incorrect result: "
                           + nrOfSegs + ", expected 1",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("numberOfSegments failed: " + e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 1.7 Calling setMessageListener on closed connection  => IOException<br>
     */
    public void setListenerOnClosedConnection()
    {


        // Test preparations.
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection("mms://:test.appid");
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Set listener on closed connection.
        try
        {
            conn.setMessageListener(new MessageListener()
            {
                public void notifyIncomingMessage(MessageConnection conn)
                {
                    // Do nothing.
                }
            });
            assertTrue("Calling setMessageListener on closed connection "
                       + "did not throw IOException",false);
        }
        catch (IOException ioe)
        {
            // OK
        }
        catch (Exception e)
        {
            assertTrue(e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 1.8 Calling setMessageListener on client connection  => IOException<br>
     */
    public void setListenerOnClientConnection()
    {


        // Test preparations.
        MessageConnection conn = null;
        try
        {
            conn = getConnection(getMsisdnAddress(0, true) + ":test.appid");
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Set listener on client connection.
        try
        {
            conn.setMessageListener(new MessageListener()
            {
                public void notifyIncomingMessage(MessageConnection conn)
                {
                    // Do nothing.
                }
            });
            assertTrue("Calling setMessageListener on client connection "
                       + "did not throw IOException",false);
        }
        catch (IOException ioe)
        {
            // OK
        }
        catch (Exception e)
        {
            assertTrue(e.toString(),false);
        }

        // Cleanup.
        try
        {
            // Close the client connection.
            conn.close();
        }
        catch (Exception e)
        {
        }

        assertTrue("",true);
    }

    /**
     * 1.9 Calling close on closed connection => OK<br>
     */
    public void closeOnClosedConnection()
    {

        // Test preparations.
        MessageConnection conn = null, conn2 = null;
        try
        {
            conn = getClosedConnection(getMsisdnAddress(0, true) + ":test.appid");
            conn2 = getClosedConnection("mms://:test.appid");
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Call close on closed client connection.
        try
        {
            conn.close();
        }
        catch (Exception e)
        {
            assertTrue("Client connection closing failed: " + e.toString(),false);
        }

        // Call close on closed server connection.
        try
        {
            conn2.close();
        }
        catch (Exception e)
        {
            assertTrue("Server connection closing failed: " + e.toString(),false);
        }

        assertTrue("",true);

    }

    /**
     * 2.1 newMessage with null parameter => NullPointerException<br>
     * 2.2 newMessage with invalid message type => IllegalArgumentException<br>
     */
    public void newMessageInvalidParams()
    {
        // Test preparations.
        String addr = getEmailAddress(0, true);
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection(getMsisdnAddress(0, true) + ":test.appid");
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Call newMessage(String) with null parameter.
        try
        {
            Message msg = conn.newMessage(null);
            assertTrue(
                "newMessage(null) did not throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("newMessage(null) => " + e.toString(),false);
        }

        // Call newMessage(String, String) with null parameter.
        try
        {
            Message msg = conn.newMessage(null, addr);
            assertTrue(
                "newMessage(null, addr) did not throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("newMessage(null, addr) => " + e.toString(),false);
        }

        // Call newMessage(String, String) with null parameter.
        try
        {
            Message msg = conn.newMessage(MessageConnection.MULTIPART_MESSAGE, null);
            // OPEN: Should this throw an exception?
            //assertTrue(
            //    "newMessage(MULTIPART_MESSAGE, null) did not throw NullPointerException");
        }
        catch (NullPointerException npe)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("newMessage(MULTIPART_MESSAGE, null) => " + e.toString(),false);
        }

        // Call newMessage(String) with invalid parameter.
        try
        {
            Message msg = conn.newMessage("foo");
            assertTrue(
                "newMessage(foo) did not throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("newMessage(foo) => " + e.toString(),false);
        }

        // Call newMessage(String, String) with invalid parameter.
        try
        {
            Message msg = conn.newMessage("foo", addr);
            assertTrue(
                "newMessage(foo, addr) did not throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("newMessage(foo, addr) => " + e.toString(),false);
        }

        // If we got this far test case was passed.
        assertTrue("",true);
    }

    /**
     * 2.3 check default state for multipart using client without address<br>
     * 2.4 check default state for multipart using server without address<br>
     * 2.9 check default state for multipart using client with address<br>
     * 2.10 check default state for multipart using server with address<br>
     */
    public void defaultNewMessage()
    {
        // Test preparations.
        String addr = getEmailAddress(0, true);
        String msgError = null;
        MessageConnection clientConn = null, serverConn = null;
        try
        {
            clientConn = getClosedConnection(getMsisdnAddress(0, true) + ":test.appid");
            serverConn = getClosedConnection("mms://:test.appid");
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Get default MultipartMessage and check that it contains valid fields.
        try
        {
            MultipartMessage msg = (MultipartMessage)clientConn.newMessage(
                                       MessageConnection.MULTIPART_MESSAGE);
            if ((msgError = validDefaultMessage(
                                msg, getMsisdnAddress(0, true) + ":test.appid"))
                    != null)
            {
                assertTrue(
                    "Message created without address from client connection "
                    + "does not contain valid default content:\n" + msgError,false);
            }
        }
        catch (Exception e)
        {
            assertTrue("clientConn.newMessage => " + e.toString(),false);
        }

        // Get default MultipartMessage and check that it contains valid fields.
        try
        {
            MultipartMessage msg = (MultipartMessage)serverConn.newMessage(
                                       MessageConnection.MULTIPART_MESSAGE);
            if ((msgError = validDefaultMessage(msg, null)) != null)
            {
                assertTrue(
                    "Message created without address from server connection "
                    + "does not contain valid default content:\n" + msgError,false);
            }
        }
        catch (Exception e)
        {
            assertTrue("serverConn.newMessage => " + e.toString(),false);
        }

        // Get default MultipartMessage and check that it contains valid fields.
        try
        {
            MultipartMessage msg = (MultipartMessage)clientConn.newMessage(
                                       MessageConnection.MULTIPART_MESSAGE, addr);
            if ((msgError = validDefaultMessage(msg, addr)) != null)
            {
                assertTrue(
                    "Message created with address from client connection "
                    + "does not contain valid default content:\n" + msgError,false);
            }
        }
        catch (Exception e)
        {
            assertTrue("clientConn.newMessage2 => " + e.toString(),false);
        }

        // Get default MultipartMessage and check that it contains valid fields.
        try
        {
            MultipartMessage msg = (MultipartMessage)serverConn.newMessage(
                                       MessageConnection.MULTIPART_MESSAGE, addr);
            if ((msgError = validDefaultMessage(msg, addr)) != null)
            {
                assertTrue(
                    "Message created with address from server connection "
                    + "does not contain valid default content:\n" + msgError,false);
            }
        }
        catch (Exception e)
        {
            assertTrue("serverConn.newMessage2 => " + e.toString(),false);
        }

        // If we got this far test case was passed.
        assertTrue("",true);
    }

    /**
     * 5.1 addMessagePart with valid parameter => OK<br>
     * 5.2 addMessagePart with invalid parameter (conflicting contentId) => IllegalArgumentException<br>
     * 5.3 addMessagePart with invalid parameter (too large content) => SizeExceededException<br>
     * 5.4 addMessagePart with null parameter => NullPointerException<br>
     */
    public void addMessagePart()
    {
        // Test preparations.
        String addr = getEmailAddress(0, true);
        String contentId = "1";
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection(getMsisdnAddress(0, true) + ":test.appid");
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create message.
        MultipartMessage msg = null;
        try
        {
            msg = (MultipartMessage)conn.newMessage(
                      MessageConnection.MULTIPART_MESSAGE);
            if (msg == null)
            {
                assertTrue("newMessage returned null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Message creation failed: " + e.toString(),false);
        }

        // Create messagePart.
        MessagePart msgPart = null;
        try
        {
            msgPart = getDummyMessagePart(512, contentId);
        }
        catch (Exception e)
        {
            assertTrue("MessagePart creation failed: " + e.toString(),false);
        }

        // Add messagePart.
        String msgInfo = null;
        try
        {
            if (msg.getMessageParts() == null)
            {
                msgInfo = "msg.getMessageParts()=null";
            }
            else
            {
                msgInfo = "msg.getMessageParts().length="
                          + msg.getMessageParts().length;
            }
            msgInfo += ", msgPart.getLength()=" + msgPart.getLength();
            msg.addMessagePart(msgPart);
            msgInfo += ", msgPart adding succeeded";
            MessagePart msgPart2 = msg.getMessagePart(contentId);
            if (msgPart != msgPart2)
            {
                assertTrue("getMessagePart returned wrong MessagePart",false);
            }
            MessagePart msgParts[] = msg.getMessageParts();
            if (msgParts == null)
            {
                assertTrue("getMessageParts returned null",false);
            }
            if (msgParts.length != 1)
            {
                assertTrue(
                    "getMessageParts returned wrong number of MessageParts: "
                    + msgParts.length + ", expected 1",false);
            }
            if (msgParts[0] != msgPart)
            {
                assertTrue("getMessageParts returned wrong MessagePart",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("MessagePart (" + msgInfo
                       + ") adding failed: " + e.toString(),false);
        }

        // Add a new messagePart with the same contentId.
        try
        {
            MessagePart msgPart2 = getDummyMessagePart(1024, contentId);
            msg.addMessagePart(msgPart);
            assertTrue("Adding MessagePart with existing contentId "
                       + "should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("Adding MessagePart with existing contentId failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Add messagePart with too large content.
        try
        {
            MessagePart msgPart2 = getDummyMessagePart(
                                       MAX_CONTENT_SIZE + 1, "2");
            msg.addMessagePart(msgPart2);
            assertTrue("Adding too large MessagePart "
                       + "should throw SizeExceededException",false);
        }
        catch (SizeExceededException see)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("Adding too large MessagePart failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Add messagePart with null parameter.
        try
        {
            msg.addMessagePart(null);
            assertTrue("Adding MessagePart with null parameter "
                       + "should throw NullPointerException",false);
        }
        catch (NullPointerException npe)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("Adding MessagePart with null parameter failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 5.5 addAddress with valid parameters => OK<br>
     * 5.6 addAddress with null parameter => IllegalArgumentException<br>
     * 5.7 addAddress with invalid parameter => IllegalArgumentException<br>
     * 5.8 getAddresses when more than one address exist<br>
     * 5.9 getAddresses for unknown type => IllegalArgumentException<br>
     */
    public void addAddress()
    {
        // Test preparations.
        String addr0 = getMsisdnAddress(0, true);
        String addr1 = getEmailAddress(0, true);
        String addr2 = getEmailAddress(1, true);
        String addr3 = getEmailAddress(2, true);
        String addr4 = getEmailAddress(3, true);
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection(addr0);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create message.
        MultipartMessage msg = null;
        try
        {
            msg = (MultipartMessage)conn.newMessage(
                      MessageConnection.MULTIPART_MESSAGE);
            if (msg == null)
            {
                assertTrue("newMessage returned null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Message creation failed: " + e.toString(),false);
        }

        // Add addresses.
        try
        {
            msg.setAddress(addr1);
            msg.addAddress("to", addr2);
            msg.addAddress("cc", addr3);
            msg.addAddress("bcc", addr4);

            String getAddr = msg.getAddress();
            if (getAddr == null)
            {
                assertTrue("getAddress should not return null",false);
            }
            if (!getAddr.equals(addr0))
            {
                assertTrue("getAddress returned wrong address, got "
                           + getAddr + ", expected " + addr0,false);
            }

            String addrs[] = msg.getAddresses("from");
            if (addrs != null)
            {
                assertTrue("getAddresses(from) should return null, got: "
                           + getAddressString("from", addrs),false);
            }

            addrs = msg.getAddresses("to");
            if (addrs == null)
            {
                assertTrue("getAddresses(to) should not return null",false);
            }
            if (addrs.length != 3)
            {
                assertTrue(
                    "getAddresses(to) returned wrong number of addresses: "
                    + addrs.length + ", expected 2",false);
            }
            if (!addr0.equals(addrs[0]) || !addr1.equals(addrs[1]) || !addr2.equals(addrs[2]))
            {
                assertTrue("getAddresses(to) returned wrong address, got "
                           + addrs[0] + ", expected " + addr0 + ", got "
                           + addrs[1] + ", expected " + addr1 + ", got "
                           + addrs[2] + ", expected " + addr2,false);
            }

            addrs = msg.getAddresses("cc");
            if (addrs == null)
            {
                assertTrue("getAddresses(cc) should not return null",false);
            }
            if (addrs.length != 1)
            {
                assertTrue(
                    "getAddresses(cc) returned wrong number of addresses: "
                    + addrs.length + ", expected 1",false);
            }
            if (!addr3.equals(addrs[0]))
            {
                assertTrue("getAddresses(cc) returned wrong address, got "
                           + addrs[0] + ", expected " + addr3,false);
            }

            addrs = msg.getAddresses("bcc");
            if (addrs == null)
            {
                assertTrue("getAddresses(bcc) should not return null",false);
            }
            if (addrs.length != 1)
            {
                assertTrue(
                    "getAddresses(bcc) returned wrong number of addresses: "
                    + addrs.length + ", expected 1",false);
            }
            if (!addr4.equals(addrs[0]))
            {
                assertTrue("getAddresses(bcc) returned wrong address, got "
                           + addrs[0] + ", expected " + addr4,false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Adding address failed: " + e.toString(),false);
        }


        // Get invalid address type.
        try
        {
            String addrs[] = msg.getAddresses("foo");
            assertTrue("getAddresses(foo) should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("Getting invalid adddres type failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Add invalid address type.
        try
        {
            msg.addAddress("foo", addr1);
            assertTrue("Adding invalid address type "
                       + "should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("Adding invalid adddres type failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Add invalid address.
        try
        {
            msg.addAddress("to", "mmmmss://358401234567");
            assertTrue("Adding invalid address "
                       + "should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("Adding invalid adddres failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        //IPv6 not supported
        try
        {
            msg.addAddress("to", "mms://1234:1234:1234:1234:1234:1234:1234:1234");
            assertTrue("Adding IPv6 address "
                       + "should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("Adding invalid adddres failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        //IPv4 not supported
        try
        {
            msg.addAddress("to", "mms://127.0.0.1");
            assertTrue("Adding IPv4 address "
                       + "should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("Adding invalid adddres failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        //alpha numerical short code not supported, but we have
        //to allow adding such addresses since TCK mandates it
        /*try {
            msg.addAddress("to", "mms://alpha");
            assertTrue("Adding alpha-numerical short-code address "
                + "should throw IllegalArgumentException",false);
        } catch (IllegalArgumentException iae) {
            // passed
            iae.printStackTrace();
        } catch (Exception e) {
            assertTrue("Adding invalid adddres failed "
                + "with wrong exception: " + e.toString(),false);
        }*/

        //MSISDN with length bigger than 256 not supported
        try
        {
            msg.addAddress("to", "mms://" + getContentString(257));
            assertTrue("MSISDN with length bigger than 256 "
                       + "should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("Adding invalid adddres failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        //numerical short code supported
        try
        {
            msg.addAddress("to", "mms://1234");
        }
        catch (Exception e)
        {
            assertTrue("Adding invalid adddres failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Add address with null parameter.
        try
        {
            msg.addAddress(null, addr1);
            assertTrue("addAddress(null, addr) "
                       + "should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("addAddress(null, addr) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Add address with null parameter.
        try
        {
            msg.addAddress("to", null);
            assertTrue("addAddress(to, null) "
                       + "should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("addAddress(to, null) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 5.10 removeAddress with valid parameters => OK<br>
     * 5.11 removeAddress with invalid parameters => IllegalArgumentException<br>
     * 5.12 removeAddresses => OK<br>
     */
    public void removeAddress()
    {
        // Test preparations.
        String addr0 = getMsisdnAddress(0, true);
        String addr1 = getEmailAddress(0, true);
        String addr2 = getEmailAddress(1, true);
        String addr3 = getEmailAddress(2, true);
        String addr4 = getEmailAddress(3, true);
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection(addr0);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create message.
        MultipartMessage msg = null;
        try
        {
            msg = (MultipartMessage)conn.newMessage(
                      MessageConnection.MULTIPART_MESSAGE);
            if (msg == null)
            {
                assertTrue("newMessage returned null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Message creation failed: " + e.toString(),false);
        }

        // Add addresses.
        try
        {
            msg.setAddress(addr1);
            msg.addAddress("to", addr2);
            msg.addAddress("cc", addr3);
            msg.addAddress("bcc", addr4);
        }
        catch (Exception e)
        {
            assertTrue("Adding address failed: " + e.toString(),false);
        }


        // Remove address with valid parameters.
        try
        {
            boolean removed = msg.removeAddress("bcc", addr4);
            if (!removed)
            {
                assertTrue("Removing existing address should succeed",false);
            }
            String addrs[] = msg.getAddresses("bcc");
            if (addrs != null && addrs.length > 0)
            {
                assertTrue("Address still exists after its removal",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Removing existing adddres failed "
                       + "with exception: " + e.toString(),false);
        }

        // Remove address with invalid parameters.
        try
        {
            boolean removed = msg.removeAddress("to", "nomatch@domain.com");
            if (removed)
            {
                assertTrue("Removing nonexisting address should not succeed",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Removing nonexisting adddres failed "
                       + "with exception: " + e.toString(),false);
        }

        // Remove address of unknown type.
        try
        {
            boolean removed = msg.removeAddress("foo", "nomatch@domain.com");
            assertTrue("Removing address of unknown address type "
                       + "should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("Removing address of uknown adddress type failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Remove address with null parameter.
        try
        {
            boolean removed = msg.removeAddress(null, addr1);
            assertTrue("removeAddress(null, addr) "
                       + "should throw NullPointerException",false);
        }
        catch (NullPointerException npe)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("removeAddress(null, addr) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Remove addresses with valid parameters.
        try
        {
            msg.removeAddresses("to");
            String addrs[] = msg.getAddresses("to");
            if (addrs != null && addrs.length > 0)
            {
                assertTrue("Addresses still exist after their removal",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Removing adddreses failed "
                       + "with exception: " + e.toString(),false);
        }

        // Remove addresses of unknown type.
        try
        {
            msg.removeAddresses("foo");
            assertTrue("Removing addresses of unknown address type "
                       + "should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("Removing addresses of uknown adddress type failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Remove addresses with null parameter.
        try
        {
            msg.removeAddresses(null);
            assertTrue("removeAddresses(null) "
                       + "should throw NullPointerException",false);
        }
        catch (NullPointerException npe)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("removeAddresses(null) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 5.13 set/getHeader for delivery time and priority headers<br>
     * 5.14 set/getHeader for subject, to from, cc, bcc headers => IllegalArgumentException<br>
     * 5.15 getHeader for unknown header => IllegalArgumentException<br>
     * 5.16 set/getSubject with valid value => OK<br>
     * 5.17 set/getSubject with null => OK<br>
     */
    public void getSetHeader()
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, true) + ":test.appid";
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection(addr);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create message.
        MultipartMessage msg = null;
        try
        {
            msg = (MultipartMessage)conn.newMessage(
                      MessageConnection.MULTIPART_MESSAGE);
            if (msg == null)
            {
                assertTrue("newMessage returned null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Message creation failed: " + e.toString(),false);
        }

        // Set valid header fields.
        try
        {
            msg.setHeader("X-Mms-Delivery-Time", "1");
            msg.setHeader("X-Mms-Priority", "High");
        }
        catch (Exception e)
        {
            assertTrue("Setting header failed: " + e.toString(),false);
        }

        // Set valid header field with illegal value.
        try
        {
            msg.setHeader("X-Mms-Priority", "Foo");
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("setHeader(X-MMS-Priority, Foo) failed with "
                       + "wrong exception: " + e.toString(),false);
        }

        // Set header with illegal header field.
        try
        {
            msg.setHeader("dummyHeader", "dummyValue");
            assertTrue("setHeader(dummyHeader, dummyValue) "
                       + "should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("setHeader(dummyHeader, dummyValue) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Set header with illgeal X-Mms-To header field.
        try
        {
            msg.setHeader("X-Mms-To", getEmailAddress(0, true));
            assertTrue("setHeader(X-Mms-To, ...) "
                       + "should throw SecurityException",false);
        }
        catch (SecurityException se)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("setHeader(X-Mms-To, ...) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Set header with illegal subject header field.
        try
        {
            msg.setHeader("subject", "Dummy mail subject");
            assertTrue("setHeader(subject, ...) "
                       + "should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("setHeader(subject, ...) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Set header with null parameter.
        try
        {
            msg.setHeader(null, "dummyValue");
            assertTrue("setHeader(null, dummyValue) "
                       + "should throw NullPointerException",false);
        }
        catch (NullPointerException npe)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("setHeader(null, dummyValue) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Get valid header fields.
        try
        {
            String header = msg.getHeader("X-Mms-Delivery-Time");
            if (!"1".equals(header))
            {
                assertTrue(
                    "Getting delivery time returned wrong value, got "
                    + header + ", expected 1",false);
            }
            header = msg.getHeader("X-Mms-Priority");
            if (!"High".equals(header))
            {
                assertTrue(
                    "Getting priority returned wrong value, got "
                    + header + ", expected High",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Getting header failed: " + e.toString(),false);
        }

        // Get header with illegal header field.
        try
        {
            String header = msg.getHeader("foo");
            assertTrue("getHeader(foo) "
                       + "should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("getHeader(foo) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Get header with null parameter.
        try
        {
            String header = msg.getHeader(null);
            assertTrue("getHeader(null) "
                       + "should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("getHeader(null) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 5.16 set/getSubject with valid value => OK<br>
     * 5.17 set/getSubject with null => OK<br>
     */
    public void getSetSubject()
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, true) + ":test.appid";
        String subject = "Dummy mail subject";
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection(addr);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create message.
        MultipartMessage msg = null;
        try
        {
            msg = (MultipartMessage)conn.newMessage(
                      MessageConnection.MULTIPART_MESSAGE);
            if (msg == null)
            {
                assertTrue("newMessage returned null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Message creation failed: " + e.toString(),false);
        }

        // Set subject.
        try
        {
            msg.setSubject(subject);
        }
        catch (Exception e)
        {
            assertTrue("Setting subject failed: " + e.toString(),false);
        }

        // Get subject.
        try
        {
            if (!subject.equals(msg.getSubject()))
            {
                assertTrue(
                    "Getting subject returned incorrect subject: "
                    + msg.getSubject(),false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Getting subject failed: " + e.toString(),false);
        }

        // Set subject to null.
        try
        {
            msg.setSubject(null);
        }
        catch (Exception e)
        {
            assertTrue("Setting subject to null failed: " + e.toString(),false);
        }

        // Get subject.
        try
        {
            if (msg.getSubject() != null)
            {
                assertTrue(
                    "Getting subject returned incorrect subject: "
                    + msg.getSubject(),false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Getting subject failed: " + e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 5.18 getMessagePart with valid contentId => OK<br>
     * 5.19 getMessagePart with null parameter => NullPointerException<br>
     * 5.20 getStartContentId when startMessagePart is set => OK<br>
     * 5.21 getStartContentId when startMessagePart is not set => null<br>
     * 5.22 setStartContentId with valid contentId => OK<br>
     * 5.23 setStartContentId with not matching contentId => IllegalArgumentException<br>
     */
    public void getStartContentId()
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, true) + ":test.appid";
        String contentId = "1";
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection(addr);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create message.
        MultipartMessage msg = null;
        try
        {
            msg = (MultipartMessage)conn.newMessage(
                      MessageConnection.MULTIPART_MESSAGE);
            if (msg == null)
            {
                assertTrue("newMessage returned null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Message creation failed: " + e.toString(),false);
        }

        // Create MessagePart and add it to the message.
        MessagePart msgPart = null;
        try
        {
            msgPart = getDummyMessagePart(1024, contentId);
            msg.addMessagePart(msgPart);
        }
        catch (Exception e)
        {
            assertTrue("MessagePart creation failed: " + e.toString(),false);
        }

        // Get MessagePart.
        try
        {
            MessagePart msgPart2 = msg.getMessagePart(contentId);
            if (msgPart != msgPart2)
            {
                assertTrue(
                    "Got incorrect MessagePart: " + msgPartToString(msgPart2)
                    + ", expected: " + msgPartToString(msgPart),false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Getting MessagePart failed: " + e.toString(),false);
        }

        // Get MessagePart with invalid contentId.
        try
        {
            MessagePart msgPart2 = msg.getMessagePart("nomatch");
            if (msgPart2 != null)
            {
                assertTrue("getMessagePart(nomatch) should return null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("getMessagePart(nomatch) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Get MessagePart with null parameter.
        try
        {
            MessagePart msgPart2 = msg.getMessagePart(null);
            assertTrue("getMessagePart(null) "
                       + "should throw NullPointerException",false);
        }
        catch (NullPointerException npe)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("getMessagePart(null) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Get startContentId when startMessagePart is not set.
        try
        {
            String startCid = msg.getStartContentId();
            if (startCid != null)
            {
                assertTrue("getStartContentId() should return null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("getStartContentId() failed "
                       + "with exception: " + e.toString(),false);
        }

        // Set start content id with not matching content id.
        try
        {
            msg.setStartContentId("nomatch");
            assertTrue("setStartContentId(nomatch) "
                       + "should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("setStartContentId(nomatch) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Set start content id with matching content id.
        try
        {
            msg.setStartContentId(contentId);
        }
        catch (Exception e)
        {
            assertTrue("setStartContentId(contentId) failed "
                       + "with exception: " + e.toString(),false);
        }

        // Get startContentId when startMessagePart is set.
        try
        {
            String startCid = msg.getStartContentId();
            if (startCid == null)
            {
                assertTrue("getStartContentId() should not return null",false);
            }
            if (!startCid.equals(contentId))
            {
                assertTrue(
                    "getStartContentId() did not match to expected, got "
                    + startCid + ", expected " + contentId,false);
            }
        }
        catch (Exception e)
        {
            assertTrue("getStartContentId() failed "
                       + "with exception: " + e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 5.24 removeMessagePart with matching messagePart parameter => OK, true<br>
     * 5.25 removeMessagePart with non-matching messagePart parameter => OK, false<br>
     * 5.26 removeMessagePart with null parameter => NullPointerException<br>
     */
    public void removeMessagePart()
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, true) + ":test.appid";
        String contentId = "1";
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection(addr);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create message.
        MultipartMessage msg = null;
        try
        {
            msg = (MultipartMessage)conn.newMessage(
                      MessageConnection.MULTIPART_MESSAGE);
            if (msg == null)
            {
                assertTrue("newMessage returned null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Message creation failed: " + e.toString(),false);
        }

        // Create MessagePart and add it to the message.
        MessagePart msgPart = null;
        try
        {
            msgPart = getDummyMessagePart(1024, contentId);
            msg.addMessagePart(msgPart);
        }
        catch (Exception e)
        {
            assertTrue("MessagePart creation failed: " + e.toString(),false);
        }

        // Remove MessagePart with not matching parameter.
        try
        {
            boolean removed = msg.removeMessagePart(getDummyMessagePart(1024, "2"));
            if (removed)
            {
                assertTrue("removeMessagePart(nomatch) should return false",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("removeMessagePart(nomatch) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Remove MessagePart with null parameter.
        try
        {
            boolean removed = msg.removeMessagePart(null);
            assertTrue("removeMessagePart(null) "
                       + "should throw NullPointerException",false);
        }
        catch (NullPointerException npe)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("removeMessagePart(null) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Remove MessagePart with matching parameter.
        try
        {
            boolean removed = msg.removeMessagePart(msgPart);
            if (!removed)
            {
                assertTrue("removeMessagePart(msgPart) should return true",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("removeMessagePart(msgPart) failed "
                       + "with exception: " + e.toString(),false);
        }

        // Try to re-remove MessagePart with matching parameter.
        try
        {
            boolean removed = msg.removeMessagePart(msgPart);
            if (removed)
            {
                assertTrue("Re-calling removeMessagePart(msgPart) should return false",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Recalling removeMessagePart(msgPart) failed "
                       + "with exception: " + e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 5.27 removeMessagePartId with matching contentId parameter => OK, true<br>
     * 5.28 removeMessagePartId with non-matching contentId parameter => OK, false<br>
     * 5.28 removeMessagePartId with null parameter => NullPointerException<br>
     * 5.30 removeMessagePartLocation with matching contentLocation parameter => OK, true<br>
     * 5.31 removeMessagePartLocation with non-matching contentLocation parameter => OK, false<br>
     * 5.32 removeMessagePartLocation with null parameter => NullPointerException<br>
     */
    public void removeMessagePartId()
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, true) + ":test.appid";
        String contentId = "1";
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection(addr);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create message.
        MultipartMessage msg = null;
        try
        {
            msg = (MultipartMessage)conn.newMessage(
                      MessageConnection.MULTIPART_MESSAGE);
            if (msg == null)
            {
                assertTrue("newMessage returned null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Message creation failed: " + e.toString(),false);
        }

        // Create MessagePart and add it to the message.
        MessagePart msgPart = null;
        try
        {
            msgPart = getDummyMessagePart(1024, contentId);
            msg.addMessagePart(msgPart);
        }
        catch (Exception e)
        {
            assertTrue("MessagePart creation failed: " + e.toString(),false);
        }

        // Remove MessagePart with not matching parameter.
        try
        {
            boolean removed = msg.removeMessagePartId("2");
            if (removed)
            {
                assertTrue("removeMessagePartId(nomatch) should return false",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("removeMessagePartId(nomatch) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Remove MessagePart with null parameter.
        try
        {
            boolean removed = msg.removeMessagePartId(null);
            assertTrue("removeMessagePartId(null) "
                       + "should throw NullPointertException",false);
        }
        catch (NullPointerException npe)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("removeMessagePartId(null) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Remove MessagePart with matching parameter.
        try
        {
            boolean removed = msg.removeMessagePartId(contentId);
            if (!removed)
            {
                assertTrue("removeMessagePartId(contentId) should return true",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("removeMessagePartId(contentId) failed "
                       + "with exception: " + e.toString(),false);
        }

        // Try to re-remove MessagePart with matching parameter.
        try
        {
            boolean removed = msg.removeMessagePartId(contentId);
            if (removed)
            {
                assertTrue("Re-calling removeMessagePartId(contentId) should return false",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Recalling removeMessagePartId(contentId) failed "
                       + "with exception: " + e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 5.30 removeMessagePartLocation with matching contentLocation parameter => OK, true<br>
     * 5.31 removeMessagePartLocation with non-matching contentLocation parameter => OK, false<br>
     * 5.32 removeMessagePartLocation with null parameter => NullPointerException<br>
     */
    public void removeMessagePartLocation()
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, true) + ":test.appid";
        String contentId = "1";
        String contentLoc = "contentLocation";
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection(addr);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create message.
        MultipartMessage msg = null;
        try
        {
            msg = (MultipartMessage)conn.newMessage(
                      MessageConnection.MULTIPART_MESSAGE);
            if (msg == null)
            {
                assertTrue("newMessage returned null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Message creation failed: " + e.toString(),false);
        }

        // Create MessagePart and add it to the message.
        MessagePart msgPart = null;
        try
        {
            msgPart = getDummyMessagePart(1024, contentId, contentLoc, "iso-8859-1");
            msg.addMessagePart(msgPart);
        }
        catch (Exception e)
        {
            assertTrue("MessagePart creation failed: " + e.toString(),false);
        }

        // Remove MessagePart with not matching parameter.
        try
        {
            boolean removed = msg.removeMessagePartLocation("nomatch");
            if (removed)
            {
                assertTrue("removeMessagePartLocation(nomatch) "
                           + "should return false",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("removeMessagePartLocation(nomatch) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Remove MessagePart with null parameter.
        try
        {
            boolean removed = msg.removeMessagePartLocation(null);
            assertTrue("removeMessagePartLocation(null) "
                       + "should throw NullPointerException",false);
        }
        catch (NullPointerException npe)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("removeMessagePartLocation(null) failed "
                       + "with wrong exception: " + e.toString(),false);
        }

        // Remove MessagePart with matching parameter.
        try
        {
            boolean removed = msg.removeMessagePartLocation(contentLoc);
            if (!removed)
            {
                assertTrue("removeMessagePartLocation(contentLoc) should return true",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("removeMessagePartLocation(contentLoc) failed "
                       + "with exception: " + e.toString(),false);
        }

        // Try to re-remove MessagePart with matching parameter.
        try
        {
            boolean removed = msg.removeMessagePartLocation(contentLoc);
            if (removed)
            {
                assertTrue("Re-calling removeMessagePartLocation(contentLoc) should return false",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Recalling removeMessagePartLocation(contentLoc) failed "
                       + "with exception: " + e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 5.33.1 get/setAddress - null address<br>
     * 5.33.2 get/setAddress - valid address<br>
     * 5.33.3 get/setAddress - invalid address => IllegalArgumentException<br>
     */
    public void getSetAddress()
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, true) + ":test.appid";
        String addr2 = getMsisdnAddress(1, true) + ":test.appid";
        String addr3 = getMsisdnAddress(2, true) + ":test.appid";
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection(addr);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create message.
        MultipartMessage msg = null;
        try
        {
            msg = (MultipartMessage)conn.newMessage(
                      MessageConnection.MULTIPART_MESSAGE);
            if (msg == null)
            {
                assertTrue("newMessage returned null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Message creation failed: " + e.toString(),false);
        }

        // Get address.
        try
        {
            String getAddr = msg.getAddress();
            if (getAddr == null)
            {
                assertTrue("getAddress should not return null",false);
            }
            if (!getAddr.equals(addr))
            {
                assertTrue("getAddress returned wrong address, got "
                           + getAddr + ", expected " + addr,false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Getting address failed: " + e.toString(),false);
        }

        // Set address.
        try
        {
            msg.setAddress(addr2);
            String getAddr = msg.getAddress();
            if (getAddr == null)
            {
                assertTrue("getAddress after setting should not return null",false);
            }
            if (!getAddr.equals(addr))
            {
                assertTrue("getAddress after setting returned wrong address, got "
                           + getAddr + ", expected " + addr,false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Setting address failed: " + e.toString(),false);
        }

        // Set address to null.
        try
        {
            msg.addAddress("cc", addr3);
            msg.setAddress(null);
            // Setting address to null shouldn't do anything.
            String getAddr = msg.getAddress();
            if (getAddr == null)
            {
                assertTrue("getAddress after setting to null should not return null",false);
            }
            if (!getAddr.equals(addr))
            {
                assertTrue("getAddress after setting to null returned wrong address, got "
                           + getAddr + ", expected " + addr,false);
            }

            String addrs[] = msg.getAddresses("to");
            if (addrs == null)
            {
                assertTrue("getAddresses(to) should not return null",false);
            }
            if (addrs.length != 2)
            {
                assertTrue(
                    "getAddresses(to) returned wrong number of addresses: "
                    + addrs.length + ", expected 2",false);
            }

            addrs = msg.getAddresses("cc");
            if (addrs == null)
            {
                assertTrue("getAddresses(cc) should not return null",false);
            }
            if (addrs.length != 1)
            {
                assertTrue(
                    "getAddresses(cc) returned wrong number of addresses: "
                    + addrs.length + ", expected 1",false);
            }
            if (!addr3.equals(addrs[0]))
            {
                assertTrue("getAddresses(cc) returned wrong address, got "
                           + addrs[0] + ", expected " + addr3,false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Setting null address failed: " + e.toString(),false);
        }

        // Set invalid address.
        try
        {
            msg.setAddress("invalid.address");
            assertTrue("setAddress(invalid.address) "
                       + "should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("setAddress(invalid.address) failed with wrong exception: "
                       + e.toString(),false);
        }
        try
        {
            msg.setAddress("mms://");
            assertTrue("setAddress(mms://) "
                       + "should throw IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("setAddress(invalid.address) failed with wrong exception: "
                       + e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 5.33.4 getTimestamp from not sent message => null<br>
     * 5.33.5 getTimestamp on sent message => Date<br>
     */
    public void getTimestamp()
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, true) + ":test.appid";
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection(addr);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create message.
        MultipartMessage msg = null;
        try
        {
            msg = (MultipartMessage)conn.newMessage(
                      MessageConnection.MULTIPART_MESSAGE);
            if (msg == null)
            {
                assertTrue("newMessage returned null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Message creation failed: " + e.toString(),false);
        }

        // Get timestamp.
        try
        {
            Date date = msg.getTimestamp();
            if (date != null)
            {
                assertTrue("getTimestamp should return null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Getting timestamp failed: " + e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 5.34 addAddress with invalid applicationId parameter => IllegalArgumentException<br>
     */
    public void addAddressWithInvalidAppId()
    {
        // Test preparations.
        MessageConnection conn = null;
        try
        {
            conn = getClosedConnection(getMsisdnAddress(0, true) + ":test.appid");
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create message.
        MultipartMessage msg = null;
        try
        {
            msg = (MultipartMessage)conn.newMessage(
                      MessageConnection.MULTIPART_MESSAGE);
            if (msg == null)
            {
                assertTrue("newMessage returned null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Message creation failed: " + e.toString(),false);
        }

        // Add address.
        try
        {
            msg.addAddress("bcc", getMsisdnAddress(1, true) + ":test.appid");
        }
        catch (Exception e)
        {
            assertTrue("Adding address failed: " + e.toString(),false);
        }

        // Add address without appId, default appId will be used.
        try
        {
            msg.addAddress("to", getMsisdnAddress(2, true));
        }
        catch (Exception e)
        {
            assertTrue("Adding address without appId failed with exception: "
                       + e.toString(),false);
        }

        try
        {
            msg.addAddress("cc", getMsisdnAddress(3, true) + ":test.appid.wrong");
            assertTrue("Adding address with wrong appId should not succeed",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("Adding address with wrong appId failed with wrong exception: "
                       + e.toString(),false);
        }

        try
        {
            // Close the client connection.
            conn.close();
        }
        catch (Exception e)
        {
        }

        try
        {
            // Open connection without appId.
            conn = getClosedConnection(getMsisdnAddress(0, true));
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create message.
        try
        {
            msg = (MultipartMessage)conn.newMessage(
                      MessageConnection.MULTIPART_MESSAGE);
            if (msg == null)
            {
                assertTrue("newMessage without appId returned null",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Message creation without appId failed: " + e.toString(),false);
        }

        // Add addresses.
        try
        {
            msg.addAddress("to", getMsisdnAddress(1, true));
        }
        catch (Exception e)
        {
            assertTrue("Adding address without appId failed: " + e.toString(),false);
        }

        try
        {
            msg.addAddress("bcc", getMsisdnAddress(2, true) + ":test.appid");
        }
        catch (Exception e)
        {
            assertTrue("Adding address with appId failed with exception: "
                       + e.toString(),false);
        }

        try
        {
            msg.addAddress("cc", getMsisdnAddress(3, true) + ":test.appid.wrong");
            assertTrue("Adding address with wrong appId should not succeed",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("Adding address with wrong appId failed with wrong exception: "
                       + e.toString(),false);
        }

        try
        {
            // Close the client connection.
            conn.close();
        }
        catch (Exception e)
        {
        }

        assertTrue("",true);
    }

    // BEGIN - Private helper methods for this test suite.

    /**
     * Checks if given message contains correct default values.
     * Returns null if message was valid, error description otherwise.
     */
    private String validDefaultMessage(MultipartMessage msg, String toAddr)
    {
        if (msg == null)
        {
            return "message is null";
        }

        // Message created without address from client connection:
        // to: toAddr X-Mms-Priority: Normal
        // Message created without address from server connection
        // X-Mms-Priority: Normal
        // Message created with address from client connection
        // to: toAddr X-Mms-Priority: Normal
        // Message created with address from server connection
        // to: toAddr X-Mms-Priority: Normal
        // Note: priority header removed from default message 25.8.2005

        String msgString = msgToString(msg).toLowerCase();
        String expectedMsg =
            "getAddress: " + toAddr + "\n" +
            (toAddr == null? "": "to: " + toAddr + "\n");
        //"x-mms-priority: normal\n";
        expectedMsg = expectedMsg.toLowerCase();
        if (!msgString.equals(expectedMsg))
        {
            return "invalid default message:\n" + msgString
                   + "\nexpected:\n" + expectedMsg;
        }

        return null;
    }

    // END - Private helper methods for this test suite.
}
