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

public class TestFile_2930 extends FileTestBase
{
    public TestFile_2930()
    {
    }

    public TestFile_2930(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2930("run_2930", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2930) tc).run_2930();
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

    public void run_2930()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // 1 'Two connection object points to the same file' case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test291/newFile293", Connector.READ_WRITE);
            counter = 1;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test291/newFile293", Connector.READ_WRITE);
            counter = 2;
            conn2.create();
            counter = 3;
            OutputStream stream = conn2.openOutputStream();
            counter = 4;
            long modifValue1 = conn2.lastModified();
            counter = 5;
            long modifValue2 = conn1.lastModified();
            counter = 6;
            if (modifValue1 != modifValue2)
                throw new TestFailedException("a: " + modifValue1 + "\n"
                                              + "b: " + modifValue2 + "\n");
            counter = 7;
            byte[] byteArray1 = new byte[10];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 75;
            }
            Thread.sleep(3000);
            stream.write(byteArray1);
            counter = 8;
            stream.close();
            Thread.sleep(3000);
            long modifValue3 = conn1.lastModified();
            // round modifValue2 down to nearest minute
            modifValue2 -= modifValue2 % 60000;
            // round endTime up to nearest minute
            modifValue3 += 60000 - (modifValue3 % 60000);
            if (modifValue3 < modifValue2)
                throw new TestFailedException("modifValue2: " + modifValue2
                                              + "\nmodifValue3: " + modifValue3 + "\n");
            counter = 9;
            long modifValue4 = conn2.lastModified();
            modifValue4 += 60000 - (modifValue4 % 60000);
            if (modifValue4 < modifValue2)
                throw new TestFailedException("modifValue2: " + modifValue2
                                              + "\nmodifValue4: " + modifValue4 + "\n");
            counter = 10;
            conn1.close();
            conn2.close();

            // 2 'Two connection object points to the same file' case.
            // OutputStream is open when lastModified() operation is called.
            // 'Two connection points to the same directory' case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test291/newFile293_2", Connector.READ_WRITE);
            counter = 11;
            if (conn1.exists())
                throw new TestFailedException();
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test291/newFile293_2", Connector.READ_WRITE);
            counter = 12;
            modifValue1 = conn1.lastModified();
            counter = 13;
            if (modifValue1 != 0)
                throw new TestFailedException();
            counter = 14;
            modifValue2 = conn2.lastModified();
            if (modifValue2 != 0)
                throw new TestFailedException();
            counter = 15;
            long startTime = System.currentTimeMillis();
            // round startTime down to nearest minute
            startTime -= startTime % 60000;
            Thread.sleep(3000);
            conn1.create();
            Thread.sleep(3000);
            counter = 16;
            long endTime = System.currentTimeMillis();
            // round endTime up to nearest minute
            endTime += 60000 - (endTime % 60000);
            modifValue2 = conn2.lastModified();
            boolean passed = modifValue2 >= startTime && modifValue2 <= endTime;
            if (false == passed)
                throw new TestFailedException("\ns: " + startTime + "\n"
                                              + "m: " + modifValue2 + "\n" + "e: " + endTime + "\n");

            counter = 17;
            OutputStream outStream = conn1.openOutputStream();
            counter = 18;
            byteArray1 = new byte[1000];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 75;
            }
            startTime = System.currentTimeMillis();
            // round startTime down to nearest minute
            startTime -= startTime % 60000;
            Thread.sleep(1000);
            counter = 19;
            outStream.write(byteArray1);
            outStream.flush();
            Thread.sleep(1000);
            endTime = System.currentTimeMillis();
            // round endTime up to nearest minute
            endTime += 60000 - (endTime % 60000);
            counter = 20;
            modifValue2 = conn2.lastModified();
            passed = modifValue2 >= startTime && modifValue2 <= endTime;
            if (false == passed)
                throw new TestFailedException();
            counter = 21;
            outStream.close();
            conn1.close();
            conn2.close();

            assertTrue(" run_2930() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2930() counter: " + counter + "\n");
            assertTrue(" run_2930() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2930()

}
