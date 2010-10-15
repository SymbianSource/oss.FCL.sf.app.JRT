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
 * StorageSession write test cases. See test methods for test case details.
 */
public class TestWrite extends TestCase implements UnitTestSuiteCreator, StorageNames
{
    private StorageSession iSession = null;
    private StorageSessionTestUtils iJtu = null;

    public TestSuite createTestSuite(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestWrite("testWriteArguments", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestWrite)tc).testWriteArguments();
            }
        }));

        suite.addTest(new TestWrite("testWrite", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestWrite)tc).testWrite();
            }
        }));

        return suite;
    }

    public TestWrite()
    {
    }

    public TestWrite(String aTestName, TestMethod aTestMethod)
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
     * Test write arguments.
     * 1. Test empty tableName throws SE.
     * 2. Test null tableName throws SE.
     * 3. Test empty entry throws SE.
     * 4. Test null entry throws SE.
     * 5. Test non-existent table throws SE.
     * 6. Test write non-existent entry throws SE.
     * 7. Test write no connection throws SE.
     */
    public void testWriteArguments()
    {
        String tableName = null;
        StorageEntry entry = null;
        String appUid = null;

        // 1. Test empty tableName throws SE.
        try
        {
            iSession.open();
            tableName = "";
            appUid = "Not_Used";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(ID, appUid));

            entry.addAttribute(
                new StorageAttribute(VENDOR, "TestWrite"));

            iSession.write(tableName, entry);
            assertTrue("No exp. when empty table name", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp1: " + t.toString(), false);
        }

        iSession.close();

        // 2. Test null tableName throws SE.
        try
        {
            iSession.open();
            tableName = null;
            appUid = "Not_Used";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(ID, appUid));

            entry.addAttribute(
                new StorageAttribute(VENDOR, "TestWrite"));

            iSession.write(tableName, entry);
            assertTrue("No exp. when null table name", false);
        }
        catch (StorageException se)
        {
            // PASSSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp2: " + t.toString(), false);
        }

        iSession.close();

        // 3. Test empty entry throws SE.
        try
        {
            iSession.open();
            tableName = APPLICATION_PACKAGE_TABLE;
            entry = new StorageEntry();

            iSession.write(tableName, entry);
            assertTrue("No exp. when empty entry", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp3: " + t.toString(), false);
        }

        iSession.close();

        // 4. Test null entry throws SE.
        try
        {
            iSession.open();
            tableName = APPLICATION_PACKAGE_TABLE;
            entry = null;

            iSession.write(tableName, entry);
            assertTrue("No exp. when null entry", false);
        }
        catch (StorageException se)
        {
            // PASSSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp4: " + t.toString(), false);
        }

        iSession.close();

        // 5. Test non-existent table throws SE.
        try
        {
            iSession.open();
            tableName = "FOO_BAR";
            appUid = "Not_Used";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(ID, appUid));

            entry.addAttribute(
                new StorageAttribute(VENDOR, "TestWrite"));

            iSession.write(tableName, entry);
            assertTrue("No exp. when non-existent table", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp5: " + t.toString(), false);
        }

        iSession.close();

        // 6. Test write non-existent entry throws SE.
        try
        {
            iSession.open();
            tableName = APPLICATION_PACKAGE_TABLE;
            appUid = "Not_Used";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(ID, appUid));

            entry.addAttribute(
                new StorageAttribute(VENDOR, "TestWrite"));

            entry.addAttribute(
                new StorageAttribute("FOO_BAR", "FOO_BAR_VALUE"));

            iSession.write(tableName, entry);
            assertTrue("No exp. when non-existent entry", false);
        }
        catch (StorageException se)
        {
            // PASSSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp6: " + t.toString(), false);
        }

        iSession.close();

        // 7. Test write no connection throws SE.
        try
        {
            tableName = APPLICATION_PACKAGE_TABLE;
            appUid = "Not_Used";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(ID, appUid));

            entry.addAttribute(
                new StorageAttribute(VENDOR, "TestWrite"));

            iSession.write(tableName, entry);
            assertTrue("No exp. when no connection", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp2: " + t.toString(), false);
        }
    }

    /**
     * Test Write.
     * 1. Test data is written to database.
     * 2. Test write one entry.
     * 3. Test write whole table.
     * 4. Test write string to INT entry. SE must be thrown.
     * 5. Test write entry with existing primaryKey. SE must be thrown.
     * 6. Test write two same entries. First one is stored.
     * 7. Test write empty value.
     * 8. Test write null value.
     */
    public void testWrite()
    {
        String tableName = APPLICATION_PACKAGE_TABLE;
        StorageEntry entry = null;
        String appUid = null;

        // 1. Test data is written to database.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestWrite1";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(ID, appUid));

            entry.addAttribute(
                new StorageAttribute(VENDOR, "SuiteVenor"));

            iSession.write(tableName, entry);

            assertTrue("Check failed", iJtu.checkParams(iSession,
                       tableName,
                       entry,
                       1));
            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp1: " + t.toString(), false);
        }

        iSession.close();

        // 2. Test write one entry.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestWrite2";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(ID, appUid));

            iSession.write(tableName, entry);

            assertTrue("Check failed", iJtu.checkParams(iSession,
                       tableName,
                       entry,
                       1));
            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp1: " + t.toString(), false);
        }

        iSession.close();

        // 3. Test write whole table.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestWrite3";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(ID, appUid));

            entry.addAttribute(
                new StorageAttribute(PACKAGE_NAME, "WriteSuite"));
            entry.addAttribute(
                new StorageAttribute(VENDOR, "WriteVenor"));
            entry.addAttribute(
                new StorageAttribute(VERSION, "1.2.3.4.5.6"));
            entry.addAttribute(
                new StorageAttribute(ROOT_PATH, "/Write/Root"));
            entry.addAttribute(
                new StorageAttribute(MEDIA_ID,
                                     "12345",
                                     StorageAttribute.INT_TYPE));
            entry.addAttribute(
                new StorageAttribute(INITIAL_SIZE,
                                     "12",
                                     StorageAttribute.INT_TYPE));
            entry.addAttribute(
                new StorageAttribute(JAD_PATH, "/write/jad"));
            entry.addAttribute(
                new StorageAttribute(JAR_PATH, "/write/jar"));
            entry.addAttribute(
                new StorageAttribute(ACCESS_POINT, "writePoint"));

            iSession.write(tableName, entry);

            assertTrue("Check failed", iJtu.checkParams(iSession,
                       tableName,
                       entry,
                       1));
            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp3: " + t.toString(), false);
        }

        // Currently this deletes instance.
        iSession.close();
        iSession = StorageFactory.createSession();

        // 4. Test write string to INT entry. SE must be thrown.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestWrite4";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(ID, appUid));

            // This is originally INT_TYPE.
            entry.addAttribute(
                new StorageAttribute(INITIAL_SIZE,
                                     "HellouHellouHellou",
                                     StorageAttribute.INT_TYPE));

            iSession.write(tableName, entry);
            assertTrue("Writing String to Int do not throw exp", false);
        }
        catch (StorageException se)
        {
            // PASSED
            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp4: " + t.getMessage(), false);
        }

        iSession.close();

        // 5. Test write entry with existing primaryKey. SE must be thrown.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestWrite5";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(ID, appUid));

            iSession.write(tableName, entry);
            iSession.write(tableName, entry);
        }
        catch (StorageException se)
        {
            // PASSED
            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp5: " + t.toString(), false);
        }

        iSession.close();

        // 6. Test write two same entries. First one is taken care.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestWrite6";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(ID, appUid));
            entry.addAttribute(
                new StorageAttribute(JAD_PATH, "/write/jad"));
            entry.addAttribute(
                new StorageAttribute(JAD_PATH,
                                     "/write/secondJAD"));

            iSession.write(tableName, entry);

            // Remove second attribute that we can compare to first entry
            entry.removeAttribute(new StorageAttribute(JAD_PATH,
                                  "/write/secondJAD"));

            assertTrue("Comparing",
                       iJtu.checkParams(iSession, tableName, entry, 1));
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp6: " + t.toString(), false);
        }

        iSession.close();

        // 7. Test write empty value. Entries are written to storage.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestWrite1";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(ID, appUid));

            entry.addAttribute(
                new StorageAttribute(VENDOR, ""));

            iSession.write(tableName, entry);

            // New entry must be created for search. If VENDOR is empty it is
            // assed as attributes to be returned whereas ID is used as search
            // pattern.
            entry.removeAll();
            entry.addAttribute(
                new StorageAttribute(ID, appUid));
            assertTrue("Check failed7", iJtu.checkParams(iSession,
                       tableName,
                       entry,
                       1));
            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp7: " + t.toString(), false);
        }

        iSession.close();

        // 8. Test write null value. Entries are written to storage.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestWrite1";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(ID, appUid));

            String value = null;
            entry.addAttribute(
                new StorageAttribute(
                    VENDOR, value, StorageAttribute.NULL_TYPE));

            iSession.write(tableName, entry);

            // New entry must be created for search. If VENDOR is empty it is
            // assed as attributes to be returned whereas ID is used as search
            // pattern.
            entry.removeAll();
            entry.addAttribute(
                new StorageAttribute(ID, appUid));
            assertTrue("Check failed8", iJtu.checkParams(iSession,
                       tableName,
                       entry,
                       1));
            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp8: " + t.toString(), false);
        }

        iSession.close();
    }
}
