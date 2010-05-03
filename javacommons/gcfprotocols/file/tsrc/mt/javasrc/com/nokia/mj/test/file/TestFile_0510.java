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

public class TestFile_0510 extends FileTestBase
{
    public TestFile_0510()
    {
    }

    public TestFile_0510(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0510("run_0510", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0510) tc).run_0510();
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

    public void run_0510()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Try to open OutputStream to the directory.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test020/subDir1/", Connector.READ_WRITE);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            OutputStream tmpOutStream = null;
            try
            {
                tmpOutStream = conn1.openDataOutputStream();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 2;
            conn1.close();

            // Try to open OutputStream to non-existing file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/fgfg.txt", Connector.READ_WRITE);
            if (true == conn1.exists())
                throw new TestFailedException();
            counter = 4;
            try
            {
                tmpOutStream = conn1.openDataOutputStream();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 5;
            conn1.close();

            // Try to open an OutputStream using a READ access mode.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/binaryFile.dat", Connector.READ);
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 6;
            try
            {
                tmpOutStream = conn1.openDataOutputStream();
                throw new TestFailedException();
            }
            catch (IllegalModeException ex)
            {
            }
            counter = 7;
            conn1.close();

            // Calling twice a FileConnection.openOutputStream() operation in
            // the
            // same connection case.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test040/binaryFile.dat",
                                                Connector.READ_WRITE);
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 8;
            OutputStream dataStream5 = conn1.openDataOutputStream();
            try
            {
                OutputStream dataStream6 = conn1.openDataOutputStream();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            System.out.println(" run_0510: end counter 8 ");
            counter = 9;
            conn1.close();
            dataStream5.close();

            // Opening a WRITE connection to the read-only file.
            // openOutputStream() operation should throw an IOException in this
            // case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/binaryFile2.dat", Connector.WRITE);
            counter = 10;
            try
            {
                conn1.openDataOutputStream();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 11;
            conn1.close();

            // Same test as above except connection mode is READ_WRITE in this
            // test.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/binaryFile2.dat",
                                                    Connector.READ_WRITE);
            counter = 12;
            try
            {
                conn1.openDataOutputStream();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 13;
            conn1.close();

            assertTrue(" run_0510() ok ", true);
            System.out.println(" run_0510() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0510() counter: " + counter + "\n");
            assertTrue(" run_0510() FAILED!!! Counter: " + counter, false);
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
                append("TEST FAILED #2!!! Case: run_0510() \n");
                assertTrue(" run_0510() FAILED!!! Counter: " + counter, false);
            }
        }//end finally
    }//end run_0510()

}