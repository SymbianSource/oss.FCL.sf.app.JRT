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
public class TestUpdate extends TestCase implements InstallerMain, StorageNames
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

        suite.addTest(new TestUpdate("testUpdateArguments", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestUpdate)tc).testUpdateArguments();
            }
        }));

        suite.addTest(new TestUpdate("testUpdate", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestUpdate)tc).testUpdate();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestUpdate()
    {
    }

    public TestUpdate(String aTestName, TestMethod aTestMethod)
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
     * Test update arguments.
     * 1. Test empty tableName throws SE.
     * 2. Test null tableName throws SE.
     * 3. Test empty update entry throws SE.
     * 4. Test null update entry throws SE.
     * 5. Test non-existent table throws SE.
     * 6. Test update non-existent attribute throws SE.
     * 7. Test update no connection throws SE.
     */
    public void testUpdateArguments()
    {
        String tableName = null;
        StorageEntry updateEntry = null;
        StorageEntry matchEntry = null;
        String appUid = "UpdateMe";

        try
        {
            iSession.open();
            iSession.startTransaction();

            assertTrue("PreCondition failed",
                       iJtu.populateWithDefault(iSession, appUid));

            assertTrue("PreCondition: population failed",
                       iJtu.checkDefaultParams(iSession, appUid));
        }
        catch (StorageException se)
        {
            // PASSSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp0: " + t.toString(), false);
        }

        // 1. Test empty tableName throws SE.
        try
        {
            iSession.open();
            tableName = "";

            updateEntry = new StorageEntry();
            updateEntry.addAttribute(
                new StorageAttribute(VENDOR,
                                     "TestUpdateVal"));

            matchEntry = new StorageEntry();
            matchEntry.addAttribute(
                new StorageAttribute(ID, appUid));

            iSession.update(tableName, updateEntry, matchEntry);
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
            appUid = "UpdateMe";

            updateEntry = new StorageEntry();
            updateEntry.addAttribute(
                new StorageAttribute(VENDOR,
                                     "TestUpdateVal"));

            matchEntry = new StorageEntry();
            matchEntry.addAttribute(
                new StorageAttribute(ID, appUid));

            iSession.update(tableName, updateEntry, matchEntry);
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

        // 3. Test empty update entry throws SE.
        try
        {
            iSession.open();
            tableName = APPLICATION_PACKAGE_TABLE;

            updateEntry = new StorageEntry();

            matchEntry = new StorageEntry();
            matchEntry.addAttribute(
                new StorageAttribute(ID, appUid));

            iSession.update(tableName, updateEntry, matchEntry);
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

        // 4. Test null update entry throws SE.
        try
        {
            iSession.open();
            tableName = APPLICATION_PACKAGE_TABLE;
            updateEntry = null;

            matchEntry = new StorageEntry();
            matchEntry.addAttribute(
                new StorageAttribute(ID, appUid));

            iSession.update(tableName, updateEntry, matchEntry);
            assertTrue("No exp. when null entry", false);
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

        // 5. Test non-existent table throws SE.
        try
        {
            iSession.open();
            tableName = "FOO_BAR";
            appUid = "UpdateMe";

            updateEntry = new StorageEntry();
            updateEntry.addAttribute(
                new StorageAttribute(VENDOR,
                                     "TestUpdateVal"));

            matchEntry = new StorageEntry();
            matchEntry.addAttribute(
                new StorageAttribute(ID, appUid));

            iSession.update(tableName, updateEntry, matchEntry);
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

        // 6. Test update non-existent attribute throws SE.
        try
        {
            iSession.open();
            tableName = APPLICATION_PACKAGE_TABLE;
            appUid = "UpdateMe";

            updateEntry = new StorageEntry();
            updateEntry.addAttribute(
                new StorageAttribute(VENDOR,
                                     "TestUpdateVal"));

            updateEntry.addAttribute(
                new StorageAttribute("FOO_BAR", "FOO_BAR_VALUE"));

            matchEntry = new StorageEntry();
            matchEntry.addAttribute(
                new StorageAttribute(ID, appUid));

            iSession.update(tableName, updateEntry, matchEntry);
            assertTrue("No exp. when non-existent attribute", false);
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

        // 7. Test update no connection throws SE.
        try
        {
            tableName = APPLICATION_PACKAGE_TABLE;
            appUid = "UpdateMe";

            updateEntry = new StorageEntry();
            updateEntry.addAttribute(
                new StorageAttribute(VENDOR,
                                     "TestUpdateVal"));

            matchEntry = new StorageEntry();
            matchEntry.addAttribute(
                new StorageAttribute(ID, appUid));

            iSession.update(tableName, updateEntry, matchEntry);
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
     * Test update.
     * 1. Test update.
     * 2. Test update whole table.
     * 3. Test update string to INT entry. SE must be thrown.
     * 4. Test update two same entries. Second one is updated.
     * 5. Test update empty value. Entries are updated to storage.
     * 6. Test update null value. Entries are updated to storage.
     * 7. Test update no data in DB. Nothing updated.
     * 8. Test partial update. One attribute has value in DB and other
     *    one does not. Both are updated.
     * 9. Test update primary key.
     */
    public void testUpdate()
    {
        String tableName = APPLICATION_PACKAGE_TABLE;
        StorageEntry updateEntry = null;
        StorageEntry matchEntry = null;
        String appUid = null;

        // 1. Test update.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestUpdate1";

            assertTrue("Populate", iJtu.populateWithDefault(
                           iSession, appUid));

            assertTrue("Check population", iJtu.checkDefaultParams(
                           iSession, appUid));

            updateEntry = new StorageEntry();
            updateEntry.addAttribute(
                new StorageAttribute(VENDOR,
                                     "UpdatedSuiteVenor"));

            // This is left empty as test utils add appUid as match attribute.
            matchEntry = new StorageEntry();

            assertTrue("Update", iJtu.update(iSession,
                                             tableName,
                                             appUid,
                                             updateEntry,
                                             matchEntry));

            assertTrue("Check failed", iJtu.checkParams(iSession,
                       tableName,
                       updateEntry,
                       1));
            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp1: " + t.toString(), false);
        }

        iSession.close();

        // 2. Test update whole table.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestUpdate2";

            StorageEntry entry = iJtu.createAppPackageEntry();

            assertTrue("Populate", iJtu.populate(
                           iSession, tableName, appUid, entry));

            assertTrue("Check population", iJtu.checkParams(
                           iSession, tableName, entry, 1));

            updateEntry = new StorageEntry();

            updateEntry.addAttribute(new StorageAttribute(
                                         PACKAGE_NAME, "UDPMIDlet"));
            updateEntry.addAttribute(new StorageAttribute(
                                         VENDOR, "UDPVendor"));
            updateEntry.addAttribute(new StorageAttribute(
                                         VERSION, "9.9.9.9"));
            updateEntry.addAttribute(new StorageAttribute(
                                         ROOT_PATH, "/UDPAPI/path"));
            updateEntry.addAttribute(new StorageAttribute(
                                         MEDIA_ID,
                                         "899089",
                                         StorageAttribute.INT_TYPE));
            updateEntry.addAttribute(new StorageAttribute(
                                         INITIAL_SIZE,
                                         "65464",
                                         StorageAttribute.INT_TYPE));
            updateEntry.addAttribute(new StorageAttribute(
                                         JAD_PATH, "UDPAPI/JAD/"));
            updateEntry.addAttribute(new StorageAttribute(
                                         JAR_PATH, "UDPAPI/JAR/"));
            updateEntry.addAttribute(new StorageAttribute(
                                         ACCESS_POINT, "Streaming AP"));

            // This is left empty as test utils add appUid as match attribute.
            matchEntry = new StorageEntry();

            assertTrue("Update", iJtu.update(iSession,
                                             tableName,
                                             appUid,
                                             updateEntry,
                                             matchEntry));

            assertTrue("Check failed", iJtu.checkParams(iSession,
                       tableName,
                       updateEntry,
                       1));
            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp2: " + t.toString(), false);
        }

        iSession.close();

        // 3. Test update string to INT entry. SE must be thrown.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestUpdate3";

            StorageEntry entry = new StorageEntry();

            entry.addAttribute(new StorageAttribute(
                                   ID, appUid));

            entry.addAttribute(new StorageAttribute(
                                   PACKAGE_NAME, "INT_MIDlet"));

            entry.addAttribute(new StorageAttribute(
                                   INITIAL_SIZE, "12"));

            assertTrue("Populate", iJtu.populate(
                           iSession, tableName, appUid, entry));

            assertTrue("Check population", iJtu.checkParams(
                           iSession, tableName, entry, 1));

            updateEntry = new StorageEntry();
            updateEntry.addAttribute(
                new StorageAttribute(INITIAL_SIZE,
                                     "MyGreatStringID",
                                     StorageAttribute.INT_TYPE));

            iSession.update(tableName, updateEntry, entry);
            assertTrue("No exp update String to INT", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp3: " + t.getMessage(), false);
        }

        iSession.close();

        // 4. Test update two same entries. Last one is updated.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestUpdate4";

            StorageEntry entry = new StorageEntry();

            entry.addAttribute(new StorageAttribute(
                                   ID, appUid));

            entry.addAttribute(new StorageAttribute(
                                   PACKAGE_NAME, "INT_MIDlet"));

            entry.addAttribute(new StorageAttribute(
                                   INITIAL_SIZE, "12"));

            assertTrue("Populate", iJtu.populate(
                           iSession, tableName, appUid, entry));

            assertTrue("Check population", iJtu.checkParams(
                           iSession, tableName, entry, 1));

            updateEntry = new StorageEntry();
            updateEntry.addAttribute(
                new StorageAttribute(INITIAL_SIZE,
                                     "1"));
            updateEntry.addAttribute(
                new StorageAttribute(INITIAL_SIZE,
                                     "2"));

            // This is left empty as test utils add appUid as match attribute.
            matchEntry = new StorageEntry();

            assertTrue("Update", iJtu.update(iSession,
                                             tableName,
                                             appUid,
                                             updateEntry,
                                             matchEntry));


            // Remove the first same entry as last one should be updated.
            updateEntry.removeAttribute(
                new StorageAttribute(INITIAL_SIZE, "1"));

            assertTrue("Check failed2", iJtu.checkParams(iSession,
                       tableName,
                       updateEntry,
                       1));
            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp4: " + t.toString(), false);
        }

        iSession.close();

        // 5. Test update empty value. Entries are updated to storage.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestUpdate1";

            assertTrue("Populate", iJtu.populateWithDefault(
                           iSession, appUid));

            assertTrue("Check population", iJtu.checkDefaultParams(
                           iSession, appUid));

            updateEntry = new StorageEntry();
            updateEntry.addAttribute(
                new StorageAttribute(VENDOR,
                                     ""));

            // This is left empty as test utils add appUid as match attribute.
            matchEntry = new StorageEntry();

            assertTrue("Update", iJtu.update(iSession,
                                             tableName,
                                             appUid,
                                             updateEntry,
                                             matchEntry));

            // Check only updated column
            assertTrue("Check failed", iJtu.checkParams(iSession,
                       tableName,
                       updateEntry,
                       1));

            matchEntry = new StorageEntry();
            matchEntry.addAttribute(new StorageAttribute(ID, appUid));

            // Check other params are intact.
            assertTrue("Check failed", iJtu.checkParams(iSession,
                       tableName,
                       matchEntry,
                       1));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp5: " + t.toString(), false);
        }

        iSession.close();

        // 6. Test update null value. Entries are updated to storage.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestUpdate6";

            assertTrue("Populate", iJtu.populateWithDefault(
                           iSession, appUid));

            assertTrue("Check population", iJtu.checkDefaultParams(
                           iSession, appUid));

            updateEntry = new StorageEntry();
            String value = null;
            updateEntry.addAttribute(
                new StorageAttribute(VENDOR,
                                     value,
                                     StorageAttribute.NULL_TYPE));

            // This is left empty as test utils add appUid as match attribute.
            matchEntry = new StorageEntry();

            assertTrue("Update", iJtu.update(iSession,
                                             tableName,
                                             appUid,
                                             updateEntry,
                                             matchEntry));

            assertTrue("Check failed", iJtu.checkParams(iSession,
                       tableName,
                       updateEntry,
                       1) == false);

            matchEntry = new StorageEntry();
            matchEntry.addAttribute(new StorageAttribute(ID, appUid));

            // Check other params are intact.
            assertTrue("Check failed", iJtu.checkParams(iSession,
                       tableName,
                       matchEntry,
                       1));
            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp6: " + t.toString(), false);
        }

        iSession.close();

        // 7. Test update no data in DB. Nothing updated.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "TestUpdate7";

            updateEntry = new StorageEntry();
            String value = null;
            updateEntry.addAttribute(
                new StorageAttribute(VENDOR,
                                     value,
                                     StorageAttribute.NULL_TYPE));

            matchEntry = new StorageEntry();

            matchEntry.addAttribute(
                new StorageAttribute(VENDOR,
                                     value,
                                     StorageAttribute.NULL_TYPE));

            iSession.update(tableName, updateEntry, matchEntry);
            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp7: " + t.toString(), false);
        }

        iSession.close();

        // 8. Test partial update. One attribute has value in DB and other
        //    one does not. Both are updated.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestUpdate8";

            assertTrue("Populate", iJtu.populateWithDefault(
                           iSession, appUid));

            assertTrue("Check population", iJtu.checkDefaultParams(
                           iSession, appUid));

            updateEntry = new StorageEntry();
            updateEntry.addAttribute(
                new StorageAttribute(VENDOR,
                                     "UpdatedSuiteVenor"));
            // This is not default attribute
            updateEntry.addAttribute(
                new StorageAttribute(JAR_PATH,
                                     "HooooHaaa!!!"));

            // This is left empty as test utils add appUid as match attribute.
            matchEntry = new StorageEntry();

            assertTrue("Update", iJtu.update(iSession,
                                             tableName,
                                             appUid,
                                             updateEntry,
                                             matchEntry));

            assertTrue("Check failed", iJtu.checkParams(iSession,
                       tableName,
                       updateEntry,
                       1));
            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp8: " + t.toString(), false);
        }

        iSession.close();

        // 9. Test update primary key.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestUpdate9";

            assertTrue("Populate", iJtu.populateWithDefault(
                           iSession, appUid));

            assertTrue("Check population", iJtu.checkDefaultParams(
                           iSession, appUid));

            updateEntry = new StorageEntry();
            updateEntry.addAttribute(
                new StorageAttribute(ID,
                                     "UPDATED_UID"));

            // This is left empty as test utils add appUid as match attribute.
            matchEntry = new StorageEntry();

            assertTrue("Update", iJtu.update(iSession,
                                             tableName,
                                             appUid,
                                             updateEntry,
                                             matchEntry));

            assertTrue("Check failed", iJtu.checkParams(iSession,
                       tableName,
                       updateEntry,
                       1));
            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp9: " + t.toString(), false);
        }

        iSession.close();
    }
}
