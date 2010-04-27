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

public class TestFile_2710 extends FileTestBase
{
    public TestFile_2710()
    {
    }

    public TestFile_2710(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2710("run_2710", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2710) tc).run_2710();
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

    public void run_2710()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // File case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/", Connector.READ_WRITE);
            counter = 1;
            if (!conn1.exists())
                throw new TestFailedException();
            Enumeration fileList = conn1.list();
            counter = 2;
            if (false == fileList.hasMoreElements())
                throw new TestFailedException();
            String fileName = null;
            while (fileList.hasMoreElements())
            {
                String obj = (String) fileList.nextElement();
                if (obj.equals("file100.txt"))
                    fileName = obj;
            }// end while
            counter = 3;
            if (fileName == null)
                throw new TestFailedException();
            counter = 4;
            conn1.setFileConnection(fileName);
            String name = conn1.getName();
            if (false == name.equals("file100.txt"))
                throw new TestFailedException();
            counter = 5;
            String path = conn1.getPath();
            if (false == path.equals("/" + iRoot + "mainTestDir/test100/"))
                throw new TestFailedException("path: " + path);
            counter = 6;
            String uri = conn1.getURL();
            if (false == uri.equals("file:///" + iRoot
                                    + "mainTestDir/test100/file100.txt"))
                throw new TestFailedException("uri: " + uri);
            counter = 7;
            if (true == conn1.isDirectory())
                throw new TestFailedException();
            counter = 8;
            if (false == conn1.exists())
                throw new TestFailedException();
            conn1.close();

            // Directory case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test100/", Connector.READ_WRITE);
            counter = 9;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 10;
            fileList = conn1.list();
            if (false == fileList.hasMoreElements())
                throw new TestFailedException();
            counter = 11;
            String dirName = null;
            while (fileList.hasMoreElements())
            {
                String obj = (String) fileList.nextElement();
                if (obj.equals("subDir/"))
                    dirName = obj;
            }// end while
            counter = 12;
            if (dirName == null)
                throw new TestFailedException();
            counter = 13;
            conn1.setFileConnection(dirName);
            name = conn1.getName();
            if (false == name.equals("subDir/"))
                throw new TestFailedException();
            counter = 14;
            path = conn1.getPath();
            if (false == path.equals("/" + iRoot + "mainTestDir/test100/"))
                throw new TestFailedException();
            counter = 15;
            uri = conn1.getURL();
            if (false == uri.equals("file:///" + iRoot
                                    + "mainTestDir/test100/subDir/"))
                throw new TestFailedException();
            counter = 16;
            if (false == conn1.isDirectory())
                throw new TestFailedException();
            counter = 17;
            if (false == conn1.exists())
                throw new TestFailedException();
            conn1.close();

            assertTrue(" run_2710() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2710() counter: " + counter + "\n");
            assertTrue(" run_2710() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2710()

}
