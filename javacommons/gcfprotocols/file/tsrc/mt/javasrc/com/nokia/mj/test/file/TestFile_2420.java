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

public class TestFile_2420 extends FileTestBase
{
    public TestFile_2420()
    {
    }

    public TestFile_2420(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2420("run_2420", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2420) tc).run_2420();
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

    public void run_2420()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test240/file", Connector.READ_WRITE);
            counter = 1;
            if (!conn1.exists())
                throw new TestFailedException();
            InputStream stream = conn1.openInputStream();
            counter = 2;
            byte[] byteArray1 = new byte[600];
            int retValue = stream.read(byteArray1);
            counter = 3;
            if (retValue != 600)
                throw new TestFailedException("retValue: " + retValue);
            counter = 4;
            conn1.delete();
            counter = 5;
            try
            {
                retValue = stream.read(byteArray1);
                throw new TestFailedException("retValue: " + retValue);
            }
            catch (IOException ex)
            {
            }
            counter = 6;
            stream.close();
            conn1.close();

            // Re-open a file to check that it is really deleted.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test220/newFile240_2", Connector.READ_WRITE);
            counter = 8;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.close();

            // Calling delete() operation when InputStream is opened to the
            // file by other connection.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "data/Images/myDir/myImages2/myImagesFile5.txt");
            counter = 9;
            conn1.create();
            counter = 10;
            stream = conn1.openInputStream();
            counter = 11;
            FileConnection conn2 = (FileConnection) Connector.open("file:///"
                                   + iRoot + "data/Images/myDir/myImages2/myImagesFile5.txt");
            counter = 12;
            try
            {
                conn2.delete();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 13;
            stream.close();
            conn2.delete();
            counter = 14;
            conn1.create();
            counter = 16;
            conn2.delete();
            counter = 17;
            conn1.close();
            conn2.close();

            assertTrue(" run_2420() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2420() counter: " + counter + "\n");
            assertTrue(" run_2420() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2420()

}
