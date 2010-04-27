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

public class TestFile_1700 extends FileTestBase
{
    public TestFile_1700()
    {
    }

    public TestFile_1700(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1700("run_1700", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1700) tc).run_1700();
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

    public void run_1700()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test170/hiddenFile.txt",
                                                Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            if (false == conn1.isHidden())
                throw new TestFailedException();
            conn1.setHidden(false);
            conn1.close();
            counter = 1;

            // Re-open a connection to the file.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test170/hiddenFile.txt",
                                                Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            if (true == conn1.isHidden())
                throw new TestFailedException();
            conn1.setHidden(true);
            conn1.close();

            counter = 2;

            // Re-open a connection to the file.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test170/hiddenFile.txt",
                                                Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            if (false == conn1.isHidden())
                throw new TestFailedException();
            conn1.close();

            counter = 3;
            // Same test as above to the directory.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test170/hiddenDir/", Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            if (false == conn1.isHidden())
                throw new TestFailedException();
            conn1.setHidden(false);
            conn1.close();
            counter = 4;

            // Re-open a connection to the file.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test170/hiddenDir/", Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            if (true == conn1.isHidden())
                throw new TestFailedException();
            conn1.setHidden(true);
            conn1.close();

            counter = 5;
            // Re-open a connection to the file.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test170/hiddenDir/", Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            if (false == conn1.isHidden())
                throw new TestFailedException();
            conn1.close();
            counter = 6;

            // Calling setHidden() operation when InputStream is opened to the
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
                conn2.setHidden(true);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            stream.close();
            counter = 11;
            conn2.setHidden(true);
            counter = 12;
            conn2.setHidden(false);
            counter = 13;
            conn2.close();
            conn1.close();

            assertTrue(" run_1700() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1700() counter: " + counter + "\n");
            assertTrue(" run_1700() FAILED!!! Counter: " + counter, false);
        }
    }//end run_1700()

}
