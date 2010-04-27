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


package com.nokia.mj.test.file;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import java.util.*;
import java.io.*;

import java.io.IOException;
import javax.microedition.io.file.*;
import javax.microedition.io.*;

import com.nokia.mj.test.file.utils.FileTestUtils;
import com.nokia.mj.test.file.utils.FileTestBase;
import com.nokia.mj.test.file.utils.TestFailedException;
import com.nokia.mj.impl.installer.utils.InstallerMain;

public class TestFile_2100 extends FileTestBase
{
    public TestFile_2100()
    {
    }

    public TestFile_2100(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2100("run_2100", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2100) tc).run_2100();
            }
        }));
    }


    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());
        registerTests(suite);

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public void run_2100()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Uri contains trailing slash "/" in this case.
            FileTestUtils.createDir("file:///" + iRoot
                                    + "mainTestDir/test210/newDir/");
            counter = 1;
            // Uri doesn't contain trailing slash "/" in this case.
            FileTestUtils.createDir("file:///" + iRoot
                                    + "mainTestDir/test210/newDir2");
            counter = 2;
            FileTestUtils.createDir("file:///" + iRoot
                                    + "mainTestDir/test210/��Dir");
            counter = 3;
            assertTrue(" run_2100() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2100() counter: " + counter + "\n");
            assertTrue(" run_2100() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2100()
}
