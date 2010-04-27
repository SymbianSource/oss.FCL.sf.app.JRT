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

public class TestFile_0300 extends FileTestBase
{
    public TestFile_0300()
    {
    }

    public TestFile_0300(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0300("run_0300", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0300) tc).run_0300();
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

    public void run_0300()
    {
        FileConnection conn1 = null;
        DataInputStream stream = null;
        int counter = 0;
        try
        {
            // Binary file test.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test020/binaryFile.dat",
                                                Connector.READ_WRITE);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            stream = conn1.openDataInputStream();
            counter = 2;
            byte[] byteArray1 = new byte[600];
            int retValue = stream.read(byteArray1);
            if (retValue != 600)
                throw new TestFailedException();
            counter = 3;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 4)
                    throw new TestFailedException();
            }
            counter = 4;
            byte[] byteArray2 = new byte[424];
            retValue = stream.read(byteArray2);
            counter = 5;
            if (retValue != 424)
                throw new TestFailedException();
            counter = 6;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray2[i] != 4)
                    throw new TestFailedException();
            }
            counter = 7;

            byteArray2 = new byte[700];
            retValue = stream.read(byteArray2);
            counter = 8;
            if (retValue != 1)
                throw new TestFailedException();
            counter = 9;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray2[i] != 4)
                    throw new TestFailedException();
            }

            byteArray2 = new byte[700];
            retValue = stream.read(byteArray2);
            counter = 10;
            if (retValue != -1)
                throw new TestFailedException();
            stream.close();
            conn1.close();

            // Re-opening a file to check that closing was done successfully.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test020/binaryFile.dat",
                                                Connector.READ_WRITE);
            counter = 11;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 12;
            stream = conn1.openDataInputStream();
            byte[] byteArray3 = new byte[1026];
            counter = 13;
            retValue = stream.read(byteArray3);
            counter = 14;
            if (retValue != 1025)
                throw new TestFailedException("retValue: " + retValue);
            retValue = stream.read(byteArray3);
            counter = 15;
            if (retValue != -1)
                throw new TestFailedException();
            counter = 16;
            // Closing a InputStream.
            stream.close();
            stream = null;

            // Re-opening a InputStream.
            counter = 17;
            stream = conn1.openDataInputStream();
            byteArray3 = new byte[1026];
            counter = 18;
            retValue = stream.read(byteArray3);
            counter = 19;
            if (retValue != 1025)
                throw new TestFailedException("retValue: " + retValue);
            retValue = stream.read(byteArray3);
            counter = 20;
            if (retValue != -1)
                throw new TestFailedException("retValue: " + retValue);
            // Closing a InputStream.
            stream.close();
            stream = null;
            conn1.close();

            assertTrue(" run_0300() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0300() counter: " + counter + "\n");
            assertTrue(" run_0300() FAILED!!! Counter: " + counter, false);
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
                if (stream != null)
                    stream.close();
            }
            catch (Exception ex)
            {
                append(ex.toString());
                append("TEST FAILED #2!!! Case: run_0300() \n");
                assertTrue(" run_0300() FAILED!!! Counter: " + counter, false);
            }
        }// end finally
    }// end run_0300()

}