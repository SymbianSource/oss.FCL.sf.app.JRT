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
 * Checks cases related to isFile()
 */
public class TestIsFile extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestIsFile("testOnFile", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestIsFile) tc).testOnFile();
            }
        }));

        suite.addTest(new TestIsFile("testOnDir", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestIsFile) tc).testOnDir();
            }
        }));

        suite.addTest(new TestIsFile("testOnAbsent", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestIsFile) tc).testOnAbsent();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestIsFile()
    {
    }

    public TestIsFile(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    protected void setUp()
    {
        path = TestEnvironment.Main_Test_Folder + "testisfile/";
    }

    public void testOnFile()
    {
        String fileName = "testOnFileTest";
        try
        {
            FileUtility check = new FileUtility(path + fileName);
            assertTrue(" TestExists returned wrong value.", check.isFile());
        }
        catch (Exception e)
        {
            assertTrue(" TestExists failed " + e, false);
        }
    }

    public void testOnDir()
    {
        String fileName = "testOnDirTest";
        try
        {
            FileUtility check = new FileUtility(path + fileName);
            assertFalse(" TestExists returned wrong value.", check.isFile());
        }
        catch (Exception e)
        {
            assertTrue(" TestExists failed " + e, false);
        }
    }

    public void testOnAbsent()
    {
        String fileName = "testOnAbsent";
        try
        {
            FileUtility check = new FileUtility(path + fileName);
            assertFalse(" TestExists returned wrong value.", check.isFile());
        }
        catch (Exception e)
        {
            assertTrue(" TestExists failed " + e, false);
        }
    }

}
