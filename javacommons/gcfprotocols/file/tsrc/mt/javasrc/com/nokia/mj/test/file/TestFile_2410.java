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

public class TestFile_2410 extends FileTestBase
{
    public TestFile_2410()
    {
    }

    public TestFile_2410(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2410("run_2410", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2410) tc).run_2410();
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

    public void run_2410()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test220/newFile240_2", Connector.READ_WRITE);
            if (conn1.exists())
                throw new TestFailedException();
            counter = 1;
            conn1.create();
            OutputStream outStream = conn1.openOutputStream();
            byte[] byteArray1 = new byte[300];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 52;
            }
            outStream.write(byteArray1);
            counter = 1;
            conn1.delete();
            counter = 2;
            try
            {
                outStream.write(byteArray1);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 3;
            outStream.close();
            conn1.close();

            // Re-open a file to check that it is really deleted.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test220/newFile240_2", Connector.READ_WRITE);
            counter = 5;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.close();
            assertTrue(" run_2410() ok  ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2410() counter: " + counter + "\n");
            assertTrue(" run_2410() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2410()

}
