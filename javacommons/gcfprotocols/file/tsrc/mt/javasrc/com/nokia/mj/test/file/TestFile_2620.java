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

public class TestFile_2620 extends FileTestBase
{
    public TestFile_2620()
    {
    }

    public TestFile_2620(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2620("run_2620", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2620) tc).run_2620();
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

    public void run_2620()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test260/file262.txt", Connector.READ_WRITE);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            long fileSize = conn1.fileSize();
            if (2000 != fileSize)
                throw new TestFailedException("fileSize: " + fileSize);
            counter = 2;
            InputStream stream = conn1.openInputStream();
            byte[] byteArray1 = new byte[512];
            int retValue = stream.read(byteArray1);
            counter = 3;
            if (retValue != 512)
                throw new TestFailedException("retValue: " + retValue);
            counter = 4;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException("i: " + i
                                                  + " byteArray1[i]: " + byteArray1[i]);
            }
            counter = 5;
            conn1.truncate(600);
            fileSize = conn1.fileSize();
            if (600 != fileSize)
                throw new TestFailedException("fileSize: " + fileSize);
            counter = 6;
            retValue = stream.read(byteArray1);
            counter = 7;

            // Correct return value is 88. Before there used to be a buffer of 512
            // now we maintain no data
            if (retValue != 88)
                throw new TestFailedException("retValue: " + retValue);
            counter = 8;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException("i: " + i
                                                  + " byteArray1[i]: " + byteArray1[i]);
            }
            counter = 9;
            retValue = stream.read(byteArray1);
            counter = 3;
            if (retValue != -1)
                throw new TestFailedException("retValue: " + retValue);
            stream.close();
            conn1.close();

            assertTrue(" run_2620() ok ", true);
            System.out.println(" run_2620() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2620() counter: " + counter + "\n");
            assertTrue(" run_2620() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2620()

}
