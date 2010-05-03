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

public class TestFile_0081 extends FileTestBase
{
    public TestFile_0081()
    {
    }

    public TestFile_0081(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0081("run_0081", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0081) tc).run_0081();
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

    public void run_0081()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // 1
            FileTestUtils.testGetConnectionMethods("file://localhost/Internal/", "",
                                                   "/Internal/", "file://localhost/Internal/");
            counter = 1;
            FileTestUtils.testGetConnectionMethods("file:///Memory%20caRd/", "",
                                                   "/Memory caRd/", "file:///Memory%20caRd/");
            counter = 2;
            FileTestUtils.testGetConnectionMethods("file:///Memory caRd/", "",
                                                   "/Memory caRd/", "file:///Memory%20caRd/");
            counter = 3;
            FileTestUtils.testGetConnectionMethods("file:///Temporary/", "", "/Temporary/",
                                                   "file:///Temporary/");
            counter = 4;
            FileTestUtils.testGetConnectionMethods("file:///Rom/", "", "/Rom/",
                                                   "file:///Rom/");
            counter = 5;
            try
            {
                conn1 = (FileConnection) Connector.open("file:///Internal2/");
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 6;
            try
            {
                conn1 = (FileConnection) Connector.open("file:///Internal");
                throw new TestFailedException();
            }
            catch (IllegalArgumentException ex)
            {
            }
            counter = 7;

            if (iMultiDriveSupportTests)
            {
                FileTestUtils.testGetConnectionMethods("file:///InterNal G/", "",
                                                       "/InterNal G/", "file:///InterNal%20G/");
                counter = 8;
            }

            assertTrue(" run_0081() ok ", true);
        }

        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_0081(): counter: " + counter + "\n");
            assertTrue(" run_0081() FAILED!!! Counter: "+counter, false);
            return;
        }
    }//end run_0081()
}