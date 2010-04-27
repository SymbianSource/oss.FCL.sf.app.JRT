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

public class TestFile_1640 extends FileTestBase
{
    public TestFile_1640()
    {
    }

    public TestFile_1640(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1640("run_1640", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1640) tc).run_1640();
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

    public void run_1640()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test160/binaryFile2.txt",
                                                    Connector.READ_WRITE);
            counter = 1;
            boolean writeFlag = conn1.canWrite();
            if (writeFlag == true)
                throw new TestFailedException();
            counter = 2;
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
            conn1.setWritable(false);
            byte[] byteArray2 = new byte[1000];
            retValue = stream.read(byteArray2);
            counter = 6;
            if (retValue != 525)
                throw new TestFailedException("retValue: " + retValue);
            counter = 7;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray2[i] != 56)
                    throw new TestFailedException("byteArray2[i]: "
                                                  + byteArray2[i] + "i: " + i);
            }
            counter = 8;
            stream.close();
            conn1.close();

            assertTrue(" run_1640() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1640() counter: " + counter + "\n");
            assertTrue(" 1640() FAILED!!! Counter: " + counter, false);
        }
    }//end run_1640()
}
