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

public class TestFile_1130 extends FileTestBase
{
    public TestFile_1130()
    {
    }

    public TestFile_1130(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1130("run_1130", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1130) tc).run_1130();
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

    public void run_1130()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // Making a connection to the new directory.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/newFile113", Connector.READ_WRITE);
            counter = 1;
            long retValue = conn1.fileSize();
            if (-1 != retValue)
                throw new TestFailedException();
            counter = 2;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/newFile113", Connector.READ_WRITE);
            counter = 3;
            conn2.create();
            counter = 4;
            retValue = conn1.fileSize();
            if (0 != retValue)
                throw new TestFailedException();
            conn1.close();
            conn2.close();

            // Calling fileSize() when another connection uses file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/newFile113_2", Connector.READ_WRITE);
            counter = 5;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/newFile113_2", Connector.READ_WRITE);
            counter = 6;
            conn1.create();
            counter = 7;
            OutputStream outStream = conn1.openOutputStream();
            counter = 8;
            byte[] byteArray1 = new byte[1000];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 75;
            }
            outStream.write(byteArray1);
            outStream.flush();
            counter = 9;
            retValue = conn2.fileSize();
            if (1000 != retValue)
                throw new TestFailedException();
            counter = 10;
            retValue = conn1.fileSize();
            if (1000 != retValue)
                throw new TestFailedException();
            counter = 10;
            outStream.close();
            retValue = conn2.fileSize();
            if (1000 != retValue)
                throw new TestFailedException();
            counter = 11;
            retValue = conn1.fileSize();
            if (1000 != retValue)
                throw new TestFailedException();
            conn1.close();
            conn2.close();

            assertTrue(" run_1130() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1130() counter: " + counter + "\n");
            assertTrue(" run_1130() FAILED!!! Counter: " + counter, false);
        }
    }//end run_1130()
}