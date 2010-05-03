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
import com.nokia.mj.impl.utils.Uid;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * StorageSession remove test cases. See test methods for test case details.
 */
public class TestRemove extends TestCase implements InstallerMain, StorageNames
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

        suite.addTest(new TestRemove("testRemoveArguments", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRemove)tc).testRemoveArguments();
            }
        }));

        suite.addTest(new TestRemove("testRemove", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRemove)tc).testRemove();
            }
        }));

        suite.addTest(new TestRemove("testRemoveByUid", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRemove)tc).testRemoveByUid();
            }
        }));

        suite.addTest(new TestRemove("testRemoveByInt", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRemove)tc).testRemoveByInt();
            }
        }));

        suite.addTest(new TestRemove("testRemoveAll", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRemove)tc).testRemoveAll();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestRemove()
    {
    }

    public TestRemove(String aTestName, TestMethod aTestMethod)
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
            catch (Throwable se)
            {
                // No can do
                System.out.println("TearDown failed: " + se.toString());
            }
        }
    }

    /**
     * Test remove arguments.
     * 1. Test empty tableName throws SE.
     * 2. Test null tableName throws SE.
     * 3. Test empty entry does not throw SE. Remove all.
     * 4. Test null entry does not throw SE. Remove all.
     * 5. Test non-existent table throws SE.
     * 6. Test remove non-existent entry throws SE.
     * 7. Test remove no connection throws SE.
     */
    public void testRemoveArguments()
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
                new StorageAttribute(VENDOR, "TestRemove"));

            iSession.remove(tableName, entry);
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
                new StorageAttribute(VENDOR, "TestRemove"));

            iSession.remove(tableName, entry);
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

        // 3. Test empty entry throws SE.
        try
        {
            iSession.open();
            tableName = APPLICATION_PACKAGE_TABLE;
            entry = new StorageEntry();
            // Remove all.
            iSession.remove(tableName, entry);
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

            // Remove all.
            iSession.remove(tableName, entry);
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
                new StorageAttribute(VENDOR, "TestRemove"));

            iSession.remove(tableName, entry);
            assertTrue("No exp. when non-existent table", false);
        }
        catch (StorageException se)
        {
            // PASSSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp5: " + t.toString(), false);
        }

        iSession.close();

        // 6. Test search non-existent entry throws SE.
        try
        {
            iSession.open();
            tableName = APPLICATION_PACKAGE_TABLE;
            appUid = "Not_Used";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(ID, appUid));

            entry.addAttribute(
                new StorageAttribute(VENDOR, "TestRemove"));

            entry.addAttribute(
                new StorageAttribute("FOO_BAR", "FOO_BAR_VALUE"));

            iSession.remove(tableName, entry);
            assertTrue("No exp. when non-existent entry", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp6: " + t.toString(), false);
        }

        iSession.close();

        // 7. Test search no connection throws SE.
        try
        {
            tableName = APPLICATION_PACKAGE_TABLE;
            appUid = "Not_Used";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(ID, appUid));

            entry.addAttribute(
                new StorageAttribute(VENDOR, "TestRemove"));

            iSession.remove(tableName, entry);
            assertTrue("No exp. when no connection", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp7: " + t.toString(), false);
        }
    }

    /**
     * Test Remove.
     * 1. Test remove no values in storage. Call is success.
     * 2. Test remove non-existent entry value. Call is success.
     * 3. Test remove pattern. Only matching entries are removed. Test
     *    other ones remain.
     * 4. Test remove pattern that consist of one Not matching entry name.
     *    StorageException is thrown.
     * 5. Test remove pattern that consist of one Not matching entry value.
     *    No entries removed.
     */
    public void testRemove()
    {
        String tableName = APPLICATION_PACKAGE_TABLE;
        StorageEntry entry = null;
        String appUid = null;

        // 1. Test search.
        try
        {
            iSession.open();
            iSession.startTransaction();

            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(VENDOR, "TestRemove"));

            int removed = iSession.remove(tableName, entry);
            assertTrue("Invalid amount of removed entries", 0 == removed);

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp1: " + t.toString(), false);
        }

        iSession.close();

        // 2. Test remove non-existent entry value. Call is success.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestRemove2";

            assertTrue("Populate failed",
                       iJtu.populateWithDefault(iSession, appUid));

            entry = new StorageEntry();

            entry.addAttribute(new StorageAttribute(
                                   PACKAGE_NAME, "/Not/Exists"));

            int removed = iSession.remove(tableName, entry);
            assertTrue("Invalid amount of removed entries", 0 == removed);

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp2: " + t.toString(), false);
        }

        iSession.close();

        // 3. Test remove pattern. Only matching entries are removed. Test
        //    other ones remain.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestRemove3";

            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   JAD_PATH, "/Match/path"));
            entry.addAttribute(new StorageAttribute(
                                   JAR_PATH, "/Match/JAR/path"));

            assertTrue("First entry", iJtu.populate(
                           iSession,
                           tableName,
                           "Entry1",
                           entry));

            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   JAD_PATH, "/Match/path"));
            entry.addAttribute(new StorageAttribute(
                                   JAR_PATH, "/Match/JAR/path"));

            assertTrue("Second entry", iJtu.populate(
                           iSession,
                           tableName,
                           "Entry2",
                           entry));

            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   JAD_PATH, "/Match/path"));
            entry.addAttribute(new StorageAttribute(
                                   JAR_PATH, "/Match/JAR/path"));

            assertTrue("Third entry", iJtu.populate(
                           iSession,
                           tableName,
                           "Entry3",
                           entry));

            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   JAD_PATH, "/No/Match/Path"));
            entry.addAttribute(new StorageAttribute(
                                   JAR_PATH, "/Match/JAR/path"));

            assertTrue("No match entry", iJtu.populate(
                           iSession,
                           tableName,
                           "Entry4",
                           entry));

            StorageEntry query = new StorageEntry();
            query.addAttribute(new StorageAttribute(
                                   JAD_PATH, "/Match/path"));
            query.addAttribute(new StorageAttribute(
                                   JAR_PATH, "/Match/JAR/path"));

            StorageEntry[] foundEntries = iSession.search(tableName, query);
            assertTrue("Found entries", foundEntries.length == 3);

            int removed = iSession.remove(tableName, query);
            assertTrue("Invalid amount of removed entries", 3 == removed);

            // Verify removal
            foundEntries = iSession.search(tableName, query);
            assertTrue("Entries found after removal", foundEntries == null);

            // Verify no match is found
            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   JAD_PATH, "/No/Match/Path"));
            entry.addAttribute(new StorageAttribute(
                                   JAR_PATH, "/Match/JAR/path"));

            foundEntries = iSession.search(tableName, entry);
            assertTrue("Found entries", foundEntries.length == 1);

            assertTrue("Not Match", foundEntries[0]
                       .getAttribute(JAD_PATH)
                       .equals(new StorageAttribute(
                                   JAD_PATH, "/No/Match/Path")));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp3: " + t.getMessage(), false);
        }

        iSession.close();

        //  4. Test remove pattern that consist of one Not matching entry name.
        //     StorageException is thrown.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestRemove4";

            assertTrue("Populate failed",
                       iJtu.populateWithDefault(iSession, appUid));

            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   ID, "Entry1"));
            entry.addAttribute(new StorageAttribute(
                                   "FOO_BAR", "BAR_FOOO"));

            iSession.remove(tableName, entry);

            iSession.rollbackTransaction();

            assertTrue("No exp", false);
        }
        catch (StorageException se)
        {
            // PASSED
            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp4: " + t.toString(), false);
        }

        iSession.close();

        // 5. Test remove pattern that consist of one Not matching entry value.
        //     No entries removed.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestRemove5";

            assertTrue("Populate failed",
                       iJtu.populateWithDefault(iSession, appUid));

            assertTrue("Populate failed",
                       iJtu.populateWithDefault(iSession, "NewEntryy2"));

            // Verify population
            entry = new StorageEntry();

            // Match attribute
            entry.addAttribute(new StorageAttribute(
                                   PACKAGE_NAME, "JavaGame"));

            StorageEntry[] foundEntries = iSession.search(tableName, entry);

            assertTrue("Found entries", foundEntries.length == 2);

            entry = new StorageEntry();
            // Match attribute
            entry.addAttribute(new StorageAttribute(
                                   PACKAGE_NAME, "JavaGame"));

            entry.addAttribute(new StorageAttribute(
                                   JAR_PATH, "/non/matching/path"));

            // Partially matching remove
            int removed = iSession.remove(tableName, entry);
            assertTrue("Invalid amount of removed entries", 0 == removed);

            // Verify no entries removed
            entry = new StorageEntry();

            // Match attribute
            entry.addAttribute(new StorageAttribute(
                                   PACKAGE_NAME, "JavaGame"));

            foundEntries = iSession.search(tableName, entry);

            assertTrue("Found entries", foundEntries.length == 2);

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp5: " + t.toString(), false);
        }

        iSession.close();
    }

    /**
     * Test remove by uid. This is recursive call to remove(table, entry) so
     * there is no need to test all combinations as they're tested alredy
     * by testRemove(table, entry).
     * 1. Test remove null uid. StorageException is thrown.
     * 2. Test remove null tableName. StorageException is thrown.
     * 3. Test remove empty tableName. StorageException is thrown.
     * 4. Test remove by matching uid. Entry removed.
     * 5. Test remove by non-maching uid. Entry not removed.
     */
    public void testRemoveByUid()
    {
        // Test arguments
        String uidStr = null;
        Uid testUid = null;
        // Uid testUid = new Uid(uidStr);

        // 1. Test remove null uid. StorageException is thrown.
        try
        {
            iSession.open();
            String tableName = APPLICATION_PACKAGE_TABLE;

            iSession.remove(tableName, testUid);

            assertTrue("No exp. when null Uid", false);
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

        // 2. Test remove null tableName. StorageException is thrown.
        try
        {
            iSession.open();
            String tableName = null;

            uidStr = "/test/uid";
            testUid = Uid.createUid(uidStr);

            iSession.remove(tableName, testUid);
            assertTrue("No exp. when null tableName", false);
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

        // 4. Test remove empty tableName. StorageException is thrown.
        try
        {
            iSession.open();
            String tableName = "";

            uidStr = "/test/uid";
            testUid = Uid.createUid(uidStr);

            iSession.remove(tableName, testUid);
            assertTrue("No exp. when empty tableName", false);
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

        // 5. Test remove by matching uid. Entry removed.
        // 6. Test remove by non-maching uid. Entry not removed.
        try
        {
            String tableName = APPLICATION_PACKAGE_TABLE;
            iSession.open();
            iSession.startTransaction();

            String appUid = "JavaTestRemove5";
            String appUid2 = "MatchMe2";

            testUid = Uid.createUid(appUid);

            assertTrue("Populate failed",
                       iJtu.populateWithDefault(iSession, testUid.getStringValue()));

            assertTrue("Populate2 failed",
                       iJtu.populateWithDefault(iSession, appUid2));

            assertTrue("Check population", iJtu.checkDefaultParams(
                           iSession, testUid.getStringValue()));

            assertTrue("Check population2", iJtu.checkDefaultParams(
                           iSession, appUid2));

            int removed = iSession.remove(tableName, testUid);
            assertTrue("Invalid amount of removed entries", 1 == removed);

            // Matching removed
            assertTrue("Check population", iJtu.checkDefaultParams(
                           iSession, testUid.getStringValue()) == false);

            // Not matching not removed
            assertTrue("Check population2", iJtu.checkDefaultParams(
                           iSession, appUid2));

            testUid = Uid.createUid(appUid2);

            removed = iSession.remove(tableName, testUid);
            assertTrue("Invalid amount of removed entries", 1 == removed);

            // Matching removed
            assertTrue("Check population2", iJtu.checkDefaultParams(
                           iSession, appUid2) == false);

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp5: " + t.toString(), false);
        }

        iSession.close();
    }

    /**
     * Test interger data search.
     * 1. Test remove by int.
     * 2. Test remove by int and string.
     * 3. Test remove by int no type defined.
     */
    public void testRemoveByInt()
    {
        String tableName = APPLICATION_PACKAGE_TABLE;
        StorageEntry entry = null;

        // 1. Test remove by INT.
        try
        {
            iSession.open();
            iSession.startTransaction();

            entry = new StorageEntry();

            entry.addAttribute(new StorageAttribute(
                                   INITIAL_SIZE,
                                   "1234",
                                   StorageAttribute.INT_TYPE));

            assertTrue("Populate1", iJtu.populate(
                           iSession,
                           tableName,
                           "IntEntry1",
                           entry));

            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 1));

            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   INITIAL_SIZE,
                                   "1234",
                                   StorageAttribute.INT_TYPE));

            int removed = iSession.remove(tableName, entry);
            assertTrue("Invalid amount of removed entries", 1 == removed);

            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 1) == false);

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp1: " + t.toString(), false);
        }
        iSession.close();

        // 2. Test remove by INT and STRING
        try
        {
            iSession.open();
            iSession.startTransaction();

            entry = new StorageEntry();

            entry.addAttribute(new StorageAttribute(
                                   INITIAL_SIZE,
                                   "1234",
                                   StorageAttribute.INT_TYPE));

            assertTrue("Populate", iJtu.populate(
                           iSession,
                           tableName,
                           "IntEntry2",
                           entry));

            // Verify write
            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 1));

            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   INITIAL_SIZE,
                                   "1234",
                                   StorageAttribute.INT_TYPE));

            entry.addAttribute(new StorageAttribute(
                                   ID,
                                   "IntEntry2"));

            int removed = iSession.remove(tableName, entry);
            assertTrue("Invalid amount of removed entries", 1 == removed);

            // Verify removal
            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 1) == false);

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp2: " + t.toString(), false);
        }
        iSession.close();

        // 3. Test remove by INT no type set. Entry found.
        try
        {
            iSession.open();
            iSession.startTransaction();

            entry = new StorageEntry();

            entry.addAttribute(new StorageAttribute(
                                   INITIAL_SIZE,
                                   "1234",
                                   StorageAttribute.INT_TYPE));

            assertTrue("Populate3", iJtu.populate(
                           iSession,
                           tableName,
                           "IntEntry3",
                           entry));

            // Verify write
            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 1));

            entry = new StorageEntry();

            entry.addAttribute(new StorageAttribute(
                                   INITIAL_SIZE,
                                   "1234"));

            int removed = iSession.remove(tableName, entry);
            assertTrue("Invalid amount of removed entries", 1 == removed);

            // Verify removal
            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 1) == false);

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp3: " + t.toString(), false);
        }
        iSession.close();
    }

    /**
     * Test interger data search.
     * 1. Test remove all by null.
     * 2. Test remove all by empty entry.
     */
    public void testRemoveAll()
    {
        /* Result checking is not working in a case deleting all!
         * SQLite does optimizated delete and it is not working row
         * based mode that time --> no affected rows returned.
         * This is fixed SQLite >=3.6.5 versions.
         * CHECK(3 == result);
         */

        String tableName = APPLICATION_PACKAGE_TABLE;
        StorageEntry entry = null;

        // 1. Test remove all by null.
        try
        {
            iSession.open();
            iSession.startTransaction();

            entry = new StorageEntry();

            entry.addAttribute(new StorageAttribute(
                                   INITIAL_SIZE,
                                   "1234",
                                   StorageAttribute.INT_TYPE));

            assertTrue("RemoveAll1", iJtu.populate(
                           iSession,
                           tableName,
                           "RemoveAll1",
                           entry));

            assertTrue("RemoveAll2", iJtu.populate(
                           iSession,
                           tableName,
                           "RemoveAll2",
                           entry));

            assertTrue("RemoveAll3", iJtu.populate(
                           iSession,
                           tableName,
                           "RemoveAll3",
                           entry));

            StorageEntry nullEntry = null;
            int removed = iSession.remove(tableName, nullEntry);

            entry = new StorageEntry();

            entry.addAttribute(new StorageAttribute(
                                   INITIAL_SIZE,
                                   "1234",
                                   StorageAttribute.INT_TYPE));

            // Check none found.
            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 3) == false);
            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 2) == false);
            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 1) == false);
            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 0) == false);

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UExp1: " + t.toString(), false);
        }
        iSession.close();

        // 2. Test remove all by empty entry.
        try
        {
            iSession.open();
            iSession.startTransaction();

            entry = new StorageEntry();

            entry.addAttribute(new StorageAttribute(
                                   INITIAL_SIZE,
                                   "1234",
                                   StorageAttribute.INT_TYPE));

            assertTrue("RemoveAll1", iJtu.populate(
                           iSession,
                           tableName,
                           "RemoveAll1",
                           entry));

            assertTrue("RemoveAll2", iJtu.populate(
                           iSession,
                           tableName,
                           "RemoveAll2",
                           entry));

            assertTrue("RemoveAll3", iJtu.populate(
                           iSession,
                           tableName,
                           "RemoveAll3",
                           entry));

            StorageEntry emptyEntry = new StorageEntry();
            int removed = iSession.remove(tableName, emptyEntry);

            entry = new StorageEntry();

            entry.addAttribute(new StorageAttribute(
                                   INITIAL_SIZE,
                                   "1234",
                                   StorageAttribute.INT_TYPE));

            // Check none found.
            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 3) == false);
            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 2) == false);
            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 1) == false);
            // To check null is returned.
            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 0) == false);

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UExp2: " + t.toString(), false);
        }
        iSession.close();
    }

}
