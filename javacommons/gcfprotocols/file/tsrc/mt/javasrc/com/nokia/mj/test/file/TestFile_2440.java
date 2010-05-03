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

public class TestFile_2440 extends FileTestBase
{
    public TestFile_2440()
    {
    }

    public TestFile_2440(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2440("run_2440", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2440) tc).run_2440();
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

    public void run_2440()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // "Connection has been opened in the READ mode" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/file100.txt", Connector.READ);
            counter = 1;
            if (!conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (IllegalModeException ex)
            {
            }
            counter = 2;
            conn1.close();

            // "Connection closed" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/file100.txt", Connector.READ);
            if (!conn1.exists())
                throw new TestFailedException();
            conn1.close();
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (ConnectionClosedException ex)
            {
            }
            counter = 5;
            conn1.close();

            // "Directory is not empty" error case.
            // Note: Other IOExceptions has been tested in the previous
            // testcases.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/", Connector.READ_WRITE);
            counter = 6;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 7;
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 8;
            conn1.close();

            // READ_WRITE access test for read-only file.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test020/binaryFile2.dat",
                                                Connector.READ_WRITE);
            counter = 9;
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 10;
            conn1.close();

            // READ_WRITE connection established to the read-only directory.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test200/readonlyDir/", Connector.READ_WRITE);
            counter = 11;
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 12;
            conn1.close();

            assertTrue(" run_2440() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2440() counter: " + counter + "\n");
            assertTrue(" run_2440() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2440()

}
