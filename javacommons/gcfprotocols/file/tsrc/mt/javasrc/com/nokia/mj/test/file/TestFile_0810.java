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

public class TestFile_0810 extends FileTestBase
{
    public TestFile_0810()
    {
    }

    public TestFile_0810(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0810("run_0810", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0810) tc).run_0810();
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

    public void run_0810()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Below is tested IllegalModeException cases.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test060/binaryFile.dat", Connector.WRITE);
            boolean exceptionOccured = false;
            try
            {
                conn1.availableSize();
            }
            catch (IllegalModeException ex)
            {
                exceptionOccured = true;
            }
            if (exceptionOccured == false)
                throw new TestFailedException();
            exceptionOccured = false;
            // Same test to the usedSize() operation.
            try
            {
                conn1.usedSize();
            }
            catch (IllegalModeException ex)
            {
                exceptionOccured = true;
            }
            if (exceptionOccured == false)
                throw new TestFailedException();
            exceptionOccured = false;
            conn1.close();

            // Below is tested ConnectionClosedException cases.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test060/binaryFile.dat",
                                                Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            conn1.close();

            exceptionOccured = false;
            try
            {
                conn1.availableSize();
            }
            catch (ConnectionClosedException ex)
            {
                exceptionOccured = true;
            }
            if (exceptionOccured == false)
                throw new TestFailedException();
            exceptionOccured = false;
            // Same test to the usedSize() operation.
            try
            {
                conn1.usedSize();
            }
            catch (ConnectionClosedException ex)
            {
                exceptionOccured = true;
            }
            if (exceptionOccured == false)
                throw new TestFailedException();
            exceptionOccured = false;
            conn1.close();
            assertTrue(" run_0810() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0810() counter: " + counter + "\n");
            assertTrue(" run_0810() FAILED!!! Counter: " + counter, false);
        }
    }//end run_0810()
}