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

public class TestFile_2540 extends FileTestBase
{
    public TestFile_2540()
    {
    }

    public TestFile_2540(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2540("run_2540", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2540) tc).run_2540();
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

    public void run_2540()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test250/readOnlyFile", Connector.READ_WRITE);
            counter = 1;
            InputStream stream = conn1.openInputStream();
            counter = 2;
            byte[] byteArray1 = new byte[400];
            int retValue = stream.read(byteArray1);
            counter = 3;
            if (retValue != 400)
                throw new TestFailedException("retValue: " + retValue);
            counter = 4;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 77)
                    throw new TestFailedException("byteArray1[i]: "
                                                  + byteArray1[i] + "i: " + i);
            }
            counter = 5;
            conn1.rename("newReadOnlyFile");
            counter = 6;
            try
            {
                stream.read(byteArray1);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 7;
            long fileSize = conn1.fileSize();
            counter = 71;
            if (fileSize != 1200)
                throw new TestFailedException("fileSize: " + fileSize);
            counter = 72;
            try
            {
                conn1.truncate(4);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 8;
            conn1.close();

            // Re-open a file to check that renaming was done successfully.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test250/newReadOnlyFile",
                                                    Connector.READ_WRITE);
            counter = 9;
            if (!conn1.exists())
                throw new TestFailedException();
            conn1.close();

            // Renaming a directory.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test250/readOnlyDir/", Connector.READ_WRITE);
            counter = 10;
            conn1.rename("NewReadOnlyDir");
            counter = 11;
            conn1.close();

            // Re-open a directory to check that renaming was done successfully.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test250/NewReadOnlyDir/",
                                                    Connector.READ_WRITE);
            counter = 12;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 13;
            conn1.close();

            assertTrue(" run_2540() ok ", true);
            System.out.println(" run_2540() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2540() counter: " + counter + "\n");
            assertTrue(" run_2540() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2540()

}
