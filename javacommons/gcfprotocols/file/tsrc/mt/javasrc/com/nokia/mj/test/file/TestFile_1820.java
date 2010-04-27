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

public class TestFile_1820 extends FileTestBase
{
    public TestFile_1820()
    {
    }

    public TestFile_1820(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1820("run_1820", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1820) tc).run_1820();
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

    public void run_1820()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // "Not exists" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test1100/", Connector.READ_WRITE);
            if (conn1.exists())
                throw new TestFailedException();
            counter = 1;
            try
            {
                conn1.list();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            conn1.close();

            // "Connection points to the file" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/file100.txt", Connector.READ_WRITE);
            counter = 2;
            if (!conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.list();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            conn1.close();

            // "Connection has been opened in the WRITE mode" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/file100.txt", Connector.WRITE);
            counter = 3;
            try
            {
                conn1.list();
                throw new TestFailedException();
            }
            catch (IllegalModeException ex)
            {
            }
            conn1.close();

            // "Connection closed" error case.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test100/", Connector.READ_WRITE);
            counter = 4;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 5;
            conn1.close();
            try
            {
                conn1.list();
                throw new TestFailedException();
            }
            catch (ConnectionClosedException ex)
            {
            }
            conn1.close();

            // READ_WRITE connection established to the read-only file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/binaryFile2.dat",
                                                    Connector.READ_WRITE);
            counter = 6;
            try
            {
                conn1.list();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 5;
            conn1.close();

            assertTrue(" run_1820() ok ", true);
            System.out.println(" run_1820() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1820() counter: " + counter + "\n");
            assertTrue(" run_1820() FAILED!!! Counter: " + counter, false);
        }
    }//end run_1820()

}
