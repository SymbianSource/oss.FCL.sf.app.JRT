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

/**
 * Tests failure in case 2 Connections are open simultaneously. Opening
 * inputStream when another is open should fail.
 *
 * Not valid anymore
 */
public class TestFile_0250 extends FileTestBase
{
    public TestFile_0250()
    {
    }

    public TestFile_0250(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0250("run_0250", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0250) tc).run_0250();
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

    public void run_0250()
    {
        FileConnection conn1 = null;
        InputStream stream = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test000/p‰‰.txt", Connector.READ_WRITE);
            counter = 1;
            stream = conn1.openInputStream();
            counter = 2;
            byte[] byteArray1 = new byte[600];
            int retValue = stream.read(byteArray1);
            counter = 3;
            if (retValue != -1)
                throw new TestFailedException("retValue: " + retValue);
            counter = 4;
            stream.close();
            conn1.close();

            assertTrue(" run_0250() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED #2!!! Case: run_0250() counter: " + counter
                   + "\n");
            assertTrue(" run_0250() FAILED!!! Counter: " + counter, false);
        }
    }//end run_0250()
}