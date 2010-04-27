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

public class TestFile_3460 extends FileTestBase
{
    public TestFile_3460()
    {
    }

    public TestFile_3460(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_3460("run_3460", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_3460) tc).run_3460();
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

    public void run_3460()
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
            inStream.mark(7);
            byte[] byteArray = new byte[10];
            int retValue = inStream.read(byteArray, 0, 4);
            counter = 4;
            if (4 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray[i] != 75)
                    throw new TestFailedException("byteArray[i]: "
                                                  + byteArray[i] + " i: " + i);
            }// end for
            counter = 5;
            long skipped = inStream.skip(3);
            if (3 != skipped)
                throw new TestFailedException("skipped: " + skipped);
            inStream.reset();
            counter = 6;
            inStream.mark(20);
            byteArray = new byte[21];
            retValue = inStream.read(byteArray, 0, 21);
            counter = 7;
            if (20 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            for (int i = 0; i < retValue; ++i)
            {
                if (i < 10)
                {
                    if (byteArray[i] != 75)
                        throw new TestFailedException("byteArray[i]: "
                                                      + byteArray[i] + " i: " + i);
                }
                else
                {
                    if (byteArray[i] != 76)
                        throw new TestFailedException("byteArray[i]: "
                                                      + byteArray[i] + " i: " + i);
                }
            }// end for
            counter = 8;
            inStream.reset();
            byteArray = new byte[21];
            retValue = inStream.read(byteArray, 0, 21);
            counter = 9;
            if (20 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            for (int i = 0; i < retValue; ++i)
            {
                if (i < 10)
                {
                    if (byteArray[i] != 75)
                        throw new TestFailedException("byteArray[i]: "
                                                      + byteArray[i] + " i: " + i);
                }
                else
                {
                    if (byteArray[i] != 76)
                        throw new TestFailedException("byteArray[i]: "
                                                      + byteArray[i] + " i: " + i);
                }
            }// end for
            counter = 10;
            retValue = inStream.read(byteArray, 0, 21);
            counter = 11;
            if (-1 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            inStream.close();
            conn1.close();

            assertTrue(" run_3460() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_3460() counter: " + counter + "\n");
            assertTrue(" run_3460() FAILED!!! Counter: " + counter, false);
        }
    }//end run_3460()

}
