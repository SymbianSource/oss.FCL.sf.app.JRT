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
 * Transaction test cases. See test methods for test case details.<p>
 * NOTE: Multiple transactions are not supported simultaneuously so
 * they're not tested. If second transaction can be created however
 * writing data with it causes DB server busy.
 */
public class TestTransaction extends TestCase implements InstallerMain
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

        suite.addTest(new TestTransaction("testStartTransaction", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestTransaction)tc).testStartTransaction();
            }
        }));

        suite.addTest(new TestTransaction("testCommitTransaction", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestTransaction)tc).testCommitTransaction();
            }
        }));

        suite.addTest(new TestTransaction("testRollbackTransaction", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestTransaction)tc).testRollbackTransaction();
            }
        }));

        suite.addTest(new TestTransaction("testForceRollback", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestTransaction)tc).testForceRollback();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestTransaction()
    {
    }

    public TestTransaction(String aTestName, TestMethod aTestMethod)
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
     * Test transaction start.
     * 1. Start transaction when no connection.
     * 2. Start transaction when active connection. Transaction is created.
     * 3. Test two consecutive startTransactions().
     * 4. Test transactions can be used multiple times. Close active transaction
     *    using commit and initiate new one.
     * 5. Test multiple transactions can be used. Close active transaction using
     *    rollback and initiate new one.
     * 6. Start transaction after close.
     */
    public void testStartTransaction()
    {
        // 1. Start transaction when no connection.
        try
        {
            iSession.startTransaction();
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExp1: " + t.toString(), false);
        }

        // 2. Start transaction when active connection. Transaction is created.
        try
        {
            iSession.open();
            iSession.startTransaction();
            iSession.commitTransaction();
        }
        catch (StorageException se)
        {
            iSession.rollbackTransaction();
            assertTrue("Start transaction failed2: " + se.toString(), false);
        }

        iSession.close();

        // 3. Test two consecutive startTransactions().
        try
        {
            iSession.open();
            iSession.startTransaction();
            iSession.startTransaction();
            iSession.commitTransaction();
        }
        catch (StorageException se)
        {
            iSession.rollbackTransaction();
            // PASSED
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExp3: " + t.toString(), false);
        }
        iSession.close();

        // 4. Test transactions can be used multiple times. Close active
        //    transaction using commit and initiate new one.
        try
        {
            iSession.open();
            iSession.startTransaction();
            iSession.commitTransaction();
            iSession.startTransaction();
            iSession.commitTransaction();
        }
        catch (StorageException se)
        {
            iSession.rollbackTransaction();
            assertTrue("Start transaction failed4: " + se.toString(), false);
        }

        iSession.close();

        // 5. Test multiple transactions can be used. Close active transaction
        //    using rollback and initiate new one.
        try
        {
            iSession.open();
            iSession.startTransaction();
            iSession.rollbackTransaction();
            iSession.startTransaction();
            iSession.commitTransaction();
        }
        catch (StorageException se)
        {
            iSession.rollbackTransaction();
            assertTrue("Start transaction failed5: " + se.toString(), false);
        }

        iSession.close();

        // 6. Start transaction after close.
        try
        {
            iSession.startTransaction();
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExp6: " + t.toString(), false);
        }
    }

    /**
     * Test transaction commit.
     * 1. Commit without open connection.
     * 2. Test commit
     * 3. Test two consecutive commits.
     * 4. Test close connection and then commit.
     * 5. Test commit and then rollback.
     */
    public void testCommitTransaction()
    {
        // 1. Test commit when no connection.
        try
        {
            iSession.commitTransaction();
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExp1: " + t.toString(), false);
        }

        // 2. Test commit
        try
        {
            String appUid = "TestTransaction2";
            iSession.open();
            iSession.startTransaction();

            assertTrue("populateWithDefault population failed",
                       iJtu.populateWithDefault(iSession, appUid));

            assertTrue("Check defaults failed",
                       iJtu.checkDefaultParams(iSession, appUid));

            iSession.commitTransaction();

            assertTrue("Check defaults failed2",
                       iJtu.checkDefaultParams(iSession, appUid));

            iSession.startTransaction();

            assertTrue("Remove defaults failed",
                       iJtu.removeDefaultParams(iSession, appUid));

            iSession.commitTransaction();

            // Check entry stays removed also after commit session.
            assertTrue("Check defaults NOT failed",
                       iJtu.checkDefaultParams(iSession, appUid) == false);
        }
        catch (StorageException se)
        {
            iSession.rollbackTransaction();
            assertTrue("Commit failed2: " + se.toString(), false);
        }

        iSession.close();

        // 3. Test two consecutive commits.
        try
        {
            iSession.open();
            iSession.startTransaction();
            iSession.commitTransaction();
            iSession.commitTransaction();
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExp3: " + t.toString(), false);
        }

        iSession.close();

        // 4. Test close connection and then commit.
        try
        {
            iSession.open();
            iSession.startTransaction();

            iSession.close();
            iSession.commitTransaction();
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExp4: " + t.toString(), false);
        }

        // 5. Test commit and then rollback.
        try
        {
            iSession.open();
            iSession.startTransaction();
            iSession.commitTransaction();
            iSession.rollbackTransaction();
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExp5: " + t.toString(), false);
        }

        iSession.close();
    }

    /**
     * Test transaction rollback.
     * 1. Test Rollback without open connection.
     * 2. Test rollback.
     * 3. Test two consecutive rollbacks.
     * 4. Test close connection and then rollback.
     * 5. Test rollback and then commit.
     */
    public void testRollbackTransaction()
    {
        // 1. Test Rollback without open connection.
        //    ignored.
        try
        {
            iSession.rollbackTransaction();
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExp1: " + t.toString(), false);
        }

        // 2. Test rollback.
        try
        {
            String appUid = "TestRollback2";

            iSession.open();
            iSession.startTransaction();

            assertTrue("populateWithDefault population failed",
                       iJtu.populateWithDefault(iSession, appUid));

            assertTrue("Check defaults failed",
                       iJtu.checkDefaultParams(iSession, appUid));

            iSession.rollbackTransaction();

            assertTrue("Check defaults NOT failed",
                       iJtu.checkDefaultParams(iSession, appUid) == false);
        }
        catch (StorageException se)
        {
            iSession.rollbackTransaction();
            assertTrue("Rollback failed2: " + se.toString(), false);
        }

        iSession.close();

        // 3. Test two consecutive rollbacks.
        try
        {
            iSession.open();
            iSession.startTransaction();
            iSession.rollbackTransaction();
            iSession.rollbackTransaction();
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExp3: " + t.toString(), false);
        }

        iSession.close();

        // 4. Test close connection and then rollback.
        try
        {
            iSession.open();
            iSession.startTransaction();

            iSession.close();
            iSession.rollbackTransaction();
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExp4: " + t.toString(), false);
        }

        // 5. Test rollback and then commit.
        try
        {
            iSession.open();
            iSession.startTransaction();
            iSession.rollbackTransaction();
            iSession.commitTransaction();
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExp5: " + t.toString(), false);
        }
        iSession.close();
    }

    /**
     * Test open transaction is rollback if connection is closed.
     * 1. Test rollback is done if connection with open transaction is closed.
     */
    public void testForceRollback()
    {
        try
        {
            String appUid = "TestForceRollback1";

            iSession.open();
            iSession.startTransaction();

            assertTrue("populateWithDefault population failed",
                       iJtu.populateWithDefault(iSession, appUid));

            assertTrue("Check defaults failed",
                       iJtu.checkDefaultParams(iSession, appUid));

            iSession.close();

            iSession.open();

            // Check force rollback done
            assertTrue("Check defaults NOT failed",
                       iJtu.checkDefaultParams(iSession, appUid) == false);
        }
        catch (StorageException se)
        {
            iSession.rollbackTransaction();
            assertTrue("Rollback failed2: " + se.toString(), false);
        }

        iSession.close();

    }
}
