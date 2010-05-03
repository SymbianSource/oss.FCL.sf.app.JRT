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

public class TestFile_2910_2920 extends FileTestBase
{
    public TestFile_2910_2920()
    {
    }

    public TestFile_2910_2920(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2910_2920("run_2910", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2910_2920) tc).run_2910();
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

    public void run_2910()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test291/newSubDir/", Connector.READ_WRITE);
            counter = 1;
            if (conn1.exists())
                throw new TestFailedException();
            counter = 2;
            conn1.mkdir();
            long creationTime = conn1.lastModified();
            counter = 3;
            conn1.setFileConnection("../");
            counter = 4;
            long modificationTime = conn1.lastModified();
            counter = 5;
            conn1.close();
            counter = 6;
            if (creationTime != modificationTime)
                throw new TestFailedException("creationTime: " + creationTime
                                              + " modificationTime: " + modificationTime);
            counter = 7;

            assertTrue(" run_2910() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2910() counter: " + counter + "\n");
            assertTrue(" run_2910() FAILED!!! Counter: " + counter, false);
        }
    }// end run_2910()

    //
    //
    //
    //
    public void run_2920()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // "Connection has been opened in the READ mode" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test260/file260b.txt", Connector.WRITE);
            counter = 1;
            try
            {
                conn1.lastModified();
                throw new TestFailedException();
            }
            catch (IllegalModeException ex)
            {
            }
            counter = 2;
            conn1.close();
            counter = 3;

            // "Connection closed" error case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/", Connector.READ_WRITE);
            counter = 4;
            if (!conn1.exists())
                throw new TestFailedException();
            conn1.close();
            try
            {
                conn1.lastModified();
                throw new TestFailedException();
            }
            catch (ConnectionClosedException ex)
            {
            }
            counter = 5;
            assertTrue(" run_2920() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2920() counter: " + counter + "\n");
            assertTrue(" run_2920() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2920()

}
