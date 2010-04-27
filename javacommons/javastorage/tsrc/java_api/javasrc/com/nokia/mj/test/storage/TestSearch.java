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
 * StorageSession search test cases. See test methods for test case details.
 */
public class TestSearch extends TestCase implements InstallerMain, StorageNames
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

        suite.addTest(new TestSearch("testSearchArguments", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSearch)tc).testSearchArguments();
            }
        }));

        suite.addTest(new TestSearch("testSearch", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSearch)tc).testSearch();
            }
        }));

        suite.addTest(new TestSearch("testSearchByInt", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestSearch)tc).testSearchByInt();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestSearch()
    {
    }

    public TestSearch(String aTestName, TestMethod aTestMethod)
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
     * Test search arguments.
     * 1. Test empty tableName throws SE.
     * 2. Test null tableName throws SE.
     * 3. Test empty entry do not throw SE. SELECT * query.
     * 4. Test null entry do not throw SE. SELECT * query.
     * 5. Test non-existent table throws SE.
     * 6. Test search non-existent entry throws SE.
     * 7. Test search no connection throws SE.
     */
    public void testSearchArguments()
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
                new StorageAttribute(VENDOR, "TestSearch"));

            iSession.search(tableName, entry);
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
                new StorageAttribute(ID, appUid));

            entry.addAttribute(
                new StorageAttribute(VENDOR, "TestSearch"));

            iSession.search(tableName, entry);
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

        // 3. Test empty entry do not throw SE. SELECT * search
        try
        {
            iSession.open();
            tableName = APPLICATION_PACKAGE_TABLE;
            entry = new StorageEntry();

            iSession.search(tableName, entry);
        }
        catch (Throwable t)
        {
            assertTrue("UnExpected exp3: " + t.toString(), false);
        }

        iSession.close();

        // 4. Test null entry do not throw SE. SELECT * search.
        try
        {
            iSession.open();
            tableName = APPLICATION_PACKAGE_TABLE;
            entry = null;

            iSession.search(tableName, entry);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
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
                new StorageAttribute(VENDOR, "TestSearch"));

            iSession.search(tableName, entry);
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
                new StorageAttribute(VENDOR, "TestSearch"));

            entry.addAttribute(
                new StorageAttribute("FOO_BAR", "FOO_BAR_VALUE"));

            iSession.search(tableName, entry);
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

        // 7. Test search no connection throws SE.
        try
        {
            tableName = APPLICATION_PACKAGE_TABLE;
            appUid = "Not_Used";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(ID, appUid));

            entry.addAttribute(
                new StorageAttribute(VENDOR, "TestSearch"));

            iSession.search(tableName, entry);
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
     * Test Search.
     * 1. Test search.
     * 2. Test search no matches. Null is returned.
     * 3. Test multiple matches. All found. Storage data contain also items not
     *    matching the pattern.
     * 4. Test Search one match which has only one entry within.
     * 5. Test Search multiple matches which have only one entry within.
     * 6. Test SELECT * search
     * 7. Test search mixed select and where items.
     */
    public void testSearch()
    {
        String tableName = APPLICATION_PACKAGE_TABLE;
        StorageEntry entry = null;
        String appUid = null;

        // 1. Test search.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestSearch1";

            assertTrue("Populate failed",
                       iJtu.populateWithDefault(iSession, appUid));

            assertTrue("Check default failed",
                       iJtu.checkDefaultParams(iSession, appUid));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp1: " + t.toString(), false);
        }

        iSession.close();

        // 2. Test search no matches. Null is returned.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestSearch2";

            assertTrue("Populate failed",
                       iJtu.populateWithDefault(iSession, appUid));

            entry = new StorageEntry();
            // Default entry does not contain JAD_PATH attribute.
            entry.addAttribute(new StorageAttribute(
                                   JAD_PATH, "/Not/Exists"));

            StorageEntry[] foundEntries = iSession.search(tableName, entry);

            assertTrue("Found entries", foundEntries == null);

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp2: " + t.toString(), false);
        }

        iSession.close();

        // 3. Test multiple matches. All found. Storage data contain also items
        //    not matching the pattern.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestSearch3";

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

            for (int i = 0; i < foundEntries.length; i++)
            {
                assertTrue("Check JAD found; " + i,
                           foundEntries[i].getAttribute(
                               JAD_PATH).getValue()
                           .equals("/Match/path"));

                assertTrue("Check JAR found; " + i,
                           foundEntries[i].getAttribute(
                               JAR_PATH).getValue()
                           .equals("/Match/JAR/path"));
            }

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp3: " + t.toString(), false);
        }

        iSession.close();

        // 4. Test Search one match which has only one entry within.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestSearch4";

            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   ID, "Entry1"));

            iSession.write(tableName, entry);

            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   ID, "Entry2"));

            iSession.write(tableName, entry);

            StorageEntry query = new StorageEntry();
            query.addAttribute(new StorageAttribute(
                                   ID, "Entry2"));

            StorageEntry[] foundEntries = iSession.search(tableName, query);

            assertTrue("Found entries", foundEntries.length == 1);

            assertTrue("Not Match", foundEntries[0]
                       .getAttribute(ID)
                       .equals(new StorageAttribute(
                                   ID, "Entry2")));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp4: " + t.toString(), false);
        }

        iSession.close();

        // 5. Test Search multiple matches which have only one entry within.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestSearch5";

            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   ID, "Entry1"));
            entry.addAttribute(new StorageAttribute(
                                   JAD_PATH, "/Match/path"));

            iSession.write(tableName, entry);

            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   ID, "Entry2"));
            entry.addAttribute(new StorageAttribute(
                                   JAD_PATH, "/Match/path"));

            iSession.write(tableName, entry);

            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   ID, "Entry3"));
            entry.addAttribute(new StorageAttribute(
                                   JAD_PATH, "/Match/path"));

            iSession.write(tableName, entry);

            StorageEntry query = new StorageEntry();
            query.addAttribute(new StorageAttribute(
                                   JAD_PATH, "/Match/path"));

            entry.addAttribute(new StorageAttribute(
                                   JAD_PATH, "/Match/path"));

            StorageEntry[] foundEntries = iSession.search(tableName, query);

            assertTrue("Found entries", foundEntries.length == 3);

            assertTrue("Not Match1", foundEntries[0].getAttribute(
                           JAD_PATH).equals(
                           new StorageAttribute(
                               JAD_PATH, "/Match/path")));

            assertTrue("Not Match2", foundEntries[1].getAttribute(
                           JAD_PATH).equals(new StorageAttribute(
                                                JAD_PATH, "/Match/path")));

            assertTrue("Not Match3", foundEntries[2].getAttribute(
                           JAD_PATH).equals(
                           new StorageAttribute(
                               JAD_PATH, "/Match/path")));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp5: " + t.toString(), false);
        }

        iSession.close();

        // 6. Test SELECT * search
        // This fails currently as Symbian does no support getting
        // column names. Workaround enabled for this to both platforms
        // so SELECT * is currently disabled.
        try
        {
            iSession.open();
            iSession.startTransaction();
            appUid = "JavaTestSearch6";

            entry = iJtu.createAppPackageEntry();
            entry.addAttribute(
                new StorageAttribute(ID, "ID1"));

            iSession.write(tableName, entry);

            entry = iJtu.createAppPackageEntry();
            entry.addAttribute(
                new StorageAttribute(ID, "ID2"));

            iSession.write(tableName, entry);

            // Empty query equals to SELECT *
            StorageEntry query = new StorageEntry();

            StorageEntry[] foundEntries = iSession.search(tableName, query);

            assertTrue("Found entries", foundEntries.length == 2);

            StorageEntry refEntry = iJtu.createAppPackageEntry();

            // As only search pattern defined search is done as SELECT * so
            // we must check all attributes that are within default entry.
            for (int i = 0; i < foundEntries.length; i++)
            {
                // These fails to NullPointerException if not found.
                assertTrue(PACKAGE_NAME,
                           foundEntries[i].getAttribute(
                               PACKAGE_NAME)
                           .equals(refEntry.getAttribute(
                                       PACKAGE_NAME)));

                assertTrue(VENDOR,
                           foundEntries[i].getAttribute(VENDOR)
                           .equals(refEntry.getAttribute(
                                       VENDOR)));

                assertTrue(VERSION,
                           foundEntries[i].getAttribute(VERSION)
                           .equals(refEntry.getAttribute(
                                       VERSION)));

                assertTrue(ROOT_PATH,
                           foundEntries[i].getAttribute(ROOT_PATH)
                           .equals(refEntry.getAttribute(
                                       ROOT_PATH)));

                // Values compared as Type is changed.
                assertTrue(MEDIA_ID,
                           foundEntries[i].getAttribute(MEDIA_ID).getValue()
                           .equals(refEntry.getAttribute(
                                       MEDIA_ID).getValue()));

                // Values compared as Type is changed.
                assertTrue(INITIAL_SIZE,
                           foundEntries[i].getAttribute(INITIAL_SIZE).getValue()
                           .equals(refEntry.getAttribute(
                                       INITIAL_SIZE).getValue()));

                assertTrue(JAD_PATH,
                           foundEntries[i].getAttribute(JAD_PATH)
                           .equals(refEntry.getAttribute(
                                       JAD_PATH)));

                assertTrue(JAR_PATH,
                           foundEntries[i].getAttribute(JAR_PATH)
                           .equals(refEntry.getAttribute(
                                       JAR_PATH)));

                assertTrue(ACCESS_POINT,
                           foundEntries[i].getAttribute(
                               ACCESS_POINT)
                           .equals(refEntry.getAttribute(
                                       ACCESS_POINT)));
            }

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp6: " + t.toString(), false);
        }

        iSession.close();

        // 7. Test search mixed select and where items.
        try
        {
            // appUid = "JavaTestSearch7";

            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   ID, "WhereEntry1"));

            entry.addAttribute(new StorageAttribute(
                                   PACKAGE_NAME, "Diiba Daaba"));

            entry.addAttribute(new StorageAttribute(
                                   VENDOR, "Bla bla bla"));

            entry.addAttribute(new StorageAttribute(
                                   ROOT_PATH, "root path"));

            entry.addAttribute(new StorageAttribute(
                                   JAD_PATH, "Jad path"));

            entry.addAttribute(new StorageAttribute(
                                   JAR_PATH, "Pathway to JAR"));

            iSession.open();
            iSession.startTransaction();
            iSession.write(tableName, entry);

            StorageEntry query = new StorageEntry();
            query.addAttribute(new StorageAttribute(
                                   ID, "WhereEntry1"));

            query.addAttribute(new StorageAttribute(
                                   PACKAGE_NAME, ""));

            query.addAttribute(new StorageAttribute(
                                   VENDOR, ""));

            query.addAttribute(new StorageAttribute(
                                   ROOT_PATH, "root path"));

            query.addAttribute(new StorageAttribute(
                                   JAD_PATH, "Jad path"));

            query.addAttribute(new StorageAttribute(
                                   JAR_PATH, ""));

            StorageEntry[] foundEntries = iSession.search(tableName, query);

            assertTrue("Found entries", foundEntries.length == 1);

            assertTrue("No Match", foundEntries[0]
                       .getAttribute(PACKAGE_NAME)
                       .equals(new StorageAttribute(
                                   PACKAGE_NAME, "Diiba Daaba")));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExp exp7: " + t.toString(), false);
        }

        iSession.close();

    }

    /**
     * Test interger data search.
     * 1. Test search by INT.
     * 2. Test search by INT and STRING
     * 3. Test search by INT no type set. Entry found.
     */
    public void testSearchByInt()
    {
        String tableName = APPLICATION_PACKAGE_TABLE;
        StorageEntry entry = null;

        // 1. Test search by INT.
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

            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   INITIAL_SIZE,
                                   "1234",
                                   StorageAttribute.INT_TYPE));

            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 1));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp1: " + t.toString(), false);
        }

        iSession.close();

        // 2. Test search by INT and STRING
        try
        {
            iSession.open();
            iSession.startTransaction();

            entry = new StorageEntry();

            entry.addAttribute(new StorageAttribute(
                                   INITIAL_SIZE,
                                   "1234",
                                   StorageAttribute.INT_TYPE));

            entry.addAttribute(new StorageAttribute(
                                   JAD_PATH,
                                   "/Great/JAD/Path"));

            assertTrue("Populate2", iJtu.populate(
                           iSession,
                           tableName,
                           "IntEntry2",
                           entry));

            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   INITIAL_SIZE,
                                   "1234",
                                   StorageAttribute.INT_TYPE));

            entry.addAttribute(new StorageAttribute(
                                   ID,
                                   "IntEntry2"));

            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 1));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp2: " + t.toString(), false);
        }

        iSession.close();

        // 3. Test search by INT no type set. Entry found.
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

            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   INITIAL_SIZE,
                                   "1234"));

            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 1));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp3: " + t.toString(), false);
        }

        iSession.close();
    }
}
