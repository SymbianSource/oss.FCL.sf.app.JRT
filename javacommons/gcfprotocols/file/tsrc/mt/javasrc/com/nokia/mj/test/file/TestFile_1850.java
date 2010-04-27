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

public class TestFile_1850 extends FileTestBase
{
    public TestFile_1850()
    {
    }

    public TestFile_1850(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1850("run_1850", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1850) tc).run_1850();
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

    public void run_1850()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        FileConnection conn3 = null;
        int counter = 0;
        try
        {
            // 'Two connection points to the same directory' case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test185/", Connector.READ_WRITE);
            counter = 1;
            if (conn1.exists())
                throw new TestFailedException();
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test185/", Connector.READ_WRITE);
            counter = 2;
            try
            {
                conn2.list();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 3;
            conn1.mkdir();
            counter = 4;
            conn3 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test185/file1", Connector.READ_WRITE);
            counter = 5;
            conn3.create();
            counter = 6;
            conn3.close();
            Enumeration fileList = conn2.list();
            counter = 7;
            if (false == fileList.hasMoreElements())
                throw new TestFailedException();
            counter = 8;
            int elementCounter = 0;
            boolean failedFlag = false;
            while (fileList.hasMoreElements())
            {
                String obj = (String) fileList.nextElement();
                if (0 == obj.compareTo("file1"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            counter = 9;
            if (failedFlag == true)
                throw new TestFailedException();
            conn1.close();
            conn2.close();

            assertTrue(" run_1850() ok ", true);
            System.out.println(" run_1850() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1850() counter: " + counter + "\n");
            assertTrue(" run_1850() FAILED!!! Counter: " + counter, false);
        }
    }//end run_1850()

}
