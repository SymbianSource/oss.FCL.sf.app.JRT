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

import java.io.InputStreamReader;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.fileutils.*;
import com.nokia.mj.test.fileutils.TestEnvironment;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * FileURL unit tests.
 */
public class TestRenameTo extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestRenameTo("testFileRename", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRenameTo) tc).testFileRename();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestRenameTo()
    {
    }

    public TestRenameTo(String aTestName, TestMethod aTestMethod)
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
        path = TestEnvironment.Main_Test_Folder + "testrename/";
    }

    protected void tearDown()
    {
    }

    public void testFileRename()
    {
        try
        {
            String source = "C:\\Data\\Images\\renameToTestFile";
            FileUtility src = new FileUtility(source);
            if (!src.exists())
            {
                src.createNewFile();
            }

            String dest = "C:\\Data\\Videos\\notpresent\\renamed";

            assertTrue("Rename failed", src.renameTo(dest));
            FileUtility newOne = new FileUtility(dest);
            assertTrue("New File not created", newOne.exists());
        }
        catch (Exception e)
        {
            assertTrue("Test Failed. Unexpected Exception: " + e, false);
        }
    }
}
