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

import java.io.InputStream;
import java.io.IOException;

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
public class TestInputStream extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestInputStream("testInputStreamRead",
                                          new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestInputStream) tc).testInputStreamRead();
            }
        }));

        suite.addTest(new TestInputStream("testInputStreamSkip",
                                          new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestInputStream) tc).testInputStreamSkip();
            }
        }));

        suite.addTest(new TestInputStream("testInputStreamMarkAndReset",
                                          new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestInputStream) tc).testInputStreamMarkAndReset();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestInputStream()
    {
    }

    public TestInputStream(String aTestName, TestMethod aTestMethod)
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
        path = TestEnvironment.Main_Test_Folder + "testinputstream/";
    }

    public void testInputStreamRead()
    {
        String fileName = "inputFile.txt";

        try
        {
            FileUtility check = new FileUtility(path + fileName);
            InputStream ios = check.openInputStream();

            byte[] buffer = new byte[1001];
            int ret = ios.read(buffer);
            assertTrue("Amount read wrong", (1000 == ret));

            for (int i = 0; i < 1000; i++)
            {
                if (1 != buffer[i])
                {
                    assertTrue("Data read is wrong", false);
                }
            }
            ios.close();
            check = null;
        }
        catch (Exception e)
        {
            assertTrue(" TestInputStream failed " + e, false);
        }
    }

    public void testInputStreamSkip()
    {
        String fileName = "inputFile001.txt";

        try
        {
            FileUtility check = new FileUtility(path + fileName);
            InputStream ios = check.openInputStream();

            byte[] buffer = new byte[1000];
            int ret = ios.read(buffer);
            assertTrue("Amount read wrong Ret: ", (1000 == ret));

            for (int i = 0; i < 1000; i++)
            {
                if (1 != buffer[i])
                {
                    assertTrue("Data read is wrong", false);
                }
            }

            assertTrue("Skip return was invalid", (1000 == ios.skip(1000)));

            ret = ios.read(buffer);
            assertTrue("Amount read wrong Ret: " + ret, (1000 == ret));

            for (int i = 0; i < 1000; i++)
            {
                if (3 != buffer[i])
                {
                    assertTrue("Data read is wrong", false);
                }
            }
        }
        catch (Exception e)
        {
            assertTrue(" TestInputStream failed " + e, false);
        }
    }

    public void testInputStreamMarkAndReset()
    {
        String fileName = "inputFileMark.txt";

        try
        {
            FileUtility check = new FileUtility(path + fileName);
            InputStream ios = check.openInputStream();

            byte[] buffer10 = new byte[10];
            byte[] buffer20 = new byte[20];

            ios.mark(20);
            int ret = ios.read(buffer10);

            for (int i = 0; i < 10; i++)
            {
                if (5 != buffer10[i])
                {
                    assertTrue("Data read is wrong", false);
                }
            }

            ios.reset();
            ret = ios.read(buffer20);
            for (int i = 0; i < 20; i++)
            {
                if (5 != buffer20[i])
                {
                    assertTrue("Data read is wrong", false);
                }
            }

            // Try setting mark to -1
            ios.mark(-1);
        }
        catch (Exception e)
        {
            assertTrue(" TestInputStream failed " + e, false);
        }
    }
}
