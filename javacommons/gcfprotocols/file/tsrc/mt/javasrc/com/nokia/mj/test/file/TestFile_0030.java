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

import java.io.IOException;
import javax.microedition.io.file.*;
import javax.microedition.io.*;


import com.nokia.mj.test.file.utils.FileTestUtils;
import com.nokia.mj.test.file.utils.FileTestBase;
import com.nokia.mj.test.file.utils.TestFailedException;
import com.nokia.mj.impl.installer.utils.InstallerMain;

public class TestFile_0030 extends FileTestBase
{
    public TestFile_0030()
    {
    }

    public TestFile_0030(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0030("run_0030", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0030) tc).run_0030();
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

    public void run_0030()
    {
        int counter = 0;
        try
        {
            // ReadOnly file case.
            FileTestUtils.IoExceptionCheck(
                "file:///" + iRoot + "mainTestDir/test000/file004",
                Connector.READ, true);
            counter = 1;

            FileTestUtils.IoExceptionCheck(
                "file:///" + iRoot + "mainTestDir/test000/file004",
                Connector.WRITE, true);
            counter = 2;

            FileTestUtils.IoExceptionCheck(
                "file:///" + iRoot + "mainTestDir/test000/file004",
                Connector.READ_WRITE, true);
            counter = 3;

            // ReadOnly dir case.
            FileTestUtils.IoExceptionCheck("file:///" + iRoot
                                           + "mainTestDir/test000/dir 004/", Connector.READ, true);
            counter = 4;
            FileTestUtils.IoExceptionCheck("file:///" + iRoot
                                           + "mainTestDir/test000/dir 004/", Connector.WRITE, true);
            counter = 5;
            FileTestUtils.IoExceptionCheck("file:///" + iRoot
                                           + "mainTestDir/test000/dir 004/", Connector.READ_WRITE,
                                           true);
            counter = 6;
            // Make a READ connection to a new directory.
            FileTestUtils.IoExceptionCheck("file:///" + iRoot
                                           + "mainTestDir/test000/dir 020/", Connector.READ, true);
            counter = 7;

            // Hidden file case.
            FileTestUtils.IoExceptionCheck("file:///" + iRoot
                                           + "mainTestDir/test000/fileHidden.txt", Connector.READ,
                                           true);
            counter = 8;
            // Hidden file case.
            FileTestUtils.IoExceptionCheck("file:///" + iRoot
                                           + "mainTestDir/test000/fileHidden.txt", Connector.WRITE,
                                           true);
            counter = 9;
            // Hidden file case.
            FileTestUtils.IoExceptionCheck("file:///" + iRoot
                                           + "mainTestDir/test000/fileHidden.txt",
                                           Connector.READ_WRITE, true);
            counter = 10;
            // Hidden dir case.
            FileTestUtils.IoExceptionCheck("file:///" + iRoot
                                           + "mainTestDir/test000/dirHidden/", Connector.READ, true);
            counter = 11;
            // Hidden dir case.
            FileTestUtils.IoExceptionCheck("file:///" + iRoot
                                           + "mainTestDir/test000/dirHidden/", Connector.WRITE, true);
            counter = 12;
            // Hidden dir case.
            FileTestUtils.IoExceptionCheck("file:///" + iRoot
                                           + "mainTestDir/test000/dirHidden/", Connector.READ_WRITE,
                                           true);
            counter = 13;

            // System file case.
            FileTestUtils.IoExceptionCheck("file:///" + iRoot
                                           + "mainTestDir/test000/fileSystem.txt", Connector.READ,
                                           true);
            counter = 14;
            // System file case.
            FileTestUtils.IoExceptionCheck("file:///" + iRoot
                                           + "mainTestDir/test000/fileSystem.txt", Connector.WRITE,
                                           true);
            counter = 15;
            // System file case.
            FileTestUtils.IoExceptionCheck("file:///" + iRoot
                                           + "mainTestDir/test000/fileSystem.txt",
                                           Connector.READ_WRITE, true);
            counter = 16;

            // System dir case.
            FileTestUtils.IoExceptionCheck("file:///" + iRoot
                                           + "mainTestDir/test000/dirSystem/", Connector.READ, true);
            counter = 17;
            // System dir case.
            FileTestUtils.IoExceptionCheck("file:///" + iRoot
                                           + "mainTestDir/test000/dirSystem/", Connector.WRITE, true);
            counter = 18;
            // System dir case.
            FileTestUtils.IoExceptionCheck("file:///" + iRoot
                                           + "mainTestDir/test000/dirSystem/", Connector.READ_WRITE,
                                           true);
            counter = 19;

            counter = 20;

            assertTrue(" run_0030() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_0030(): counter: " + counter + "\n");
            assertTrue(" run_0030() FAILED!!! Counter: "+counter, false);
        }
    }//end run_0030()

}