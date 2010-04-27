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
 * Test seems wierd. Data expected isn't proper. Need to take a look at this once.
 * Not to be run right now.
 */
public class TestFile_0220 extends FileTestBase
{
    public TestFile_0220()
    {
    }

    public TestFile_0220(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0220("run_0220", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0220) tc).run_0220();
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

    public void run_0220()
    {
        FileConnection conn1 = null;
        OutputStream outStream = null;
        InputStream tmpInStream = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/newFile022.txt",
                                                    Connector.READ_WRITE);
            counter = 1;
            if (true == conn1.exists())
                throw new TestFailedException();
            conn1.create();
            counter = 2;
            outStream = conn1.openOutputStream();
            counter = 3;
            byte[] byteArray1 = new byte[10];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 5;
            }
            counter = 4;
            outStream.write(byteArray1);
            outStream.flush();

            // Open InputStream before OutputStream has been closed.
            boolean exceptionOccured = false;
            tmpInStream = conn1.openInputStream();
            counter = 5;
            byte[] byteArray2 = new byte[10];
            int retValue = tmpInStream.read(byteArray2);
            counter = 6;
            if (retValue != 10)
                throw new TestFailedException("retValue: " + retValue);
            counter = 7;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray2[i] != 5)
                    throw new TestFailedException();
            }
            counter = 8;

            // Adding more data.
            byteArray1 = new byte[1000];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 65;
            }
            counter = 10;
            outStream.write(byteArray1);
            outStream.flush();

            // Reading a just added data.
            byte[] byteArray4 = new byte[1000];
            retValue = tmpInStream.read(byteArray4);
            counter = 11;
            if (retValue != 512)
                throw new TestFailedException("retValue: " + retValue);
            counter = 12;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray4[i] != 65)
                {
                    throw new TestFailedException("offset: "+i);
                }
            }
            counter = 13;

            // Below read() operation reaches to the end of the file mark.
            byteArray1 = new byte[20];
            retValue = tmpInStream.read(byteArray1);
            counter = 14;
            if (retValue != -1)
                throw new TestFailedException("retValue: " + retValue);

            // Adding a more data.
            byteArray1 = new byte[10];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 67;
            }
            counter = 15;
            outStream.write(byteArray1);
            outStream.flush();

            // An added data cannot be read because InputStream has been reached
            // to the end of the file mark.
            byteArray1 = new byte[20];
            retValue = tmpInStream.read(byteArray1);
            counter = 16;
            if (retValue != -1)
                throw new TestFailedException("retValue: " + retValue);

            // Opening a DataInputStream. This should throw an IOException.
            counter = 17;
            boolean exceptionOccurred = false;
            try
            {
                DataInputStream dataStream = conn1.openDataInputStream();
            }
            catch (IOException e)
            {
                exceptionOccurred = true;
            }
            if (exceptionOccurred == false)
                throw new TestFailedException();
            counter = 18;
            tmpInStream.close();
            conn1.close();
            outStream.close();

            // Re-open a connection and checking that all data was stored on the
            // filesystem.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test020/newFile022.txt",
                                                Connector.READ_WRITE);
            counter = 19;
            if (conn1 == null)
                throw new TestFailedException();
            tmpInStream = conn1.openInputStream();
            byte[] byteArray3 = new byte[50];
            retValue = tmpInStream.read(byteArray2);
            counter = 20;
            if (retValue != 30)
                throw new TestFailedException("retValue: " + retValue);
            counter = 21;
            for (int i = 0; i < retValue; ++i)
            {
                if (i < 10)
                {
                    if (byteArray2[i] != 5)
                        throw new TestFailedException();
                }
                else if (i < 19)
                {
                    if (byteArray2[i] != 65)
                        throw new TestFailedException();
                }
                else
                {
                    if (byteArray2[i] != 67)
                        throw new TestFailedException();
                }
            }//end for
            counter = 22;
            assertTrue(" run_0220() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0220() counter: " + counter + "\n");
            assertTrue(" run_0220() FAILED!!! Counter: "+counter, false);
        }
        finally
        {
            try
            {
                if (outStream != null)
                    outStream.close();
                if (tmpInStream != null)
                    tmpInStream.close();
                if (conn1 != null)
                {
                    if (true == conn1.isOpen())
                        conn1.close();
                }
            }
            catch (Exception ex)
            {
                append(ex.toString());
                append("TEST FAILED #2!!! Case: run_0220() \n");
                assertTrue(" run_0220() FAILED!!! Counter: "+counter, false);
            }
        }//end finally
    }//end run_0220()
}