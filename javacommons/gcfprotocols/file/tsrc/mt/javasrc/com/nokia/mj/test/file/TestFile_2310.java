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

public class TestFile_2310 extends FileTestBase
{
    public TestFile_2310()
    {
    }

    public TestFile_2310(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2310("run_2310", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2310) tc).run_2310();
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

    public void run_2310()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // "Connection has been opened in the WRITE mode" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/file100.txt", Connector.WRITE);
            try
            {
                conn1.isDirectory();
                throw new TestFailedException();
            }
            catch (IllegalModeException ex)
            {
            }
            conn1.close();

            // "Connection closed" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/file100.txt", Connector.READ);
            if (!conn1.exists())
                throw new TestFailedException();
            conn1.close();
            try
            {
                conn1.isDirectory();
                throw new TestFailedException();
            }
            catch (ConnectionClosedException ex)
            {
            }
            conn1.close();

            // READ_WRITE connection established to the read-only directory.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test200/readonlyDir/", Connector.READ_WRITE);
            counter = 9;
            boolean isDirFlag = conn1.isDirectory();
            if (false == isDirFlag)
                throw new TestFailedException();
            counter = 10;
            conn1.close();

            // READ_WRITE connection established to the read-only file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/binaryFile2.dat",
                                                    Connector.READ_WRITE);
            counter = 11;
            isDirFlag = conn1.isDirectory();
            if (true == isDirFlag)
                throw new TestFailedException();
            counter = 12;
            conn1.close();

            // Calling isDirectory() operation when InputStream is opened to the
            // file by other connection.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "data/Images/myDir/myImages2/myImagesFile.txt");
            counter = 13;
            InputStream stream = conn1.openInputStream();
            counter = 14;
            FileConnection conn2 = (FileConnection) Connector.open("file:///"
                                   + iRoot + "data/Images/myDir/myImages2/myImagesFile.txt");
            counter = 15;
            if (true == conn2.isDirectory())
                throw new TestFailedException();
            counter = 16;
            conn1.close();
            stream.close();
            conn2.close();

            assertTrue(" run_2310() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2310() counter: " + counter + "\n");
            assertTrue(" run_2310() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2310()

}
