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
 * TEST CASE SPECIFICATION
 *
 * Code complexity rate (of the tested module) if measured:
 *
 * The purpose of the module test:<br>
 *
 * Test message sending with MessageConnection.<br>
 *
 * Test case descriptions and inputs:<br>
 *
 * 1 Exception tests<br>
 * 1.1 Send null message => NullPointerException<br>
 * 1.2 Send too large message => IllegalArgumentException<br>
 * 1.3 Send wrong type message => IllegalArgumentException<br>
 * 1.4 Send message created with different MessageConnection => IllegalArgumentException<br>
 *
 * 2 sendMultipartAsClient tests<br>
 * 2.1 Send a multipart message without attachments using client connection,
 * receive using server connection, check headers<br>
 * 2.2 Send a multipart message with attachments using client connection,
 * receive using server connection, check headers and attachments<br>
 * 2.3 Send several multipart messages with attachments using two client connections,
 * receive using server connection<br>
 * 2.4 Send a multipart message with attachments to several recipients
 * using client connection, receive using server connection, check
 * headers and attachments<br>
 *
 * 3 sendMultipartAsServer tests<br>
 * 3.1 Send a multipart message without attachments using server connection,
 * receive using server connection, check headers<br>
 * 3.2 Send a multipart message with attachments using server connection,
 * receive using server connection, check headers and attachments<br>
 * 3.3 Send several multipart messages with attachments using two server connections,
 * receive using server connection<br>
 * 3.4 Send a multipart message with attachments to several recipients
 * using server connection, receive using server connection, check
 * headers and attachments<br>
 *
 * 4 sendBinaryAsClient tests<br>
 * N/A: not multipart message test.
 * 4.1 Send a binary message using client connection, receive using server connection<br>
 *
 * 5 sendBinaryAsServer tests<br>
 * N/A: not multipart message test.
 * 5.1 Send a binary message using server connection, receive using server connection<br>
 *
 * 6 sendTextAsClient tests<br>
 * N/A: not multipart message test.
 * 6.1 Send a text message using client connection, receive using server connection<br>
 *
 * 7 sendTextAsServer tests<br>
 * N/A: not multipart message test.
 * 7.1 Send a text message using server connection, receive using server connection<br>
 *
 * Used stubs and description:
 *
 * Setting up the environment:
 *
 * How to run the test:
 *
 * Is it tested with WINSCW and/or HW:
 *
 * Outputs and expected results:
 *
 * Branch coverage percentage (explanation if it is under 70%):
 *
 * In case some of the tests cannot be run, an explanation should be given:
 *
 * Included test files:
 *
 * Build number (in which the tested module is integrated):
 *
 * Note whether the test case belongs to the MRT set:
 *
 **/
package com.nokia.mj.test.wma;

import j2meunit.framework.*;

import javax.microedition.io.Connector;
import javax.microedition.io.ConnectionNotFoundException;
import javax.wireless.messaging.Message;
import javax.wireless.messaging.MessageConnection;
import javax.wireless.messaging.MessageListener;
import javax.wireless.messaging.MessagePart;
import javax.wireless.messaging.MultipartMessage;
import javax.wireless.messaging.SizeExceededException;
import javax.wireless.messaging.TextMessage;

public class TestSend extends WmaTestBase
{

    public TestSend()
    {
    }

    public TestSend(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public Test suite()
    {
        TestSuite aSuite = new TestSuite();


        aSuite.addTest(new TestSend("sendSimpleMultipart", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).sendSimpleMultipart(false);
            }
        }));


        aSuite.addTest(new TestSend("sendReceiveSimpleMultipart", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).sendSimpleMultipart(true);
            }
        }));


        aSuite.addTest(new TestSend("sendToShortAppID", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).sendToShortAppID();
            }
        }));


        aSuite.addTest(new TestSend("sendExceptions", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).sendExceptions();
            }
        }));

        // Send test cases

        aSuite.addTest(new TestSend("sendMultipartThroughClient", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).sendMultipart(false,false);
            }
        }));


        aSuite.addTest(new TestSend("sendMultipartsThroughClient", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).sendMultiparts(false,false);
            }
        }));


        aSuite.addTest(new TestSend("sendMultipartToSeveralRecipientsThroughClient", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).sendMultipartToSeveralRecipients(false,false);
            }
        }));


        aSuite.addTest(new TestSend("sendMultipartThroughServer", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).sendMultipart(true,false);
            }
        }));


        aSuite.addTest(new TestSend("sendMultipartsThroughServer", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).sendMultiparts(true,false);
            }
        }));


        aSuite.addTest(new TestSend("sendMultipartToSeveralRecipientsThroughServer", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).sendMultipartToSeveralRecipients(true,false);
            }
        }));

        // Send and receive test cases

        aSuite.addTest(new TestSend("sendReceiveMultipartThroughClient", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).sendMultipart(false,true);
            }
        }));


        aSuite.addTest(new TestSend("sendReceiveMultipartsThroughClient", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).sendMultiparts(false,true);
            }
        }));


        aSuite.addTest(new TestSend("sendReceiveMultipartToSeveralRecipientsThroughClient", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).sendMultipartToSeveralRecipients(false,true);
            }
        }));


        aSuite.addTest(new TestSend("sendReceiveMultipartThroughServer", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).sendMultipart(true,true);
            }
        }));


        aSuite.addTest(new TestSend("sendReceiveMultipartsThroughServer", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).sendMultiparts(true,true);
            }
        }));


        aSuite.addTest(new TestSend("sendReceiveMultipartToSeveralRecipientsThroughServer", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).sendMultipartToSeveralRecipients(true,true);
            }
        }));

        aSuite.addTest(new TestSend("testReply", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).testReply();
            }
        }));

        aSuite.addTest(new TestSend("testForward", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).testForward();
            }
        }));

        aSuite.addTest(new TestSend("testTimestamp", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSend) tc).testTimestamp();
            }
        }));

        return aSuite;
    }

    // Bogdan: counters that make connections different.
    static int simpleMultipart_Counter = 1;
    static int sendMultipart_Counter = 1;
    static int sendMultiparts_Counter = 1;
    static int sendMultipartToSeveralRec_Counter = 1;

    /**
     * 1 Exception tests<br>
     * 1.1 Send null message => NullPointerException<br>
     * 1.2 Send too large message => IllegalArgumentException<br>
     * 1.3 Send wrong type message => IllegalArgumentException<br>
     * 1.4 Send message created with different MessageConnection => IllegalArgumentException<br>
     */
    public void sendExceptions()
    {
        // Test preparations.
        String url = getMsisdnAddress(0, true) + ":test.appid";
        MessageConnection conn = null, conn2 = null;
        try
        {
            conn = getConnection(url);
            conn2 = getConnection(url);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Send null message.
        try
        {
            conn.send(null);
            assertTrue("Sending null message should fail with NullPointerException",false);
        }
        catch (NullPointerException ne)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("Sending null message failed with wrong exception: "
                       + e.toString(),false);
        }

        // Send too large message.
        /*
                // OPEN: How to test sending too large message? It is not possible
                // to create one because in message creation SizeExceededException
                // will be thrown.
                try {
                    MultipartMessage msg = getTooLargeMessage(conn, "Too large message");
                    conn.send(msg);
                    assertTrue("Sending too large message should fail with IllegalArgumentException");
                    // send method does not throw SizeExceededException
                } catch (IllegalArgumentException iae) {
                    // passed
                } catch (Exception e) {
                    assertTrue("Sending too large message failed with wrong exception: "
                        + e.toString());
                }
        */

        // Create text message.
        try
        {
            TextMessage textMsg = (TextMessage)conn.newMessage(MessageConnection.TEXT_MESSAGE);
            assertTrue("Creating text message should fail with IllegalArgumentException",false);
        }
        catch (IllegalArgumentException iae)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("Creating text message failed with wrong exception: "
                       + e.toString(),false);
        }

        // Send message created with different connection.
        try
        {
            MultipartMessage msg = (MultipartMessage)conn2.newMessage(
                                       MessageConnection.MULTIPART_MESSAGE);
            conn.send(msg);
        }
        catch (Exception e)
        {
            assertTrue("Sending message created with another connection "
                       + "should succeed but failed with exception: "
                       + e.toString(),false);
        }

        // Cleanup.
        try
        {
            // Close the client connection.
            conn.close();
        }
        catch (Exception e) {}
        try
        {
            // Close the client connection.
            conn2.close();
        }
        catch (Exception e) {}

        assertTrue("",true);
    }

    /**
     * 2 sendMultipartAsClient tests<br>
     * 2.1 Send a multipart message without attachments using client connection,
     * receive using server connection, check headers<br>
     * 2.2 Send a multipart message with attachments using client connection,
     * receive using server connection, check headers and attachments<br>
     * <p>
     * 3 sendMultipartAsServer tests<br>
     * 3.1 Send a multipart message without attachments using server connection,
     * receive using server connection, check headers<br>
     * 3.2 Send a multipart message with attachments using server connection,
     * receive using server connection, check headers and attachments<br>
     */
    public void sendMultipart(boolean useServerConnection, boolean receive)
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, false);
        System.out.println("msisdn address" + addr);
        // Bogdan: add counter that makes the appId unique
        String appId = "test.appid.multipart" + sendMultipart_Counter;
        sendMultipart_Counter++;
        String clientUrl = "mms://"+addr+":"+appId;
        String serverUrl = "mms://:"+appId;
        String toUrl = clientUrl;
        String fromUrl = clientUrl; // for S60
        //String fromUrl = "mms://"+addr; // for RI
        String subject = "Sample message";
        MultipartMessage msg1 = null, msg2 = null;
        MessageConnection clientConn = null, serverConn = null;
        try
        {
            try
            {
                if (useServerConnection)
                {
                    clientConn = getConnection(serverUrl+".client");
                    fromUrl = fromUrl + ".client";
                }
                else
                {
                    clientConn = getConnection(clientUrl);
                    fromUrl = "mms://"+addr;
                }
                if (receive)
                {
                    serverConn = getConnection(serverUrl);
                }
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }

            // Send a message without attachments.
            try
            {
                msg1 = (MultipartMessage)clientConn.newMessage(
                           MessageConnection.MULTIPART_MESSAGE);
                msg1.setAddress(toUrl);
                msg1.setSubject(subject);
                msg1.setHeader("X-Mms-Delivery-Time", "1");
                msg1.setHeader("X-Mms-Priority", "High");
                clientConn.send(msg1);
            }
            catch (Exception e)
            {
                assertTrue("Sending message failed with exception: "
                           + e.toString(),false);
            }

            if (receive)
            {
                // Receive the message in a separate thread.
                try
                {
                    msg2 = receiveMsg(serverConn);
                }
                catch (WmaException wmae)
                {
                    assertTrue(wmae.toString(),false);
                }
                // Compare sent and received messages.
                if (!messagesEqual(msg1, msg2, toUrl, fromUrl))
                {
                    assertTrue("Sent and received messages differ, sent:\n"
                               + msgToString(msg1) + "\nreceived:\n" + msgToString(msg2)
                               + "\nexpected sent getAddress: " + toUrl
                               + "\nexpected received getAddress: " + fromUrl,false);
                }
            }

            // Send a message with attachments.
            try
            {
                msg1 = (MultipartMessage)clientConn.newMessage(
                           MessageConnection.MULTIPART_MESSAGE);
                msg1.setAddress(toUrl);
                msg1.setSubject(subject);
                msg1.setHeader("X-Mms-Delivery-Time", "1000000");
                msg1.setHeader("X-Mms-Priority", "Low");
                msg1.addMessagePart(getDummyMessagePart(1024, "1", null, null)); // use null encoding
                msg1.addMessagePart(getDummyMessagePart(100, "2"));
                msg1.addMessagePart(getDummyMessagePart(5000, "3"));
                msg1.setStartContentId("1");
                clientConn.send(msg1);
            }
            catch (Exception e)
            {
                assertTrue("Sending message with attachments failed with exception: "
                           + e.toString(),false);
            }

            if (receive)
            {
                // Receive the message in a separate thread.
                try
                {
                    msg2 = receiveMsg(serverConn);
                }
                catch (WmaException wmae)
                {
                    assertTrue(wmae.toString(),false);
                }
                // Compare sent and received messages.
                if (!messagesEqual(msg1, msg2, toUrl, fromUrl))
                {
                    assertTrue("Sent and received messages differ, sent:\n"
                               + msgToString(msg1) + "\nreceived:\n" + msgToString(msg2)
                               + "\nexpected sent getAddress: " + toUrl
                               + "\nexpected received getAddress: " + fromUrl,false);
                }
            }

        }
        finally
        {
            // Cleanup.
            try
            {
                // Close the client connection.
                if (clientConn != null)
                {
                    clientConn.close();
                }
            }
            catch (Exception e) {}
            try
            {
                // Close the server connection.
                if (serverConn != null)
                {
                    serverConn.close();
                }
            }
            catch (Exception e) {}
        }

        assertTrue("",true);
    }

    /**
     * 2 sendMultipartAsClient tests<br>
     * 2.3 Send several multipart messages with attachments using two
     * client connections, receive using server connection<br>
     * <p>
     * 3 sendMultipartAsServer tests<br>
     * 3.3 Send several multipart messages with attachments using two server connections,
     * receive using server connection<br>
     */
    public void sendMultiparts(boolean useServerConnection, boolean receive)
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, false);
        // Bogdan: add counter that makes the appId unique
        String appId = "test.appid.multiparts" + sendMultiparts_Counter;
        sendMultiparts_Counter++;
        String clientUrl = "mms://"+addr+":"+appId;
        String serverUrl = "mms://:"+appId;
        String toUrl = clientUrl;
        String fromUrl = clientUrl;
        //String fromUrl = "mms://"+addr;
        String subject = "Sample message with attachments";
        String urlExt1 = "", urlExt2 = "";
        MultipartMessage msg1 = null, msg2 = null, msg3 = null;
        MultipartMessage rec1 = null, rec2 = null, rec3 = null;
        MessageConnection clientConn = null, clientConn2 = null, serverConn = null;
        try
        {
            try
            {
                if (useServerConnection)
                {
                    clientUrl = serverUrl;
                    // Bogdan: from address should not include the application id
                    //fromUrl = toUrl;
                    urlExt1 = ".client1";
                    urlExt2 = ".client2";
                    // Two server connections to different server urls.
                    clientConn = getConnection(clientUrl+urlExt1);
                    clientConn2 = getConnection(clientUrl+urlExt2);
                }
                else
                {
                    // Two client connections to the same url.
                    fromUrl = "mms://"+addr;
                    clientConn = getConnection(clientUrl);
                    clientConn2 = getConnection(clientUrl);
                }
                if (receive)
                {
                    serverConn = getConnection(serverUrl);
                }
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }

            // Create messages.
            try
            {
                msg1 = (MultipartMessage)clientConn.newMessage(
                           MessageConnection.MULTIPART_MESSAGE);
                // Bogdan: take urlExt1 and urlExt2 into use everywhere
                msg1.setAddress(toUrl + urlExt1);
                msg1.setSubject(subject);
                msg1.setHeader("X-Mms-Delivery-Time", "1");
                msg1.setHeader("X-Mms-Priority", "High");
                msg1.addMessagePart(getDummyMessagePart(1, "a"));
                msg1.addMessagePart(getDummyMessagePart(1024, "b"));
                msg1.addMessagePart(getDummyMessagePart(5000, "c"));
                msg1.setStartContentId("a");

                msg2 = (MultipartMessage)clientConn2.newMessage(
                           MessageConnection.MULTIPART_MESSAGE);
                // Bogdan: take urlExt1 and urlExt2 into use everywhere
                msg2.setAddress(toUrl + urlExt2);
                msg2.setSubject(subject+"2");
                msg2.setHeader("X-Mms-Delivery-Time", "500");
                msg2.setHeader("X-Mms-Priority", "Low");
                msg2.addMessagePart(getDummyMessagePart(10, "AAA"));
                msg2.addMessagePart(getDummyMessagePart(100, "BBB"));
                msg2.addMessagePart(getDummyMessagePart(1000, "CCC"));
                msg2.setStartContentId("AAA");

                msg3 = (MultipartMessage)clientConn.newMessage(
                           MessageConnection.MULTIPART_MESSAGE);
                // Bogdan: take urlExt1 and urlExt2 into use everywhere
                msg3.setAddress(toUrl + urlExt1);
                msg3.setSubject(subject+"3");
                msg3.setHeader("X-Mms-Priority", "Normal");
                msg3.addMessagePart(getDummyMessagePart(2000, "foo"));
                msg3.addMessagePart(getDummyMessagePart(2000, "bar"));
            }
            catch (Exception e)
            {
                assertTrue("Creating messages failed with exception: "
                           + e.toString(),false);
            }

            // Send messages.
            try
            {
                clientConn.send(msg1);
                clientConn2.send(msg2);
                clientConn.send(msg3);
            }
            catch (Exception e)
            {
                assertTrue("Sending messages failed with exception: "
                           + e.toString(),false);
            }

            if (receive)
            {
                // Receive the message in a separate thread.
                // OPEN: Receiving the first message fails with RI:
                // java.io.IOException: Bad fragmentation
                try
                {
                    if (useServerConnection)
                    {
                        rec1 = receiveMsg(clientConn);
                        rec2 = receiveMsg(clientConn2);
                        rec3 = receiveMsg(clientConn);
                    }
                    else
                    {
                        rec1 = receiveMsg(serverConn);
                        rec2 = receiveMsg(serverConn);
                        rec3 = receiveMsg(serverConn);
                    }
                }
                catch (WmaException wmae)
                {
                    assertTrue(wmae.toString(),false);
                }
                // Compare sent and received messages.
                // Bogdan: from address should not include the application id
                java.util.Vector sentMessages = new java.util.Vector();
                sentMessages.addElement(msg1);
                sentMessages.addElement(msg2);
                sentMessages.addElement(msg3);
                java.util.Vector recvMessages = new java.util.Vector();
                recvMessages.addElement(rec1);
                recvMessages.addElement(rec2);
                recvMessages.addElement(rec3);
                for (int i=0; i<sentMessages.size(); i++)
                {
                    MultipartMessage sentMessage = (MultipartMessage)sentMessages.elementAt(i);
                    boolean found = false;
                    for (int j=0; j<recvMessages.size(); j++)
                    {
                        MultipartMessage recvMessage = (MultipartMessage)recvMessages.elementAt(j);
                        if (messagesEqual(sentMessage, recvMessage, toUrl+urlExt1, fromUrl+urlExt1) ||
                                messagesEqual(sentMessage, recvMessage, toUrl+urlExt2, fromUrl+urlExt2))
                        {
                            recvMessages.removeElementAt(j);
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        assertTrue("Message sent but not received " + msgToString(sentMessage),false);
                    }
                }
            }

        }
        finally
        {
            // Cleanup.
            try
            {
                // Close the client connection.
                if (clientConn != null)
                {
                    clientConn.close();
                }
            }
            catch (Exception e) {}
            try
            {
                // Close another client connection.
                if (clientConn2 != null)
                {
                    clientConn2.close();
                }
            }
            catch (Exception e) {}
            try
            {
                // Close the server connection.
                if (serverConn != null)
                {
                    serverConn.close();
                }
            }
            catch (Exception e) {}
        }

        assertTrue("",true);
    }

    /**
     * 2 sendMultipartAsClient tests<br>
     * 2.4 Send a multipart message with attachments to several recipients
     * using client connection, receive using server connection, check
     * headers and attachments<br>
     * 3.4 Send a multipart message with attachments to several recipients
     * using server connection, receive using server connection, check
     * headers and attachments<br>
     */
    public void sendMultipartToSeveralRecipients(
        boolean useServerConnection, boolean receive)
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, false);
        // Bogdan: the new limit for appID is 32
        // String appId = ":test.appid.multipart.several.recipients";
        String appId = ":test.appid.several.rec" + sendMultipartToSeveralRec_Counter;
        sendMultipartToSeveralRec_Counter++;
        String clientUrl = "mms://"+addr+appId;
        String serverUrl = "mms://"+appId;
        String toUrl = clientUrl;
        String fromUrl = clientUrl; // for S60
        //String fromUrl = "mms://"+addr; // for RI
        String subject = "Sample message sent to multiple recipients";
        MultipartMessage msg1 = null, msg2 = null;
        MessageConnection clientConn = null, serverConn = null;
        try
        {
            try
            {
                if (useServerConnection)
                {
                    clientConn = getConnection(serverUrl+".client");
                    fromUrl = fromUrl + ".client";
                }
                else
                {
                    clientConn = getConnection(clientUrl);
                    fromUrl = "mms://"+addr;
                }
                if (receive)
                {
                    serverConn = getConnection(serverUrl);
                }
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }

            // Send a message without attachments.
            try
            {
                msg1 = (MultipartMessage)clientConn.newMessage(
                           MessageConnection.MULTIPART_MESSAGE);
                msg1.setAddress(toUrl);
                msg1.addAddress("to", getMsisdnAddress(1, true)+appId);
                msg1.addAddress("to", getMsisdnAddress(2, true)+appId);
                msg1.addAddress("to", getMsisdnAddress(3, true)+appId);
                msg1.addAddress("cc", getMsisdnAddress(4, true)+appId);
                msg1.addAddress("cc", getMsisdnAddress(5, true)+appId);
                msg1.addAddress("cc", getMsisdnAddress(6, true)+appId);
                msg1.addAddress("bcc", getMsisdnAddress(7, true)+appId);
                msg1.addAddress("bcc", getMsisdnAddress(8, true)+appId);
                msg1.addAddress("bcc", getMsisdnAddress(9, true)+appId);
                msg1.addAddress("bcc", getMsisdnAddress(10, true)+appId);
                msg1.addAddress("bcc", getMsisdnAddress(11, true)+appId);
                msg1.setSubject(subject);
                msg1.setHeader("X-Mms-Priority", "Normal");
                clientConn.send(msg1);
            }
            catch (Exception e)
            {
                assertTrue("Sending message failed with exception: "
                           + e.toString(),false);
            }

            if (receive)
            {
                // Receive the message in a separate thread.
                try
                {
                    msg2 = receiveMsg(serverConn);
                }
                catch (WmaException wmae)
                {
                    assertTrue(wmae.toString(),false);
                }
                // Compare sent and received messages.
                if (!messagesEqual(msg1, msg2, toUrl, fromUrl))
                {
                    assertTrue("Sent and received messages differ, sent:\n"
                               + msgToString(msg1) + "\nreceived:\n" + msgToString(msg2),false);
                }
            }

            // Send a message with attachments.
            try
            {
                msg1 = (MultipartMessage)clientConn.newMessage(
                           MessageConnection.MULTIPART_MESSAGE);
                msg1.setAddress(toUrl);
                msg1.addAddress("to", getMsisdnAddress(1, true)+appId);
                msg1.addAddress("to", getMsisdnAddress(2, true)+appId);
                msg1.addAddress("to", getMsisdnAddress(3, true)+appId);
                msg1.addAddress("cc", getMsisdnAddress(4, true)+appId);
                msg1.addAddress("cc", getMsisdnAddress(5, true)+appId);
                msg1.addAddress("cc", getMsisdnAddress(6, true)+appId);
                msg1.addAddress("bcc", getMsisdnAddress(7, true)+appId);
                msg1.addAddress("bcc", getMsisdnAddress(8, true)+appId);
                msg1.addAddress("bcc", getMsisdnAddress(9, true)+appId);
                msg1.addAddress("bcc", getMsisdnAddress(10, true)+appId);
                msg1.addAddress("bcc", getMsisdnAddress(11, true)+appId);
                msg1.setSubject(subject);
                msg1.setHeader("X-Mms-Priority", "Normal");
                msg1.addMessagePart(getDummyMessagePart(100*1024, "100K"));
                msg1.addMessagePart(getDummyMessagePart(20, "20"));
                msg1.addMessagePart(getDummyMessagePart(5000, "5000"));
                msg1.addMessagePart(getDummyMessagePart(1, "1"));
                msg1.setStartContentId("20");
                clientConn.send(msg1);
            }
            catch (Exception e)
            {
                assertTrue("Sending message with attachments failed with exception: "
                           + e.toString(),false);
            }

            if (receive)
            {
                // Receive the message in a separate thread.
                try
                {
                    msg2 = receiveMsg(serverConn);
                }
                catch (WmaException wmae)
                {
                    assertTrue(wmae.toString(),false);
                }
                // Compare sent and received messages.
                if (!messagesEqual(msg1, msg2, toUrl, fromUrl))
                {
                    assertTrue("Sent and received messages with attachment differ, sent:\n"
                               + msgToString(msg1) + "\nreceived:\n" + msgToString(msg2),false);
                }
            }

        }
        finally
        {
            // Cleanup.
            try
            {
                // Close the client connection.
                if (clientConn != null)
                {
                    clientConn.close();
                }
            }
            catch (Exception e) {}
            try
            {
                // Close the server connection.
                if (serverConn != null)
                {
                    serverConn.close();
                }
            }
            catch (Exception e) {}
        }

        assertTrue("",true);
    }

    /**
     * Sends a simple multipart message.
     */
    public void sendSimpleMultipart(boolean receive)
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, false);
        // Bogdan : add counter that makes the appId unique
        String appId = "test.appid.simplemultipart" + simpleMultipart_Counter;
        simpleMultipart_Counter++;
        String clientUrl = "mms://"+addr+":"+appId;
        String serverUrl = "mms://:"+appId;
        String toUrl = clientUrl;
        String fromUrl = clientUrl; // for S60
        //String fromUrl = "mms://"+addr; // for RI
        String subject = "Simple sample message";
        MultipartMessage msg1 = null, msg2 = null;
        MessageConnection clientConn = null, serverConn = null;
        try
        {
            try
            {
                clientConn = getConnection(clientUrl);
                fromUrl = "mms://"+addr;
                if (receive)
                {
                    serverConn = getConnection(serverUrl);
                    /*
                                        wmaDebug("got server connection");
                                        serverConn.setMessageListener(new MessageListener() {
                                            public void notifyIncomingMessage(MessageConnection conn) {
                                                // Receive the message in a separate thread.
                                                try {
                                                    wmaDebug("listener called");
                                                    MultipartMessage msg = receiveMsg(conn);
                                                    wmaDebug("received to listener: " + msgToString(msg));
                                                } catch (WmaException wmae) {
                                                    wmaDebug("exception in listener: " + wmae.toString());
                                                }
                                            }
                                        });
                                        wmaDebug("set listener");
                    */
                }
                /*
                            } catch (IOException ioe) {
                                assertTrue(ioe.toString());
                */
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }

            // Send a message with attachments.
            try
            {
                msg1 = (MultipartMessage)clientConn.newMessage(
                           MessageConnection.MULTIPART_MESSAGE);
                msg1.setAddress(toUrl);
                msg1.setSubject(subject);
                msg1.setHeader("X-Mms-Delivery-Time", "1000000");
                msg1.setHeader("X-Mms-Priority", "Low");
                msg1.addMessagePart(getDummyMessagePart(1024, "1","/res/path.gif\\file\\path/Dummy - !\"%&/()=?++|><*ContentLocation" , null)); // use null encoding
                msg1.addMessagePart(getDummyMessagePart(100, "2", null, "utf-8"));
                msg1.addMessagePart(getDummyMessagePart(100*1024, "3"));
                msg1.setStartContentId("1");
                clientConn.send(msg1);
            }
            catch (Exception e)
            {
                assertTrue("Sending message with attachments failed with exception: "
                           + e.toString(),false);
            }

            if (receive)
            {
                // Receive the message in a separate thread.
                try
                {
                    msg2 = receiveMsg(serverConn);
                }
                catch (WmaException wmae)
                {
                    assertTrue(wmae.toString(),false);
                }
                // Compare sent and received messages.
                if (!messagesEqual(msg1, msg2, toUrl, fromUrl))
                {
                    assertTrue("Sent and received messages differ, sent:\n"
                               + msgToString(msg1) + "\nreceived:\n" + msgToString(msg2),false);
                }
            }

        }
        finally
        {
            // Cleanup.
            try
            {
                // Close the client connection.
                if (clientConn != null)
                {
                    clientConn.close();
                }
            }
            catch (Exception e) {}
            try
            {
                // Close the server connection.
                if (serverConn != null)
                {
                    serverConn.close();
                }
            }
            catch (Exception e) {}
        }

        assertTrue("",true);
    }

    /**
     * Sends a simple multipart message to a "length 1" app ID.
     */
    public void sendToShortAppID()
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, false);
        String appId = "" + simpleMultipart_Counter;
        simpleMultipart_Counter++;
        String clientUrl = "mms://"+addr+":"+appId;
        String serverUrl = "mms://:"+appId;
        String toUrl = clientUrl;
        String fromUrl = clientUrl; // for S60
        String subject = "Simple sample message";
        MultipartMessage msg1 = null, msg2 = null;
        MessageConnection clientConn = null, serverConn = null;
        try
        {
            try
            {
                clientConn = getConnection(clientUrl);
                serverConn = getConnection(serverUrl);
                fromUrl = "mms://"+addr;
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }

            // Send a message with attachments.
            try
            {
                msg1 = (MultipartMessage)clientConn.newMessage(
                           MessageConnection.MULTIPART_MESSAGE);
                msg1.setAddress(toUrl);
                msg1.setSubject(subject);
                msg1.setHeader("X-Mms-Delivery-Time", "1000000");
                msg1.setHeader("X-Mms-Priority", "Low");
                msg1.addMessagePart(getDummyMessagePart(1024, "1", "DummyContentLocation", null)); // use null encoding
                msg1.addMessagePart(getDummyMessagePart(100, "2", null, "utf-8"));
                msg1.addMessagePart(getDummyMessagePart(100*1024, "3"));
                msg1.setStartContentId("1");
                clientConn.send(msg1);
            }
            catch (Exception e)
            {
                assertTrue("Sending message with attachments failed with exception: "
                           + e.toString(),false);
            }

            // Receive the message in a separate thread.
            try
            {
                msg2 = receiveMsg(serverConn);
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }
            // Compare sent and received messages.
            if (!messagesEqual(msg1, msg2, toUrl, fromUrl))
            {
                assertTrue("Sent and received messages differ, sent:\n"
                           + msgToString(msg1) + "\nreceived:\n" + msgToString(msg2),false);
            }

        }
        finally
        {
            // Cleanup.
            try
            {
                // Close the client connection.
                if (clientConn != null)
                {
                    clientConn.close();
                }
            }
            catch (Exception e) {}
            try
            {
                // Close the server connection.
                if (serverConn != null)
                {
                    serverConn.close();
                }
            }
            catch (Exception e) {}
        }

        assertTrue("",true);
    }

    /**
     * Tests the reply mechanism:
     * 1) It uses a server connection to send and receive
     * 2) It uses a client connection to send
     **/
    public void testReply()
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, false);
        String appId = "test.appid.reply";
        String clientUrl = "mms://"+addr+":"+appId;
        String serverUrl = "mms://:"+appId;
        String toUrl = clientUrl;
        String fromUrl = clientUrl; // for S60
        String subject = "Sample message";
        MultipartMessage msg1 = null, msg2 = null, msg3 = null, msg4 = null;
        MessageConnection sender = null, receiver = null;
        try
        {
            try
            {
                sender = getConnection(serverUrl+".serverSender");
                fromUrl = fromUrl + ".serverSender";
                receiver = getConnection(serverUrl);
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }

            // Send a message without attachments.
            try
            {
                msg1 = (MultipartMessage)sender.newMessage(
                           MessageConnection.MULTIPART_MESSAGE);
                msg1.setAddress(toUrl);
                msg1.setSubject(subject);
                msg1.setHeader("X-Mms-Delivery-Time", "1");
                msg1.setHeader("X-Mms-Priority", "High");
                sender.send(msg1);
            }
            catch (Exception e)
            {
                assertTrue("Sending message failed with exception: "
                           + e.toString(),false);
            }

            try
            {
                msg2 = receiveMsg(receiver);
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }
            // Compare sent and received messages.
            if (!messagesEqual(msg1, msg2, toUrl, fromUrl))
            {
                assertTrue("Sent and received messages differ, sent:\n"
                           + msgToString(msg1) + "\nreceived:\n" + msgToString(msg2)
                           + "\nexpected sent getAddress: " + toUrl
                           + "\nexpected received getAddress: " + fromUrl,false);
            }

            // now the receiver makes a reply and the initial sender should get the message
            try
            {
                receiver.send(msg2);
            }
            catch (Exception e)
            {
                assertTrue("Sending message failed with exception: "
                           + e.toString(),false);
            }
            fromUrl = clientUrl;
            try
            {
                msg3 = receiveMsg(sender);
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }
            // Compare sent and received messages.
            if (!messagesEqual(msg1, msg3, toUrl, fromUrl))
            {
                assertTrue("Sent and received messages differ, sent:\n"
                           + msgToString(msg1) + "\nreceived:\n" + msgToString(msg3)
                           + "\nexpected sent getAddress: " + toUrl
                           + "\nexpected received getAddress: " + fromUrl,false);
            }
            // the initial sends a reply
            try
            {
                sender.send(msg3);
            }
            catch (Exception e)
            {
                assertTrue("Sending message failed with exception: "
                           + e.toString(), false);
            }
            toUrl = clientUrl + ".serverSender";
            fromUrl = clientUrl + ".serverSender";
            try
            {
                msg4 = receiveMsg(receiver);
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }
            // Compare sent and received messages.
            if (!messagesEqual(msg3, msg4, toUrl, fromUrl))
            {
                assertTrue("Sent and received messages differ, sent:\n"
                           + msgToString(msg1) + "\nreceived:\n" + msgToString(msg3)
                           + "\nexpected sent getAddress: " + toUrl
                           + "\nexpected received getAddress: " + fromUrl, false);
            }

            // the sender is now a client type of connection
            try
            {
                sender = getConnection(clientUrl);
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }

            // Send a message without attachments.
            toUrl = clientUrl;
            fromUrl = "mms://"+addr;
            try
            {
                msg1 = (MultipartMessage)sender.newMessage(
                           MessageConnection.MULTIPART_MESSAGE);
                msg1.setAddress(toUrl);
                msg1.setSubject(subject);
                msg1.setHeader("X-Mms-Delivery-Time", "1");
                msg1.setHeader("X-Mms-Priority", "High");
                sender.send(msg1);
            }
            catch (Exception e)
            {
                assertTrue("Sending message failed with exception: "
                           + e.toString(),false);
            }

            try
            {
                msg2 = receiveMsg(receiver);
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }
            // Compare sent and received messages.
            if (!messagesEqual(msg1, msg2, toUrl, fromUrl))
            {
                assertTrue("Sent and received messages differ, sent:\n"
                           + msgToString(msg1) + "\nreceived:\n" + msgToString(msg2)
                           + "\nexpected sent getAddress: " + toUrl
                           + "\nexpected received getAddress: " + fromUrl,false);
            }

            // now the receiver tries to make a reply
            try
            {
                receiver.send(msg2);
                assertTrue("Reply to a non-reply type of message MUST not be allowed", false);
            }
            catch (IllegalArgumentException iae)
            {
                // this is ok
            }
            catch (Exception e)
            {
                assertTrue("Sending message failed with exception: "
                           + e.toString(),false);
            }

        }
        finally
        {
            // Cleanup.
            try
            {
                // Close the client connection.
                if (sender != null)
                {
                    sender.close();
                }
            }
            catch (Exception e) {}
            try
            {
                // Close the server connection.
                if (receiver != null)
                {
                    receiver.close();
                }
            }
            catch (Exception e) {}
        }

        assertTrue("",true);
    }

    /**
     * Tests the reply mechanism:
     * 1) It uses a server connection to send and receive
     * 2) It uses a client connection to send
     **/
    private void testForward()
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, false);
        String appId = "test.appid.forward";
        String clientUrl = "mms://"+addr+":"+appId;
        String serverUrl = "mms://:"+appId;
        String toUrl = clientUrl;
        String fromUrl = clientUrl; // for S60
        String subject = "Sample message";
        MultipartMessage msg1 = null, msg2 = null, msg3 = null, msg4 = null;
        MessageConnection sender = null, receiver = null, receiver2 = null;
        try
        {
            try
            {
                sender = getConnection(serverUrl+".serverSender");
                fromUrl = fromUrl + ".serverSender";
                receiver = getConnection(serverUrl);
                receiver2 = getConnection(serverUrl+".serverRec");
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }

            // Send a message without attachments.
            try
            {
                msg1 = (MultipartMessage)sender.newMessage(
                           MessageConnection.MULTIPART_MESSAGE);
                msg1.setAddress(toUrl);
                msg1.setSubject(subject);
                msg1.setHeader("X-Mms-Delivery-Time", "1");
                msg1.setHeader("X-Mms-Priority", "High");
                sender.send(msg1);
            }
            catch (Exception e)
            {
                assertTrue("Sending message failed with exception: "
                           + e.toString(), false);
            }

            try
            {
                msg2 = receiveMsg(receiver);
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }
            // Compare sent and received messages.
            if (!messagesEqual(msg1, msg2, toUrl, fromUrl))
            {
                assertTrue("Sent and received messages differ, sent:\n"
                           + msgToString(msg1) + "\nreceived:\n" + msgToString(msg2)
                           + "\nexpected sent getAddress: " + toUrl
                           + "\nexpected received getAddress: " + fromUrl, false);
            }

            // now the receiver makes a forward
            try
            {
                msg2.addAddress("to", clientUrl + ".serverRec");
                assertTrue("Adding an address with a different app ID should have failed", false);
            }
            catch (IllegalArgumentException e)
            {
                // that's ok
            }

            try
            {
                msg2.removeAddresses();
                msg2.setAddress(clientUrl + ".serverRec");
                receiver.send(msg2);
            }
            catch (Exception e)
            {
                assertTrue("Sending message failed with exception: "
                           + e.toString(), false);
            }

            fromUrl = clientUrl;
            try
            {
                msg3 = receiveMsg(receiver2);
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }
            // Compare sent and received messages.
            if (!messagesEqual(msg2, msg3, toUrl, fromUrl))
            {
                assertTrue("Sent and received messages differ, sent:\n"
                           + msgToString(msg1) + "\nreceived:\n" + msgToString(msg3)
                           + "\nexpected sent getAddress: " + toUrl
                           + "\nexpected received getAddress: " + fromUrl, false);
            }

        }
        finally
        {
            // Cleanup.
            try
            {
                // Close the client connection.
                if (sender != null)
                {
                    sender.close();
                }
            }
            catch (Exception e) {}
            try
            {
                // Close the server connection.
                if (receiver != null)
                {
                    receiver.close();
                }
            }
            catch (Exception e) {}
            try
            {
                // Close the server connection.
                if (receiver2 != null)
                {
                    receiver2.close();
                }
            }
            catch (Exception e) {}
        }

        assertTrue("", true);
    }


    /**
     * Tests the value returned by getTimestamp() method:
     * 1) For a sent MMS it should be null
     * 2) For a received MMS it should be a valid value
     **/
    public void testTimestamp()
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, false);
        String appId = "test.appid.timestamp";
        String clientUrl = "mms://"+addr+":"+appId;
        String serverUrl = "mms://:"+appId;
        String toUrl = clientUrl;
        String fromUrl = clientUrl; // for S60
        String subject = "Sample message";
        MultipartMessage msg1 = null, msg2 = null;
        MessageConnection senderAndReceiver = null;
        try
        {
            try
            {
                senderAndReceiver = getConnection(serverUrl);
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }

            // Send a message without attachments.
            try
            {
                msg1 = (MultipartMessage)senderAndReceiver.newMessage(
                           MessageConnection.MULTIPART_MESSAGE);
                msg1.setAddress(toUrl);
                msg1.setSubject(subject);
                if (msg1.getTimestamp() != null)
                {
                    assertTrue("For an unsent MMS the timestamp should be null",false);
                }
                senderAndReceiver.send(msg1);
                if (msg1.getTimestamp() != null)
                {
                    assertTrue("For a sent MMS the timestamp should be null",false);
                }
            }
            catch (Exception e)
            {
                assertTrue("Sending message failed with exception: "
                           + e.toString(),false);
            }

            try
            {
                msg2 = receiveMsg(senderAndReceiver);
                if (msg2.getTimestamp() == null)
                {
                    assertTrue("For a received MMS the timestamp can not be null",false);
                }

            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }
        }
        finally
        {
            // Cleanup.
            try
            {
                // Close the client connection.
                if (senderAndReceiver != null)
                {
                    senderAndReceiver.close();
                }
            }
            catch (Exception e) {}
        }

        assertTrue("",true);
    }

    // BEGIN - Private helper methods for this test suite.

    // END - Private helper methods for this test suite.
}
