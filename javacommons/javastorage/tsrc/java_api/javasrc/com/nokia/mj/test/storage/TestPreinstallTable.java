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
 * StorageSession OTA table specific test cases. See test methods for test
 * case details.
 */
public class TestPreinstallTable extends TestCase implements UnitTestSuiteCreator, StorageNames
{
    private StorageSession iSession = null;
    private StorageSessionTestUtils iJtu = null;

    public TestSuite createTestSuite(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestPreinstallTable("testPreinstallTable", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestPreinstallTable)tc).testPreinstallTable();
            }
        }));

        return suite;
    }

    public TestPreinstallTable()
    {
    }

    public TestPreinstallTable(String aTestName, TestMethod aTestMethod)
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
     * Test PREINSTALL table is functionable.
     * Test write, search and remove operations.
     */
    public void testPreinstallTable()
    {
        String tableName = PREINSTALL_TABLE;
        StorageEntry entry = null;

        try
        {
            iSession.open();
            iSession.startTransaction();

            StorageEntry entry1 = new StorageEntry();
            entry1.addAttribute(
                new StorageAttribute(NAME, "PreInstalled MIDlet Suite"));
            entry1.addAttribute(
                new StorageAttribute(VENDOR, "Suite Vendor"));
            entry1.addAttribute(
                new StorageAttribute(VERSION, "0.0.01"));
            entry1.addAttribute(new StorageAttribute(
                                    INSTALL_STATE, "1", StorageAttribute.INT_TYPE));

            assertTrue("Populate failed",
                       iJtu.populate(iSession, tableName, null, entry1));

            StorageEntry entry2 = new StorageEntry();
            entry2.addAttribute(
                new StorageAttribute(NAME, "Foo Bar MIDlet"));
            entry2.addAttribute(
                new StorageAttribute(VENDOR, "Foo Bar Factory"));
            entry2.addAttribute(
                new StorageAttribute(VERSION, "10.12.111"));
            entry2.addAttribute(new StorageAttribute(
                                    INSTALL_STATE, "2", StorageAttribute.INT_TYPE));

            assertTrue("Populate failed",
                       iJtu.populate(iSession, tableName, null, entry2));

            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry1, 1));
            assertTrue("Check failed2",
                       iJtu.checkParams(iSession, tableName, entry2, 1));

            assertTrue("Remove failed1",
                       iJtu.remove(iSession, tableName, null, entry1));
            assertTrue("Remove failed2",
                       iJtu.remove(iSession, tableName, null, entry2));

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
