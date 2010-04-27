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

public class TestFile_1400 extends FileTestBase
{
    public TestFile_1400()
    {
    }

    public TestFile_1400(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }


    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1400("run_1400", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1400) tc).run_1400();
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

    public void run_1400()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Non-hidden file case.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test000/file007.txt", Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 1;
            boolean hiddenFlag = conn1.isHidden();
            if (hiddenFlag == true)
                throw new TestFailedException();
            counter = 2;
            conn1.close();

            // Hidden file case.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test000/fileHidden.txt",
                                                Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            hiddenFlag = conn1.isHidden();
            if (hiddenFlag == false)
                throw new TestFailedException();
            counter = 3;
            conn1.close();

            // Non-hidden directory case.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test000/dir 003/", Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            hiddenFlag = conn1.isHidden();
            if (hiddenFlag == true)
                throw new TestFailedException();
            counter = 4;
            conn1.close();

            // Hidden directory case.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test000/dirHidden/", Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            hiddenFlag = conn1.isHidden();
            if (hiddenFlag == false)
                throw new TestFailedException();
            counter = 5;
            conn1.close();

            // Passing a new directory's URI as parameter to the
            // Connector.open()
            // operation.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test110/fadggt/", Connector.READ_WRITE);
            if (conn1.exists())
                throw new TestFailedException();
            counter = 6;
            hiddenFlag = conn1.isHidden();
            if (hiddenFlag == true)
                throw new TestFailedException();
            counter = 7;
            conn1.close();

            // Make a READ_WRITE connection to the read-only file and call
            // canWrite()
            // operation.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/binaryFile2.dat",
                                                    Connector.READ_WRITE);
            counter = 13;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 14;
            boolean flag = conn1.isHidden();
            counter = 15;
            if (true == flag)
                throw new TestFailedException();
            counter = 16;
            conn1.close();

            // Calling isHidden() operation when InputStream is opened to the
            // file by other connection.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "data/Images/myDir/myImages2/myImagesFile.txt");
            counter = 17;
            InputStream stream = conn1.openInputStream();
            conn1.setHidden(true);
            counter = 18;
            FileConnection conn2 = (FileConnection) Connector.open("file:///"
                                   + iRoot + "data/Images/myDir/myImages2/myImagesFile.txt");
            counter = 19;
            if (false == conn2.isHidden())
                throw new TestFailedException();
            conn1.close();
            stream.close();
            counter = 20;
            conn2.setHidden(false);
            conn2.close();

            assertTrue(" run_1400() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1400() counter: " + counter + "\n");
            assertTrue(" run_1400() FAILED!!! ", false);
        }
    }//end run_1400()

}
