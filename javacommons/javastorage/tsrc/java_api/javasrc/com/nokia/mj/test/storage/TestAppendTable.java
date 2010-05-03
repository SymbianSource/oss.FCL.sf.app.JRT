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
public class TestAppendTable extends TestCase implements InstallerMain, StorageNames
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

        suite.addTest(new TestAppendTable("TestAppendTableArguments", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestAppendTable)tc).TestAppendTableArguments();
            }
        }));

        suite.addTest(new TestAppendTable("TestAppendInvalid", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestAppendTable)tc).TestAppendInvalid();
            }
        }));

        suite.addTest(new TestAppendTable("TestAppendTable", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestAppendTable)tc).TestAppendTable();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestAppendTable()
    {
    }

    public TestAppendTable(String aTestName, TestMethod aTestMethod)
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
     * Test append table arguments.
     * 1. Test empty tableName throws SE.
     * 2. Test null tableName throws SE.
     * 3. Test empty entry throws SE.
     * 4. Test null entry throws SE.
     * 5. Test append no connection throws SE.
     */
    public void TestAppendTableArguments()
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
                new StorageAttribute("HomeTown",
                                     "",
                                     StorageAttribute.STRING_TYPE));

            entry.addAttribute(
                new StorageAttribute("Country",
                                     "",
                                     StorageAttribute.INT_TYPE));

            iSession.appendTable(tableName, entry);
            assertTrue("No exp. when appending empty table name", false);
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
                new StorageAttribute("HomeTown",
                                     "",
                                     StorageAttribute.STRING_TYPE));

            entry.addAttribute(
                new StorageAttribute("Country",
                                     "",
                                     StorageAttribute.INT_TYPE));

            iSession.appendTable(tableName, entry);
            assertTrue("No exp. when appending null table name", false);
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

            iSession.appendTable(tableName, entry);
            assertTrue("No exp. when appending empty entry", false);
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

        // 4. Test null entry throws SE.
        try
        {
            iSession.open();
            tableName = APPLICATION_PACKAGE_TABLE;
            entry = null;

            iSession.appendTable(tableName, entry);
            assertTrue("No exp. when appending null entry", false);
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
            tableName = APPLICATION_PACKAGE_TABLE;
            appUid = "Not_Used";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute("HomeTown",
                                     "",
                                     StorageAttribute.STRING_TYPE));

            entry.addAttribute(
                new StorageAttribute("Country",
                                     "",
                                     StorageAttribute.INT_TYPE));

            iSession.appendTable(tableName, entry);
            assertTrue("No exp. when appending no connection", false);
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
     * Test append invalid table structues.
     * 1. Test append no table to append throws SE.
     * 2. Test append existing column throws SE.
     * 3. Test create invalid attribute. Name contains '-char
     *    StorageException is thrown.
     */
    public void TestAppendInvalid()
    {
        String tableName = null;
        StorageEntry entry = null;
        String appUid = null;

        // 1. Test append no table to append throws SE.
        try
        {
            iSession.open();
            tableName = "I_DO_NOT_EXISTS";
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute("Home",
                                     "",
                                     StorageAttribute.STRING_TYPE));

            entry.addAttribute(
                new StorageAttribute("StreetNumber",
                                     "",
                                     StorageAttribute.INT_TYPE));

            iSession.appendTable(tableName, entry);

            assertTrue("No exp. thrown appending non-existent table", false);
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

        // 2. Test append existing column throws SE.
        try
        {
            iSession.open();
            tableName = APPLICATION_PACKAGE_TABLE;
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(StorageAttribute.PACKAGE_NAME,
                                     "",
                                     StorageAttribute.STRING_TYPE));
            entry.addAttribute(
                new StorageAttribute("StreetNumber",
                                     "",
                                     StorageAttribute.INT_TYPE));

            iSession.appendTable(tableName, entry);

            assertTrue("No exp. thrown appending already existent attribute",
                       false);
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

        // 3. Test create invalid attribute. Name contains '-char
        //    StorageException is thrown.
        try
        {
            iSession.open();
            tableName = APPLICATION_PACKAGE_TABLE;
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute("Column '1",
                                     "",
                                     StorageAttribute.INT_TYPE));
            entry.addAttribute(
                new StorageAttribute("SameTwice",
                                     "",
                                     StorageAttribute.INT_TYPE));

            iSession.appendTable(tableName, entry);

            assertTrue("No exp. thrown appending invalid attribute",
                       false);
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
    }

    /**
     * Test append table.
     * 1. Test append table. Data can be written to it.
     * 2. Test append one column. Data can be written to it.
     * 3. Test append string and int columns. Data can be written to them.
     */
    public void TestAppendTable()
    {
        String tableName = null;
        StorageEntry entry = null;
        String appUid = null;

        // 1. Test append table. Data can be written to it.
        // 3. Test append string and int columns. Data can be written to them.
        try
        {
            iSession.open();
            iSession.startTransaction();
            tableName = APPLICATION_PACKAGE_TABLE;
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute("ADDRESS",
                                     "Home town 11",
                                     StorageAttribute.STRING_TYPE));

            entry.addAttribute(
                new StorageAttribute("AGE",
                                     "12",
                                     StorageAttribute.INT_TYPE));

            iSession.appendTable(tableName, entry);

            StorageAttribute idAttr = new StorageAttribute(
                StorageAttribute.ID, "888");

            entry.addAttribute(idAttr);

            iSession.write(tableName, entry);

            assertTrue("Check write", iJtu.checkParams(iSession,
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

        // 2. Test append one column. Data can be written to it.
        try
        {
            iSession.open();
            iSession.startTransaction();
            tableName = APPLICATION_PACKAGE_TABLE;
            entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute("ADDRESS",
                                     "Home town 11",
                                     StorageAttribute.STRING_TYPE));

            iSession.appendTable(tableName, entry);

            StorageAttribute idAttr = new StorageAttribute(
                StorageAttribute.ID, "888");

            entry.addAttribute(idAttr);

            iSession.write(tableName, entry);

            assertTrue("Check write", iJtu.checkParams(iSession,
                       tableName,
                       entry,
                       1));
            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExpected exp2: " + t.toString(), false);
        }

        iSession.close();
    }
}
