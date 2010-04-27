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

public class TestFile_1720 extends FileTestBase
{
    public TestFile_1720()
    {
    }

    public TestFile_1720(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1720("run_1720", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1720) tc).run_1720();
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

    public void run_1720()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // 1 Opening a file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test170/readOnly.txt", Connector.READ_WRITE);
            counter = 1;
            InputStream stream = conn1.openInputStream();
            counter = 3;
            byte[] byteArray1 = new byte[500];
            int retValue = stream.read(byteArray1);
            counter = 4;
            if (retValue != 500)
                throw new TestFailedException("retValue: " + retValue);
            counter = 5;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException("byteArray1[i]: "
                                                  + byteArray1[i] + "i: " + i);
            }
            conn1.setHidden(true);
            conn1.setHidden(true);
            counter = 6;
            boolean hiddenFlag = conn1.isHidden();
            if (hiddenFlag == false)
                throw new TestFailedException();
            counter = 7;
            try
            {
                stream.read(byteArray1);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }

            // Re-opening a InputStream.
            stream = conn1.openInputStream();
            counter = 71;
            byte[] byteArray12 = new byte[1601];
            retValue = stream.read(byteArray12);
            counter = 72;
            if (retValue != 1600)
                throw new TestFailedException("retValue: " + retValue);
            counter = 73;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray12[i] != 56)
                    throw new TestFailedException("byteArray12[i]: "
                                                  + byteArray12[i] + "i: " + i);
            }
            stream.close();
            conn1.close();

            // 2 Re-open a file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test170/readOnly.txt", Connector.READ_WRITE);
            counter = 8;
            stream = conn1.openInputStream();
            counter = 9;
            byteArray1 = new byte[500];
            retValue = stream.read(byteArray1);
            counter = 10;
            if (retValue != 500)
                throw new TestFailedException("retValue: " + retValue);
            counter = 11;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException("byteArray1[i]: "
                                                  + byteArray1[i] + "i: " + i);
            }
            counter = 12;
            conn1.setHidden(true);
            byte[] byteArray2 = new byte[1200];
            retValue = stream.read(byteArray2);
            counter = 13;
            if (retValue != 1100)
                throw new TestFailedException("retValue: " + retValue);
            counter = 14;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray2[i] != 56)
                    throw new TestFailedException("byteArray2[i]: "
                                                  + byteArray2[i] + "i: " + i);
            }
            stream.close();
            conn1.close();

            // 3 Re-open a file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test170/readOnly.txt", Connector.READ_WRITE);
            stream = conn1.openInputStream();
            counter = 15;
            byteArray1 = new byte[500];
            retValue = stream.read(byteArray1);
            counter = 16;
            if (retValue != 500)
                throw new TestFailedException("retValue: " + retValue);
            counter = 17;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException("byteArray1[i]: "
                                                  + byteArray1[i] + "i: " + i);
            }
            counter = 18;
            System.out.println(" Test 1720: COunter 8");
            conn1.setHidden(false);
            hiddenFlag = conn1.isHidden();
            if (hiddenFlag == true)
                throw new TestFailedException();
            counter = 19;
            try
            {
                stream.read(byteArray1);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            stream.close();
            conn1.close();

            assertTrue(" run_1720() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1720() counter: " + counter + "\n");
            assertTrue(" run_1720() FAILED!!! Counter: " + counter, false);
        }
    }

}
