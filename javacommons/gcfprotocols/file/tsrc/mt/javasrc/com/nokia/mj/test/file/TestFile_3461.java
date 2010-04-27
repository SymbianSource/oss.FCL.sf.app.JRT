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

public class TestFile_3461 extends FileTestBase
{
    public TestFile_3461()
    {
    }

    public TestFile_3461(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_3461("run_3461", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_3461) tc).run_3461();
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

    public void run_3461()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // 1
            conn1 = (FileConnection)
                    // Connector.open("file:///z:/System/data/MIDP2FILECONNECTION.RSC",
                    // Connector.open("file:///z:/Resource/MIDP2FILECONNECTION.RSC",
                    Connector
                    .open("file:///z:/Resource/SmilPlayer.Rsc", Connector.READ);
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 1;
            conn1.fileSize();
            counter = 111;
            conn1.close();
            // Note: Writing to z: drive is forbidden.

            // 2
            try
            {
                conn1 = (FileConnection) Connector.open(
                            "file:///z:/Private/101f4cce/Phonebook_AIF.mif",
                            Connector.READ);
                conn1.close();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }

            // 3
            try
            {
                counter = 112;
                conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                        + "Private/10003a5b/Calendar", Connector.READ);
                conn1.close();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }

            // 4
            counter = 113;
            conn1 = (FileConnection) Connector.open(
                        "file:///d:/System/temp/total new file.txt",
                        Connector.READ_WRITE);
            counter = 2;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.create();
            counter = 3;
            OutputStream outStream = conn1.openOutputStream();
            counter = 4;
            byte[] byteArray1 = new byte[10];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 75;
            }
            outStream.write(byteArray1);
            counter = 5;
            outStream.flush();
            outStream.close();
            conn1.close();

            // Re-open a file to check that data was written to the file
            // correctly.
            conn1 = (FileConnection) Connector.open(
                        "file:///d:/System/temp/total new file.txt",
                        Connector.READ_WRITE, true);
            counter = 6;
            InputStream inStream = conn1.openInputStream();
            counter = 7;
            byte[] byteArray2 = new byte[15];
            int retValue = inStream.read(byteArray2);
            counter = 8;
            if (retValue != 10)
                throw new TestFailedException("retValue: " + retValue);
            counter = 9;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray2[i] != 75)
                    throw new TestFailedException();
            }
            counter = 10;
            inStream.close();
            counter = 11;
            conn1.delete();
            conn1.close();

            assertTrue(" run_3461() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_3461() counter: " + counter + "\n");
            assertTrue(" run_3461() FAILED!!! Counter: " + counter, false);
        }
    }//end run_3461()

}
