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
public class TestFileSize extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestFileSize("testOnFile", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileSize) tc).testOnFile();
            }
        }));

        suite.addTest(new TestFileSize("testOnDir", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileSize) tc).testOnDir();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestFileSize()
    {
    }

    public TestFileSize(String aTestName, TestMethod aTestMethod)
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
        path = TestEnvironment.Main_Test_Folder + "testfilesize/";
    }

    public void testOnFile()
    {
        String fileName = "testOnFile";
        try
        {
            FileUtility check = new FileUtility(path + fileName);
            assertTrue(" TestFileSize returned wrong value.", (1000 == check
                       .fileSize()));
        }
        catch (Exception e)
        {
            assertTrue(" TestFileSize failed " + e, false);
        }
    }

    public void testOnDir()
    {
        String fileName = "testOnDir";
        try
        {
            FileUtility check = new FileUtility(path + fileName);
            long ret = check.fileSize();
            System.out.println("Size on dir" + ret);
            assertTrue(" TestFileSize did not throw exception", false);

        }
        catch (java.io.IOException ioe)
        {
            // Good test passed.
        }
        catch (Exception e)
        {
            assertTrue(" TestFileSize failed " + e, false);
        }
    }

}
