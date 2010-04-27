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

public class TestFile_2790_2792 extends FileTestBase
{
    public TestFile_2790_2792()
    {
    }

    public TestFile_2790_2792(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        // Test expects truncate to throw IOException in case 2 connections are
        // open simultaneously
//        suite.addTest(new TestFile_2790_2792("run_2790", new TestMethod() {
//            public void run(TestCase tc)
//            {
//                ((TestFile_2790_2792) tc).run_2790();
//            }
//        }));

        suite.addTest(new TestFile_2790_2792("run_2792", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2790_2792) tc).run_2792();
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

    public void run_2790()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Calling setFileConnection() operation when InputStream is opened
            // to the
            // file by other connection.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "data/Images/myDir/myImages2/myImagesFile.txt");
            counter = 1;
            InputStream stream = conn1.openInputStream();
            counter = 2;
            FileConnection conn2 = (FileConnection) Connector.open("file:///"
                                   + iRoot + "data/Images/myDir/myImages2/");
            counter = 3;
            conn2.setFileConnection("myImagesFile.txt");
            counter = 4;
            try
            {
                conn2.truncate(800);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 5;
            stream.close();
            conn2.truncate(800);
            long fileSize = conn1.fileSize();
            if (800 != fileSize)
                throw new TestFailedException("File size returned: "+fileSize);
            counter = 6;
            conn1.close();
            conn2.close();

            assertTrue(" run_2790() ok ", true);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2790() counter: " + counter + "\n");
            assertTrue(" run_2790() FAILED!!! Counter: "+counter, false);
        }
    }// end run_2790()

    //
    //
    //
    //
    public void run_2792()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // 1
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/", Connector.READ);
            counter = 1;
            conn1.setFileConnection("binaryFile.dat");
            counter = 2;
            InputStream stream = conn1.openInputStream();
            byte[] byteArray1 = new byte[2000];
            int retValue = stream.read(byteArray1);
            counter = 3;
            if (retValue != 1025)
                throw new TestFailedException();
            counter = 4;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 4)
                    throw new TestFailedException();
            }
            counter = 5;
            stream.close();
            conn1.close();

            // 2
//            conn1 = (FileConnection) Connector.open(
//                    "file:///Internal/mainTestDir/test020/", Connector.READ);
            conn1 = (FileConnection) Connector.open(
                        "file:///"+ iRoot+ "mainTestDir/test020/", Connector.READ);

            counter = 6;

            conn1.setFileConnection("binaryFile.dat");
            counter = 7;
            stream = conn1.openInputStream();
            byteArray1 = new byte[2000];
            retValue = stream.read(byteArray1);
            counter = 8;
            if (retValue != 1025)
                throw new TestFailedException();
            counter = 9;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 4)
                    throw new TestFailedException();
            }
            counter = 10;
            stream.close();
            conn1.close();

            assertTrue(" run_2792() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2792() counter: " + counter + "\n");
            assertTrue(" run_2792() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2792()

}
