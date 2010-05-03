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

public class TestFile_2900 extends FileTestBase
{
    public TestFile_2900()
    {
    }

    public TestFile_2900(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2900("run_2900", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2900) tc).run_2900();
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

    public void run_2900()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test280/new290File", Connector.READ_WRITE);
            counter = 1;
            if (conn1.exists())
                throw new TestFailedException();
            counter = 2;
            long modifyTime = conn1.lastModified();
            if (0 != modifyTime)
                throw new TestFailedException("modifyTime: " + modifyTime);
            long startTime = System.currentTimeMillis();
            // round startTime down to nearest minute
            startTime -= startTime % 60000;
            Thread.sleep(3000);
            counter = 3;
            conn1.create();
            Thread.sleep(3000);
            long endTime = System.currentTimeMillis();
            // round endTime up to nearest minute
            endTime += 60000 - (endTime % 60000);
            counter = 4;
            modifyTime = conn1.lastModified();
            boolean passed = modifyTime >= startTime && modifyTime <= endTime;
            if (false == passed)
                throw new TestFailedException("\ns: " + startTime + "\n"
                                              + "m: " + modifyTime + "\n" + "e: " + endTime + "\n");
            counter = 5;
            conn1.close();

            // WRITE access test for read-only file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/binaryFile2.dat",
                                                    Connector.READ_WRITE);
            counter = 6;
            long lastModified = conn1.lastModified();
            counter = 7;
            if (lastModified <= 0)
                throw new TestFailedException();
            counter = 8;
            conn1.close();

            assertTrue(" run_2900() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2900() counter: " + counter + "\n");
            assertTrue(" run_2900() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2900()

}
