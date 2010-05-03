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

import java.io.IOException;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.fileutils.*;
import com.nokia.mj.test.fileutils.TestEnvironment;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * Tests FileUtility.create(). Ensure that you run after exists. Test case
 * assumes that exists() works fine.
 */
public class TestLastModified extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestLastModified("testLastModified",
                                           new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestLastModified) tc).testLastModified();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestLastModified()
    {
    }

    public TestLastModified(String aTestName, TestMethod aTestMethod)
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
        path = TestEnvironment.Main_Test_Folder + "testlastmodified/";
    }

    private void testLastModified()
    {
        String name = "newFile";
        try
        {
            FileUtility target = new FileUtility(path + name);
            long curTime = System.currentTimeMillis();

            target.createNewFile();
            Thread.sleep(100);

            long lastModified = target.lastModified();
            long curTime1 = System.currentTimeMillis();
            System.out.println("Initial Time: " + curTime + " Last modified: "
                               + lastModified + " Diff: " + (lastModified - curTime)
                               + "\n Last modified cur: " + curTime1 + " SecDiff: "
                               + (curTime1 - curTime));
        }
        catch (Exception e)
        {
            assertFalse("testLastModified failed", true);
        }
    }
}
