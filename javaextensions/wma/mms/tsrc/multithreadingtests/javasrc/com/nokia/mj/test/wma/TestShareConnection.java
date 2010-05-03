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

import j2meunit.framework.*;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.MIDlet;
//import com.nokia.impl.uitestutils.*;
import javax.wireless.messaging.MessageConnection;
import javax.wireless.messaging.MultipartMessage;


/**
 * <b>TEST CASE SPECIFICATION</b>
 *
 * <br><br><b>These set of module tests focus on the multithreading
 * testing of WMA2.0 implementation.
 * The following scenarios are covered:
 * 1) A couple of threads sending over a connection and during the
 *    sending another thread will close the connection
 * 2) Many threads sending messages over a couple of connections
 * 3) Many threads sending messages over a set of connections and
 *    another set of threads receiving messages over the same
 *    set of connections
 * 4) Many threads receiving messages over a set of connections and
 *    another set of threads sending messages from a different set of
 *    connections
 *</b>
 * <p>
 *
 * <br><br><b>Used stubs and description:</b>
 * <p>
 *
 * <br><br><b>Setting up the environment:</b>
 * Each of the module tests MUST begin with setting up the some of the constants
 * (CONNECTIONS, CON_REP_FACTOR, RECEIVERS, CLOSERS, MESSAGES, MSG_REP_FACTOR)
 * in order to achieve the desired scenario to be tested. Also the modulte tests
 * MUST beging with calling the init() method in order to initialize the notifier
 *
 * <br><br><b>Included test files:</b>
 * <p>
 *
 * <br><br><b>Note whether the test case belongs to the MRT set:</b> No
 * <p>
 *
 * @created    2006-03-23
 *
 */
public class TestShareConnection extends WmaMultiThreadingTestBase
{
    public TestShareConnection()
    {
    }

    public TestShareConnection(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /***************************************
     * Creates the test suite. You need to add a new aSuite.addTest antry for any new test methods, otherwise they won't be run.
     */
    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new TestShareConnection("testSendAndClose", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestShareConnection) tc).testSendAndClose();
            }
        }));

        aSuite.addTest(new TestShareConnection("testMultipleSend", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestShareConnection) tc).testMultipleSend();
            }
        }));

        aSuite.addTest(new TestShareConnection("testMultipleSendAndReceive", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestShareConnection) tc).testMultipleSendAndReceive();
            }
        }));

        aSuite.addTest(new TestShareConnection("testMultipleSendAndReceiveSeparateSenders", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestShareConnection) tc).testMultipleSendAndReceiveSeparateSenders();
            }
        }));

        aSuite.addTest(new TestShareConnection("testMultipleSendAndReceiveAndClose", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestShareConnection) tc).testMultipleSendAndReceiveAndClose();
            }
        }));

        return aSuite;

    }

    /**
     ** A couple of threads sharing a connection: some of them sending messages
     ** and one thread closing the connection at some point
     **/
    public void testSendAndClose()
    {
        CONNECTIONS = 1;
        MESSAGES = 5;
        CLOSERS = 1;
        init();

        String address = "mms://+123456";
        String appID = ":sendAndClose";
        String subject = "Moi!";

        try
        {
            try
            {
                conns = getConnections(address + appID);
                msgs = getDummyMultipartMessages(conns, address, subject);
            }
            catch (Exception e)
            {
                assertTrue(e.toString(),false);
            }

            // start a couple of senders
            multipleSend(conns, msgs);

            // give some time to senders
            try
            {
                Thread.sleep(500);
            }
            catch (Exception e) {};

            // start a closer
            closeConnections(conns);

            // wait for results from all the threads
            String err = waitForResults();
            if (err != null)
            {
                assertTrue(err, false);
            }

        }
        finally
        {
            cleanup();
        }
        assertTrue("",true);
    }

    /**
    ** Many threads sharing a set of connections for sending messages
    **/
    public void testMultipleSend()
    {
        CONNECTIONS = 3;
        MESSAGES = 10;
        MSG_REP_FACTOR = 2;
        init();

        String address = "mms://+123456";
        String appID = ":MSend";
        String subject = "Moi!";

        try
        {
            try
            {
                // get connections
                conns = getConnections(address + appID);
                msgs = getDummyMultipartMessages(conns, address, subject);
            }
            catch (Exception e)
            {
                assertTrue(e.toString(),false);
            }

            // start a couple of senders
            multipleSend(conns, msgs);

            // wait for results from all the threads
            String err = waitForResults();
            if (err != null)
            {
                assertTrue(err, false);
            }
        }
        finally
        {
            cleanup();
        }
        assertTrue("",true);
    }

    /**
    ** Many threads sending messages over a set of connections and
    ** another set of threads receiving messages over the same
    ** set of connections
    **/
    public void testMultipleSendAndReceive()
    {
        CONNECTIONS = 3;
        MESSAGES = 15;
        RECEIVERS = 3;
        MSG_REP_FACTOR = 1;

        String address = "mms://";
        String appID = ":MSendAndRec";
        String subject = "Moi!";
        init();

        try
        {
            try
            {
                // get receive connections
                conns = getConnections(address + appID);
                msgs = getDummyMultipartMessages(conns, address + "+12345678" + appID, subject);
            }
            catch (Exception e)
            {
                assertTrue(e.toString(),false);
            }

            // start the receivers
            multipleReceive(conns);

            // start a couple of senders
            multipleSend(conns, msgs);

            // wait for results from all the threads
            String err = waitForResults();
            if (err != null)
            {
                assertTrue(err, false);
            }

            // validate the received messages
            if (!validMessages())
            {
                assertTrue("mixed up messages", false);
            }
        }
        finally
        {
            cleanup();
        }
        assertTrue("",true);
    }

    /** Many threads receiving messages over a set of connections and
     ** another set of threads sending messages from a different set of
     ** connections
     **/
    public void testMultipleSendAndReceiveSeparateSenders()
    {
        MESSAGES = 15;
        RECEIVERS = 3;
        MSG_REP_FACTOR = 1;

        // different senders
        String address = "mms://";
        String appID = ":MSendAndRecSS";
        String subject = "Moi!";
        init();

        MessageConnection[] sendConnections = null;
        try
        {
            try
            {
                // get send connections
                CONNECTIONS = 5;
                sendConnections = getConnections(address + "12345678");

                // get receive connections
                CONNECTIONS = 3;
                conns = getConnections(address + appID);
                msgs = getDummyMultipartMessages(sendConnections, address + "+12345678" + appID, subject);
            }
            catch (Exception e)
            {
                assertTrue(e.toString(),false);
            }

            // start the receivers
            multipleReceive(conns);

            // start a couple of senders
            multipleSend(sendConnections, msgs);

            // wait for results from all the threads
            String err = waitForResults();
            if (err != null)
            {
                assertTrue(err, false);
            }

            // validate the received messages
            if (!validMessages())
            {
                assertTrue("mixed up messages", false);
            }
        }
        finally
        {
            cleanup();
        }
        assertTrue("",true);
    }

    /**
    ** Many threads sending messages over a set of connections and
    ** another set of threads receiving messages over the same
    ** set of connections
    **/
    public void testMultipleSendAndReceiveAndClose()
    {
        CONNECTIONS = 3;
        MESSAGES = 15;
        RECEIVERS = 3;
        MSG_REP_FACTOR = 1;
        CLOSERS = 3;

        String address = "mms://";
        String appID = ":MSendAndRecC";
        String subject = "Moi!";
        init();

        try
        {
            try
            {
                // get receive connections
                conns = getConnections(address + appID);
                msgs = getDummyMultipartMessages(conns, address + "+12345678" + appID, subject);
            }
            catch (Exception e)
            {
                assertTrue(e.toString(),false);
            }

            // start the receivers
            multipleReceive(conns);

            // start a couple of senders
            multipleSend(conns, msgs);

            // give some time to senders
            try
            {
                Thread.sleep(60*1000);
            }
            catch (Exception e) {};

            // start a closer
            closeConnections(conns);

            // wait for results from all the threads
            String err = waitForResults();
            if (err != null)
            {
                assertTrue(err, false);
            }
        }
        finally
        {
            cleanup();
        }
        assertTrue("",true);
    }

}


