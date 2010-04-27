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

public class TestFile_1010 extends FileTestBase
{
    public TestFile_1010()
    {
    }

    public TestFile_1010(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }


    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1010("run_1010", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1010) tc).run_1010();
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

    public void run_1010()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test101/", Connector.READ_WRITE);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 2;
            long directorySize = conn1.directorySize(true);
            counter = 3;
            conn1.close();
            if (directorySize != 403)
                throw new TestFailedException("directorySize: " + directorySize);
            counter = 4;
            assertTrue(" run_1010() ok ", true);
            System.out.println(" run_1010() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1010() counter: " + counter + "\n");
            assertTrue(" run_1010() FAILED!!! Counter: " + counter, false);
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
                append("TEST FAILED #2!!! Case: run_1010() \n");
                assertTrue(" run_1010() FAILED!!! ", false);
            }
        }//end finally
    }//end run_1010()
}