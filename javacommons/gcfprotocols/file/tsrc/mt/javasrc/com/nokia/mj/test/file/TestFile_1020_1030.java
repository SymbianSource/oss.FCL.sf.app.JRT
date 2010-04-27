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

public class TestFile_1020_1030 extends FileTestBase
{
    public TestFile_1020_1030()
    {
    }

    public TestFile_1020_1030(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1020_1030("run_1020", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1020_1030) tc).run_1020();
            }
        }));

        suite.addTest(new TestFile_1020_1030("run_1030", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1020_1030) tc).run_1030();
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

    public void run_1020()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test102/", Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            long directorySize = conn1.directorySize(true);
            conn1.close();
            if (directorySize != 2307)
                throw new TestFailedException();
            assertTrue(" run_1020() ok ", true);
            System.out.println(" run_1020() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1020() counter: " + counter + "\n");
            assertTrue(" run_1020() FAILED!!! Counter: " + counter, false);
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
                append("TEST FAILED #2!!! Case: run_1020() \n");
                assertTrue(" run_1020() FAILED!!! ", false);
            }
        }// end finally
    }// end run_1020()

    //
    //
    //
    //
    public void run_1030()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test102/newDirDFD/", Connector.READ_WRITE);
            if (conn1.exists())
                throw new TestFailedException();
            long directorySize = conn1.directorySize(true);
            conn1.close();
            if (directorySize != -1)
                throw new TestFailedException();
            assertTrue(" run_1030() ok ", true);
            System.out.println(" run_1030() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1030() counter: " + counter + "\n");
            assertTrue(" run_1030() FAILED!!! Counter: "+counter, false);
        }
    }//end run_1030()
}