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
 * 1 Receive tests<br>
 * 1.1 Tests the handling of different kind of MMSs<br>
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
import javax.microedition.midlet.MIDlet;

import javax.microedition.io.Connector;
import javax.wireless.messaging.Message;
import javax.wireless.messaging.MessageConnection;
import javax.wireless.messaging.MessagePart;
import javax.wireless.messaging.MultipartMessage;
import javax.microedition.io.file.FileConnection;
import java.io.*;

public class TestReceive extends WmaTestBase
{
    public static Object lock = new Object();
    private static final String SOURCE_PATH = "file:///c:/Data/Images/wma20/";
    private static final String DEST_PATH = "file:///c:/Data/Images/mmstest/";

    public TestReceive()
    {
    }

    public TestReceive(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /**
     * Any pretest setup can be done here
     */
    protected void setUp() throws Exception
    {
    }

    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new TestReceive("testReceive", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestReceive) tc).testReceive();
            }
        }));


        return aSuite;
    }

    /**
     * 1.1 Tests the receiving of different kinf of MMSs<br>
     */
    public void testReceive()
    {
        // Test preparations.
        String url = "mms://:javaAppID";
        MessageConnection conn = null;
        MultipartMessage msg;
        int MMSs = 19;
        try
        {
            // start the receiver thread first
            try
            {
                conn = getConnection(url);
            }
            catch (WmaException wmae)
            {
                assertTrue(wmae.toString(),false);
            }
            MessageReceiver receiver = new MessageReceiver(lock, conn, MMSs);
            receiver.start();

            // copy the ready made MMSs from c:\wma20 into s:\mmstest
            try
            {
                copyFile("Complete.mms");
                copyFile("EmptyBcc.mms");
                copyFile("EmptyCc.mms");
                copyFile("EmptyCID.mms");
                copyFile("EmptyCL.mms");
                copyFile("SameCID.mms");
                copyFile("EmptyContent.mms");
                copyFile("EmptyFrom.mms");
                copyFile("EmptyMIME.mms");
                copyFile("EmptyReplyToAppID.mms");
                copyFile("EmptySubject.mms");
                copyFile("EmptyTo.mms");
                copyFile("InvalidFrom.mms");
                copyFile("NoAddresses.mms");
                copyFile("NoMIME.mms");
                copyFile("NoPriority.mms");
                copyFile("NoReplyToAppID.mms");
                copyFile("Null.mms");
                copyFile("WhiteSpaces.mms");
            }
            catch (Exception e)
            {
                e.printStackTrace();
                assertTrue(e.toString(), false);
            }


            // wait for the receiving of the MMSs
            synchronized (lock)
            {
                try
                {
                    lock.wait();
                }
                catch (InterruptedException e)
                {
                }
            }

            if (receiver.getError() != null)
            {
                assertTrue(receiver.getError(), false);
            }

        }
        finally
        {
            // Cleanup.
            try
            {
                // Close the MMS connection.
                if (conn != null)
                {
                    conn.close();
                }
            }
            catch (Exception e) {}
        }

        assertTrue("",true);
    }

    private void copyFile(String fileName)
    throws IOException
    {
        FileConnection sFileConn = null, dFileConn = null;
        try
        {
            sFileConn = (FileConnection)Connector.open(SOURCE_PATH + fileName);
            InputStream fis = sFileConn.openInputStream();
            byte[] b = new byte[1024];
            int length = fis.read(b, 0, 1024);
            dFileConn = (FileConnection)Connector.open(DEST_PATH + fileName);
            dFileConn.create();
            OutputStream fos = dFileConn.openOutputStream();
            fos.write(b, 0, length);
            fos.flush();
        }
        finally
        {
            // Cleanup.
            try
            {
                // Close the file connection.
                if (sFileConn != null)
                {
                    sFileConn.close();
                }
            }
            catch (Exception e) {}
            try
            {
                // Close the file connection.
                if (dFileConn != null)
                {
                    dFileConn.close();
                }
            }
            catch (Exception e) {}
        }
    }

    /**
     * Thread for receiving a message from given connection.
     */
    static class MessageReceiver extends Thread
    {
        private Object owner = null;
        private MessageConnection connection = null;
        private String error = null;
        private int expectedMMSs;

        public MessageReceiver(Object anOwner, MessageConnection aConnection, int aExpectedMMSs)
        {
            owner = anOwner;
            connection = aConnection;
            expectedMMSs = aExpectedMMSs;
        }

        public void run()
        {
            for (int i=0; i<expectedMMSs; i++)
            {
                try
                {
                    MultipartMessage msg = (MultipartMessage)connection.receive();
                    if (msg == null)
                    {
                        error = "MMS not received properly";
                        break;
                    }
                    if (msg.getSubject() == null)
                    {
                        error = handleNoSubjectMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("Complete"))
                    {
                        error = handleCompleteMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("EmptyBc"))
                    {
                        error = handleNoEmptyBccMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("EmptyCc"))
                    {
                        error = handleEmptyCcMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("EmptyCID"))
                    {
                        error = handleEmptyCIDMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("EmptyCL"))
                    {
                        error = handleEmptyCLMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("EmptyContent"))
                    {
                        error = handleEmptyContentMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("EmptyFrom"))
                    {
                        error = handleEmptyFromMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("EmptyMIME"))
                    {
                        error = handleEmptyMIMEMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("EmptyReplyToAppID"))
                    {
                        error = handleEmptyReplyToAppIDMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("EmptyTo"))
                    {
                        error = handleEmptyToMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("InvalidFrom"))
                    {
                        error = handleInvalidFromMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("NoAddresses"))
                    {
                        error = handleNoAddressesMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("no--mime"))
                    {
                        error = handleNoMIMEMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("NoPriority"))
                    {
                        error = handleNoPriorityMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("NoReplyToAppID"))
                    {
                        error = handleNoReplyToAppIDMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("NullMMS"))
                    {
                        error = handleNullMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("wtspaces"))
                    {
                        error = handleWTSpacesMMS(connection, msg);
                    }
                    else if (msg.getSubject().equals("Dupl-CID"))
                    {
                        error = handleDuplCIDMMS(connection, msg);
                    }
                    else
                    {
                        error = "Unexpected MMS";
                        break;
                    }
                }
                catch (Exception e)
                {
                    error = e.toString();
                    break;
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

        public String getError()
        {
            return error;
        }

        private String handleNoSubjectMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }

        private String handleCompleteMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }

        private String handleNoEmptyBccMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }

        private String handleEmptyCcMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }

        private String handleEmptyCIDMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }

        private String handleEmptyCLMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }

        private String handleEmptyContentMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }

        private String handleEmptyFromMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }

        private String handleEmptyMIMEMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }

        private String handleEmptyReplyToAppIDMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }

        private String handleEmptyToMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }

        private String handleInvalidFromMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }

        private String handleNoAddressesMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }

        private String handleNoMIMEMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }

        private String handleNoPriorityMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }

        private String handleNoReplyToAppIDMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }

        private String handleNullMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }

        private String handleDuplCIDMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            try
            {
                aConnection.send(msg);
            }
            catch (IllegalArgumentException e)
            {
                // ok
                return null;
            }
            catch (Exception e)
            {
                // fail
            }
            return "Sending of an invalid MMS should have failed";
        }

        private String handleWTSpacesMMS(MessageConnection aConnection, MultipartMessage msg)
        {
            return null;
        }
    }
}
