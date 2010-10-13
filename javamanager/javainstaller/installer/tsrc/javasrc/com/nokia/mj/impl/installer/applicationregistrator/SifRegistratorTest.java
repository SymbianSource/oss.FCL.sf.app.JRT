/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.applicationregistrator;

import com.nokia.mj.impl.installer.storagehandler.ApplicationInfo;
import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.utils.ComponentId;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.Version;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import java.util.Vector;

/**
 * SifRegistrator unit tests.
 */
public class SifRegistratorTest extends TestCase implements InstallerMain
{
    // Begin j2meunit test framework setup

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new SifRegistratorTest("testStartAndCloseSession", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SifRegistratorTest)tc).testStartAndCloseSession();
            }
        }));

        suite.addTest(new SifRegistratorTest("testStartAndCommitSession", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SifRegistratorTest)tc).testStartAndCommitSession();
            }
        }));

        suite.addTest(new SifRegistratorTest("testRegisterSuite", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SifRegistratorTest)tc).testRegisterSuite();
            }
        }));

        suite.addTest(new SifRegistratorTest("testGetUid", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SifRegistratorTest)tc).testGetUid();
            }
        }));

        suite.addTest(new SifRegistratorTest("testAplicationAlreadyRegistered", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SifRegistratorTest)tc).testAplicationAlreadyRegistered();
            }
        }));

        suite.addTest(new SifRegistratorTest("testUnregisterSuite", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SifRegistratorTest)tc).testUnregisterSuite();
            }
        }));

        suite.addTest(new SifRegistratorTest("testRollbackRegistration", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SifRegistratorTest)tc).testRollbackRegistration();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public SifRegistratorTest()
    {
    }

    public SifRegistratorTest(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    // End j2meunit test framework setup

    protected void setUp()
    {
    }

    protected void tearDown()
    {
    }

    public void testStartAndCloseSession()
    {
        SifRegistrator sifReg = new SifRegistrator();
        try
        {
            sifReg.startSession(true);
            // Close the session immediately.
            sifReg.closeSession();
            // Make sure that closeSession can be called even if the
            // session has already been closed.
            sifReg.closeSession();
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            sifReg.closeSession();
        }
    }

    public void testStartAndCommitSession()
    {
        SifRegistrator sifReg = new SifRegistrator();
        try
        {
            sifReg.startSession(true);
            // Commit the session immediately.
            sifReg.commitSession();
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            sifReg.closeSession();
        }
    }

    public void testRegisterSuite()
    {
        SifRegistrator sifReg = new SifRegistrator();
        try
        {
            SuiteInfo si = getTestSuite();
            // Session must be started first.
            try
            {
                sifReg.registerSuite(si, false);
                assertTrue("Calling registerSuite should not succeed " +
                           "without session", false);
            }
            catch (InstallerException ie)
            {
                // Expected exception.
            }
            sifReg.startSession(true);
            sifReg.registerSuite(si, false);
            sifReg.commitSession();

            assertSuiteExists(si, true);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            sifReg.commitSession();
        }
    }

    public void testGetUid()
    {
        SifRegistrator sifReg = new SifRegistrator();
        try
        {
            SuiteInfo si = getTestSuite();
            ComponentId cid = getComponentId(si.getGlobalId());
            Uid uid = null;
            //Uid uid = sifReg.getUid(cid.getId());
            //assertTrue("Suite uid " + si.getUid() + " does not match" +
            //           " to uid " + uid + " from SCR",
            //           si.getUid().equals(uid));
            Vector apps = si.getApplications();
            for (int i = 0; i < apps.size(); i++)
            {
                cid = getComponentId(si.getGlobalId(i));
                uid = sifReg.getUid(cid.getId());
                assertTrue("Application uid " +
                           ((ApplicationInfo)apps.elementAt(i)).getUid() +
                           " does not match to uid " + uid + " from SCR",
                           ((ApplicationInfo)apps.elementAt(i)).getUid().equals(uid));
            }
            sifReg.closeSession();
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            sifReg.closeSession();
        }
    }

    public void testAplicationAlreadyRegistered()
    {
        SifRegistrator sifReg = new SifRegistrator();
        try
        {
            SuiteInfo si = getTestSuite();
            sifReg.startSession(true);
            try
            {
                sifReg.registerSuite(si, false);
                assertTrue("Calling registerSuite should not succeed " +
                           "for already registered application", false);
            }
            catch (InstallerException ie)
            {
                // Expected exception.
            }
            sifReg.rollbackSession();

            assertSuiteExists(si, true);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            sifReg.closeSession();
        }
    }

    public void testUnregisterSuite()
    {
        SifRegistrator sifReg = new SifRegistrator();
        try
        {
            SuiteInfo si = getTestSuite();
            sifReg.startSession(true);
            sifReg.unregisterSuite(si);
            sifReg.commitSession();

            assertSuiteExists(si, false);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            sifReg.closeSession();
        }
    }


    public void testRollbackRegistration()
    {
        SifRegistrator sifReg = new SifRegistrator();
        try
        {
            SuiteInfo si = getTestSuite();
            sifReg.startSession(true);
            sifReg.registerSuite(si, true);
            sifReg.rollbackSession();

            assertSuiteExists(si, false);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            sifReg.closeSession();
        }
    }

    private SuiteInfo getTestSuite()
    {
        SuiteInfo si = new SuiteInfo("TestMidlet", "TestMidletVendor");
        si.setUid(PlatformUid.createUid("[e0001000]"));
        si.setVersion(new Version(1, 1, 0));
        si.setRootDir("c:\\rootdir");
        si.setJadPath("c:\\jadpath");
        si.setJarPath("c:\\jarpath");
        si.setJadUrl("http://jadurl");
        si.setJarUrl("http://jarurl");
        si.setInitialSize(8092);
        si.addApplication(new ApplicationInfo(
                              PlatformUid.createUid("[e0001001]"),
                              "App1", "App1Main", 0));
        si.addApplication(new ApplicationInfo(
                              PlatformUid.createUid("[e0001002]"),
                              "App2", "App2Main", 0));
        si.addApplication(new ApplicationInfo(
                              PlatformUid.createUid("[e0001003]"),
                              "App3", "App3Main", 0));
        return si;
    }

    private void assertSuiteExists(SuiteInfo aSuite, boolean aExists)
    {
        String not = (aExists? " not": "");
        ComponentId cid = getComponentId(aSuite.getGlobalId());
        //assertTrue("Suite " + aSuite.getGlobalId() + not +
        //           " found from SCR", (aExists? cid != null: cid == null));
        Vector apps = aSuite.getApplications();
        for (int i = 0; i < apps.size(); i++)
        {
            cid = getComponentId(aSuite.getGlobalId(i));
            assertTrue("Application " + aSuite.getGlobalId(i) + not +
                       " found from SCR", (aExists? cid != null: cid == null));
        }
    }

    private void logSuite(SuiteInfo aSuite)
    {
        SifRegistrator sifReg = new SifRegistrator();
        sifReg.startSession(false);
        sifReg.logComponent(aSuite.getGlobalId());
        Vector apps = aSuite.getApplications();
        for (int i = 0; i < apps.size(); i++)
        {
            sifReg.logComponent(aSuite.getGlobalId(i));
        }
        sifReg.closeSession();
    }

    private ComponentId getComponentId(String aGlobalId)
    {
        SifRegistrator sifReg = new SifRegistrator();
        sifReg.startSession(false);
        ComponentId cid = sifReg.getComponentId(aGlobalId);
        sifReg.closeSession();
        return cid;
    }
}
