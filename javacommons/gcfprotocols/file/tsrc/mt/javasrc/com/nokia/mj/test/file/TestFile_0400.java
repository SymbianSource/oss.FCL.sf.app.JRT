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

public class TestFile_0400 extends FileTestBase
{
    public TestFile_0400()
    {
    }

    public TestFile_0400(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0400("run_0400", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0400) tc).run_0400();
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

    public void run_0400()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Binary file test.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test040/binaryFile.dat",
                                                Connector.READ_WRITE);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            OutputStream stream = conn1.openOutputStream();
            counter = 2;
            byte[] byteArray1 = new byte[50];
            for (int i = 0; i < byteArray1.length; ++i)
                byteArray1[i] = 55;
            counter = 3;
            stream.write(byteArray1);
            stream.close();
            long size = conn1.fileSize();
            if (100 != size)
                throw new TestFailedException("size: " + size);

            stream = conn1.openOutputStream();
            byte[] byteArray12 = new byte[10];
            for (int i = 0; i < byteArray12.length; ++i)
                byteArray12[i] = 54;
            stream.write(byteArray12);
            stream.close();
            // flush() operation shouldn't throw exception.
            stream.flush();
            conn1.close();
            stream.flush();

            // Take a new connection to the 'binaryFile.dat' file.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test040/binaryFile.dat",
                                                Connector.READ_WRITE);
            counter = 4;
            if (conn1 == null)
                throw new TestFailedException();
            byte[] byteArray2 = new byte[500];
            InputStream inStream = conn1.openInputStream();
            counter = 5;
            int retValue = inStream.read(byteArray2);
            if (retValue != 100)
                throw new TestFailedException("retValue: " + retValue);
            counter = 6;
            for (int i = 0; i < retValue; ++i)
            {
                if (i < 10)
                {
                    if (byteArray2[i] != 54)
                        throw new TestFailedException("i: " + i
                                                      + " byteArray2[i]: " + byteArray2[i]);
                }
                else if ((i < 50) && (i > 9))
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
            }
            counter = 7;
            conn1.close();
            inStream.close();

            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/new042File100.txt",
                                                    Connector.READ_WRITE);
            conn1.create();
            counter = 8;
            // Writing big buffer to the file(100 kB).
            byteArray1 = new byte[100000];
            for (int i = 0; i < byteArray1.length; ++i)
                byteArray1[i] = 58;
            stream = conn1.openOutputStream();
            stream.write(byteArray1);
            stream.flush();
            size = conn1.fileSize();
            if (100000 != size)
                throw new TestFailedException("size: " + size);
            stream.close();
            conn1.close();
            counter = 9;

            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test040/new042File100.txt",
                                                Connector.READ_WRITE);
            counter = 10;
            if (conn1 == null)
                throw new TestFailedException();
            byteArray2 = new byte[100001];
            inStream = conn1.openInputStream();
            counter = 11;
            retValue = inStream.read(byteArray2);
            if (retValue != 100000)
                throw new TestFailedException("retValue: " + retValue);
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray2[i] != 58)
                    throw new TestFailedException("i: " + i
                                                  + " byteArray2[i]: " + byteArray2[i]);
            }
            counter = 12;
            size = conn1.fileSize();
            if (100000 != size)
                throw new TestFailedException("size: " + size);
            inStream.close();
            conn1.close();

            assertTrue(" run_0400() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0400() counter: " + counter + "\n");
            assertTrue(" run_0400() FAILED!!! Counter: " + counter, false);
        }
        finally
        {
            try
            {
                if (conn1 != null)
                {
                    if (true == conn1.isOpen())
                        conn1.close();
                }
            }
            catch (Exception ex)
            {
                append(ex.toString());
                append("TEST FAILED #2!!! Case: run_0400() \n");
                assertTrue(" run_0400() FAILED!!! COunter: " + counter, false);
            }
        }//end finally
    }//end run_0400()

}