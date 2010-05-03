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

public class TestFile_0620 extends FileTestBase
{
    public TestFile_0620()
    {
    }

    public TestFile_0620(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0620("run_0620", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0620) tc).run_0620();
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

    public void run_0620()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // This tests re-opening a OutputStream.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test060/binaryFile5.dat",
                                                Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 1;
            OutputStream outStream = conn1.openOutputStream(600);
            counter = 2;
            int addedNum = 51;
            for (int i = 0; i < 100; ++i)
            {
                outStream.write(addedNum);
            }
            outStream.flush();
            outStream.close();
            counter = 3;

            // Re-opening a OutputStream.
            outStream = conn1.openOutputStream(700);
            addedNum = 52;
            for (int i = 0; i < 100; ++i)
            {
                outStream.write(addedNum);
            }
            outStream.close();
            conn1.close();
            counter = 4;

            // Read just added data.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test060/binaryFile5.dat", Connector.READ);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 5;
            byte[] byteArray3 = new byte[1001];
            InputStream inStream = conn1.openDataInputStream();
            int retValue = inStream.read(byteArray3);
            counter = 6;
            if (retValue != 1000)
                throw new TestFailedException("retValue: " + retValue);
            counter = 7;
            for (int i = 0; i < retValue; ++i)
            {
                if (i < 600)
                {
                    if (byteArray3[i] != 50)
                        throw new TestFailedException("i: " + i
                                                      + " byteArray3[i]: " + +byteArray3[i]);
                }
                else if ((i > 599) && (i < 700))
                {
                    if (byteArray3[i] != 51)
                        throw new TestFailedException("i: " + i
                                                      + " byteArray3[i]: " + +byteArray3[i]);

                }
                else if ((i > 699) && (i < 800))
                {
                    if (byteArray3[i] != 52)
                        throw new TestFailedException("i: " + i
                                                      + " byteArray3[i]: " + +byteArray3[i]);

                }
                else
                {
                    if (byteArray3[i] != 50)
                        throw new TestFailedException("i: " + i
                                                      + " byteArray3[i]: " + +byteArray3[i]);
                }
            }// end for
            inStream.close();
            conn1.close();

            assertTrue(" run_0620() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0620() counter: " + counter + "\n");
            assertTrue(" run_0620() FAILED!!! Counter: " + counter, false);
        }
    }//end run_0620()

}