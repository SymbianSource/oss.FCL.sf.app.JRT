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

public class TestFile_3410 extends FileTestBase
{
    public TestFile_3410()
    {
    }

    public TestFile_3410(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_3410("run_3410", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_3410) tc).run_3410();
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

    public void run_3410()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Calling mark() operation twice test.
            // Argument of the mark() operation is smaller in the second mark()
            // operation call.
            // Only two bytes are read before re-calling mark() operation.
            // That's a difference to run_3990() test.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test330/readFile.txt", Connector.READ_WRITE);
            counter = 1;
            if (null == conn1)
                throw new TestFailedException();
            InputStream inStream = conn1.openInputStream();
            counter = 3;
            inStream.mark(10);
            counter = 5;
            byte[] byteArray = new byte[15];
            int retValue = inStream.read(byteArray, 0, 1);
            counter = 6;
            if (1 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray[i] != 75)
                    throw new TestFailedException("byteArray[i]: "
                                                  + byteArray[i] + " i: " + i);
            }// end for
            counter = 7;
            inStream.mark(5);
            counter = 8;
            byteArray = new byte[15];
            retValue = inStream.read(byteArray, 0, 5);
            counter = 9;
            if (5 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray[i] != 75)
                    throw new TestFailedException("byteArray[i]: "
                                                  + byteArray[i] + " i: " + i);
            }// end for
            counter = 10;
            inStream.reset();
            byteArray = new byte[15];
            retValue = inStream.read(byteArray, 0, 5);
            counter = 11;
            if (5 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray[i] != 75)
                    throw new TestFailedException("byteArray[i]: "
                                                  + byteArray[i] + " i: " + i);
            }// end for
            counter = 12;
            byteArray = new byte[20];
            retValue = inStream.read(byteArray, 0, 20);
            counter = 13;
            if (14 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            for (int i = 0; i < retValue; ++i)
            {
                if (i < 4)
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
            counter = 14;
            retValue = inStream.read(byteArray, 0, 20);
            counter = 15;
            if (-1 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            inStream.close();
            conn1.close();

            assertTrue(" run_3410() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_3410() counter: " + counter + "\n");
            assertTrue(" run_3410() FAILED!!! Counter: " + counter, false);
        }
    }//end run_3410()

}
