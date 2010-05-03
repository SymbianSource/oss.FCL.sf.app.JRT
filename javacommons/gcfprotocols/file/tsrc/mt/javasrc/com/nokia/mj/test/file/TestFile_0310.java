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

public class TestFile_0310 extends FileTestBase
{
    public TestFile_0310()
    {
    }

    public TestFile_0310(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0310("run_0310", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0310) tc).run_0310();
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

    public void run_0310()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Try to open InputStream to the directory.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test020/subDir1/", Connector.READ_WRITE);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            DataInputStream tmpInStream = null;
            try
            {
                tmpInStream = conn1.openDataInputStream();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 2;
            conn1.close();

            // Try to open InputStream to non-existing file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/fgfg.txt", Connector.READ_WRITE);
            counter = 3;
            if (true == conn1.exists())
                throw new TestFailedException();
            counter = 31;
            try
            {
                tmpInStream = conn1.openDataInputStream();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            conn1.close();

            // Try to open an InputStream through a WRITE connection.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/textFile.txt", Connector.WRITE);
            counter = 4;
            try
            {
                tmpInStream = conn1.openDataInputStream();
                throw new TestFailedException();
            }
            catch (IllegalModeException ex)
            {
            }
            conn1.close();

            // Calling twice a FileConnection.openInputStream() operation in the
            // same connection case.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test020/binaryFile.dat",
                                                Connector.READ_WRITE);
            counter = 5;
            if (false == conn1.exists())
                throw new TestFailedException();
            InputStream dataStream5 = conn1.openDataInputStream();
            try
            {
                InputStream dataStream6 = conn1.openDataInputStream();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            conn1.close();
            dataStream5.close();

            // Calling FileConnection.openInputStream() operation after closing
            // a
            // connection.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test020/binaryFile.dat",
                                                Connector.READ_WRITE);
            counter = 6;
            if (false == conn1.exists())
                throw new TestFailedException();
            conn1.close();
            try
            {
                conn1.openDataInputStream();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            conn1.close();

            assertTrue(" run_0310() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0310() counter: " + counter + "\n");
            assertTrue(" run_0310() FAILED!!! Counter: " + counter, false);
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
            }
            catch (Exception ex)
            {
                append(ex.toString());
                append("TEST FAILED #2!!! Case: run_0310() \n");
                assertTrue(" run_0310() FAILED!!! Counter: " + counter, false);
            }
        }//end finally
    }//end run_0310()

}