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

public class TestFile_0700 extends FileTestBase
{
    public TestFile_0700()
    {
    }

    public TestFile_0700(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0700("run_0700", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0700) tc).run_0700();
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

    public void run_0700()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test060/binaryFile.dat",
                                                Connector.READ_WRITE);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 2;
            long totalSize = conn1.totalSize();
            counter = 3;
            long availableSize = conn1.availableSize();
            counter = 4;
            long usedSize = conn1.usedSize();
            counter = 5;
            if (totalSize != (availableSize + usedSize))
                throw new TestFailedException("totalSize: " + totalSize
                                              + " availableSize: " + availableSize + " usedSize: "
                                              + usedSize);
            counter = 6;
            conn1.close();

            // Creating a new file and add some data to it.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test070/newFile.txt", Connector.READ_WRITE);
            if (conn1.exists())
                throw new TestFailedException();
            conn1.create();
            DataOutputStream dataStream = conn1.openDataOutputStream();
            int addedNum = 57;
            for (int i = 0; i < 2000; ++i)
            {
                dataStream.write(addedNum);
            }
            dataStream.close();
            counter = 7;

            totalSize = conn1.totalSize();
            counter = 8;
            availableSize = conn1.availableSize();
            usedSize = conn1.usedSize();
            if (totalSize != (availableSize + usedSize))
                throw new TestFailedException("totalSize: " + totalSize
                                              + " availableSize: " + availableSize + " usedSize: "
                                              + usedSize);
            counter = 9;
            conn1.close();
            assertTrue(" run_0700() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0700() counter: " + counter + "\n");
            assertTrue(" run_0700() FAILED!!! Counter: " + counter, false);
        }
        finally
        {
            try
            {
                if (conn1 != null)
                {
                    if (true == conn1.isOpen())
                        conn1.close();
                }
            }
            catch (Exception ex)
            {
                append(ex.toString());
                append("TEST FAILED #2!!! Case: run_0700() \n");
                assertTrue(" run_0700() FAILED!!! Counter: " + counter, false);
            }
        }//end finally
    }//end run_0700()

}