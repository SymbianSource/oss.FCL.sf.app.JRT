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

public class TestFile_2300 extends FileTestBase
{
    public TestFile_2300()
    {
    }

    public TestFile_2300(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2300("run_2300", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2300) tc).run_2300();
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

    public void run_2300()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test220/newDir230/", Connector.READ_WRITE);
            counter = 1;
            if (conn1.exists())
                throw new TestFailedException();
            if (true == conn1.isDirectory())
                throw new TestFailedException();
            counter = 2;
            conn1.mkdir();
            if (false == conn1.isDirectory())
                throw new TestFailedException();
            conn1.close();

            // Re-opening a directory.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test220/newDir230/", Connector.READ_WRITE);
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 3;
            if (false == conn1.isDirectory())
                throw new TestFailedException();
            conn1.close();

            // Make a connection to the existing file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/file100.txt", Connector.READ_WRITE);
            counter = 4;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 5;
            if (true == conn1.isDirectory())
                throw new TestFailedException();
            counter = 6;
            conn1.close();
            assertTrue(" run_2300() ok  ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2300() counter: " + counter + "\n");
            assertTrue(" run_2300() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2300()

}
