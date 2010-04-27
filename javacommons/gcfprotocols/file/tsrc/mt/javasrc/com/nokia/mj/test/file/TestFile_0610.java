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

public class TestFile_0610 extends FileTestBase
{
    public TestFile_0610()
    {
    }

    public TestFile_0610(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }


    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0610("run_0610", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0610) tc).run_0610();
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

    public void run_0610()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            //Try to open OutputStream to the directory.
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test020/subDir1/",
                                                Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 1;
            OutputStream tmpOutStream = null;
            try
            {
                tmpOutStream = conn1.openOutputStream(1);
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 2;
            conn1.close();
            counter = 3;

            //Try to open OutputStream to non-existing file.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test020/fgfg.txt",
                                   Connector.READ_WRITE);
            if (true == conn1.exists())
                throw new TestFailedException();
            try
            {
                tmpOutStream = conn1.openOutputStream(2);
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 4;
            conn1.close();
            counter = 5;

            //Try to open an OutputStream using a READ access mode.
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test040/binaryFile.dat",
                                                Connector.READ);
            if (null == conn1)
                throw new TestFailedException();
            try
            {
                tmpOutStream = conn1.openOutputStream(3);
                throw new TestFailedException();
            }
            catch (IllegalModeException ex) {}
            counter = 6;
            conn1.close();

            //Calling twice a FileConnection.openOutputStream(long) operation in the
            //same connection case.
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test060/binaryFile.dat",
                                                Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            OutputStream dataStream5 = conn1.openOutputStream(3);
            try
            {
                OutputStream dataStream6 = conn1.openOutputStream(3);
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 7;
            conn1.close();
            dataStream5.close();

            //IllegalArgumentException case.
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test060/binaryFile.dat",
                                                Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            try
            {
                conn1.openOutputStream(-3);
                throw new TestFailedException();
            }
            catch (IllegalArgumentException ex) {}
            counter = 8;
            conn1.close();

            //Opening a WRITE connection to the read-only file.
            //openOutputStream() operation should throw an IOException in this case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test020/binaryFile2.dat",
                                   Connector.WRITE);
            counter = 10;
            try
            {
                conn1.openOutputStream(10);
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 11;
            conn1.close();

            //Same test as above except connection mode is READ_WRITE in this test.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test020/binaryFile2.dat",
                                   Connector.READ_WRITE);
            counter = 12;
            try
            {
                conn1.openOutputStream(10);
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 13;
            conn1.close();

            assertTrue(" run_0610() ok ", true);
            System.out.println(" run_0610() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0610() counter: " + counter + "\n");
            assertTrue(" run_0610() FAILED!!! Counter: "+counter, false);
        }
    }//end run_0610()

}