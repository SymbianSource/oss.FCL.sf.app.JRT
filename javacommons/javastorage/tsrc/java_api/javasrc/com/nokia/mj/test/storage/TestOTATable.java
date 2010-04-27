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
 * StorageSession OTA table specific test cases. See test methods for test
 * case details.
 */
public class TestOTATable extends TestCase implements InstallerMain, StorageNames
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

        suite.addTest(new TestOTATable("testOTATable", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestOTATable)tc).testOTATable();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestOTATable()
    {
    }

    public TestOTATable(String aTestName, TestMethod aTestMethod)
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
     * Test OTA table is functionable. Test write, search and remove operations.
     */
    public void testOTATable()
    {
        String tableName = OTA_STATUS_TABLE;
        StorageEntry entry = null;

        try
        {
            String dbName = JAVA_OTA_DATABASE_NAME;
            iSession.open(dbName);
            iSession.startTransaction();
            String appUid1 = "OTA_ID1";

            StorageEntry entry1 = new StorageEntry();
            entry1.addAttribute(
                new StorageAttribute(CREATION_TIME, "12:30am"));
            entry1.addAttribute(
                new StorageAttribute(TYPE, "1", StorageAttribute.INT_TYPE));
            entry1.addAttribute(new StorageAttribute(
                                    OTA_CODE, "901", StorageAttribute.INT_TYPE));
            entry1.addAttribute(
                new StorageAttribute(URL, "http://www.test.com/DL/access"));
            entry1.addAttribute(
                new StorageAttribute(LATEST_RETRY_TIME, "12:48am"));
            entry1.addAttribute(new StorageAttribute(
                                    RETRY_COUNT, "2", StorageAttribute.INT_TYPE));

            assertTrue("Populate failed",
                       iJtu.populate(iSession, tableName, appUid1, entry1));

            String appUid2 = "OTA_ID2";
            StorageEntry entry2 = new StorageEntry();
            entry2.addAttribute(
                new StorageAttribute(CREATION_TIME, "02:30pm"));
            entry2.addAttribute(
                new StorageAttribute(TYPE, "8", StorageAttribute.INT_TYPE));
            entry2.addAttribute(new StorageAttribute(
                                    OTA_CODE, "928", StorageAttribute.INT_TYPE));
            entry2.addAttribute(new StorageAttribute(
                                    URL, "http://www.great-test.com/DL/access"));
            entry2.addAttribute(
                new StorageAttribute(LATEST_RETRY_TIME, "03:41pm"));
            entry2.addAttribute(new StorageAttribute(
                                    RETRY_COUNT, "16", StorageAttribute.INT_TYPE));

            assertTrue("Populate failed",
                       iJtu.populate(iSession, tableName, appUid2, entry2));

            // Utils add ID to entry
            entry1.removeAttribute(ID);
            entry2.removeAttribute(ID);

            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry1, 1));
            assertTrue("Check failed2",
                       iJtu.checkParams(iSession, tableName, entry2, 1));

            assertTrue("Remove failed1",
                       iJtu.remove(iSession, tableName, appUid1, entry1));
            assertTrue("Remove failed2",
                       iJtu.remove(iSession, tableName, appUid2, entry2));

            iSession.rollbackTransaction();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExp exp1: " + t.toString(), false);
        }

        iSession.close();
    }
}
