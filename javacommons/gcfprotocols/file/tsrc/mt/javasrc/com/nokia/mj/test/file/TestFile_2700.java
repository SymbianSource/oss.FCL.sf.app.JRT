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

public class TestFile_2700 extends FileTestBase
{
    public TestFile_2700()
    {
    }

    public TestFile_2700(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2700("run_2700", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2700) tc).run_2700();
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

    public void run_2700()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test270/", Connector.READ_WRITE);
            counter = 1;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 2;
            conn1.setFileConnection("..");
            String name = conn1.getName();
            counter = 3;
            if (false == name.equals("mainTestDir/"))
                throw new TestFailedException("name: " + name);
            counter = 4;
            String path = conn1.getPath();
            if (false == path.equals("/" + iRoot + ""))
                throw new TestFailedException("path: " + path);
            counter = 5;
            String uri = conn1.getURL();
            if (false == uri.equals("file:///" + iRoot + "mainTestDir/"))
                throw new TestFailedException("uri: " + uri);
            counter = 6;
            if (false == conn1.isDirectory())
                throw new TestFailedException();
            counter = 7;
            if (false == conn1.exists())
                throw new TestFailedException();
            conn1.close();
            counter = 8;

            // Same test as above except argument of the setFileConnection()
            // operation is different.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test270/", Connector.READ_WRITE);
            counter = 9;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 10;
            conn1.setFileConnection("../");
            name = conn1.getName();
            if (false == name.equals("mainTestDir/"))
                throw new TestFailedException();
            counter = 11;
            path = conn1.getPath();
            if (false == path.equals("/" + iRoot + ""))
                throw new TestFailedException();
            counter = 12;
            uri = conn1.getURL();
            if (false == uri.equals("file:///" + iRoot + "mainTestDir/"))
                throw new TestFailedException();
            counter = 13;
            if (false == conn1.isDirectory())
                throw new TestFailedException();
            counter = 14;
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 15;
            conn1.close();

            // Making a connection from the non-created directory to the parent
            // directory.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test270/gfgfgrt/", Connector.READ_WRITE);
            counter = 16;
            if (conn1.exists())
                throw new TestFailedException();
            counter = 17;
            conn1.setFileConnection("../");
            name = conn1.getName();
            if (false == name.equals("test270/"))
                throw new TestFailedException("name: " + name);
            counter = 18;
            path = conn1.getPath();
            if (false == path.equals("/" + iRoot + "mainTestDir/"))
                throw new TestFailedException("path: " + path);
            counter = 19;
            uri = conn1.getURL();
            if (false == uri
                    .equals("file:///" + iRoot + "mainTestDir/test270/"))
                throw new TestFailedException("uri: " + uri);
            counter = 20;
            if (false == conn1.isDirectory())
                throw new TestFailedException();
            counter = 21;
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 22;
            conn1.close();

            assertTrue(" run_2700() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2700() counter: " + counter + "\n");
            assertTrue(" run_2700() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2700()

}
