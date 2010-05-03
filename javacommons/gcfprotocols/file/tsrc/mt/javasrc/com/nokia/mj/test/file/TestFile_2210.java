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

public class TestFile_2210 extends FileTestBase
{
    public TestFile_2210()
    {
    }

    public TestFile_2210(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2210("run_2210", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2210) tc).run_2210();
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

    public void run_2210()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            //"Connection has been opened in the WRITE mode" error case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test200/newfile5.txt",
                                   Connector.WRITE);
            try
            {
                conn1.exists();
                throw new TestFailedException();
            }
            catch (IllegalModeException ex) {}
            conn1.close();

            //"Connection closed" error case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test200/newfile15.txt",
                                   Connector.READ);
            conn1.close();
            try
            {
                conn1.exists();
                throw new TestFailedException();
            }
            catch (ConnectionClosedException ex) {}
            conn1.close();

            assertTrue(" run_2210() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2210() counter: " + counter + "\n");
            assertTrue(" run_2210() FAILED!!! Counter: "+counter, false);
        }
    }//end run_2210()
}
