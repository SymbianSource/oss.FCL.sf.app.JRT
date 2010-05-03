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

public class TestFile_2560 extends FileTestBase
{
    public TestFile_2560()
    {
    }

    public TestFile_2560(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2560("run_2560", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2560) tc).run_2560();
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

    public void run_2560()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // Renaming a file name in the case where two connection points
            // to the same file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test250/newFile256", Connector.READ_WRITE);
            counter = 1;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test250/newFile256", Connector.READ_WRITE);
            counter = 2;
            conn2.create();
            counter = 3;
            conn1.rename("newFile256_2");
            counter = 5;
            try
            {
                conn2.rename("newFile256_3");
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            conn1.close();
            conn2.close();

            // Try to change file name when other connection object has open
            // stream.
            conn1 = (FileConnection) Connector
                    .open("file:///" + iRoot
                          + "mainTestDir/test250/newFile256_10",
                          Connector.READ_WRITE);
            counter = 6;
            conn1.create();
            counter = 7;
            conn1.rename("newFile256_11");
            counter = 8;
            conn2 = (FileConnection) Connector
                    .open("file:///" + iRoot
                          + "mainTestDir/test250/newFile256_11",
                          Connector.READ_WRITE);
            counter = 9;
            InputStream stream = conn1.openInputStream();
            counter = 10;
            try
            {
                conn2.rename("newFile256_12");
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            stream.close();
            counter = 100;
            conn2.rename("newFile256_12");
            conn1.close();
            conn2.close();

            // Renaming a directory name in the case where two connection points
            // to the same directory.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test250/newDir256", Connector.READ_WRITE);
            counter = 11;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test250/newDir256", Connector.READ_WRITE);
            counter = 12;
            conn2.mkdir();
            counter = 13;
            conn1.rename("newDir256_2");
            counter = 14;
            try
            {
                conn2.rename("newFile256_3");
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            conn1.close();
            conn2.close();

            assertTrue(" run_2560() ok  ", true);
            System.out.println(" run_2560() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2560() counter: " + counter + "\n");
            assertTrue(" run_2560() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2560()

}
