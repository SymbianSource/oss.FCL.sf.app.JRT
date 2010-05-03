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

public class TestFile_2330_2400 extends FileTestBase
{
    public TestFile_2330_2400()
    {
    }

    public TestFile_2330_2400(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2330_2400("run_2330", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2330_2400) tc).run_2330();
            }
        }));

        suite.addTest(new TestFile_2330_2400("run_2400", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2330_2400) tc).run_2400();
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

    public void run_2330()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // 'Two connection points to the same directory' case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test233/", Connector.READ_WRITE);
            counter = 1;
            if (conn1.exists())
                throw new TestFailedException();
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test233/", Connector.READ_WRITE);
            counter = 2;
            if (true == conn1.isDirectory())
                throw new TestFailedException();
            counter = 3;
            conn1.mkdir();
            counter = 4;
            if (false == conn1.isDirectory())
                throw new TestFailedException();
            counter = 5;
            if (false == conn2.isDirectory())
                throw new TestFailedException();
            counter = 6;
            conn1.close();
            conn2.close();

            assertTrue(" run_2330() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2330() counter: " + counter + "\n");
            assertTrue(" run_2330() FAILED!!! Counter: "+counter, false);
        }
    }// end run_2330()

    //
    //
    //
    //
    public void run_2400()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // "Deleting" test for a directory.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test220/newDir240/", Connector.READ_WRITE);
            if (conn1.exists())
                throw new TestFailedException();
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
            conn1.mkdir();
            counter = 3;
            if (false == conn1.isDirectory())
                throw new TestFailedException();
            conn1.delete();
            counter = 4;
            if (true == conn1.isDirectory())
                throw new TestFailedException();
            counter = 41;
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 5;
            conn1.close();

            // Re-open a directory to check that it is really deleted.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test220/newDir240/", Connector.READ_WRITE);
            counter = 6;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.close();

            // "Deleting" test for a file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test220/newFile240", Connector.READ_WRITE);
            counter = 7;
            if (conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 8;
            conn1.create();
            counter = 9;
            if (false == conn1.exists())
                throw new TestFailedException();
            conn1.delete();
            counter = 10;
            if (true == conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 11;
            conn1.close();

            // Re-open a file to check that it is really deleted.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test220/newFile240", Connector.READ_WRITE);
            counter = 12;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.close();

            assertTrue(" run_2400() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2400() counter: " + counter + "\n");
            assertTrue(" run_2400() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2400()

}
