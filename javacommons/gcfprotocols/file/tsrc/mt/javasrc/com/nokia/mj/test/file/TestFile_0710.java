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
 * Strange test case. Passes when run alone
 */
public class TestFile_0710 extends FileTestBase
{
    public TestFile_0710()
    {
    }

    public TestFile_0710(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0710("run_0710", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0710) tc).run_0710();
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

    public void run_0710()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            //Make a connection using WRITE access mode.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test060/binaryFile.dat",
                                   Connector.WRITE);
            counter = 1;
            try
            {
                conn1.totalSize();
                throw new TestFailedException();
            }
            catch (IllegalModeException ex) {}
            counter = 3;
            conn1.close();
            counter = 4;

            //Try to call totalSize() operation after close() operation call.
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test060/binaryFile.dat",
                                                Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 5;
            conn1.close();
            counter = 6;
            try
            {
                conn1.totalSize();
                throw new TestFailedException();
            }
            catch (ConnectionClosedException ex) {}
            counter = 8;
            conn1.close();
            assertTrue(" run_0710() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0710() counter: " + counter + "\n");
            assertTrue(" run_0710() FAILED!!! COunter: "+counter, false);
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
                append("TEST FAILED #2!!! Case: run_0710() \n");
                assertTrue(" run_0710() FAILED!!! ", true);
            }
        }//end finally
    }//end run_0710()

}