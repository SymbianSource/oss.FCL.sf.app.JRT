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

public class TestFile_2430 extends FileTestBase
{
    public TestFile_2430()
    {
    }

    public TestFile_2430(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2430("run_2430", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2430) tc).run_2430();
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

    public void run_2430()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // Deleting a read-only directory.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test240/readonlydir/", Connector.READ_WRITE);
            counter = 1;
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 2;
            conn1.setWritable(true);
            counter = 3;
            conn1.delete();
            counter = 4;
            conn1.close();

            // Deleting a read-only file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test240/readonlyfile", Connector.READ_WRITE);
            counter = 5;
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 6;
            conn1.setWritable(true);
            counter = 7;
            conn1.delete();
            counter = 8;
            conn1.close();

            // Deleting a system file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test240/systemFile", Connector.READ_WRITE);
            counter = 9;
            conn1.delete();
            counter = 10;
            conn1.close();
            // Trying re-open a deleted system file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test240/systemFile", Connector.READ_WRITE);
            counter = 11;
            if (conn1.exists())
                throw new TestFailedException();
            counter = 12;
            conn1.close();

            // Deleting a system dir.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test240/systemDir", Connector.READ_WRITE);
            counter = 13;
            conn1.delete();
            counter = 14;
            conn1.close();
            // Trying re-open a deleted system dir.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test240/systemDir", Connector.READ_WRITE);
            counter = 15;
            if (conn1.exists())
                throw new TestFailedException();
            counter = 16;
            conn1.close();

            // 'Two READ_WRITE connections to the same read-only file' case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test240/test243File", Connector.READ_WRITE);
            counter = 17;
            conn1.create();
            counter = 18;
            conn1.setWritable(false);
            counter = 19;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test240/test243File", Connector.READ_WRITE);
            counter = 20;
            if (conn2.canWrite())
                throw new TestFailedException();
            counter = 200;
            try
            {
                conn2.delete();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 21;
            conn1.setWritable(true);
            counter = 22;
            conn2.delete();
            counter = 23;
            conn1.close();
            conn2.close();

            // 'Two READ_WRITE connections to the same directory' case.
            // Note: Same test for a file has been tested in the testcase 027.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test240/test243Dir/", Connector.READ_WRITE);
            counter = 24;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test240/test243Dir/", Connector.READ_WRITE);
            counter = 25;
            try
            {
                conn2.delete();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 26;
            conn1.mkdir();
            counter = 27;
            conn2.delete();
            counter = 28;
            conn1.close();
            conn2.close();

            assertTrue(" run_2430() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2430() counter: " + counter + "\n");
            assertTrue(" run_2430() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2430()

}
