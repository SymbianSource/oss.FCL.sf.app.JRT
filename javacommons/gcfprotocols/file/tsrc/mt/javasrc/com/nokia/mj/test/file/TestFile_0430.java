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

public class TestFile_0430 extends FileTestBase
{
    public TestFile_0430()
    {
    }

    public TestFile_0430(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0430("run_0430", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0430) tc).run_0430();
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

    public void run_0430()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Create a file and writing 4000 byte to it.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/new043File.txt",
                                                    Connector.READ_WRITE);
            counter = 1;
            conn1.create();
            counter = 111;
            OutputStream stream = conn1.openOutputStream();
            counter = 2;
            byte[] byteArray1 = new byte[4000];
            for (int i = 0; i < byteArray1.length; ++i)
                byteArray1[i] = 55;
            stream.write(byteArray1);
            counter = 3;
            stream.close();
            conn1.close();

            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/new043File.txt",
                                                    Connector.READ_WRITE);
            counter = 4;
            InputStream inStream = conn1.openInputStream();
            counter = 5;
            byte[] readArray = new byte[400];
            int retValue = inStream.read(readArray);
            if (retValue != 400)
                throw new TestFailedException("retValue: " + retValue);
            counter = 6;
            for (int i = 0; i < retValue; ++i)
            {
                if (readArray[i] != 55)
                    throw new TestFailedException("readArray[i]: "
                                                  + readArray[i] + "i: " + i);
            }
            counter = 7;
            stream = conn1.openOutputStream();
            byteArray1 = new byte[2000];
            for (int i = 0; i < byteArray1.length; ++i)
                byteArray1[i] = 56;
            stream.write(byteArray1);
            stream.flush();
            counter = 8;
            readArray = new byte[112];
            retValue = inStream.read(readArray);
            if (retValue != 112)
                throw new TestFailedException("retValue: " + retValue);
            counter = 9;
            for (int i = 0; i < retValue; ++i)
            {
                if (readArray[i] != 55)
                    throw new TestFailedException("readArray[i]: "
                                                  + readArray[i] + "i: " + i);
            }
            counter = 10;
            readArray = new byte[512];
            retValue = inStream.read(readArray);
            if (retValue != 512)
                throw new TestFailedException("retValue: " + retValue);
            counter = 11;
            for (int i = 0; i < retValue; ++i)
            {
                if (readArray[i] != 55)
                    throw new TestFailedException("readArray[i]: "
                                                  + readArray[i] + "i: " + i);
            }
            counter = 12;
            // Read next 512 bytes.
            readArray = new byte[512];
            retValue = inStream.read(readArray);
            if (retValue != 512)
                throw new TestFailedException("retValue: " + retValue);
            counter = 120;
            for (int i = 0; i < retValue; ++i)
            {
                if (readArray[i] != 56)
                    throw new TestFailedException("readArray[i]: "
                                                  + readArray[i] + "i: " + i);
            }
            counter = 121;
            inStream.close();
            byteArray1 = new byte[2000];
            for (int i = 0; i < byteArray1.length; ++i)
                byteArray1[i] = 57;
            stream.write(byteArray1);
            counter = 13;
            stream.close();
            conn1.close();

            // Re-opening a file and checking that all data was stored
            // successfully.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/new043File.txt",
                                                    Connector.READ_WRITE);
            counter = 14;
            inStream = conn1.openInputStream();
            counter = 15;
            readArray = new byte[5000];
            retValue = inStream.read(readArray);
            if (retValue != 4000)
                throw new TestFailedException("retValue: " + retValue);
            counter = 16;
            for (int i = 0; i < retValue; ++i)
            {
                if (i < 2000)
                {
                    if (readArray[i] != 56)
                        throw new TestFailedException("readArray[i]: "
                                                      + readArray[i] + "i: " + i);
                }
                else
                {
                    if (readArray[i] != 57)
                        throw new TestFailedException("readArray[i]: "
                                                      + readArray[i] + "i: " + i);

                }
            }
            counter = 17;
            inStream.close();
            conn1.close();

            assertTrue(" run_0430() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0430() counter: " + counter + "\n");
            assertTrue(" run_0430() FAILED!!! Counter: " + counter, false);
        }
    }//end run_0430()

}