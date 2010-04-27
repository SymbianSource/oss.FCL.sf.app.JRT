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

public class TestFile_1600 extends FileTestBase
{
    public TestFile_1600()
    {
    }

    public TestFile_1600(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1600("run_1600", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1600) tc).run_1600();
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

    public void run_1600()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // "Setting a ReadOnly flag to the file" case.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test160/writable.txt", Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 1;
            conn1.setWritable(false);
            System.out.println("  run_1600(): second setWritable()");
            conn1.setWritable(true);
            counter = 2;
            conn1.close();
            // Re-opening a file in the READ_WRITE mode have to be possible.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test160/writable.txt", Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 3;
            if (false == conn1.canWrite())
                throw new TestFailedException();
            counter = 4;
            conn1.close();

            // "Setting a ReadOnly flag to the directory" case.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test160/writableDir/", Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            conn1.setWritable(false);
            counter = 5;
            conn1.setWritable(true);
            conn1.close();
            counter = 6;
            // Re-opening a directory in the READ_WRITE mode have to be
            // possible.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test160/writableDir/", Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 7;
            if (false == conn1.canWrite())
                throw new TestFailedException();
            conn1.close();

            // Calling setWritable() operation when InputStream is opened to the
            // file by other connection.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "data/Images/myDir/myImages2/myImagesFile.txt");
            counter = 8;
            InputStream stream = conn1.openInputStream();
            counter = 9;
            FileConnection conn2 = (FileConnection) Connector.open("file:///"
                                   + iRoot + "data/Images/myDir/myImages2/myImagesFile.txt");
            counter = 10;
            try
            {
                conn2.setWritable(false);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            stream.close();
            counter = 11;
            conn2.setWritable(false);
            counter = 12;
            conn2.setWritable(true);
            counter = 13;
            conn2.close();
            conn1.close();

            assertTrue(" run_1600() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1600() counter: " + counter + "\n");
            assertTrue(" run_1600() FAILED!!! Counter: " + counter, false);
        }
    }//end run_1600()

}
