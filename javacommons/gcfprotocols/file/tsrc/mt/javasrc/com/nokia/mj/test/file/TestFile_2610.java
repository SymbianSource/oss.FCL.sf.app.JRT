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

public class TestFile_2610 extends FileTestBase
{
    public TestFile_2610()
    {
    }

    public TestFile_2610(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2610("run_2610", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2610) tc).run_2610();
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

    public void run_2610()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Call truncate() operation before flushing or closing an
            // OutputStream.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test260/file260b.txt", Connector.READ_WRITE);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 2;
            OutputStream outStream = conn1.openOutputStream(101);
            byte[] byteArray1 = new byte[10];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 57;
            }
            outStream.write(byteArray1);
            counter = 3;
            conn1.truncate(105);
            counter = 5;
            InputStream stream = conn1.openInputStream();
            byteArray1 = new byte[500];
            int retValue = stream.read(byteArray1);
            counter = 6;
            if (retValue != 105)
                throw new TestFailedException("retValue: " + retValue);
            counter = 7;
            for (int i = 0; i < retValue; ++i)
            {
                if (i < 101)
                {
                    if (byteArray1[i] != 56)
                        throw new TestFailedException("i: " + i
                                                      + " byteArray1[i]: " + byteArray1[i]);
                }
                else
                {
                    if (byteArray1[i] != 57)
                        throw new TestFailedException("i: " + i
                                                      + " byteArray1[i]: " + byteArray1[i]);
                }
            }
            counter = 8;
            stream.close();
            outStream.close();
            conn1.close();

            // Same test as above except DataOutputStream was used instead of
            // the
            // OutputStream.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test260/file260e.txt", Connector.READ_WRITE);
            counter = 9;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 10;
            DataOutputStream dataOutStream = conn1.openDataOutputStream();
            byteArray1 = new byte[100];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 57;
            }
            dataOutStream.write(byteArray1);
            counter = 11;
            conn1.truncate(60);
            counter = 12;
            DataInputStream dataInStream = conn1.openDataInputStream();
            byteArray1 = new byte[500];
            retValue = dataInStream.read(byteArray1);
            counter = 13;
            if (retValue != 60)
                throw new TestFailedException("retValue: " + retValue);
            counter = 14;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 57)
                    throw new TestFailedException("i: " + i
                                                  + " byteArray1[i]: " + byteArray1[i]);
            }
            counter = 15;
            dataInStream.close();
            conn1.close();

            // Pass size of the file as argument to the truncate() operation.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test260/file260c.txt", Connector.READ_WRITE);
            counter = 16;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 17;
            // conn1.truncate(10);
            conn1.truncate(conn1.fileSize());
            counter = 18;
            stream = conn1.openInputStream();
            byteArray1 = new byte[101];
            retValue = stream.read(byteArray1);
            counter = 19;
            if (retValue != 10)
                throw new TestFailedException("retValue: " + retValue);
            counter = 20;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException("i: " + i
                                                  + " byteArray1[i]: " + byteArray1[i]);
            }
            counter = 21;
            stream.close();
            conn1.close();

            // Pass 0 as argument to the truncate() operation.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test260/file260d.txt", Connector.READ_WRITE);
            counter = 22;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 23;
            conn1.truncate(0);
            counter = 24;
            stream = conn1.openInputStream();
            byteArray1 = new byte[101];
            retValue = stream.read(byteArray1);
            counter = 25;
            if (retValue != -1)
                throw new TestFailedException("retValue: " + retValue);
            counter = 26;
            stream.close();
            conn1.close();

            assertTrue(" run_2610() ok ", true);
            System.out.println(" run_2610() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2610() counter: " + counter + "\n");
            assertTrue(" run_2610() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2610()

}
