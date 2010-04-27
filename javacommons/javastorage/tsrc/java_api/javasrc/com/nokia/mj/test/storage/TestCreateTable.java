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
import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * StorageSession write test cases. See test methods for test case details.
 */
public class TestCreateTable extends TestCase implements InstallerMain
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

        suite.addTest(new TestCreateTable("TestCreateTableArguments", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCreateTable)tc).TestCreateTableArguments();
            }
        }));

        suite.addTest(new TestCreateTable("TestCreateTable", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCreateTable)tc).TestCreateTable();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestCreateTable()
    {
    }

    public TestCreateTable(String aTestName, TestMethod aTestMethod)
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
     * Test create table arguments.
     * 1. Test empty tableName throws SE.
     * 2. Test null tableName throws SE.
     * 3. Test empty entry and null attribute throws SE.
     * 4. Test null entry and null attribute throws SE.
     * 5. Test create no connection throws SE.
     */
    public void TestCreateTableArguments()
    {
        String tableName = null;
        StorageEntry entry = null;
        StorageAttribute attr =
            new StorageAttribute(StorageAttribute.NAME, "");
        String appUid = null;

        // 1. Test empty tableName throws SE.
        try
        {
            iSession.open();
            tableName = "";
            appUid = "Not_Used";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(StorageAttribute.ID, appUid));

            entry.addAttribute(
                new StorageAttribute(StorageAttribute.VENDOR, ""));

            iSession.createTable(tableName, entry, attr);
            assertTrue("No exp. when empty table name", false);
        }
        catch (StorageException se)
        {
            // PASSSED
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
                new StorageAttribute(StorageAttribute.ID, appUid));

            entry.addAttribute(
                new StorageAttribute(StorageAttribute.VENDOR, ""));

            iSession.createTable(tableName, entry, attr);
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

        // 3. Test empty entry and null attr throws SE.
        try
        {
            iSession.open();
            tableName = "NewTable1";
            entry = new StorageEntry();
            StorageAttribute nullAttr = null;

            iSession.createTable(tableName, entry, nullAttr);
            assertTrue("No exp. when empty entry", false);
        }
        catch (StorageException se)
        {
            // PASSSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp3: " + t.toString(), false);
        }

        iSession.close();

        // 4. Test null entry and null attr throws SE.
        try
        {
            iSession.open();
            tableName = "NewTable2";
            entry = null;
            StorageAttribute nullAttr = null;

            iSession.createTable(tableName, entry, nullAttr);
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

        // 5. Test createTable no connection throws SE.
        try
        {
            tableName = "NewTable3";
            appUid = "Not_Used";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(StorageAttribute.ID, appUid));

            entry.addAttribute(
                new StorageAttribute(StorageAttribute.VENDOR,
                                     ""));

            iSession.createTable(tableName, entry, attr);
            assertTrue("No exp. when no connection", false);
        }
        catch (StorageException se)
        {
            // PASSSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp5: " + t.toString(), false);
        }
    }

    /**
     * Test create table.
     * 1. Test create table.
     * 2. Test create one column.
     * 3. Test create int and string columns.
     * 4. Test create existing table. SE is thrown.
     * 5. Test create primary key String type.
     * 6. Test create primary key Int type.
     * 7. Test create only primary key.
     * 8. Test create invalid attribute. Name contains '-char
     *    StorageException is thrown.
     */
    public void TestCreateTable()
    {
        // 1. Test create table.
        try
        {
            iSession.open();
            iSession.startTransaction();

            String tableName = "MY_FIRST_JAVA_TABLE";
            String appUid = "I am Created!";
            StorageEntry entry = iJtu.getDefaultEntry();
            StorageAttribute idAttr =
                new StorageAttribute(StorageAttribute.ID, appUid);

            entry.addAttribute(idAttr);

            iSession.createTable(tableName, entry, null);

            // TestUtils creates ID attribute from appUid.
            entry.removeAttribute(idAttr);

            // Check table works
            assertTrue(iJtu.populate(iSession, tableName, appUid, entry));
            assertTrue(iJtu.checkParams(iSession, tableName, entry, 1));
            assertTrue(iJtu.remove(iSession, tableName, appUid, entry));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp1: " + t.toString(), false);
        }

        iSession.close();

        // 2. Test create one column.
        try
        {
            iSession.open();
            iSession.startTransaction();

            String tableName = "MY_Second_JAVA_TABLE";
            String appUid = "Thou Bee!";
            StorageEntry entry = new StorageEntry();
            StorageAttribute attribute =
                new StorageAttribute("GreatColumn", "Has Great Value!");

            entry.addAttribute(attribute);

            iSession.createTable(tableName, entry, null);

            // Check table works
            iSession.write(tableName, entry);
            StorageEntry[] foundEntries = iSession.search(tableName, entry);

            assertTrue(foundEntries != null);
            assertTrue(foundEntries.length == 1);

            iSession.remove(tableName, entry);
            foundEntries = iSession.search(tableName, entry);

            assertTrue(foundEntries == null);

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp2: " + t.toString(), false);
        }

        iSession.close();

        // 3. Test create int and string columns.
        try
        {
            iSession.open();
            iSession.startTransaction();

            String tableName = "third";
            String appUid = "Bird";
            StorageEntry entry = new StorageEntry();

            entry.addAttribute(new StorageAttribute(
                                   "inventedOne",
                                   "Has no value",
                                   StorageAttribute.STRING_TYPE));

            entry.addAttribute(new StorageAttribute(
                                   "INT_ONE",
                                   "18",
                                   StorageAttribute.INT_TYPE));

            iSession.createTable(tableName, entry, null);

            // Check table works
            iSession.write(tableName, entry);
            StorageEntry[] foundEntries = iSession.search(tableName, entry);

            assertTrue(foundEntries != null);
            assertTrue(foundEntries.length == 1);

            iSession.remove(tableName, entry);
            foundEntries = iSession.search(tableName, entry);

            assertTrue(foundEntries == null);

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp3: " + t.toString(), false);
        }

        iSession.close();

        // 4. Test create existing table. SE is thrown.
        try
        {
            iSession.open();
            iSession.startTransaction();

            String tableName = "FeelGood";
            String appUid = "Again";
            StorageEntry entry = new StorageEntry();

            entry.addAttribute(new StorageAttribute(
                                   "inventedOne",
                                   "Has no value",
                                   StorageAttribute.STRING_TYPE));

            iSession.createTable(tableName, entry, null);
            iSession.createTable(tableName, entry, null);
            assertTrue("Trying to create same table again. No Exp.", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp4: " + t.toString(), false);
        }

        iSession.close();

        // 5. Test create primary key String type.
        try
        {
            iSession.open();
            iSession.startTransaction();

            String tableName = "PrimaryKeyTable";
            String appUid = "OnlyOne";
            StorageEntry entry = iJtu.getDefaultEntry();
            StorageAttribute idAttr =
                new StorageAttribute(StorageAttribute.ID, appUid);

            entry.addAttribute(idAttr);

            StorageAttribute primaryKey =
                new StorageAttribute("XXX", "", StorageAttribute.STRING_TYPE);

            iSession.createTable(tableName, entry, primaryKey);

            // TestUtils creates ID attribute from appUid.
            entry.removeAttribute(idAttr);

            StorageAttribute primValue =
                new StorageAttribute("XXX", "PrimaryKeyValue");
            entry.addAttribute(primValue);

            // Check table works
            assertTrue("Populate",
                       iJtu.populate(iSession, tableName, appUid, entry));
            assertTrue("Check defs",
                       iJtu.checkParams(iSession, tableName, entry, 1));

            // verify that first entry is primaryKey i.e. cannot write with
            // same value again.
            StorageEntry primaryEntry = new StorageEntry();
            primaryEntry.addAttribute(entry.getAttribute("XXX"));
            assertTrue("Populate primaryKey",
                       iJtu.populate(iSession,
                                     tableName,
                                     appUid,
                                     primaryEntry) == false);

            // But with different one it is ok.
            primaryEntry = new StorageEntry();
            primaryEntry.addAttribute(new StorageAttribute(
                                          "XXX", "NewPrimaryKey"));
            assertTrue("Popuplate new primaryKey",
                       iJtu.populate(iSession, tableName, appUid, primaryEntry));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp5: " + t.toString(), false);
        }

        iSession.close();

        // 6. Test create primary key Int type.
        try
        {
            iSession.open();
            iSession.startTransaction();

            String tableName = "PrimaryKeyINTTable";
            String appUid = "OnlyOne";
            StorageEntry entry = iJtu.getDefaultEntry();
            StorageAttribute idAttr =
                new StorageAttribute(StorageAttribute.ID, appUid);

            entry.addAttribute(idAttr);

            StorageAttribute primaryKey =
                new StorageAttribute("XXX", "", StorageAttribute.INT_TYPE);

            iSession.createTable(tableName, entry, primaryKey);

            // TestUtils creates ID attribute from appUid.
            entry.removeAttribute(idAttr);

            StorageAttribute primValue =
                new StorageAttribute("XXX", "112", StorageAttribute.INT_TYPE);
            entry.addAttribute(primValue);

            // Check table works
            assertTrue("Populate",
                       iJtu.populate(iSession, tableName, appUid, entry));
            assertTrue("Check defs",
                       iJtu.checkParams(iSession, tableName, entry, 1));

            // verify that first entry is primaryKey i.e. cannot write with
            // same value again.
            StorageEntry primaryEntry = new StorageEntry();
            primaryEntry.addAttribute(entry.getAttribute("XXX"));
            assertTrue("Populate primaryKey",
                       iJtu.populate(iSession,
                                     tableName,
                                     appUid,
                                     primaryEntry) == false);

            // But with different one it is ok.
            primaryEntry = new StorageEntry();
            primaryEntry.addAttribute(new StorageAttribute(
                                          "XXX", "118", StorageAttribute.INT_TYPE));
            assertTrue("Popuplate new primaryKey",
                       iJtu.populate(iSession, tableName, appUid, primaryEntry));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp6: " + t.toString(), false);
        }

        iSession.close();

        // 7. Test create only primary key.
        try
        {
            iSession.open();
            iSession.startTransaction();
            StorageEntry entry = new StorageEntry();
            String tableName = "OnlyPrimKey";

            StorageAttribute primaryKey =
                new StorageAttribute("XXX", "");
            iSession.createTable(tableName, null, primaryKey);

            StorageAttribute primValue =
                new StorageAttribute("XXX", "112");
            entry.addAttribute(primValue);

            // Check table works
            iSession.write(tableName, entry);

            assertTrue("Check defs",
                       iJtu.checkParams(iSession, tableName, entry, 1));

            try
            {
                iSession.write(tableName, entry); // Write primary key again.
                assertTrue("Was not primary key", false);
            }
            catch (StorageException se)
            {
                // PASSED
            }

            // But with different one it is ok.
            StorageEntry primaryEntry = new StorageEntry();
            primaryEntry.addAttribute(new StorageAttribute(
                                          "XXX", "118"));

            try
            {
                iSession.write(tableName, primaryEntry);
            }
            catch (StorageException se)
            {
                assertTrue("Write failed", false);
            }

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp7: " + t.toString(), false);
        }

        iSession.close();

        // 8. Test create invalid attribute.
        // Name is VARCHAR StorageException is thrown.
        try
        {
            iSession.open();
            iSession.startTransaction();

            String tableName = "FeelGood";
            String appUid = "Again";
            StorageEntry entry = new StorageEntry();

            entry.addAttribute(new StorageAttribute(
                                   "inventedOne",
                                   "Has no value",
                                   StorageAttribute.STRING_TYPE));

            entry.addAttribute(new StorageAttribute(
                                   "I'm invalid",
                                   "",
                                   StorageAttribute.STRING_TYPE));

            iSession.createTable(tableName, entry, null);
            assertTrue("No exp. when invalid name.", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp8: " + t.toString(), false);
        }

        iSession.close();
    }
}
