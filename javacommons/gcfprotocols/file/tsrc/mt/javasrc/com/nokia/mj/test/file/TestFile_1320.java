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

public class TestFile_1320 extends FileTestBase
{
    public TestFile_1320()
    {
    }

    public TestFile_1320(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1320("run_1320", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1320) tc).run_1320();
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

    public void run_1320()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // 1
            // Making a connection to the new file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/newFile132", Connector.READ_WRITE);
            counter = 1;
            if (true == conn1.canWrite())
                throw new TestFailedException();
            counter = 2;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/newFile132", Connector.READ_WRITE);
            counter = 3;
            conn2.create();
            counter = 4;
            if (false == conn1.canWrite())
                throw new TestFailedException();
            conn1.close();
            conn2.close();

            // 2
            // Making a connection to a new directory.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/newDir132/", Connector.READ_WRITE);
            counter = 5;
            if (true == conn1.canWrite())
                throw new TestFailedException();
            counter = 6;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/newDir132/", Connector.READ_WRITE);
            counter = 7;
            conn2.mkdir();
            counter = 8;
            if (false == conn1.canWrite())
                throw new TestFailedException();
            conn1.close();
            conn2.close();

            // 3
            // 'Two connection points to the same file' case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/newFile132_2", Connector.READ_WRITE);
            counter = 9;
            if (conn1.exists())
                throw new TestFailedException();
            counter = 91;
            if (true == conn1.canWrite())
                throw new TestFailedException();
            counter = 10;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/newFile132_2", Connector.READ_WRITE);
            counter = 11;
            if (true == conn2.canWrite())
                throw new TestFailedException();
            counter = 12;
            conn1.create();
            counter = 13;
            OutputStream outStream = conn1.openOutputStream();
            counter = 14;
            byte[] byteArray1 = new byte[1000];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 75;
            }
            outStream.write(byteArray1);
            outStream.flush();
            counter = 15;
            if (false == conn1.canWrite())
                throw new TestFailedException();
            counter = 16;
            if (false == conn2.canWrite())
                throw new TestFailedException();
            outStream.close();
            counter = 17;
            if (false == conn1.canWrite())
                throw new TestFailedException();
            counter = 18;
            if (false == conn2.canWrite())
                throw new TestFailedException();
            conn1.close();
            conn2.close();
            outStream.close();

            assertTrue(" run_1320() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1320() counter: " + counter + "\n");
            assertTrue(" run_1320() FAILED!!! Counter: " + counter, false);
        }
    }//end run_1320()

}
