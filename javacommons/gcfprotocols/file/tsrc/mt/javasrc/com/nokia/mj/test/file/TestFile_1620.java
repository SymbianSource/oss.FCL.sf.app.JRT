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

public class TestFile_1620 extends FileTestBase
{
    public TestFile_1620()
    {
    }

    public TestFile_1620(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1620("run_1620", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1620) tc).run_1620();
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

    public void run_1620()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            //"Not exists" error case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test110/fadggt",
                                   Connector.READ_WRITE);
            counter = 1;
            if (conn1.exists())
                throw new TestFailedException();
            boolean exceptionOccured = false;
            try
            {
                conn1.setWritable(true);
            }
            catch (IOException ex)
            {
                exceptionOccured = true;
            }
            counter = 2;
            if (exceptionOccured == false)
                throw new TestFailedException();
            exceptionOccured = false;
            conn1.close();
            counter = 3;

            //"Connection has been opened in the READ mode" error case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test101/file101.txt",
                                   Connector.READ);
            counter = 4;
            exceptionOccured = false;
            try
            {
                conn1.setWritable(true);
            }
            catch (IllegalModeException ex)
            {
                exceptionOccured = true;
            }
            counter = 5;
            if (exceptionOccured == false)
                throw new TestFailedException();
            exceptionOccured = false;
            conn1.close();
            counter = 6;

            //"Connection closed" error case.
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test101/file101.txt",
                                                Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            conn1.close();
            counter = 7;
            try
            {
                conn1.setWritable(true);
            }
            catch (ConnectionClosedException ex)
            {
                exceptionOccured = true;
            }
            counter = 8;
            if (exceptionOccured == false)
                throw new TestFailedException();
            exceptionOccured = false;
            assertTrue(" run_1620() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1620() counter: " + counter + "\n");
            assertTrue(" run_1620() FAILED!!! Counter: "+counter, false);
        }
    }//end run_1620()
}
