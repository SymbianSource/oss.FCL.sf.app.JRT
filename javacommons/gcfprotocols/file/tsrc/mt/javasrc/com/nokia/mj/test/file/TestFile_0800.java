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

public class TestFile_0800 extends FileTestBase
{
    public TestFile_0800()
    {
    }

    public TestFile_0800(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0800("run_0800", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0800) tc).run_0800();
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

    public void run_0800()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test070/liuik.txt", Connector.READ_WRITE);
            counter = 1;
            if (conn1.exists())
                throw new TestFailedException();
            counter = 2;
            long availableSize = conn1.availableSize();
            long totalSize = conn1.totalSize();
            long usedSize = conn1.usedSize();
            counter = 3;
            if (totalSize != (availableSize + usedSize))
                throw new TestFailedException();

            conn1.create();
            counter = 4;
            DataOutputStream dataStream = conn1.openDataOutputStream();
            int addedNum = 57;
            for (int i = 0; i < 2000; ++i)
            {
                dataStream.write(addedNum);
            }
            dataStream.flush();
            dataStream.close();
            counter = 5;

            long availableSize2 = conn1.availableSize();
            long totalSize2 = conn1.totalSize();
            long usedSize2 = conn1.usedSize();
            counter = 6;
            // Comparing these values exactly e.g. "if(availableSize2 !=
            // availableSize)"
            // didn't work. It looks like that size of disk change between
            // writing to
            // the file and reading 'size' values of the disk.
            if (availableSize2 == availableSize)
                throw new TestFailedException("availableSize: " + availableSize
                                              + " availableSize2: " + availableSize2);
            if (usedSize2 == usedSize)
                throw new TestFailedException("usedSize: " + usedSize
                                              + " usedSize2: " + usedSize2);
            if (totalSize != totalSize2)
                throw new TestFailedException("totalSize: " + totalSize
                                              + " totalSize2: " + totalSize2);
            conn1.close();
            assertTrue(" run_0800() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0800() counter: " + counter + "\n");
            assertTrue(" run_0800() FAILED!!! Counter: " + counter, false);
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
                append("TEST FAILED #2!!! Case: run_0800() \n");
                assertTrue(" run_0800() FAILED!!! ", false);
            }
        }//end finally
    }//end run_0800()

}