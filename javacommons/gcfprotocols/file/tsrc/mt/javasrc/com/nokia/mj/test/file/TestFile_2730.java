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

public class TestFile_2730 extends FileTestBase
{
    public TestFile_2730()
    {
    }

    public TestFile_2730(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2730("run_2730", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2730) tc).run_2730();
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

    public void run_2730()
    {
        FileConnection conn1 = null;
        DataInputStream stream = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test273/", Connector.READ_WRITE);
            counter = 1;
            conn1.setFileConnection("binaryFile");
            counter = 2;
            if (conn1.canWrite())
                throw new TestFailedException();
            counter = 3;
            if (100 != conn1.fileSize())
                throw new TestFailedException();
            conn1.rename("binaryFile2");
            try
            {
                conn1.truncate(2);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 4;
            conn1.close();

            // Re-open a file.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test273/binaryFile2", Connector.READ_WRITE);
            counter = 5;
            if (!conn1.exists())
                throw new TestFailedException();

            assertTrue(" run_2730() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2730() counter: " + counter + "\n");
            assertTrue(" run_2730() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2730()
}
