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

public class TestFile_0200 extends FileTestBase
{
    public TestFile_0200()
    {
    }

    public TestFile_0200(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0200("run_0200", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0200) tc).run_0200();
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

    public void run_0200()
    {
        FileConnection conn1 = null;
        InputStream stream = null;
        int counter = 0;
        try
        {
            // Binary file test. #1
            // Connection is opened in the READ_WRITE mode because then it is
            // easier
            // check whether connection has been closed successfully.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test020/binaryFile.dat",
                                                Connector.READ_WRITE);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            stream = conn1.openInputStream();
            counter = 2;
            byte[] byteArray1 = new byte[600];
            int retValue = stream.read(byteArray1);
            counter = 3;
            if (retValue != 600)
                throw new TestFailedException("retValue: " + retValue);
            counter = 4;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 4)
                    throw new TestFailedException("byteArray1[i]: "
                                                  + byteArray1[i] + "i: " + i);
            }
            counter = 5;
            byte[] byteArray12 = new byte[424];
            retValue = stream.read(byteArray12);
            if (retValue != 424)
                throw new TestFailedException("retValue: " + retValue);
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray12[i] != 4)
                    throw new TestFailedException("byteArray12[i]: "
                                                  + byteArray12[i] + "i: " + i);
            }
            byte[] byteArray2 = new byte[3];
            retValue = stream.read(byteArray2);
            counter = 6;
            if (retValue != 1)
                throw new TestFailedException();
            counter = 7;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray2[i] != 4)
                    throw new TestFailedException();
            }
            counter = 8;
            retValue = stream.read(byteArray2);
            counter = 9;
            if (retValue != -1)
                throw new TestFailedException("retValue: " + retValue);
            stream.close();
            conn1.close();
            counter = 10;

            // Below is checked whether 'conn1' was closed successfully.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test020/binaryFile.dat",
                                                Connector.READ_WRITE);
            counter = 11;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 12;
            stream = conn1.openInputStream();
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
            // stream = null;

            // Re-opening a InputStream.
            counter = 17;
            stream = conn1.openInputStream();
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

            assertTrue(" run_0200() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0200() " + counter + "\n");
            assertTrue(" run_0200() FAILED!!! Counter: "+counter, false);
            return;
        }
        finally
        {
            try
            {
                if (conn1 != null)
                    if (true == conn1.isOpen())
                        conn1.close();
                if (stream != null)
                    stream.close();
            }
            catch (Exception ex)
            {
                append(ex.toString());
                append("TEST FAILED!!! Case: run_0200() " + counter + "\n");
                assertTrue(" run_0200() FAILED!!! Counter: "+counter, false);
            }
        }
    }//end run_0200()

}