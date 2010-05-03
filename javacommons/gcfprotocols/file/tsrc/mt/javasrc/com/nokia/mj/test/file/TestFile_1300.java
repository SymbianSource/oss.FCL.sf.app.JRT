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

public class TestFile_1300 extends FileTestBase
{
    public TestFile_1300()
    {
    }

    public TestFile_1300(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1300("run_1300", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1300) tc).run_1300();
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

    public void run_1300()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Writable file case.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test100/file100.txt", Connector.READ_WRITE);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 2;
            boolean writeFlag = conn1.canWrite();
            if (writeFlag == false)
                throw new TestFailedException();
            conn1.close();
            counter = 3;
            // Read-only file case.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test100/file100b.txt", Connector.READ);
            counter = 4;
            if (conn1 == null)
                throw new TestFailedException();
            writeFlag = conn1.canWrite();
            counter = 41;
            if (writeFlag == true)
                throw new TestFailedException();
            conn1.close();
            counter = 5;

            // Writable directory case.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test100/subDir", Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 6;
            writeFlag = conn1.canWrite();
            if (writeFlag == false)
                throw new TestFailedException();
            conn1.close();
            counter = 7;

            // Read-only directory case.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test000/dir 004", Connector.READ);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 8;
            writeFlag = conn1.canWrite();
            if (writeFlag == true)
                throw new TestFailedException();
            conn1.close();
            counter = 9;

            // Passing a new directory's URI as parameter to the
            // Connector.open()
            // operation.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test110/fadggt/", Connector.READ_WRITE);
            if (conn1.exists())
                throw new TestFailedException();
            counter = 10;
            writeFlag = conn1.canWrite();
            if (writeFlag == true)
                throw new TestFailedException();
            conn1.close();

            // Make a READ_WRITE connection to the read-only file and call
            // canWrite()
            // operation.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/binaryFile2.dat",
                                                    Connector.READ_WRITE);
            counter = 13;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 14;
            boolean flag = conn1.canWrite();
            counter = 15;
            if (true == flag)
                throw new TestFailedException();
            counter = 16;
            conn1.close();

            // Calling canWrite() operation when InputStream is opened to the
            // file by other connection.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "data/Images/myDir/myImages2/myImagesFile.txt");
            counter = 17;
            InputStream stream = conn1.openInputStream();
            counter = 18;
            FileConnection conn2 = (FileConnection) Connector.open("file:///"
                                   + iRoot + "data/Images/myDir/myImages2/myImagesFile.txt");
            counter = 19;
            if (false == conn2.canWrite())
                throw new TestFailedException();
            conn1.close();
            conn2.close();
            stream.close();

            assertTrue(" run_1300() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1300() counter: " + counter + "\n");
            assertTrue(" run_1300() FAILED!!! Counter: "+counter, false);
        }
    }//end run_1300()
}