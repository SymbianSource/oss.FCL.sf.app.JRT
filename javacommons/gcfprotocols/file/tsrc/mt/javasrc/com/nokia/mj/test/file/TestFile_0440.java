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
 * Invalid test case. Tries to open outputstream to 2 connection at the same time
 * and expects to throw IOException.<br/>
 * Confirmed from spec lead that such a restriction need not be there.
 *
 */
public class TestFile_0440 extends FileTestBase
{
    public TestFile_0440()
    {
    }

    public TestFile_0440(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0440("run_0440", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0440) tc).run_0440();
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

    public void run_0440()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // 'Two connections try to write to the same file' case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/newFile044", Connector.READ_WRITE);
            counter = 1;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test040/newFile044", Connector.READ_WRITE);
            counter = 2;
            conn1.create();
            counter = 3;
            OutputStream outStream1 = conn1.openOutputStream();
            byte[] byteArray1 = new byte[4000];
            for (int i = 0; i < byteArray1.length; ++i)
                byteArray1[i] = 55;
            outStream1.write(byteArray1);
            outStream1.flush();
            counter = 4;
            try
            {
                OutputStream outStream2 = conn2.openOutputStream();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 5;
            outStream1.close();
            conn2.delete();
            conn1.close();
            conn2.close();

            // Same test as above except second connection is opened after
            // creation of the file.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/newFile044", Connector.READ_WRITE);
            counter = 8;
            conn1.create();
            counter = 9;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/newFile044", Connector.READ_WRITE);
            counter = 10;
            outStream1 = conn1.openOutputStream();
            outStream1.write(byteArray1);
            counter = 11;
            outStream1.flush();
            try
            {
                conn2.openOutputStream();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 12;
            outStream1.close();
            conn1.close();
            conn2.close();

            // Writing data to the file via connection2 after OutputStream of
            // first
            // connection has been closed.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test027/test027b.jar", Connector.READ_WRITE);
            conn1.create();
            counter = 20;
            outStream1 = conn1.openOutputStream();
            byte[] byteArray3 = new byte[50];
            counter = 26;
            for (int i = 0; i < byteArray3.length; ++i)
                byteArray3[i] = 58;
            outStream1.write(byteArray3);
            outStream1.flush();
            counter = 21;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test027/test027b.jar", Connector.READ_WRITE);
            counter = 22;
            OutputStream outStream2 = null;
            try
            {
                outStream2 = conn2.openOutputStream();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 23;
            outStream1.close();
            counter = 24;
            outStream2 = conn2.openOutputStream();
            counter = 25;
            byte[] byteArray2 = new byte[100];
            counter = 26;
            for (int i = 0; i < byteArray2.length; ++i)
                byteArray2[i] = 58;
            outStream2.write(byteArray2);
            counter = 29;
            outStream2.close();
            conn1.close();
            conn2.close();

            counter = 30;
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test027/test027b.jar", Connector.READ_WRITE);
            counter = 31;
            long filesize = conn1.fileSize();
            if (filesize != 100)
                throw new TestFailedException("filesize: " + filesize);
            conn1.delete();
            conn1.close();

            assertTrue(" run_0440() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0440() counter: " + counter + "\n");
            assertTrue(" run_0440() FAILED!!! Counter: " + counter, false);
        }
    }//end run_0440()

}