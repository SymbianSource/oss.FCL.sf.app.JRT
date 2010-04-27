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
 * Checks cases related to canRead()
 */
public class TestCanWrite extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestCanWrite("testOnReadOnlyFile", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCanWrite) tc).testOnReadOnlyFile();
            }
        }));

        suite.addTest(new TestCanWrite("testOnReadOnlyDir", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCanWrite) tc).testOnReadOnlyDir();
            }
        }));

        suite.addTest(new TestCanWrite("testOnFile", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCanWrite) tc).testOnFile();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestCanWrite()
    {
    }

    public TestCanWrite(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    protected void setUp()
    {
        path = TestEnvironment.Main_Test_Folder + "testcanwrite/";
    }

    public void testOnReadOnlyFile()
    {
        String fileName = "testOnReadOnlyFile";
        try
        {
            FileUtility check = new FileUtility(path + fileName);
            assertFalse(" testOnReadOnlyFile returned wrong value.", check
                        .canWrite());
        }
        catch (Exception e)
        {
            assertTrue(" testOnReadOnlyFile failed " + e, false);
        }
    }

    public void testOnReadOnlyDir()
    {
        String fileName = "testOnReadOnlyDir";
        try
        {
            FileUtility check = new FileUtility(path + fileName);
            assertFalse(" testOnReadOnlyDir returned wrong value.", check
                        .canWrite());
        }
        catch (Exception e)
        {
            assertTrue(" testOnReadOnlyDir failed " + e, false);
        }
    }

    public void testOnFile()
    {
        String fileName = "testOnFile";
        try
        {
            FileUtility check = new FileUtility(path + fileName);
            assertTrue(" testOnFile returned wrong value.", check.canWrite());
        }
        catch (Exception e)
        {
            assertTrue(" testOnFile failed " + e, false);
        }
    }

}
