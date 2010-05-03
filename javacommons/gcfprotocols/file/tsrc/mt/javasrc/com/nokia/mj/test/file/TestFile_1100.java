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

public class TestFile_1100 extends FileTestBase
{
    public TestFile_1100()
    {
    }

    public TestFile_1100(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1100("run_1100", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1100) tc).run_1100();
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

    public void run_1100()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Making a connection to the existing file.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test060/binaryFile.dat",
                                                Connector.READ_WRITE);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 2;
            long fileSize = conn1.fileSize();
            counter = 3;
            conn1.close();
            if (fileSize != 100)
                throw new TestFailedException("fileSize: " + fileSize);
            counter = 4;
            // A new file case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test110/newfile.txt", Connector.READ_WRITE);
            counter = 5;
            if (conn1.exists())
                throw new TestFailedException();
            counter = 6;
            fileSize = conn1.fileSize();
            conn1.close();
            if (fileSize != -1)
                throw new TestFailedException("fileSize: " + fileSize);
            counter = 7;

            // Calling fileSize() after creation of the new file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test110/newfile2.txt", Connector.READ_WRITE);
            counter = 8;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.create();
            counter = 9;
            DataOutputStream dataStream = conn1.openDataOutputStream();
            int addedNum = 57;
            for (int i = 0; i < 2001; ++i)
            {
                dataStream.write(addedNum);
            }
            counter = 10;
//  Not true anymore.
//            fileSize = conn1.fileSize();
//            if (fileSize != 0)
//                throw new TestFailedException("fileSize: " + fileSize);
            counter = 11;
            dataStream.flush();
            dataStream.close();
            fileSize = conn1.fileSize();
            if (fileSize != 2001)
                throw new TestFailedException("fileSize: " + fileSize);
            counter = 12;
            conn1.close();

            // Testing that file size can be read from the read-only file using
            // READ_WRITE connection.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/binaryFile2.dat",
                                                    Connector.READ_WRITE);
            counter = 13;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 14;
            fileSize = conn1.fileSize();
            counter = 15;
            if (fileSize != 1025)
                throw new TestFailedException("fileSize: " + fileSize);
            counter = 16;
            conn1.close();

            // Calling fileSize() operation when InputStream is opened to the
            // file by other connection.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "data/Images/myDir/myImages2/myImagesFile.txt");
            counter = 17;
            InputStream stream = conn1.openInputStream();
            counter = 18;
            FileConnection conn2 = (FileConnection) Connector.open("file:///"
                                   + iRoot + "data/Images/myDir/myImages2/myImagesFile.txt");
            counter = 19;
            fileSize = conn2.fileSize();
            if (fileSize != 1000)
                throw new TestFailedException("fileSize: " + fileSize);
            conn1.close();
            conn2.close();
            stream.close();

            assertTrue(" run_1100() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1100() counter: " + counter + "\n");
            assertTrue(" run_1100() FAILED!!! Counter: " + counter, false);
        }
    }//end run_1100()
}