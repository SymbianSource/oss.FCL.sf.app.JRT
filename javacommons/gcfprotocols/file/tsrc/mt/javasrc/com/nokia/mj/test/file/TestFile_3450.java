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

public class TestFile_3450 extends FileTestBase
{
    public TestFile_3450()
    {
    }

    public TestFile_3450(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_3450("run_3450", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_3450) tc).run_3450();
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

    public void run_3450()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            //'Mark point' is set beyond end-of-file in this test.
            conn1 =
                FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test330/readFile3.txt",
                                            Connector.READ_WRITE);
            counter = 1;
            if (null == conn1)
                throw new TestFailedException();
            InputStream inStream = conn1.openInputStream();
            counter = 3;
            inStream.mark(700);
            byte[] byteArray = new byte[701];
            int retValue = inStream.read(byteArray,0,700);
            counter = 4;
            if (700 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            for (int i=0; i<retValue; ++i)
            {
                if (byteArray[i] != 75)
                    throw new TestFailedException("byteArray[i]: " + byteArray[i] +
                                                  " i: " + i);
            }//end for
            counter = 5;
            inStream.reset();
            byteArray = new byte[700];
            retValue = inStream.read(byteArray,0,700);
            counter = 6;
            if (700 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            for (int i=0; i<retValue; ++i)
            {
                if (byteArray[i] != 75)
                    throw new TestFailedException("byteArray[i]: " + byteArray[i] +
                                                  " i: " + i);
            }//end for
            counter = 7;
            inStream.reset();
            inStream.mark(1001);
            byteArray = new byte[1001];
            retValue = inStream.read(byteArray,0,1001);
            counter = 8;
            if (1000 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            for (int i=0; i<retValue; ++i)
            {
                if (i < 700)
                {
                    if (byteArray[i] != 75)
                        throw new TestFailedException("byteArray[i]: " + byteArray[i] +
                                                      " i: " + i);
                }
                else
                {
                    if (byteArray[i] != 76)
                        throw new TestFailedException("byteArray[i]: " + byteArray[i] +
                                                      " i: " + i);
                }
            }//end for
            counter = 9;
            inStream.reset();
            byteArray = new byte[1001];
            retValue = inStream.read(byteArray,0,1001);
            counter = 10;
            if (1000 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            for (int i=0; i<retValue; ++i)
            {
                if (i < 700)
                {
                    if (byteArray[i] != 75)
                        throw new TestFailedException("byteArray[i]: " + byteArray[i] +
                                                      " i: " + i);
                }
                else
                {
                    if (byteArray[i] != 76)
                        throw new TestFailedException("byteArray[i]: " + byteArray[i] +
                                                      " i: " + i);
                }
            }//end for
            counter = 11;
            retValue = inStream.read(byteArray,0,1001);
            counter = 12;
            if (-1 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            conn1.close();
            inStream.close();

            assertTrue(" run_3450() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_3450() counter: " + counter + "\n");
            assertTrue(" run_3450() FAILED!!! Counter: "+counter, false);
        }
    }//end run_3450()

}
