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
 * Test message receiving with MessageConnection.
 *
 * Test case descriptions and inputs:<br>
 *
 * 1 Exception tests<br>
 * 1.1 Call blocking receive and close the connection => InterruptedIOException<br>
 *
 * 2 Listener tests<br>
 * 2.1 Listener is deregistered when another is set => messages are delivered
 * only to last registered listener<br>
 * 2.2 Listener is deregistered when null listener is set => messages not
 * delivered to listener<br>
 * 2.3 Listener is notified of incoming messages<br>
 *
 * 3 Receive tests (for some receive tests see TestSend.java)
 * 3.1 Send and receive through the same server connection.
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

import java.io.InterruptedIOException;
import java.util.Vector;

import javax.microedition.io.Connector;
import javax.wireless.messaging.Message;
import javax.wireless.messaging.MessageConnection;
import javax.wireless.messaging.MessageListener;
import javax.wireless.messaging.MessagePart;
import javax.wireless.messaging.MultipartMessage;

public class TestReceive extends WmaTestBase
{

    public TestReceive()
    {
    }

    public TestReceive(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public Test suite()
    {
        TestSuite aSuite = new TestSuite();


        aSuite.addTest(new TestReceive("receiveConnectionClosed", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestReceive) tc).receiveConnectionClosed();
            }
        }));


        aSuite.addTest(new TestReceive("listenerReRegistration", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestReceive) tc).listenerReRegistration();
            }
        }));


        aSuite.addTest(new TestReceive("nullListenerRegistration", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestReceive) tc).nullListenerRegistration();
            }
        }));


        aSuite.addTest(new TestReceive("listenerNotifications", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestReceive) tc).listenerNotifications();
            }
        }));


        aSuite.addTest(new TestReceive("sendReceiveSameServerConn", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestReceive) tc).sendReceiveSameServerConn();
            }
        }));


        return aSuite;
    }

    /**
     * 1.1 Call blocking receive and close the connection =>
     * InterruptedIOException<br>
     */
    public void receiveConnectionClosed()
    {
        // Test preparations.
        String url = "mms://:test.appid";
        MessageConnection conn = null;
        try
        {
            conn = getConnection(url);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Start the closer and call receive
        Closer closer = new Closer(3000, conn);
        closer.start();

        // Call receive.
        try
        {
            Message msg = conn.receive();
            assertTrue("Closing connection during receive should fail "
                       + "with InterruptedIOException",false);
        }
        catch (InterruptedIOException iioe)
        {
            // passed
        }
        catch (Exception e)
        {
            assertTrue("Closing connection during receive failed with "
                       + "wrong exception: " + e.toString(),false);
        }

        // Cleanup.
        try
        {
            // Close the connection.
            conn.close();
        }
        catch (Exception e) {}

        assertTrue("",true);
    }

    /**
     * 2.1 Listener is deregistered when another is set => messages are delivered
     * only to last registered listener<br>
     */
    public void listenerReRegistration()
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, false);
        String appId = "test.listenerreregistration";
        String clientUrl = "mms://"+addr+":"+appId;
        String serverUrl = "mms://:"+appId;
        String toUrl = clientUrl;
        String fromUrl = clientUrl;
        //String fromUrl = "mms://"+addr;
        MessageConnection conn = null;
        try
        {
            conn = getConnection(serverUrl);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create and register listeners.
        CounterListener listener1 = new CounterListener();
        CounterListener listener2 = new CounterListener();
        try
        {
            conn.setMessageListener(listener1);
            conn.setMessageListener(listener2);
        }
        catch (Exception e)
        {
            assertTrue("Setting listeners failed with exception: "
                       + e.toString(),false);
        }

        // Send a message.
        MultipartMessage msg = null;
        try
        {
            msg = getDummyMultipartMessage(conn, clientUrl, "Test message");
            conn.send(msg);
        }
        catch (Exception e)
        {
            assertTrue("Sending message failed with exception: "
                       + e.toString(),false);
        }

        // Wait a while for the notification
        try
        {
            wait(getNotificationWaitTime());
        }
        catch (Exception e)
        {
            assertTrue("Notification wait failed with exception: " + e.toString(),false);
        }

        // Check that correct listener receives correct number of notifications.
        if (listener1.getNotificationsCount() != 0)
        {
            assertTrue(
                "Wrong listener received notification, expected 0, got "
                + listener1.getNotificationsCount(),false);
        }
        if (listener2.getNotificationsCount() != 1)
        {
            assertTrue(
                "Wrong number of notifications received, expected 1, got "
                + listener2.getNotificationsCount(),false);
        }

        // Check that correct message is received.
        try
        {
            MultipartMessage msg2 = (MultipartMessage)conn.receive();
            if (!messagesEqual(msg, msg2, toUrl, fromUrl))
            {
                assertTrue("Sent and received messages differ, sent:\n"
                           + msgToString(msg) + "\nreceived:\n" + msgToString(msg2),false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Receiving message failed with exception: "
                       + e.toString(),false);
        }

        // Cleanup.
        try
        {
            // Close the connection.
            conn.close();
        }
        catch (Exception e) {}

        assertTrue("",true);
    }

    /**
     * 2.2 Listener is deregistered when null listener is set => messages not
     * delivered to listener<br>
     */
    public void nullListenerRegistration()
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, false);
        String appId = "test.nulllistenerregistration";
        String clientUrl = "mms://"+addr+":"+appId;
        String serverUrl = "mms://:"+appId;
        String toUrl = clientUrl;
        String fromUrl = clientUrl;
        //String fromUrl = "mms://"+addr;
        MessageConnection conn = null;
        try
        {
            conn = getConnection(serverUrl);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create and register listener and null listener.
        CounterListener listener = new CounterListener();
        try
        {
            conn.setMessageListener(listener);
            conn.setMessageListener(null);
        }
        catch (Exception e)
        {
            assertTrue("Setting listener failed with exception: "
                       + e.toString(),false);
        }

        // Send a message.
        MultipartMessage msg = null;
        try
        {
            msg = getDummyMultipartMessage(conn, clientUrl, "Test message");
            conn.send(msg);
        }
        catch (Exception e)
        {
            assertTrue("Sending message failed with exception: "
                       + e.toString(),false);
        }

        // Wait a while for the notification
        try
        {
            wait(getNotificationWaitTime());
        }
        catch (Exception e)
        {
            assertTrue("Notification wait failed with exception: " + e.toString(),false);
        }

        // Check that listener does not receive notifications.
        if (listener.getNotificationsCount() != 0)
        {
            assertTrue(
                "Listener received notification, expected 0, got "
                + listener.getNotificationsCount(),false);
        }

        // Check that correct message is received.
        try
        {
            MultipartMessage msg2 = (MultipartMessage)conn.receive();
            if (!messagesEqual(msg, msg2, toUrl, fromUrl))
            {
                assertTrue("Sent and received messages differ, sent:\n"
                           + msgToString(msg) + "\nreceived:\n" + msgToString(msg2),false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Receiving message failed with exception: "
                       + e.toString(),false);
        }

        // Cleanup.
        try
        {
            // Close the connection.
            conn.close();
        }
        catch (Exception e) {}

        assertTrue("",true);
    }

    /**
     * 2.3 Listener is notified of incoming messages<br>
     */
    public void listenerNotifications()
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, false);
        String appId = "test.listenernotifications";
        String clientUrl = "mms://"+addr+":"+appId;
        String serverUrl = "mms://:"+appId;
        String toUrl = clientUrl;
        String fromUrl = clientUrl;
        //String fromUrl = "mms://"+addr;
        MessageConnection conn = null, conn2 = null;
        try
        {
            conn = getConnection(serverUrl);
            conn2 = getConnection(clientUrl);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Create and register listener.
        CounterListener listener = new CounterListener();
        try
        {
            conn.setMessageListener(listener);
        }
        catch (Exception e)
        {
            assertTrue("Setting listener failed with exception: "
                       + e.toString(),false);
        }

        // Send a message.
        int msgCount = 6;
        MultipartMessage msg = null;
        try
        {
            msg = getDummyMultipartMessage(conn, clientUrl, "Test message");
            conn.send(msg);
            conn.send(msg);
            conn2.send(msg);
            conn.send(msg);
            conn2.send(msg);
            conn2.send(msg);
        }
        catch (Exception e)
        {
            assertTrue("Sending messages failed with exception: "
                       + e.toString(),false);
        }

        // Wait a while for the notifications
        try
        {
            wait(getNotificationWaitTime());
        }
        catch (Exception e)
        {
            assertTrue("Notification wait failed with exception: " + e.toString(),false);
        }

        // Check that listener receives correct number of notifications.
        if (listener.getNotificationsCount() != msgCount)
        {
            assertTrue(
                "Listener received notification, expected " + msgCount
                + ", got " + listener.getNotificationsCount(),false);
        }

        // Check that correct messages are received.
        int clientMsgsCount = 0;
        for (int i = 0; i < msgCount; i++)
        {
            try
            {
                MultipartMessage msg2 = (MultipartMessage)conn.receive();
                if (msg2.getAddress().indexOf(":", 5) == -1)
                {
                    // it's been sent from a client type of connection
                    fromUrl = "mms://"+addr;
                    clientMsgsCount++;
                }
                else
                {
                    fromUrl = clientUrl;
                }
                if (clientMsgsCount > 3 ||
                        !messagesEqual(msg, msg2, toUrl, fromUrl))
                {
                    assertTrue("Sent and received message " + i
                               + " differ, sent:\n" + msgToString(msg)
                               + "\nreceived:\n" + msgToString(msg2),false);
                }
            }
            catch (Exception e)
            {
                assertTrue("Receiving message " + i
                           + " failed with exception: " + e.toString(),false);
            }
        }

        // Cleanup.
        try
        {
            // Close the connection.
            conn.close();
        }
        catch (Exception e) {}
        try
        {
            // Close the connection.
            conn2.close();
        }
        catch (Exception e) {}

        assertTrue("",true);
    }

    /**
     * 3 Receive tests (for some receive tests see TestSend.java)
     * 3.1 Send and receive through the same server connection.
     */
    public void sendReceiveSameServerConn()
    {
        // Test preparations.
        String addr = getMsisdnAddress(0, false);
        String appId = "test.appid.sendreceive.sameconn";
        String clientUrl = "mms://"+addr+":"+appId;
        String serverUrl = "mms://:"+appId;
        String toUrl = clientUrl;
        String fromUrl = clientUrl; // for S60
        //String fromUrl = "mms://"+addr; // for RI
        String subject = "Sample message";
        MultipartMessage msg1 = null, msg2 = null;
        MessageConnection serverConn = null;
        try
        {
            try
            {
                serverConn = getConnection(serverUrl);
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }

            // Send a message without attachments.
            try
            {
                msg1 = (MultipartMessage)serverConn.newMessage(
                           MessageConnection.MULTIPART_MESSAGE);
                msg1.setAddress(toUrl);
                msg1.setSubject(subject);
                msg1.setHeader("X-Mms-Priority", "LoW");
                serverConn.send(msg1);
            }
            catch (Exception e)
            {
                assertTrue("Sending message failed with exception: "
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

            // Send a message with attachments.
            try
            {
                msg1 = (MultipartMessage)serverConn.newMessage(
                           MessageConnection.MULTIPART_MESSAGE);
                msg1.setAddress(toUrl);
                msg1.setSubject(subject);
                msg1.setHeader("X-Mms-Priority", "NormaL");
                msg1.addMessagePart(getDummyMessagePart(10000, "1"));
                msg1.addMessagePart(getDummyMessagePart(1000, "2"));
                msg1.addMessagePart(getDummyMessagePart(100, "3"));
                msg1.addMessagePart(getDummyMessagePart(10, "43"));
                msg1.setStartContentId("3");
                serverConn.send(msg1);
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

    // BEGIN - Private helper methods for this test suite.

    // END - Private helper methods for this test suite.

    /**
     * Waits for a specified period of time and then closes a connection.
     */
    class Closer extends Thread
    {
        MessageConnection connection = null;
        long sleepTime = 3000;
        boolean done = false;
        public Closer(long aSleepTime, MessageConnection aConnection)
        {
            sleepTime = aSleepTime;
            connection = aConnection;
        }
        public void run()
        {
            try
            {
                Thread.sleep(sleepTime);
            }
            catch (Exception ex) {}
            try
            {
                connection.close();
            }
            catch (Exception ex) {}
            done = true;
        }
        public boolean isDone()
        {
            return done;
        }
    }

    /**
     * Simple implementation of the MessageListener interface.
     * Counts number of received notifications.
     */
    class CounterListener implements MessageListener
    {
        private int notificationsCount = 0;
        public void notifyIncomingMessage(MessageConnection Connection)
        {
            notificationsCount++;
        }
        public int getNotificationsCount()
        {
            return notificationsCount;
        }
    }
}
