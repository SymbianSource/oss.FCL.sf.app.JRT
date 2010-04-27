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

public class TestFile_1920_1930 extends FileTestBase
{
    public TestFile_1920_1930()
    {
    }

    public TestFile_1920_1930(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1920_1930("run_1920", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1920_1930) tc).run_1920();
            }
        }));

        suite.addTest(new TestFile_1920_1930("run_1930", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1920_1930) tc).run_1930();
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

    private void testFilterParam(FileConnection aConn, String aFilter)
    throws IOException
    {
        Enumeration fileList = aConn.list(aFilter, true);
        if (false == fileList.hasMoreElements())
            throw new TestFailedException();
        int counter = 0;
        boolean failedFlag = false;
        while (fileList.hasMoreElements())
        {
            String obj = (String) fileList.nextElement();
            if (obj.equals("temp.txt"))
                ++counter;
            else if (obj.equals("tempo.txt"))
                ++counter;
            else if (obj.equals("tmp.txt"))
                ++counter;
            else
                failedFlag = true;
        }// end while
        if (counter != 3)
            throw new TestFailedException();
        if (failedFlag == true)
            throw new TestFailedException();
    }//end testFilterParam()

    public void run_1920()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test192/", Connector.READ);
            if (conn1 == null)
                throw new TestFailedException();
            testFilterParam(conn1, "*.txt");
            counter++;
            // Below is '*.txt' string in the enescape format.
            testFilterParam(conn1, "%2A%2E%74%78%74");
            counter++;
            conn1.close();
            assertTrue(" run_1920() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1920() counter: " + counter + "\n");
            assertTrue(" run_1920() FAILED!!! Counter: " + counter, false);
        }
    }// end run_1920()

    //
    //
    //
    //
    private void incorrectFilter(FileConnection aConn, String aFilter)
    throws IOException
    {
        try
        {
            aConn.list(aFilter, true);
            throw new TestFailedException();
        }
        catch (IllegalArgumentException ex)
        {
        }
    }

    //
    //
    //
    //
    public void run_1930()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // "Not exists" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test1100/", Connector.READ_WRITE);
            if (conn1.exists())
                throw new TestFailedException();
            counter = 1;
            try
            {
                conn1.list("tmp", true);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 2;
            conn1.close();

            // "Connection points to the file" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/file100.txt", Connector.READ_WRITE);
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 3;
            try
            {
                conn1.list("tmp", true);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 4;
            conn1.close();

            // "Connection has been opened in the WRITE mode" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/file100.txt", Connector.WRITE);
            try
            {
                conn1.list("tmp", true);
                throw new TestFailedException();
            }
            catch (IllegalModeException ex)
            {
            }
            counter = 6;
            conn1.close();

            // "Connection closed" error case.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test100/", Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            conn1.close();
            counter = 7;
            try
            {
                conn1.list("tmp", true);
                throw new TestFailedException();
            }
            catch (ConnectionClosedException ex)
            {
            }
            counter = 8;
            conn1.close();

            // "Filter parameter has null value" error case.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test192/", Connector.READ);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 9;
            try
            {
                conn1.list(null, true);
                throw new TestFailedException();
            }
            catch (NullPointerException ex)
            {
            }
            counter = 10;
            conn1.close();

            // "Filter parameter is incorrect" error case.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test192/", Connector.READ);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 12;
            incorrectFilter(conn1, "temporar/*.txt");
            counter = 13;
            incorrectFilter(conn1, "temporar%5C*.txt");
            counter = 14;
            conn1.close();
            assertTrue(" run_1930() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1930() counter: " + counter + "\n");
            assertTrue(" run_1930() FAILED!!! Counter: " + counter, false);
        }
    }//end run_1930()

}
