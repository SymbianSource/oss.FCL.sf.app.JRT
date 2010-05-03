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
 * Tests various scenarios on opening files.
 *
 * NOTE: In case it is run in Installer context, private dir will not have
 * scratch directory created. So, tests 21 and 22 will fail then. <br/>
 * To get the test to pass, create a folder called "scratch" in installer's
 * private dir.
 */
public class TestFile_0040 extends FileTestBase
{
    public TestFile_0040()
    {
    }

    public TestFile_0040(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0040("run_0040", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0040) tc).run_0040();
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

    public void run_0040()
    {
        int counter = 0;
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        try
        {
            // 1
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test000/file007.txt", Connector.READ);
            counter = 1;
            InputStream inStream1 = conn1.openInputStream();
            counter = 2;
            conn2 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test000/file007.txt", Connector.READ);
            counter = 3;
            InputStream inStream2 = conn2.openInputStream();
            counter = 4;
            conn1.close();
            conn2.close();
            inStream1.close();
            inStream2.close();
            counter = 5;

            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test000/file007.txt", Connector.READ_WRITE);
            counter = 11;
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 12;
            conn2 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test000/file007.txt", Connector.READ_WRITE);
            if (null == conn2)
                throw new TestFailedException();
            counter = 13;
            if (false == conn2.exists())
                throw new TestFailedException();
            counter = 14;
            conn1.close();
            conn2.close();

            // 4
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test000/file007.txt", Connector.READ_WRITE);
            counter = 15;
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 16;
            inStream1 = conn1.openInputStream();
            inStream1.close();
            conn2 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test000/file007.txt", Connector.READ_WRITE);
            counter = 17;
            if (null == conn2)
                throw new TestFailedException();
            counter = 18;
            if (false == conn2.exists())
                throw new TestFailedException();
            counter = 19;

            // 5
            String uri = System.getProperty("fileconn.dir.private");
            counter = 20;
            conn1 = (FileConnection) Connector.open(uri, Connector.READ);
            counter = 21;
            if (!conn1.exists())
                throw new TestFailedException();
            conn1.close();
            counter = 22;

            // 6
            uri = uri + "myDir5/myFile6";
            conn1 = (FileConnection) Connector.open(uri, Connector.READ_WRITE,
                                                    true);
            counter = 23;
            String uri2 = System.getProperty("fileconn.dir.private")
                          + "myDir5/";
            conn2 = (FileConnection) Connector.open(uri2, Connector.READ_WRITE,
                                                    true);
            counter = 24;
            conn2.mkdir();
            counter = 25;
            conn1.create();
            counter = 26;
            conn1.delete();
            counter = 27;
            conn2.delete();
            conn1.close();
            conn2.close();

            assertTrue(" run_0040() ok ", true);
        }
        catch (Exception ex)
        {
            System.out.println("Expect this test to fail in case it is not run as Manufacturer domain application.");
            System.out.println("Fails in installer runtime.");
            append(ex.toString());
            append("TEST FAILED!!!: run_0040(): counter: " + counter + "\n");
            assertTrue(" run_0040() FAILED!!! Counter: "+counter, false);
        }
    }//end run_0040()

}