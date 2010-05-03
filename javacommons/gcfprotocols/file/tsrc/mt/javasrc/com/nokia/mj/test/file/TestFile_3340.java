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

public class TestFile_3340 extends FileTestBase
{
    public TestFile_3340()
    {
    }

    public TestFile_3340(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }


    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_3340("run_3340", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_3340) tc).run_3340();
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

    public void run_3340()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test330/readFile.txt", Connector.READ_WRITE);
            counter = 1;
            if (null == conn1)
                throw new TestFailedException();
            InputStream inStream = conn1.openInputStream();
            counter = 3;
            byte[] byteArray = new byte[15];
            inStream.mark(10);
            long skippedBytes = inStream.skip(10);
            if (skippedBytes != 10)
                throw new TestFailedException("skippedBytes: " + skippedBytes);
            int retValue = inStream.read(byteArray, 0, 11);
            counter = 5;
            if (10 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray[i] != 76)
                    throw new TestFailedException("byteArray[i]: "
                                                  + byteArray[i] + " i: " + i);

            }// end for
            counter = 6;
            retValue = inStream.read(byteArray, 0, 11);
            counter = 7;
            if (-1 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            inStream.close();
            conn1.close();

            // Re-opening a connection.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test330/readFile.txt", Connector.READ_WRITE);
            counter = 8;
            if (null == conn1)
                throw new TestFailedException();
            inStream = conn1.openInputStream();
            counter = 9;
            inStream.mark(10);
            skippedBytes = inStream.skip(20);
            if (skippedBytes != 20)
                throw new TestFailedException("skippedBytes: " + skippedBytes);
            counter = 10;
            retValue = inStream.read(byteArray, 0, 11);
            counter = 11;
            if (-1 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            inStream.close();
            conn1.close();

            assertTrue(" run_3340() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_3340() counter: " + counter + "\n");
            assertTrue(" run_3340() FAILED!!! Counter: " + counter, false);
        }
    }//end run_3340()

}
