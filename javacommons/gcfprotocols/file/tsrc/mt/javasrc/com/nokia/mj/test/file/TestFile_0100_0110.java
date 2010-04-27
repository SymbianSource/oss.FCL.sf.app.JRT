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

public class TestFile_0100_0110 extends FileTestBase
{
    public TestFile_0100_0110()
    {
    }

    public TestFile_0100_0110(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0100_0110("run_0100_0110", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0100_0110) tc).run_0100_0110();
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

    public void run_0100_0110()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test000/file007.txt", Connector.READ);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            conn1.close();
            counter = 2;
            if (true == conn1.isOpen())
                throw new TestFailedException();
            counter = 3;
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test000/file007.txt", Connector.READ_WRITE);
            counter = 4;
            if (conn1 == null)
                throw new TestFailedException();
            if (false == conn1.isOpen())
                throw new TestFailedException();
            counter = 5;
            conn1.close();

            assertTrue(" run_0100_0110() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            assertTrue("TEST FAILED!!! Case: run_0100_0110() " + counter, false);
            return;
        }
        finally
        {
            try
            {
                if (conn1 != null)
                    conn1.close();
            }
            catch (Exception ex)
            {
                append(ex.toString());
                append("TEST FAILED!!!: run_0100_0110() " + counter + "\n");
                assertTrue(" run_0100_0110() FAILED!!! Counter: " + counter,
                           false);
                return;
            }
        }
    }//end run_0100_0110()
}