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
 * Simultaneous connections case. Expects truncate to throw IOException when
 * file is open by another connection.
 */
public class TestFile_2650 extends FileTestBase
{
    public TestFile_2650()
    {
    }

    public TestFile_2650(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2650("run_2650", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2650) tc).run_2650();
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

    public void run_2650()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // Truncating using two connections.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test260/newFile265", Connector.READ_WRITE);
            counter = 1;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test260/newFile265", Connector.READ_WRITE);
            counter = 2;
            conn2.create();
            counter = 3;
            OutputStream outStream = conn2.openOutputStream();
            counter = 4;
            byte[] byteArray1 = new byte[1000];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 75;
            }
            outStream.write(byteArray1);
            outStream.flush();
            counter = 5;
            try
            {
                conn1.truncate(8);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            outStream.close();
            counter = 51;
            conn1.truncate(8);
            counter = 6;
            InputStream inStream = conn2.openInputStream();
            inStream.close();
            counter = 61;
            inStream = conn1.openInputStream();
            byte[] byteArray2 = new byte[15];
            counter = 7;
            int retValue = inStream.read(byteArray2);
            counter = 8;
            if (retValue != 8)
                throw new TestFailedException("retValue: " + retValue);
            conn1.close();
            conn2.close();
            inStream.close();

            // Trying to truncate in the case where other connection object has
            // deleted file from the filesystem.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test260/newFile265_2", Connector.READ_WRITE);
            counter = 9;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test260/newFile265_2", Connector.READ_WRITE);
            counter = 10;
            conn2.create();
            counter = 100;
            outStream = conn1.openOutputStream();
            counter = 11;
            byteArray1 = new byte[10];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 75;
            }
            outStream.write(byteArray1);
            counter = 12;
            outStream.flush();
            outStream.close();
            counter = 13;
            conn2.delete();
            counter = 14;
            try
            {
                conn1.truncate(3);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 15;
            conn1.close();
            conn2.close();

            assertTrue(" run_2650() ok ", true);
            System.out.println(" run_2650() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2650() counter: " + counter + "\n");
            assertTrue(" run_2650() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2650()

}
