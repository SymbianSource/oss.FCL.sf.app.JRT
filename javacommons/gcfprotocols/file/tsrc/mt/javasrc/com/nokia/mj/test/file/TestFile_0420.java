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

public class TestFile_0420 extends FileTestBase
{
    public TestFile_0420()
    {
    }

    public TestFile_0420(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0420("run_0420", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0420) tc).run_0420();
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

    public void run_0420()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Make a connection to the new file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/new042File.txt",
                                                    Connector.READ_WRITE);
            counter = 1;
            System.out.println(" run_0420: counter = 1 ");
            if (conn1.exists())
                throw new TestFailedException();
            counter = 2;
            System.out.println(" run_0420: counter = 2 ");
            conn1.create();
            counter = 3;
            System.out.println(" run_0420: counter = 3 ");
            OutputStream stream = conn1.openOutputStream();
            counter = 4;
            System.out.println(" run_0420: counter = 4 ");
            byte[] byteArray1 = new byte[50];
            for (int i = 0; i < byteArray1.length; ++i)
                byteArray1[i] = 55;
            counter = 5;
            System.out.println(" run_0420: counter = 5 ");
            stream.write(byteArray1);
            stream.close();
            counter = 6;
            System.out.println(" run_0420: counter = 6 ");
            // Re-open an OutputStream.
            stream = conn1.openOutputStream(conn1.fileSize());
            counter = 7;
            System.out.println(" run_0420: counter = 7 ");
            for (int i = 0; i < byteArray1.length; ++i)
                byteArray1[i] = 56;
            conn1.close();
            counter = 8;
            stream.write(byteArray1);
            counter = 9;
            stream.close();

            System.out.println(" Going to second part of run_0420");

            // Re-open a connection.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/new042File.txt",
                                                    Connector.READ_WRITE);
            counter = 10;
            System.out.println(" run_0420: counter = 10 ");
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 11;
            System.out.println(" run_0420: counter = 11 ");
            InputStream inStream = conn1.openInputStream();
            counter = 12;
            System.out.println(" run_0420: counter = 12 ");
            byte[] byteArray2 = new byte[110];
            int retValue = inStream.read(byteArray2);
            if (retValue != 100)
                throw new TestFailedException("retValue: " + retValue);
            counter = 13;
            System.out.println(" run_0420: counter = 13 ");
            for (int i = 0; i < retValue; ++i)
            {
                if (i < 50)
                {
                    if (byteArray2[i] != 55)
                        throw new TestFailedException("i: " + i
                                                      + " byteArray2[i]: " + byteArray2[i]);
                }
                else if (i > 49)
                {
                    if (byteArray2[i] != 56)
                        throw new TestFailedException("i: " + i
                                                      + " byteArray2[i]: " + byteArray2[i]);
                }
            }// end for
            counter = 14;
            conn1.close();
            inStream.close();

            assertTrue(" run_0420() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0420() counter: " + counter + "\n");
            assertTrue(" run_0420() FAILED!!! Counter: " + counter, false);
        }
    }//end run_0420()

}