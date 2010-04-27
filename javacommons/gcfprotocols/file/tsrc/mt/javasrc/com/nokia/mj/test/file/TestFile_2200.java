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

public class TestFile_2200 extends FileTestBase
{
    public TestFile_2200()
    {
    }

    public TestFile_2200(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2200("run_2200", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2200) tc).run_2200();
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

    public void run_2200()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // File case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test220/newFile", Connector.READ_WRITE);
            if (conn1.exists())
                throw new TestFailedException();
            counter = 1;
            conn1.create();
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 2;
            conn1.close();

            // Directory case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test220/newDir/", Connector.READ_WRITE);
            counter = 3;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.mkdir();
            counter = 4;
            if (!conn1.exists())
                throw new TestFailedException();
            conn1.close();

            // Make a READ_WRITE connection to the read-only file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test220/ReadOnlyFile220.txt",
                                                    Connector.READ_WRITE);
            counter = 5;
            if (!conn1.exists())
                throw new TestFailedException();
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test220/ReadOnlyFile220.txt",
                                                    Connector.READ_WRITE);
            counter = 6;
            try
            {
                conn1.openOutputStream();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 61;
            conn2.setWritable(true);
            counter = 7;
            if (!conn2.exists())
                throw new TestFailedException();
            counter = 8;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 81;
            OutputStream outStream = conn1.openOutputStream();
            outStream.close();
            counter = 82;
            conn2.delete();
            counter = 9;
            if (conn1.exists())
                throw new TestFailedException();
            counter = 10;
            if (conn2.exists())
                throw new TestFailedException();
            counter = 11;
            conn1.close();
            conn2.close();

            // One connection has open InputStream when other connection call
            // exists() operation.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/binaryFile.dat",
                                                    Connector.READ_WRITE);
            counter = 12;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/binaryFile.dat",
                                                    Connector.READ_WRITE);
            counter = 13;
            InputStream stream = conn1.openInputStream();
            counter = 14;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 15;
            if (!conn2.exists())
                throw new TestFailedException();
            counter = 16;
            conn1.close();
            conn2.close();
            stream.close();

            // Same test as above except for a new file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test220/newFile2.dat", Connector.READ_WRITE);
            counter = 17;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test220/newFile2.dat", Connector.READ_WRITE);
            counter = 18;
            conn1.create();
            stream = conn1.openInputStream();
            counter = 19;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 20;
            if (!conn2.exists())
                throw new TestFailedException();
            counter = 21;
            conn1.close();
            conn2.close();
            stream.close();

            assertTrue(" run_2200() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2200() counter: " + counter + "\n");
            assertTrue(" run_2200() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2200()
}
