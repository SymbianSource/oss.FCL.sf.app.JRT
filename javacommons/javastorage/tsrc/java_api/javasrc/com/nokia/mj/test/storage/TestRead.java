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

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.utils.Uid;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * StorageSession read test cases. See test methods for test case details.
 */
public class TestRead extends TestCase implements InstallerMain, StorageNames
{
    /**
     * Directory for JavaStorage tests.
     */
    private static final String iTestRoot = "./jstest";

    /**
     * Directory for JavaStorage journal and temp files.
     */
    private static final String iIsRoot = iTestRoot + "/js";

    private StorageSession iSession = null;
    private StorageSessionTestUtils iJtu = null;

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestRead("testReadEntryArguments", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRead)tc).testReadEntryArguments();
            }
        }));

        suite.addTest(new TestRead("testReadEntriesArguments", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRead)tc).testReadEntriesArguments();
            }
        }));

        suite.addTest(new TestRead("testReadEntry", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRead)tc).testReadEntry();
            }
        }));

        suite.addTest(new TestRead("testReadEntries", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRead)tc).testReadEntries();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestRead()
    {
    }

    public TestRead(String aTestName, TestMethod aTestMethod)
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
     * Test readEntry arguments.
     * 1. Test empty tableName throws SE.
     * 2. Test null tableName throws SE.
     * 3. Test null Uid throws SE.
     * 4. Test non-existent table throws SE.
     * 5. Test read no connection throws SE.
     */
    public void testReadEntryArguments()
    {
        String tableName = APPLICATION_PACKAGE_TABLE;

        StorageEntry entry = null;
        Uid uid = Uid.createUid("TestReadArgs1");

        // 1. Test empty tableName throws SE.
        try
        {
            iSession.open();
            String emptyName = "";

            entry = iSession.readEntry(emptyName, uid);
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
            String nullName = null;

            entry = iSession.readEntry(nullName, uid);
            assertTrue("No exp. when null table name", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp2: " + t.toString(), false);
        }

        iSession.close();

        // 3. Test null Uid throws SE.
        try
        {
            // Returns null.
            Uid emptyUid = Uid.createUid("");
            iSession.open();

            entry = iSession.readEntry(tableName, emptyUid);
            assertTrue("No exp. when null Uid", false);
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

        // 4. Test non-existent table throws SE.
        try
        {
            iSession.open();
            String nonExisting = "FOO_BAR_TABLE";

            entry = iSession.readEntry(nonExisting, uid);
            assertTrue("No exp. when non existing table", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp4: " + t.toString(), false);
        }

        iSession.close();

        // 5. Test read no connection throws SE.
        try
        {

            entry = iSession.readEntry(tableName, uid);
            assertTrue("No exp. when no connection", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp5: " + t.toString(), false);
        }
    }
    /**
     * Test readEntries arguments.
     * 1. Test empty tableName throws SE.
     * 2. Test null tableName throws SE.
     * 3. Test null Uid throws SE.
     * 4. Test non-existent table throws SE.
     * 5. Test read no connection throws SE.
     */
    public void testReadEntriesArguments()
    {
        String tableName = APPLICATION_PACKAGE_TABLE;

        StorageEntry[] entry = null;
        Uid uid = Uid.createUid("TestReadArgs1");

        // 1. Test empty tableName throws SE.
        try
        {
            iSession.open();
            String emptyName = "";

            entry = iSession.readEntries(emptyName, uid);
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
            String nullName = null;

            entry = iSession.readEntries(nullName, uid);
            assertTrue("No exp. when null table name", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp2: " + t.toString(), false);
        }

        iSession.close();

        // 3. Test null Uid throws SE.
        try
        {
            // Returns null.
            Uid emptyUid = Uid.createUid("");
            iSession.open();

            entry = iSession.readEntries(tableName, emptyUid);
            assertTrue("No exp. when null Uid", false);
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

        // 4. Test non-existent table throws SE.
        try
        {
            iSession.open();
            String nonExisting = "FOO_BAR_TABLE";

            entry = iSession.readEntries(nonExisting, uid);
            assertTrue("No exp. when non existing table", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp4: " + t.toString(), false);
        }

        iSession.close();

        // 5. Test read no connection throws SE.
        try
        {
            entry = iSession.readEntries(tableName, uid);
            assertTrue("No exp. when no connection", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp5: " + t.toString(), false);
        }
    }

    /**
     * Test readEntry.
     * 1. Test readEntry no data in database.
     * 2. Test readEntry one column.
     * 3. Test readEntry multiple columns.
     * 4. Test readEntry no matching entry.
     * 5. Test multple matches. This is possible when Uid is not primary key
     *    of the table.
     * 6. Test readEntry empty column between.
     */
    public void testReadEntry()
    {
        String tableName = APPLICATION_PACKAGE_TABLE;

        StorageEntry entry = null;
        Uid uid = Uid.createUid("TestRead1");

        // 1. Test readEntry no data in database.
        try
        {
            iSession.open();

            entry = iSession.readEntry(tableName, uid);
            assertTrue("Not null", entry == null);
        }
        catch (Throwable t)
        {
            assertTrue("UnExp exp1: " + t.toString(), false);
        }

        iSession.close();

        // 2. Test readEntry one column.
        try
        {
            iSession.open();
            StorageEntry populateEntry = new StorageEntry();
            StorageAttribute idAttr = new StorageAttribute(ID, "TestRead1");
            populateEntry.addAttribute(idAttr);

            iSession.startTransaction();

            assertTrue(iJtu.populate(
                           iSession, tableName, null, populateEntry));

            entry = iSession.readEntry(tableName, uid);
            assertTrue("Entry null", entry != null);
            assertTrue("Attr not match",
                       entry.getAttribute(ID).equals(idAttr));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExp exp2: " + t.toString(), false);
        }

        iSession.close();

        // 3. Test readEntry multiple columns.
        try
        {
            Uid uid2 = Uid.createUid("TestRead2");

            iSession.open();
            StorageEntry populateEntry = iJtu.createAppPackageEntry();
            StorageAttribute idAttr = new StorageAttribute(ID,
                    uid2.getStringValue());
            populateEntry.addAttribute(idAttr);

            iSession.startTransaction();

            assertTrue(iJtu.populate(
                           iSession, tableName, null, populateEntry));

            entry = iSession.readEntry(tableName, uid2);
            assertTrue("Entry null", entry != null);
            assertTrue("Attr not match",
                       iJtu.compareEntry(populateEntry, entry));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExp exp3: " + t.toString(), false);
        }

        iSession.close();

        // 4. Test readEntry no matching entry.
        try
        {
            Uid uid3 = Uid.createUid("TestRead3");
            Uid noExists = Uid.createUid("I Do Not Exists");

            iSession.open();

            StorageEntry populateEntry = iJtu.createAppPackageEntry();
            StorageAttribute idAttr = new StorageAttribute(ID,
                    uid3.getStringValue());
            populateEntry.addAttribute(idAttr);

            iSession.startTransaction();

            assertTrue(iJtu.populate(
                           iSession, tableName, null, populateEntry));

            entry = iSession.readEntry(tableName, noExists);
            assertTrue("Entry not null", entry == null);

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExp exp4: " + t.toString(), false);
        }

        iSession.close();

        // 5. Test multple matches. This is possible when Uid is not primary
        //    key of the table.
        try
        {
            String pushTable = PUSH_REGISTRATIONS_TABLE;
            Uid uid5 = Uid.createUid("TestRead5");

            iSession.open();

            StorageEntry populateEntry1 = new StorageEntry();
            populateEntry1.addAttribute(new StorageAttribute(ID,
                                        uid5.getStringValue()));
            populateEntry1.addAttribute(new StorageAttribute(URL,
                                        "http://geeks.com"));
            populateEntry1.addAttribute(new StorageAttribute(NAME,
                                        "Do not push me!"));
            populateEntry1.addAttribute(new StorageAttribute(FILTER,
                                        "*;Blacklist=*me.com"));
            populateEntry1.addAttribute(new StorageAttribute(REGISTRATION_TYPE,
                                        "1", StorageAttribute.INT_TYPE));

            StorageEntry populateEntry2 = new StorageEntry();
            populateEntry2.addAttribute(new StorageAttribute(ID,
                                        uid5.getStringValue()));
            populateEntry2.addAttribute(new StorageAttribute(URL,
                                        "http://www.notify.me.com"));
            populateEntry2.addAttribute(new StorageAttribute(NAME,
                                        "Auto notification"));
            populateEntry2.addAttribute(new StorageAttribute(FILTER,
                                        "*;Whitelist=*allow.me.com"));
            populateEntry2.addAttribute(new StorageAttribute(REGISTRATION_TYPE,
                                        "2", StorageAttribute.INT_TYPE));

            iSession.startTransaction();

            assertTrue(iJtu.populate(
                           iSession, pushTable, null, populateEntry1));
            assertTrue(iJtu.populate(
                           iSession, pushTable, null, populateEntry2));

            entry = iSession.readEntry(pushTable, uid5);
            assertTrue("Entry null", entry != null);
            assertTrue("Invalid size", entry.size() == populateEntry1.size());
            assertTrue("Invalid attr",
                       entry.getAttribute(ID).equals(
                           populateEntry1.getAttribute(ID)));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExp exp5: " + t.toString(), false);
        }

        iSession.close();

        //  6. Test readEntry empty column between.
        try
        {
            String pushTable = PUSH_REGISTRATIONS_TABLE;
            Uid uid6 = Uid.createUid("TestRead6");

            iSession.open();

            StorageEntry populateEntry1 = new StorageEntry();

            populateEntry1.addAttribute(new StorageAttribute(ID,
                                        uid6.getStringValue()));
            populateEntry1.addAttribute(new StorageAttribute(URL,
                                        "http://geeks.com"));
            populateEntry1.addAttribute(new StorageAttribute(NAME,
                                        ""));
            populateEntry1.addAttribute(new StorageAttribute(FILTER,
                                        "*;Blacklist=*me.com"));
            populateEntry1.addAttribute(new StorageAttribute(REGISTRATION_TYPE,
                                        "1", StorageAttribute.INT_TYPE));

            iSession.startTransaction();

            assertTrue(iJtu.populate(
                           iSession, pushTable, null, populateEntry1));

            entry = iSession.readEntry(pushTable, uid6);
            assertTrue("Entry null", entry != null);
            assertTrue("Invalid size", entry.size() == populateEntry1.size());
            assertTrue("Attr not match",
                       iJtu.compareEntry(populateEntry1, entry));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExp exp6: " + t.toString(), false);
        }
        iSession.close();
    }

    /**
     * Test readEntries.
     * 1. Test readEntries no data in database.
     * 2. Test readEntries one match.
     * 3. Test readEntries multiple maches.
     * 4. Test readEntries no matching entries.
     */
    public void testReadEntries()
    {
        String tableName = PUSH_REGISTRATIONS_TABLE;

        StorageEntry[] entries = null;
        Uid uid = Uid.createUid("TestReadEntries1");

        // 1. Test readEntries no data in database.
        try
        {
            iSession.open();

            entries = iSession.readEntries(tableName, uid);
            assertTrue("Not null", entries == null);
        }
        catch (Throwable t)
        {
            assertTrue("UnExp exp1: " + t.toString(), false);
        }

        iSession.close();

        // 2. Test readEntries one match.
        try
        {
            iSession.open();
            StorageEntry populateEntry = new StorageEntry();
            populateEntry.addAttribute(new StorageAttribute(ID,
                                       uid.getStringValue()));
            populateEntry.addAttribute(new StorageAttribute(URL,
                                       "http://geeks.com"));
            populateEntry.addAttribute(new StorageAttribute(NAME,
                                       "Work is hard!"));
            populateEntry.addAttribute(new StorageAttribute(FILTER,
                                       "*;Blacklist=*me.com"));
            populateEntry.addAttribute(new StorageAttribute(REGISTRATION_TYPE,
                                       "1", StorageAttribute.INT_TYPE));

            iSession.startTransaction();

            assertTrue(iJtu.populate(
                           iSession, tableName, null, populateEntry));

            entries = iSession.readEntries(tableName, uid);
            assertTrue("Entry null", entries != null);
            assertTrue("Entry null", entries.length == 1);

            assertTrue("Entry does not match",
                       iJtu.compareEntry(populateEntry, entries[0]));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExp exp2: " + t.toString(), false);
        }

        iSession.close();

        // 3. Test readEntries multiple maches.
        try
        {
            iSession.open();
            StorageEntry[] refEntries = new StorageEntry[2];
            StorageEntry populateEntry1 = new StorageEntry();
            populateEntry1.addAttribute(new StorageAttribute(ID,
                                        uid.getStringValue()));
            populateEntry1.addAttribute(new StorageAttribute(URL,
                                        "http://geeks.com"));
            populateEntry1.addAttribute(new StorageAttribute(NAME,
                                        "Geeks are geeks"));
            populateEntry1.addAttribute(new StorageAttribute(FILTER,
                                        "*;Blacklist=*me.com"));
            populateEntry1.addAttribute(new StorageAttribute(REGISTRATION_TYPE,
                                        "1", StorageAttribute.INT_TYPE));
            refEntries[0] = populateEntry1;

            StorageEntry populateEntry2 = new StorageEntry();
            populateEntry2.addAttribute(new StorageAttribute(ID,
                                        uid.getStringValue()));
            populateEntry2.addAttribute(new StorageAttribute(URL,
                                        "http://blocked.com"));
            populateEntry2.addAttribute(new StorageAttribute(NAME,
                                        "Really Blocked"));
            populateEntry2.addAttribute(new StorageAttribute(FILTER,
                                        "*;Blacklist=*not.me.com"));
            populateEntry2.addAttribute(new StorageAttribute(REGISTRATION_TYPE,
                                        "2", StorageAttribute.INT_TYPE));
            refEntries[1] = populateEntry2;

            iSession.startTransaction();

            assertTrue(iJtu.populate(
                           iSession, tableName, null, populateEntry1));
            assertTrue(iJtu.populate(
                           iSession, tableName, null, populateEntry2));

            entries = iSession.readEntries(tableName, uid);
            assertTrue("Entry null", entries != null);
            assertTrue("Entry null", entries.length == 2);
            assertTrue("Entries does not match",
                       iJtu.compareEntries(refEntries, entries));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExp exp3: " + t.toString(), false);
        }

        iSession.close();

        // 4. Test readEntries no matching entries.
        try
        {
            Uid noMatch = Uid.createUid("No match at all");

            iSession.open();
            StorageEntry populateEntry = new StorageEntry();
            populateEntry.addAttribute(new StorageAttribute(ID,
                                       uid.getStringValue()));
            populateEntry.addAttribute(new StorageAttribute(URL,
                                       "http://geeks.com"));
            populateEntry.addAttribute(new StorageAttribute(NAME,
                                       "MyNameIsName"));
            populateEntry.addAttribute(new StorageAttribute(FILTER,
                                       "*;Blacklist=*me.com"));
            populateEntry.addAttribute(new StorageAttribute(REGISTRATION_TYPE,
                                       "1", StorageAttribute.INT_TYPE));

            iSession.startTransaction();

            assertTrue(iJtu.populate(
                           iSession, tableName, null, populateEntry));

            entries = iSession.readEntries(tableName, noMatch);
            assertTrue("Entry not null", entries == null);

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExp exp4: " + t.toString(), false);
        }

        iSession.close();
    }
}
