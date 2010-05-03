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

public class TestFile_0500 extends FileTestBase
{
    public TestFile_0500()
    {
    }

    public TestFile_0500(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0500("run_0500", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0500) tc).run_0500();
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

    public void run_0500()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open(
                        "file:///C:/mainTestDir/test050/newFile",
                        Connector.READ_WRITE);
            counter = 1;
            if (conn1.exists())
                throw new TestFailedException();
            counter = 2;
            conn1.create();
            counter = 3;
            DataOutputStream dataStream = conn1.openDataOutputStream();
            counter = 4;
            int addedNum = 57;
            for (int i = 0; i < 1000; ++i)
            {
                dataStream.write(addedNum);
            }
            dataStream.close();

            // Re-opening a DataOutputStream.
            dataStream = conn1.openDataOutputStream();
            addedNum = 56;
            for (int i = 0; i < 10; ++i)
            {
                dataStream.write(addedNum);
            }
            dataStream.close();
            conn1.close();
            counter = 5;

            // Take a new connection to the 'binaryFile.dat' file.
            // conn1 =
            // openExistsURI("file:///"+iRoot+"mainTestDir/test050/NewFile/",
            // Connector.READ);
            conn1 = FileTestUtils.openExistsURI("file:///C:/MAINTESTDIR/TEST050/NEWFILE/",
                                                Connector.READ);
            counter = 6;
            if (conn1 == null)
                throw new TestFailedException();
            byte[] byteArray2 = new byte[1001];
            DataInputStream inStream = conn1.openDataInputStream();
            counter = 7;
            int retValue = inStream.read(byteArray2);
            if (retValue != 1000)
                throw new TestFailedException();
            counter = 8;
            for (int i = 0; i < retValue; ++i)
            {
                if (i < 10)
                {
                    if (byteArray2[i] != 56)
                        throw new TestFailedException("i: " + i
                                                      + " byteArray2[i]: " + byteArray2[i]);
                }
                else
                {
                    if (byteArray2[i] != 57)
                        throw new TestFailedException("i: " + i
                                                      + " byteArray2[i]: " + byteArray2[i]);
                }
            }// end for
            counter = 9;
            retValue = inStream.read(byteArray2);
            if (retValue != -1)
                throw new TestFailedException();
            conn1.close();
            inStream.close();

            assertTrue(" run_0500() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0500() counter: " + counter + "\n");
            assertTrue(" run_0500() FAILED!!! Counter: " + counter, false);
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
                append("TEST FAILED #2!!! Case: run_0500() \n");
                assertTrue(" run_0500() FAILED!!! Counter: " + counter, false);
            }
        }//end finally
    }//end run_0500()

}