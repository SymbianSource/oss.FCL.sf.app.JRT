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

public class TestFile_2000 extends FileTestBase
{
    public TestFile_2000()
    {
    }

    public TestFile_2000(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2000("run_2000", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2000) tc).run_2000();
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

    public void run_2000()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // File URI without trailing slash "/" character.
            FileTestUtils.createFile("file:///" + iRoot
                                     + "mainTestDir/test200/newFile1");
            counter = 1;
            FileTestUtils.createFile("file:///" + iRoot
                                     + "mainTestDir/test200/t��t�� File");
            counter = 2;

            // Calling create() operation when InputStream is opened to the
            // file by other connection.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "data/Images/myDir/myImages2/myImagesFile2.txt");
            counter = 3;
            conn1.create();
            counter = 4;
            InputStream stream = conn1.openInputStream();
            counter = 5;
            FileConnection conn2 = (FileConnection) Connector.open("file:///"
                                   + iRoot + "data/Images/myDir/myImages2/myImagesFile2.txt");
            counter = 6;
            try
            {
                conn2.create();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            stream.close();
            counter = 7;
            conn1.delete();
            counter = 8;
            conn2.create();
            counter = 9;
            stream = conn1.openInputStream();
            counter = 10;
            stream.close();
            counter = 11;
            conn2.delete();
            counter = 12;
            conn2.close();
            conn1.close();

            assertTrue(" run_2000() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2000() counter: " + counter + "\n");
            assertTrue(" run_2000() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2000()

}
