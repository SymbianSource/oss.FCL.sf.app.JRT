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

public class TestFile_1800 extends FileTestBase
{
    public TestFile_1800()
    {
    }

    public TestFile_1800(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1800("run_1800", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1800) tc).run_1800();
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

    public void run_1800()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test180/", Connector.READ);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 1;
            Enumeration fileList = conn1.list();
            counter = 2;
            if (true == fileList.hasMoreElements())
                throw new TestFailedException();
            counter = 3;

            // Calling list() operation when InputStream is opened to the
            // file by other connection.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "data/Images/myDir/myImages2/myImagesFile.txt");
            counter = 4;
            InputStream stream = conn1.openInputStream();
            counter = 5;
            FileConnection conn2 = (FileConnection) Connector.open("file:///"
                                   + iRoot + "data/Images/myDir/myImages2/");
            counter = 6;
            try
            {
                conn1.list();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 61;
            fileList = conn2.list();
            if (false == fileList.hasMoreElements())
                throw new TestFailedException();
            counter = 7;
            int elementCounter = 0;
            boolean failedFlag = false;
            while (fileList.hasMoreElements())
            {
                String obj = (String) fileList.nextElement();
                if (0 == obj.compareTo("myImagesFile.txt"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            counter = 8;
            if (failedFlag == true)
                throw new TestFailedException();
            stream.close();
            conn2.close();
            conn1.close();

            assertTrue(" run_1800() ok ", true);
            System.out.println(" run_1800() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1800() counter: " + counter + "\n");
            assertTrue(" run_1800() FAILED!!! Counter: " + counter, false);
        }
    }//end run_1800()

}
