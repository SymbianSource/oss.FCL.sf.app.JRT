/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.test.storage;

import com.nokia.mj.test.storage.utils.StorageSessionTestUtils;
import com.nokia.mj.impl.storage.*;

import com.nokia.mj.impl.rt.test.UnitTestSuiteCreator;
import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * StorageSession connection test cases. See test methods for test case details.
 */
public class TestConnection extends TestCase implements UnitTestSuiteCreator, StorageNames
{
    private StorageSession iSession = null;
    private StorageSessionTestUtils iJtu = null;

    public TestSuite createTestSuite(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestConnection("testOpen", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestConnection)tc).testOpen();
            }
        }));

        suite.addTest(new TestConnection("testMultiOpen", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestConnection)tc).testMultiOpen();
            }
        }));

        suite.addTest(new TestConnection("testClose", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestConnection)tc).testClose();
            }
        }));

        return suite;
    }

    public TestConnection()
    {
    }

    public TestConnection(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    protected void setUp()
    {
        iSession = StorageFactory.createSession();
        iJtu = new StorageSessionTestUtils();
    }

    protected void tearDown()
    {
        if (iSession != null)
        {
            try
            {
                iSession.destroySession();
            }
            catch (StorageException se)
            {
                // No can do
                System.out.println("TearDown failed: " + se.toString());
            }
        }
    }

    /**
     * Test connection open
     * 1. Test open().
     * 2. Test open(JAVA_DATABASE_NAME)
     * 3. Test open to FOO_BAR.db.
     * 4. Test open with empty name. JavaStorageException is thrown.
     * 5. Test open with null name. JavaStorageException is thrown.
     * 6. Test two consecutive opens.
     * 7. Test re-open.
     */
    public void testOpen()
    {
        // 1. Test open
        try
        {
            String appUID = "TestOpen1";
            iSession.open();
            iSession.startTransaction();

            // Write and read to verify connection.
            assertTrue("Populate with default",
                       iJtu.populateWithDefault(iSession, appUID));
            assertTrue("Check defaults",
                       iJtu.checkDefaultParams(iSession, appUID));

            // Do not store changes.
            iSession.rollbackTransaction();
        }
        catch (StorageException se)
        {
            iSession.rollbackTransaction();
            assertTrue("Database open failed: " + se.getMessage(), false);
        }

        iSession.close();

        // 2. Test open (JAVA_DATABASE_NAME.)
        try
        {
            String appUID = "TestOpen1";
            iSession.open(JAVA_DATABASE_NAME);
            iSession.startTransaction();

            // Write and read to verify connection.
            assertTrue("Populate with default",
                       iJtu.populateWithDefault(iSession, appUID));
            assertTrue("Check defaults",
                       iJtu.checkDefaultParams(iSession, appUID));

            // Do not store changes.
            iSession.rollbackTransaction();
        }
        catch (StorageException se)
        {
            iSession.rollbackTransaction();
            assertTrue("Database open with name failed: " + se.getMessage(),
                       false);
        }

        iSession.close();

        // 3. Test open ("FOO_BAR.db".)
        try
        {
            // Depending on the platform this should change.
            // based on system variable.
            iSession.open("c:\\FOO_BAR.db");
        }
        catch (StorageException se)
        {
            assertTrue("Database open to foo bar failed: " + se.getMessage(),
                       false);
        }

        iSession.close();

        // 4. Test open with empty name
        try
        {
            iSession.open("");
            assertTrue("Opening with empty name did not thrown exception",
                       false);

        }
        catch (StorageException se)
        {
            // PASSED
        }

        // 5. Test open with null name
        try
        {
            String name = null;
            iSession.open(name);
            assertTrue("Opening with null name did not thrown exception",
                       false);

        }
        catch (StorageException se)
        {
            // PASSED
        }

        // 6. Test two consecutive opens.
        try
        {
            iSession.open();
            iSession.open();
            assertTrue("Consecutive open does not throw exp", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExp6: " + t.toString(), false);
        }

        iSession.close();

        // 7. Test re-open
        try
        {
            String appUID = "TestOpen2";
            iSession.open();
            iSession.startTransaction();

            assertTrue("Re-Open: Populate with default",
                       iJtu.populateWithDefault(iSession, appUID));

            // Write and read to verify connection.
            assertTrue("Re-Open: Check defaults",
                       iJtu.checkDefaultParams(iSession, appUID));

            assertTrue("Re-Open: Remove defaults",
                       iJtu.removeDefaultParams(iSession, appUID));

            // Store changes for re-open test cases.
            iSession.rollbackTransaction();
        }
        catch (StorageException se)
        {
            iSession.rollbackTransaction();
            assertTrue("Re-open: " + se.getMessage(), false);
        }

        // Currently this deletes instance.
        iSession.close();
    }

    /**
     * Test connection open.
     * 1. Test two connections to same target can be opened.
     * 2. Test two connections to separate targets can be opened.
     */
    public void testMultiOpen()
    {
        // 1. Two connections to same target.
        StorageSession session2 = null;
        try
        {
            String appUID = "TwoConnections";
            iSession.open();

            session2 = StorageFactory.createSession();
            session2.open();

            iSession.startTransaction();
            // session2.startTransaction(); // This will fail as not supported

            // Write and read to verify connection.
            assertTrue("Conn1: Populate with default",
                       iJtu.populateWithDefault(iSession, appUID));
            assertTrue("Conn1: Check defaults",
                       iJtu.checkDefaultParams(iSession, appUID));

            // Cannot have two concurrent transactions to same db without"
            // getting database busy.
            iSession.rollbackTransaction();
            session2.startTransaction();

            StorageEntry entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(PACKAGE_ID,
                                                    "GreatSuite"));
            entry.addAttribute(new StorageAttribute(NAME, "MyJavaApp"));
            entry.addAttribute(new StorageAttribute(MAIN_CLASS,
                                                    "com.my.java.app.GreatApp"));
            entry.addAttribute(new StorageAttribute(AUTORUN, "0",
                                                    StorageAttribute.INT_TYPE));

            assertTrue("Conn2: Populate",
                       iJtu.populate(session2, APPLICATION_TABLE,
                                     appUID, entry));
            assertTrue("Conn2: Check",
                       iJtu.checkParams(session2,
                                        APPLICATION_TABLE, entry, 1));

            session2.rollbackTransaction();
        }
        catch (StorageException se)
        {
            iSession.rollbackTransaction();

            if (session2 != null)
            {
                session2.rollbackTransaction();
            }

            assertTrue("Test two connections: " + se.getMessage(), false);
        }
        iSession.close();
        session2.close();

        // 2. Two connections to different target.
        try
        {
            String appUID = "TwoConnections";
            iSession.open();
            session2.open("c:\\FOO_BAR.db");

            iSession.startTransaction();
            session2.startTransaction();

            // Write and read to verify connection.
            assertTrue("Conn1: Populate with default",
                       iJtu.populateWithDefault(iSession, appUID));
            assertTrue("Conn1: Check defaults",
                       iJtu.checkDefaultParams(iSession, appUID));

            StorageEntry entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(ID, ""));
            entry.addAttribute(new StorageAttribute(PACKAGE_ID,
                                                    "ROOT_PACKAGE"));
            entry.addAttribute(new StorageAttribute(NAME, "MyJavaApp"));
            entry.addAttribute(new StorageAttribute(MAIN_CLASS,
                                                    "com.my.java.app.GreatApp"));
            entry.addAttribute(new StorageAttribute(AUTORUN, "0",
                                                    StorageAttribute.INT_TYPE));

            // Create table to new database.
            session2.createTable(APPLICATION_TABLE, entry, null);

            entry.removeAttribute(ID);

            assertTrue("DB2Conn: Populate",
                       iJtu.populate(session2, APPLICATION_TABLE,
                                     appUID, entry));

            assertTrue("DB2Conn: Check",
                       iJtu.checkParams(session2,
                                        APPLICATION_TABLE, entry, 1));

            iSession.rollbackTransaction();
            session2.rollbackTransaction();
        }
        catch (StorageException se)
        {
            iSession.rollbackTransaction();

            if (session2 != null)
            {
                session2.rollbackTransaction();
            }

            assertTrue("Test two diff connections: " + se.getMessage(), false);
        }

        iSession.close();
        session2.close();
        session2.destroySession();
    }

    /**
     * Test connection close.
     * 1. Test close not open connection.
     * 2. Close connection. JavaStorage cannot be read after that i.e.
     *    StorageException is thrown.
     * 3. Test two consecutive close.
     */
    public void testClose()
    {
        // 1. Close not open.
        try
        {
            iSession.close();
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("Close not open: " + t.toString(), false);
        }

        // 2. Read not open.
        try
        {
            StorageEntry[] entry = iSession.search(
                                       JAVA_DATABASE_NAME, iJtu.getDefaultEntry());

            assertTrue("Read closed connection did not thrown exp.", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }

        iSession = StorageFactory.createSession();

        // 3. Two closes.
        try
        {
            iSession.close();
            iSession.close();
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("Two closes: " + t.toString(), false);

        }
    }
}
