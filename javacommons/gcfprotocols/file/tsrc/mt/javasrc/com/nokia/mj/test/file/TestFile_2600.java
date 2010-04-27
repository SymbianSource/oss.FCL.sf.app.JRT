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
 * Problem with 2 connections open at the same time.<br/> Expects truncate to throw
 * IOException in case File Stream is open by other connection.
 */
public class TestFile_2600 extends FileTestBase
{
    public TestFile_2600()
    {
    }

    public TestFile_2600(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2600("run_2600", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2600) tc).run_2600();
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

    public void run_2600()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test260/file260.txt", Connector.READ_WRITE);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 2;
            conn1.truncate(99);
            conn1.close();

            // Re-opened a file and check truncated file.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test260/file260.txt", Connector.READ_WRITE);
            counter = 3;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 4;
            InputStream stream = conn1.openInputStream();
            byte[] byteArray1 = new byte[100];
            int retValue = stream.read(byteArray1);
            if (retValue != 99)
                throw new TestFailedException("retValue: " + retValue);
            counter = 5;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 54)
                    throw new TestFailedException();
            }
            stream.close();
            conn1.close();

            // Calling truncate() operation when InputStream is opened to the
            // file by other connection.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "data/Images/myDir/myImages2/myImagesFile.txt");
            counter = 6;
            stream = conn1.openInputStream();
            counter = 8;
            FileConnection conn2 = (FileConnection) Connector.open("file:///"
                                   + iRoot + "data/Images/myDir/myImages2/myImagesFile.txt");
            counter = 9;
            try
            {
                conn2.truncate(900);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 10;
            stream.close();
            conn2.truncate(900);
            if (900 != conn1.fileSize())
                throw new TestFailedException();
            counter = 11;
            conn1.close();
            conn2.close();

            assertTrue(" run_2600() ok ", true);
            System.out.println(" run_2600() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2600() counter: " + counter + "\n");
            assertTrue(" run_2600() FAILED!!! Counter: " + counter, false);
        }
    }// end run_2600()

}
