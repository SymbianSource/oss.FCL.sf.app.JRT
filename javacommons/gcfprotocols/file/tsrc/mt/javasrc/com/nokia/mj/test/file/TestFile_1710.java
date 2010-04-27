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

public class TestFile_1710 extends FileTestBase
{
    public TestFile_1710()
    {
    }

    public TestFile_1710(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        System.out.println("Registering 1710");
        suite.addTest(new TestFile_1710("run_1710", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1710) tc).run_1710();
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

    public void run_1710()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test170/notHiddenFile.txt",
                                                Connector.READ_WRITE);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 2;
            if (true == conn1.isHidden())
                throw new TestFailedException();
            counter = 3;
            DataOutputStream dataStream = conn1.openDataOutputStream();
            int addedNum = 57;
            for (int i = 0; i < 101; ++i)
            {
                dataStream.write(addedNum);
            }
            counter = 4;
            conn1.setHidden(true);
            for (int i = 0; i < 101; ++i)
            {
                dataStream.write(addedNum);
            }
            dataStream.flush();
            dataStream.close();
            conn1.close();
            counter = 5;

            // Re-open a file to check that data was stored successfully.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test170/notHiddenFile.txt",
                                                Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 6;
            if (false == conn1.isHidden())
                throw new TestFailedException();
            counter = 7;
            InputStream stream = conn1.openInputStream();
            byte[] byteArray1 = new byte[210];
            int retValue = stream.read(byteArray1);
            if (retValue != 202)
                throw new TestFailedException();
            counter = 8;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 57)
                    throw new TestFailedException();
            }
            stream.close();
            conn1.close();
            assertTrue(" run_1710() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1710() counter: " + counter + "\n");
            assertTrue(" run_1710() FAILED!!! Counter: " + counter, false);
        }
    }

}
