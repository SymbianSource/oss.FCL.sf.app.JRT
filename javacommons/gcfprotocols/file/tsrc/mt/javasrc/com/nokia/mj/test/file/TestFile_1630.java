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

public class TestFile_1630 extends FileTestBase
{
    public TestFile_1630()
    {
    }

    public TestFile_1630(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1630("run_1630", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1630) tc).run_1630();
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

    public void run_1630()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test160/binaryFile.txt",
                                                    Connector.READ_WRITE);
            counter = 1;
            boolean writeFlag = conn1.canWrite();
            if (writeFlag == true)
                throw new TestFailedException();
            counter = 2;
            InputStream stream = conn1.openInputStream();
            counter = 3;
            byte[] byteArray1 = new byte[512]; // CSB: was 512 before
            int retValue = stream.read(byteArray1);
            counter = 4;
            if (retValue != 512)
                throw new TestFailedException("retValue: " + retValue);
            counter = 5;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException("byteArray1[i]: "
                                                  + byteArray1[i] + " i: " + i);
            }
            counter = 6;
            try
            {
                conn1.openOutputStream();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 7;
            conn1.setWritable(true);
            conn1.setWritable(true);
            counter = 8;
            writeFlag = conn1.canWrite();
            if (writeFlag == false)
                throw new TestFailedException();
            counter = 9;
            try
            {
                retValue = stream.read(byteArray1);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }

            // Re-opening an InputStrem.
            stream = conn1.openInputStream();
            byte[] byteArray12 = new byte[1200];
            retValue = stream.read(byteArray12);
            counter = 91;
            if (retValue != 1025)
                throw new TestFailedException("retValue: " + retValue);
            counter = 92;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray12[i] != 56)
                    throw new TestFailedException("byteArray12[i]: "
                                                  + byteArray12[i] + "i: " + i);
            }
            stream.close();
            counter = 10;
            OutputStream outStream = conn1.openOutputStream();
            byte[] byteArray2 = new byte[1000];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 57;
            }
            counter = 11;
            outStream.write(byteArray1);
            counter = 12;
            conn1.truncate(50);
            counter = 13;
            conn1.close();
            outStream.close();

            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test160/binaryFile.txt",
                                                Connector.READ_WRITE);
            counter = 14;
            stream = conn1.openInputStream();
            counter = 15;
            byteArray1 = new byte[512];
            retValue = stream.read(byteArray1);
            counter = 16;
            if (retValue != 50)
                throw new TestFailedException("retValue: " + retValue);
            counter = 17;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 57)
                    throw new TestFailedException("byteArray1[i]: "
                                                  + byteArray1[i] + "i: " + i);
            }
            stream.close();
            conn1.close();

            assertTrue(" run_1630() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1630() counter: " + counter + "\n");
            assertTrue(" run_1630() FAILED!!! Counter: " + counter, false);
        }
    }//end run_1630()
}
