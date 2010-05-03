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
 * To test opening and closing of MessageConnections.<br>
 *
 * Test case descriptions and inputs:<br>
 *
 * 1 Exception tests<br>
 *<br>
 * 1.0 Open server connection but do not close it. Force garbage collection.
 * Try to re-open server connection after it is garbage collected
 * => Opening should succeed.
 * 1.0.1 Open server connection but do not close it. Later check
 * that it is closed when garbage collected.
 * 1.0.2 Try to re-open server connection after it should be
 * garbage collected.
 * 1.1 Try to open server connection on already opened server URL => IOException<br>
 * Open a server connection to URL "mms://:test.appid".
 * Open a new server connection with the same URL and check
 * that an IOException is thrown. [JSR205 D.4.0]
 *<br>
 * 1.2 Testing invalid URLs => IllegalArgumentException/ConnectionNotFoundException<br>
 * Check that the correct exception is thrown when a connection
 * is openend with invalid URLs.
 *<br>
 * "foo://3580401234567" => ConnectionNotFoundException<br>
 * "MMS://3580401234567" => ConnectionNotFoundException<br>
 * "Mms://3580401234567" => ConnectionNotFoundException<br>
 * "mms" => IllegalArgumentException<br>
 * "mms:" => IllegalArgumentException<br>
 * "mms:/" => IllegalArgumentException<br>
 * "mms://" => IllegalArgumentException<br>
 * "mms:///" => IllegalArgumentException<br>
 * "mms:/12" => IllegalArgumentException<br>
 * "mms://12shortcode:a" => IllegalArgumentException<br>
 * "mms://+5550000:app-id" => IllegalArgumentException<br>
 * "mms://user@domain.net:app-id" => IllegalArgumentException<br>
 * "mms://127.0.0.1:app.id" => IllegalArgumentException<br>
 * "mms://1:1:1:1:1:1:1:1:app.id" => IllegalArgumentException<br>
 *<br>
 *
 * 2 Open/close tests<br>
 *<br>
 * 2.1 Opening a connection with valid URL<br>
 * Open a connection with a valid URL "mms://358401234567".
 * Check that no exceptions are thrown.
 *<br>
 * 2.2 Opening and closing a connection with valid URL<br>
 * Open a connection with a valid URL "mms://358401234567" and close
 * the connection. Check that no exceptions are thrown.
 *<br>
 * 2.3 Opening, closing and reopening a connection with valid URL<br>
 * Open a connection with a valid URL "mms://358401234567", close the
 * connection, and open a new connection to the same URL. Check that
 * no exceptions are thrown.
 *<br>
 *
 * 3 Open multiple connections tests<br>
 *<br>
 * 3.1 Open many client connections to the same URL<br>
 * Open three client connections to the same URL "mms://358401234567".
 * Check that no exceptions are thrown.
 *<br>
 * 3.2 Open many client connections to different URLs<br>
 * Open three client connections to different URLs ("mms://358401234567",
 * "mms://358401234567:test.appid", and "mms://358401234567:test.appid2").
 * Check that no exceptions are thrown.
 *<br>
 * 3.3 Open many server connections<br>
 * Open three server connections to different URLs ("mms://:test.appid",
 * "mms://:test.appid2", "mms://:test.appid3").
 * Check that no exceptions are thrown.
 *<br>
 * 3.4 Open one client and server connection<br>
 * Open client connections to URLs "mms://358401234567:test.appid",
 * and server connection to URL "mms://:test.appid".
 * Check that no exceptions are thrown.
 *<br>
 *
 * 4 Read/Write tests<br>
 *<br>
 * 4.1 Connector.open(Server,Read) => OK<br>
 * Open read-only server connection to URL "mms://:test.appid".
 * Check that no exceptions are thrown.
 *<br>
 * 4.2 Connector.open(Server,Write) => OK<br>
 * Open read-write server connection to URL "mms://:test.appid".
 * Check that no exceptions are thrown.
 *<br>
 * 4.3 Connector.open(Server,Read/Write) => OK<br>
 * Open write-only server connection to URL "mms://:test.appid".
 * Check that no exceptions are thrown.
 *<br>
 * 4.4 Connector.open(Client,Read) => IllegalArgumentException<br>
 * Open read-only client connection to URL "mms://358401234567:test.appid".
 * Check that IllegalArgumentException is thrown.
 *<br>
 * 4.5 Connector.open(Client,Write) => OK<br>
 * Open write-only client connection to URL "mms://358401234567:test.appid".
 * Check that no exceptions are thrown.
 *<br>
 * 4.6 Connector.open(Client,Read/Write) => IllegalArgumentException<br>
 * Open read-write client connection to URL "mms://358401234567:test.appid".
 * Check that IllegalArgumentException is thrown.
 *<br>
 *
 * 5 Stream tests<br>
 *<br>
 * 5.1 openDataInputStream raises IllegalArgumentException<br>
 * Open DataInputStream to URL "mms://:test.appid".
 * Check that IllegalArgumentException is thrown.
 *<br>
 * 5.2 openDataOutputStream raises IllegalArgumentException<br>
 * Open DataOutputStream to URL "mms://358401234567:test.appid".
 * Check that IllegalArgumentException is thrown.
 *<br>
 * 5.3 openInputStream raises IllegalArgumentException<br>
 * Open InputStream to URL "mms://:test.appid".
 * Check that IllegalArgumentException is thrown.
 *<br>
 * 5.4 openOutputStream raises IllegalArgumentException<br>
 * Open OutputStream to URL "mms://358401234567:test.appid".
 * Check that IllegalArgumentException is thrown.
 *<br>
 * 6 Getting MMSC tests<br>
 *<br>
 * 6.1 Check that system property wireless.messaging.mms.mmsc
 * has been defined.<br>
 *<br>
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
import java.util.Vector;

import javax.microedition.io.Connector;
import javax.microedition.io.ConnectionNotFoundException;
import javax.wireless.messaging.Message;
import javax.wireless.messaging.MessageConnection;
import javax.wireless.messaging.MessagePart;
import javax.wireless.messaging.MultipartMessage;

public class TestOpenConnection extends WmaTestBase
{

    public TestOpenConnection()
    {
    }

    public TestOpenConnection(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }



    public Test suite()
    {
        TestSuite aSuite = new TestSuite();


        aSuite.addTest(new TestOpenConnection("checkServerConnAfterGC", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestOpenConnection) tc).checkServerConnAfterGC();
            }
        }));


        aSuite.addTest(new TestOpenConnection("openServerConnTwice", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestOpenConnection) tc).openServerConnTwice();
            }
        }));


        aSuite.addTest(new TestOpenConnection("openInvalidUrl", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestOpenConnection) tc).openInvalidUrl();
            }
        }));


        aSuite.addTest(new TestOpenConnection("openCloseClientTest", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestOpenConnection) tc).openCloseClientTest();
            }
        }));


        aSuite.addTest(new TestOpenConnection("openCloseServerTest", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestOpenConnection) tc).openCloseServerTest();
            }
        }));


        aSuite.addTest(new TestOpenConnection("openClientConnsToSameUrl", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestOpenConnection) tc).openClientConnsToSameUrl();
            }
        }));


        aSuite.addTest(new TestOpenConnection("openClientConnsToDiffUrls", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestOpenConnection) tc).openClientConnsToDiffUrls();
            }
        }));


        aSuite.addTest(new TestOpenConnection("openServerConnsToDiffUrls", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestOpenConnection) tc).openServerConnsToDiffUrls();
            }
        }));


        aSuite.addTest(new TestOpenConnection("openClientAndServerConn", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestOpenConnection) tc).openClientAndServerConn();
            }
        }));


        aSuite.addTest(new TestOpenConnection("openReadWriteConns", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestOpenConnection) tc).openReadWriteConns();
            }
        }));


        aSuite.addTest(new TestOpenConnection("openConnStreams", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestOpenConnection) tc).openConnStreams();
            }
        }));


        aSuite.addTest(new TestOpenConnection("getMmscProperty", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestOpenConnection) tc).getMmscProperty();
            }
        }));


        aSuite.addTest(new TestOpenConnection("checkServerConnAfterGC", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestOpenConnection) tc).checkServerConnAfterGC();
            }
        }));

        return aSuite;
    }

    /** MSISDN URL used for opening and closing connections. */
    private static final String msisdnUrl = "mms://358401234567";


    /**
     * 1.0.1 Open server connection but do not close it. Later check
     * that it is closed when garbage collected.
     */
    public void openServerConnForGC()
    {
        String gcUrl= "mms://:test.appid.gc";
        MessageConnection gcConn = null;

        // Test preparations.
        try
        {
            gcConn = getConnection(gcUrl);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Do nothing else. In the end try to re-open connection;
        // if re-opening fails gc has not cleaned connection properly.
        assertTrue("",true);
    }

    /**
     * 1.0.2 Try to re-open server connection after it should be
     * garbage collected.
     */
    public void checkServerConnAfterGC()
    {
        String gcUrl= "mms://:test.appid.gc";
        MessageConnection gcConn = null;

        // Test preparations.
        try
        {
            gcConn = getConnection(gcUrl);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Connection was opened successfully => cleanup was made
        // correctly during gc => test case passed.

        // Cleanup.
        try
        {
            // Close the client connection.
            if (gcConn != null)
            {
                gcConn.close();
                gcConn = null;
            }
        }
        catch (Exception e)
        {
            assertTrue("Connection closing assertTrue: " + e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 1.0 Try to re-open server connection after it should be
     * garbage collected.
     */
    public void checkServerConnAfterGC2()
    {
        String url = "mms://:test.appid.gc";

        // Open server connection.
        try
        {
            MessageConnection conn = (MessageConnection)Connector.open(url);
            if (conn == null)
            {
                assertTrue("Connection opening to url \"" + url + "\" assertTrue",false);
            }
            conn = null;
        }
        catch (Exception e)
        {
            assertTrue("Connection opening to url \"" + url + "\" assertTrue: " + e,false);
        }

        // Create some garbage strings so that garbage collector has
        // some garbage to collect.
        {
            int iMax = 100000;
            String strings[] = new String[iMax];
            for (int i = 0; i < iMax; i++)
            {
                strings[i] = new String("garbage string " + i);
            }
        }

        // Force garbage collection.
        System.gc();

        // Try to open the connection after gc.
        MessageConnection conn = null;
        try
        {
            // Open connection.
            conn = (MessageConnection)Connector.open(url);
            if (conn == null)
            {
                assertTrue("Connection re-opening to url \"" + url + "\" assertTrue",false);
            }
        }
        catch (Exception e)
        {
            assertTrue("Connection re-opening to url \"" + url + "\" assertTrue: " + e,false);
        }

        // Connection was opened successfully => cleanup was made
        // correctly during gc => test case passed.

        // Cleanup.
        try
        {
            // Close the client connection.
            if (conn != null)
            {
                conn.close();
                conn = null;
            }
        }
        catch (Exception e)
        {
            assertTrue("Connection closing assertTrue: " + e.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 1.1 Try to open server connection on already opened server URL
     * => IOException<br>
     * Open a server connection to URL "mms://:test.appid".
     * Open a new server connection with the same URL and check
     * that an IOException is thrown. [JSR205 D.4.0]
     */
    public void openServerConnTwice()
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

        // Try to open another server connection with the same url.
        try
        {
            MessageConnection conn2 =
                (MessageConnection)Connector.open(url);
            assertTrue("Re-opening server connection did not throw IOException",false);
        }
        catch (IOException ioe)
        {
            //OK
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
     * 1.2 Testing invalid URLs =>
     * IllegalArgumentException/ConnectionNotFoundException<br>
     * Check that the correct exception is thrown when a connection
     * is openend with invalid URLs.
     */
    public void openInvalidUrl()
    {
        try
        {
            // RI included in WTK 2.2 crashes with ClassNotFoundException,
            // according to javadocs the api should throw
            // ConnectionNotFoundException.
            //openConnCheckConnectionNotFound("foo://3580401234567");
            //openConnCheckConnectionNotFound("MMS://3580401234567");
            //openConnCheckConnectionNotFound("Mms://3580401234567");
            openConnCheckIllegalArgument("mms");
            openConnCheckIllegalArgument("mms:");
            openConnCheckIllegalArgument("mms:/");
            openConnCheckIllegalArgument("mms://");
            openConnCheckIllegalArgument("mms:///");
            openConnCheckIllegalArgument("mms:/12");
            openConnCheckIllegalArgument("mms://12shortcode:a");
            //openConnCheckIllegalArgument("mms/://");
            openConnCheckIllegalArgument("mms://+5550000:app-id");
            openConnCheckIllegalArgument("mms://user@domain.net:app.id");
            // Following URLs work with the RI.
            openConnCheckIllegalArgument("mms://127.0.0.1:app.id");
            openConnCheckIllegalArgument("mms://1:1:1:1:1:1:1:1:app.id");
            openConnCheckIllegalArgument("mms://1:1:1:1:1:1:1:1");
            openConnCheckIllegalArgument("mms://1.1.1.1");
            //openConnCheckIllegalArgument("mms://abc");
            //openConnCheckIllegalArgument("mms://12abc");
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 2 Open/close tests<br>
     *<br>
     * 2.1 Opening a connection with valid URL<br>
     * Open a connection with a valid URL "mms://358401234567".
     * Check that no exceptions are thrown.
     *<br>
     * 2.2 Opening and closing a connection with valid URL<br>
     * Open a connection with a valid URL "mms://358401234567" and close
     * the connection. Check that no exceptions are thrown.
     *<br>
     * 2.3 Opening, closing and reopening a connection with valid URL<br>
     * Open a connection with a valid URL "mms://358401234567", close the
     * connection, and open a new connection to the same URL. Check that
     * no exceptions are thrown.
     */
    public void openCloseClientTest()
    {
        String url = msisdnUrl;
        MessageConnection conn = null;

        // Try to open connection, if it fails, the test case fails.
        try
        {
            conn = getConnection(url);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Try to close the connection, if it fails, the test case fails.
        try
        {
            conn.close();
        }
        catch (Exception e)
        {
            assertTrue("Connection closing assertTrue: " + e,false);
        }

        // Try to re-open connection, if it fails, the test case fails.
        try
        {
            conn = getConnection(url);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Cleanup.
        try
        {
            if (conn != null)
            {
                // Close the client connection.
                conn.close();
            }
        }
        catch (Exception e)
        {
        }

        assertTrue("",true);
    }

    /**
     * 2 Open/close tests<br>
     *<br>
     * 2.1 Opening a connection with valid URL<br>
     * Open a connection with a valid URL "mms://:test.appid.opencloseopen".
     * Check that no exceptions are thrown.
     *<br>
     * 2.2 Opening and closing a connection with valid URL<br>
     * Open a connection with a valid URL "mms://:test.appid.opencloseopen" and close
     * the connection. Check that no exceptions are thrown.
     *<br>
     * 2.3 Opening, closing and reopening a connection with valid URL<br>
     * Open a connection with a valid URL "mms://:test.appid.opencloseopen", close the
     * connection, and open a new connection to the same URL. Check that
     * no exceptions are thrown.
     */
    public void openCloseServerTest()
    {
        String url = "mms://:test.appid.opencloseopen";
        MessageConnection conn = null;

        // Try to open connection, if it fails, the test case fails.
        try
        {
            conn = getConnection(url);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Try to close the connection, if it fails, the test case fails.
        try
        {
            conn.close();
        }
        catch (Exception e)
        {
            assertTrue("Connection closing assertTrue: " + e,false);
        }

        // Try to re-open connection, if it fails, the test case fails.
        try
        {
            conn = getConnection(url);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Cleanup.
        try
        {
            if (conn != null)
            {
                // Close the client connection.
                conn.close();
            }
        }
        catch (Exception e)
        {
        }

        assertTrue("",true);
    }

    /**
     * 3.1 Open many client connections to the same URL<br>
     * Open three client connections to the same URL "mms://358401234567".
     * Check that no exceptions are thrown.
     */
    public void openClientConnsToSameUrl()
    {
        String url = msisdnUrl;
        MessageConnection conn1 = null, conn2 = null, conn3 = null;

        // Try to open several connections to the same url.
        // If opening fails, the test case fails.
        try
        {
            conn1 = getConnection(url);
            conn2 = getConnection(url);
            conn3 = getConnection(url);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Cleanup.
        try
        {
            // Close the client connections.
            conn1.close();
            conn2.close();
            conn3.close();
        }
        catch (Exception e)
        {
        }

        assertTrue("",true);
    }

    /**
     * 3.2 Open many client connections to different URLs<br>
     * Open three client connections to different URLs ("mms://358401234567",
     * "mms://358401234567:test.appid", and "mms://358501234567:test.appid").
     * Check that no exceptions are thrown.
     */
    public void openClientConnsToDiffUrls()
    {
        MessageConnection conn1 = null, conn2 = null, conn3 = null;

        // Try to open several connections to different urls.
        // If opening fails, the test case fails.
        try
        {
            conn1 = getConnection(msisdnUrl);
            conn2 = getConnection(msisdnUrl + ":test.appid");
            // Bogdan: the new limit for appID is 32
            // conn3 = getConnection(msisdnUrl + ":test.appid.this.appid.is.very.very.very.long.app.id.or.at.least.longer.than.most.other.app.ids");
            conn3 = getConnection(msisdnUrl + ":test.appid.this.appid.is.longer");
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Cleanup.
        try
        {
            // Close the client connections.
            conn1.close();
            conn2.close();
            conn3.close();
        }
        catch (Exception e)
        {
        }

        assertTrue("",true);
    }

    /**
     * 3.3 Open many server connections<br>
     * Open three server connections to different URLs ("mms://:test.appid",
     * "mms://:test.appid2", "mms://:test.appid3").
     * Check that no exceptions are thrown.
     */
    public void openServerConnsToDiffUrls()
    {
        MessageConnection conn1 = null, conn2 = null, conn3 = null;

        // Try to open several server connections to different urls.
        // If opening fails, the test case fails.
        try
        {
            conn1 = getConnection("mms://:test.appid");
            conn2 = getConnection("mms://:test.appid2");
            conn3 = getConnection("mms://:test.appid3");
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Cleanup.
        try
        {
            // Close the client connections.
            conn1.close();
            conn2.close();
            conn3.close();
        }
        catch (Exception e)
        {
        }

        assertTrue("",true);
    }

    /**
     * 3.4 Open one client and server connection<br>
     * Open client connections to URLs "mms://358401234567:test.appid",
     * and server connection to URL "mms://:test.appid".
     * Check that no exceptions are thrown.
     */
    public void openClientAndServerConn()
    {
        MessageConnection conn1 = null, conn2 = null;

        // Try to open client and server connection.
        // If opening fails, the test case fails.
        try
        {
            conn1 = getConnection(msisdnUrl + ":test.appid");
            conn2 = getConnection("mms://:test.appid");
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        // Cleanup.
        try
        {
            // Close the client connections.
            conn1.close();
            conn2.close();
        }
        catch (Exception e)
        {
        }

        assertTrue("",true);
    }

    /**
     * 4 Read/Write tests<br>
     *<br>
     * 4.1 Connector.open(Server,Read) => OK<br>
     * Open read-only server connection to URL "mms://:test.appid".
     * Check that no exceptions are thrown.
     *<br>
     * 4.2 Connector.open(Server,Write) => OK<br>
     * Open read-write server connection to URL "mms://:test.appid".
     * Check that no exceptions are thrown.
     *<br>
     * 4.3 Connector.open(Server,Read/Write) => OK<br>
     * Open write-only server connection to URL "mms://:test.appid".
     * Check that no exceptions are thrown.
     *<br>
     * 4.4 Connector.open(Client,Read) => IllegalArgumentException<br>
     * Open read-only client connection to URL "mms://358401234567:test.appid".
     * Check that IllegalArgumentException is thrown.
     *<br>
     * 4.5 Connector.open(Client,Write) => OK<br>
     * Open write-only client connection to URL "mms://358401234567:test.appid".
     * Check that no exceptions are thrown.
     *<br>
     * 4.6 Connector.open(Client,Read/Write) => IllegalArgumentException<br>
     * Open read-write client connection to URL "mms://358401234567:test.appid".
     * Check that IllegalArgumentException is thrown.
     */
    public void openReadWriteConns()
    {
        MessageConnection conn = null;
        String clientUrl = msisdnUrl + ":test.appid";
        String serverUrl = "mms://:test.appid";

        try
        {
            // Try to open read-only, read-write, and write-only server
            // connections, and write-only client connection.
            // If opening fails, the test case fails.
            openReadWriteConn(serverUrl, Connector.READ, false);
            openReadWriteConn(serverUrl, Connector.READ_WRITE, false);
            openReadWriteConn(serverUrl, Connector.WRITE, false);
            openReadWriteConn(clientUrl, Connector.WRITE, false);
            // Try to open read-only and read-write client
            // connections. If opening does not throw
            // IllegalArgumentException, the test case fails.
            openReadWriteConn(clientUrl, Connector.READ, true);
            openReadWriteConn(clientUrl, Connector.READ_WRITE, true);
        }
        catch (WmaException wmae)
        {
            assertTrue(wmae.toString(),false);
        }

        assertTrue("",true);
    }

    /**
     * 5 Stream tests<br>
     *<br>
     * 5.1 openDataInputStream raises IllegalArgumentException<br>
     * Open DataInputStream to URL "mms://:test.appid".
     * Check that IllegalArgumentException is thrown.
     *<br>
     * 5.2 openDataOutputStream raises IllegalArgumentException<br>
     * Open DataOutputStream to URL "mms://358401234567:test.appid".
     * Check that IllegalArgumentException is thrown.
     *<br>
     * 5.3 openInputStream raises IllegalArgumentException<br>
     * Open InputStream to URL "mms://:test.appid".
     * Check that IllegalArgumentException is thrown.
     *<br>
     * 5.4 openOutputStream raises IllegalArgumentException<br>
     * Open OutputStream to URL "mms://358401234567:test.appid".
     * Check that IllegalArgumentException is thrown.
     */
    public void openConnStreams()
    {
        MessageConnection conn = null;
        String clientUrl = msisdnUrl + ":test.appid";
        String serverUrl = "mms://:test.appid";

        try
        {
            conn = (MessageConnection)Connector.openDataInputStream(serverUrl);
            assertTrue("Opening DataInputStream should not succeed",false);
        }
        catch (IllegalArgumentException iae)
        {
            // OK
        }
        catch (Exception e)
        {
            assertTrue("Wrong exception when opening DataInputStream: " + e,false);
        }

        try
        {
            conn = (MessageConnection)Connector.openDataOutputStream(clientUrl);
            assertTrue("Opening DataOutputStream should not succeed",false);
        }
        catch (IllegalArgumentException iae)
        {
            // OK
        }
        catch (Exception e)
        {
            assertTrue("Wrong exception when opening DataOutputStream: " + e,false);
        }

        try
        {
            conn = (MessageConnection)Connector.openInputStream(serverUrl);
            assertTrue("Opening InputStream should not succeed",false);
        }
        catch (IllegalArgumentException iae)
        {
            // OK
        }
        catch (Exception e)
        {
            assertTrue("Wrong exception when opening InputStream: " + e,false);
        }

        try
        {
            conn = (MessageConnection)Connector.openOutputStream(clientUrl);
            assertTrue("Opening OutputStream should not succeed",false);
        }
        catch (IllegalArgumentException iae)
        {
            // OK
        }
        catch (Exception e)
        {
            assertTrue("Wrong exception when opening OutputStream: " + e,false);
        }

        assertTrue("",true);
    }

    /**
     * 6 Getting MMSC tests<br>
     *<br>
     * 6.1 Check that system property wireless.messaging.mms.mmsc
     * has been defined.<br>
     */
    public void getMmscProperty()
    {
        String mmscPropertyName = "wireless.messaging.mms.mmsc";
        String mmscPropertyValue = System.getProperty(mmscPropertyName);
        if (mmscPropertyValue == null)
        {
            assertTrue("System property " + mmscPropertyName
                       + " has not been defined (null)",false);
        }
        if (mmscPropertyValue.length() == 0)
        {
            assertTrue("System property " + mmscPropertyName
                       + " has not been defined (length == 0)",false);
        }
        assertTrue("System property " + mmscPropertyName + "="
                   + mmscPropertyValue,true);
    }

    // BEGIN - Private helper methods for this test suite.

    /**
     * Opens a connection to given URL. If connection opening succeeds
     * or throws other than IllegalArgumentException, throws
     * WmaException.
     */
    private void openConnCheckIllegalArgument(String url)
    throws WmaException
    {
        MessageConnection conn = null;
        try
        {
            // Try to open connection.
            conn = (MessageConnection)Connector.open(url);
            throw new WmaException("Connection opening should not succeed with url \"" +
                                   url + "\"");
        }
        catch (IllegalArgumentException iae)
        {
            // OK, expected this exception.
        }
        catch (WmaException wmae)
        {
            // OK, throw again this exception.
            throw wmae;
        }
        catch (Exception e)
        {
            throw new WmaException("Connection opening to url \"" + url +
                                   "\" assertTrue with wrong exception: " + e);
        }
        finally
        {
            if (conn != null)
            {
                try
                {
                    conn.close();
                }
                catch (IOException ioe)
                {
                }
            }
        }
    }

    /**
     * Opens a connection to given URL. If connection opening succeeds
     * or throws other than ConnectionNotFoundException, throws
     * WmaException.
     */
    private void openConnCheckConnectionNotFound(String url)
    throws WmaException
    {
        MessageConnection conn = null;
        try
        {
            // Try to open connection.
            conn = (MessageConnection)Connector.open(url);
            throw new WmaException("Connection opening should not succeed with url \"" +
                                   url + "\"");
        }
        catch (ConnectionNotFoundException cnfe)
        {
            // OK, expected this exception.
        }
        catch (WmaException wmae)
        {
            // OK, throw again this exception.
            throw wmae;
        }
        catch (Exception e)
        {
            throw new WmaException("Connection opening to url \"" + url +
                                   "\" assertTrue with wrong exception: " + e);
        }
        finally
        {
            if (conn != null)
            {
                try
                {
                    conn.close();
                }
                catch (IOException ioe)
                {
                }
            }
        }
    }

    /**
     * Tries to open a connection to given url in given mode.
     * If result is not expected, throws WmaException.
     */
    private void openReadWriteConn(
        String url, int mode, boolean expectIllegalArgumentException)
    throws WmaException
    {
        MessageConnection conn = null;
        try
        {
            conn = (MessageConnection)Connector.open(url, mode);
        }
        catch (IllegalArgumentException iae)
        {
            if (!expectIllegalArgumentException)
            {
                throw new WmaException(connectorModeToString(mode) +
                                       " connection opening to url \"" +
                                       url + "\" assertTrue: " + iae);
            }
        }
        catch (Exception e)
        {
            throw new WmaException(connectorModeToString(mode) +
                                   " connection opening to url \"" +
                                   url + "\" assertTrue: " + e);
        }
        finally
        {
            try
            {
                if (conn != null)
                {
                    conn.close();
                }
            }
            catch (Exception e) {}
        }
    }

    /**
     * Returns a string presentation of given Connector mode
     * (READ, WRITE, READ_WRITE).
     */
    private String connectorModeToString(int mode)
    {
        String result = null;
        switch (mode)
        {
        case Connector.READ:
            result = "Read-only";
            break;
        case Connector.READ_WRITE:
            result = "Read-Write";
            break;
        case Connector.WRITE:
            result = "Write-only";
            break;
        }
        return result;
    }

    // END - Private helper methods for this test suite.
}
