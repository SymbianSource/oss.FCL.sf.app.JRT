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

public class TestFile_2020 extends FileTestBase
{
    public TestFile_2020()
    {
    }

    public TestFile_2020(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2020("run_2020", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2020) tc).run_2020();
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

    public void run_2020()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // Opening a existing file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test200/ExistFile202.txt",
                                                    Connector.READ_WRITE);
            counter = 1;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 2;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test200/ExistFile202.txt",
                                                    Connector.READ_WRITE);
            counter = 3;
            conn1.delete();
            counter = 4;
            if (conn1.exists())
                throw new TestFailedException();
            counter = 41;
            if (conn2.exists())
                throw new TestFailedException();
            counter = 42;
            conn2.create();
            counter = 5;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 6;
            if (!conn2.exists())
                throw new TestFailedException();
            counter = 7;
            conn1.close();
            conn2.close();

            // Opening read-only file in the READ_WRITE mode.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test200/ReadOnlyFile202.txt",
                                                    Connector.READ_WRITE);
            counter = 8;
            if (true == conn1.canWrite())
                throw new TestFailedException();
            counter = 9;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 10;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test200/ReadOnlyFile202.txt",
                                                    Connector.READ_WRITE);
            counter = 11;
            conn1.setWritable(true);
            counter = 12;
            conn1.delete();
            counter = 13;
            if (conn1.exists())
                throw new TestFailedException();
            counter = 14;
            if (conn2.exists())
                throw new TestFailedException();
            counter = 15;
            conn2.create();
            counter = 16;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 17;
            if (!conn2.exists())
                throw new TestFailedException();
            counter = 18;
            conn1.close();
            conn2.close();

            assertTrue(" run_2020() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2020() counter: " + counter + "\n");
            assertTrue(" run_2020() FAILED!!! Counter: "+counter, false);
        }
    }//end run_2020()

}
