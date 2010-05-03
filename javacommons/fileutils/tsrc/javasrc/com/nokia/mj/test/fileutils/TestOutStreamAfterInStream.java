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

import java.io.OutputStream;

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
public class TestOutStreamAfterInStream extends TestCase implements
        InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestOutStreamAfterInStream("testStream",
                      new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestOutStreamAfterInStream) tc).testStream();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestOutStreamAfterInStream()
    {
    }

    public TestOutStreamAfterInStream(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    protected void setUp()
    {
        path = TestEnvironment.Main_Test_Folder + "testinputstream/";
    }

    public void testStream()
    {
        String name = "inputFile.txt";
        try
        {
            FileUtility check = new FileUtility(path + name);
            check.openInputStream();
            OutputStream fos = check.openOutputStream();
            fos.write(66);
            fos.close();
        }
        catch (Exception e)
        {
            assertTrue(" TestExists failed " + e, false);
        }
    }
}