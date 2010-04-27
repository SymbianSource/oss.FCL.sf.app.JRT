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

public class TestFile_2720 extends FileTestBase
{
    public TestFile_2720()
    {
    }

    public TestFile_2720(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2720("run_2720", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2720) tc).run_2720();
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

    public void run_2720()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // "Null pointer argument" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/", Connector.READ_WRITE);
            counter = 1;
            if (!conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.setFileConnection(null);
                throw new TestFailedException();
            }
            catch (NullPointerException ex)
            {
            }
            counter = 2;
            conn1.close();
            counter = 3;

            // "Argument contains path information" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/", Connector.READ_WRITE);
            counter = 4;
            if (!conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.setFileConnection("test100/subDir/");
                throw new TestFailedException();
            }
            catch (IllegalArgumentException ex)
            {
            }
            counter = 5;
            conn1.close();
            counter = 6;

            // "Connection has been invoked to the file" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/file100.txt", Connector.READ_WRITE);
            counter = 7;
            if (!conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.setFileConnection("subDir/");
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 8;
            conn1.close();
            counter = 9;

            // "A new target file does not exists" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/", Connector.READ_WRITE);
            counter = 10;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 100;
            try
            {
                conn1.setFileConnection("sdfgsyt");
                throw new TestFailedException();
            }
            catch (IllegalArgumentException ex)
            {
            }
            counter = 11;
            conn1.close();
            counter = 12;

            // "A new target directory does not exists" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/", Connector.READ_WRITE);
            counter = 140;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 141;
            try
            {
                conn1.setFileConnection("sdfgsyt/");
                throw new TestFailedException();
            }
            catch (IllegalArgumentException ex)
            {
            }
            counter = 142;
            conn1.close();
            counter = 143;

            // "Argument contains forbidden characters" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/", Connector.READ_WRITE);
            counter = 13;
            if (!conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.setFileConnection("?file100.txt");
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 14;
            conn1.close();
            counter = 15;

            // "Connection closed" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/", Connector.READ_WRITE);
            counter = 16;
            if (!conn1.exists())
                throw new TestFailedException();
            conn1.close();
            try
            {
                conn1.setFileConnection("file100.txt");
                throw new TestFailedException();
            }
            catch (ConnectionClosedException ex)
            {
            }
            counter = 17;
            conn1.close();

            // "Try to establish connection to parent directory in the root
            // level" case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot + "",
                                                    Connector.READ_WRITE);
            counter = 18;
            if (!conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.setFileConnection("..");
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 19;
            conn1.close();

            // READ_WRITE access test for read-only file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/binaryFile2.dat",
                                                    Connector.READ_WRITE);
            counter = 20;
            try
            {
                conn1.setFileConnection("binaryFile.dat");
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 21;
            conn1.close();

// CSB: Why is this restriction? Not needed.
//            // Manufacturer domain midlet doesn't have access to the
//            // "+iRoot+"private
//            // directory.
//            conn1 = (FileConnection) Connector.open("file:///" + iRoot + "",
//                    Connector.READ_WRITE);
//            counter = 21;
//            try
//            {
//                conn1.setFileConnection("private/");
//                conn1.close();
//                throw new TestFailedException();
//            }
//            catch (SecurityException ex)
//            {
//            }
//            conn1.close();
            counter = 22;

            assertTrue(" run_2720() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2720() counter: " + counter + "\n");
            assertTrue(" run_2720() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2720()

}
