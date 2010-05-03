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
 * Tests failure in case 2 Connections are open simultaneously.
 * Opening inputStream when another is open should fail.
 *
 * Not valid anymore
 */
public class TestFile_0210 extends FileTestBase
{
    public TestFile_0210()
    {
    }

    public TestFile_0210(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0210("run_0210", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0210) tc).run_0210();
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

    public void run_0210()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test020/binaryFile.dat",
                                                Connector.READ_WRITE);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            InputStream stream = conn1.openInputStream();
            byte[] byteArray1 = new byte[5];
            int retValue = stream.read(byteArray1);
            counter = 2;
            if (retValue != 5)
                throw new TestFailedException();
            counter = 3;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 4)
                    throw new TestFailedException();
            }
            counter = 4;
            // Note: Connection is closed here but stream could be still alive
            // until
            // stream is closed.
            // System.out.println("conn1.close(); is called");
            conn1.close();
            byteArray1 = new byte[507];
            counter = 41;
            retValue = stream.read(byteArray1);
            counter = 5;
            // Note: Return value is 512 - 5 because 5 bytes has been already
            // read.
            if (retValue != 507)
                throw new TestFailedException("retValue: " + retValue);
            counter = 6;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 4)
                    throw new TestFailedException();
            }
            counter = 7;
            byteArray1 = new byte[30];
            retValue = stream.read(byteArray1);
            counter = 8;
            if (retValue != 30)
                throw new TestFailedException("retValue: " + retValue);

            counter = 9;
            // This should fails because stream has not beeen closed yet.
            FileConnection tmpConn = null;
            tmpConn = (FileConnection) Connector.open("file:///" + iRoot
                      + "mainTestDir/test020/binaryFile.dat",
                      Connector.READ_WRITE);
            try
            {
                tmpConn.openInputStream();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            tmpConn.close();
            counter = 10;
            // Connection should be closed after calling InputStream.close()
            // operation.
            stream.close();
            try
            {
                stream.available();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 11;

            // Re-opening should be done successfully because all streams are
            // now closed.
            tmpConn = FileTestUtils.openExistsURI("file:///" + iRoot
                                                  + "mainTestDir/test020/binaryFile.dat",
                                                  Connector.READ_WRITE);
            counter = 13;
            if (tmpConn == null)
                throw new TestFailedException();
            tmpConn.close();

            assertTrue(" run_0210() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0210() counter: " + counter + "\n");
            assertTrue(" run_0210() FAILED!!! Counter: "+counter, false);
            return;
        }
        finally
        {
            try
            {
                if (conn1 != null)
                    if (true == conn1.isOpen())
                        conn1.close();
            }
            catch (Exception ex)
            {
                append(ex.toString());
                append("TEST FAILED!!! Case: run_0210() " + counter + "\n");
                assertTrue(" run_0210() FAILED!!! Counter: "+counter, false);
            }
        }//end finally
    }//end run_0210()

}