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
 * Tests failure in case 2 Connections are open simultaneously. Opening
 * inputStream when another is open should fail.
 *
 * Not valid anymore
 *
 */
public class TestFile_0270 extends FileTestBase
{
    public TestFile_0270()
    {
    }

    public TestFile_0270(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0270("run_0270", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0270) tc).run_0270();
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

    public void run_0270()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            //'Two connections trying to read a same file' case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test020/newFile027",
                                   Connector.READ_WRITE);
            counter = 1;
            conn2 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test020/newFile027",
                                   Connector.READ_WRITE);
            counter = 2;
            conn1.create();
            counter = 3;
            InputStream inStream1 = conn1.openInputStream();
            counter = 4;
            try
            {
                InputStream inStream2 = conn2.openInputStream();
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 5;
            inStream1.close();
            conn2.delete();
            counter = 6;
            conn1.close();
            conn2.close();

            //Same test as above except second connection is opened after
            //creation of the file.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test020/newFile027",
                                   Connector.READ_WRITE);
            counter = 8;
            conn1.create();
            counter = 9;
            conn2 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test020/newFile027",
                                   Connector.READ_WRITE);
            counter = 10;
            inStream1 = conn1.openInputStream();
            counter = 11;
            try
            {
                InputStream inStream2 = conn2.openInputStream();
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 12;
            inStream1.close();
            conn1.close();
            conn2.close();

            //Same test as above except second connection is opened after
            //creation of the file.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test020/newFile027",
                                   Connector.READ_WRITE);
            counter = 13;
            inStream1 = conn1.openInputStream();
            counter = 14;
            conn2 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test020/newFile027",
                                   Connector.READ_WRITE);
            counter = 15;
            try
            {
                InputStream inStream2 = conn2.openInputStream();
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 16;
            inStream1.close();
            conn1.close();
            conn2.close();

            //Reading an InputStream after InputStream of first connection has been
            //closed.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test027/test027.jar",
                                   Connector.READ_WRITE);
            counter = 20;
            inStream1 = conn1.openInputStream();
            counter = 21;
            conn2 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test027/test027.jar",
                                   Connector.READ_WRITE);
            counter = 22;
            InputStream inStream2 = null;
            try
            {
                inStream2 = conn2.openInputStream();
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 23;
            inStream1.close();
            counter = 24;
            inStream2 = conn2.openInputStream();
            counter = 25;
            byte[] byteArray2 = new byte[50];
            counter = 26;
            int retValue = inStream2.read(byteArray2);
            counter = 27;
            if (retValue != 25)
                throw new TestFailedException("retValue: " + retValue);
            counter = 28;
            for (int i=0; i<retValue; ++i)
            {
                if (byteArray2[i] != 57)
                    throw new TestFailedException
                    ("byteArray2[i]: " + byteArray2[i] + "i: " + i);
            }
            counter = 29;
            inStream2.close();
            conn1.close();
            conn2.close();

            assertTrue(" run_0270() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0270() counter: " + counter + "\n");
            assertTrue(" run_0270() FAILED!!! Counter: "+counter, false);
        }
    }//end run_0270()

}