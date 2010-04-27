/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.test.fileutils;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.fileutils.*;
import com.nokia.mj.test.fileutils.TestEnvironment;

/**
 * Checks cases related to Exists()
 */
public class TestExists extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestExists("testExistsOnFile", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestExists) tc).testExistsOnFile();
            }
        }));

        suite.addTest(new TestExists("testExistsOnDir", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestExists) tc).testExistsOnDir();
            }
        }));

        suite.addTest(new TestExists("testExistsOnAbsent", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestExists) tc).testExistsOnAbsent();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestExists()
    {
    }

    public TestExists(String aTestName, TestMethod aTestMethod)
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
        path = TestEnvironment.Main_Test_Folder + "testexists/";
    }

    public void testExistsOnFile()
    {
        String fileName = "testExistsFile";

        try
        {
            FileUtility check = new FileUtility(path + fileName);
            assertTrue(" TestExists returned wrong value.", check.exists());
        }
        catch (Exception e)
        {
            assertTrue(" TestExists failed " + e, false);
        }
    }

    public void testExistsOnDir()
    {
        String dirName = "testExistsDir";

        try
        {
            FileUtility check = new FileUtility(path + dirName);
            assertTrue(" TestExists returned wrong value" + check.getURL(),
                       check.exists());
        }
        catch (Exception e)
        {
            assertTrue(" TestExists failed " + e, false);
        }
    }

    public void testExistsOnAbsent()
    {
        try
        {
            FileUtility check = new FileUtility(path + "nonexistentfile");
            assertFalse(" TestExists returned wrong value.", check.exists());
        }
        catch (Exception e)
        {
            assertTrue(" TestExists failed " + e, false);
        }
    }

}
