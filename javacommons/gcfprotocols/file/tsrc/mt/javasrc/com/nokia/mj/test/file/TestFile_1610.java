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

public class TestFile_1610 extends FileTestBase
{
    public TestFile_1610()
    {
    }

    public TestFile_1610(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1610("run_1610", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1610) tc).run_1610();
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

    public void run_1610()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test160/writable2.txt",
                                                Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 1;
            DataOutputStream dataStream = conn1.openDataOutputStream();
            counter = 11;
            int addedNum = 57;
            for (int i=0; i < 1001; ++i)
            {
                dataStream.write(addedNum);
            }
            counter = 12;
            //Writing is not possible after setting file a read-only state
            //in the Symbian filesystem. In Windows this is possible.
            //conn1.setWritable(false);
            for (int i=0; i < 600; ++i)
            {
                dataStream.write(addedNum);
            }
            dataStream.flush();
            dataStream.close();
            conn1.setWritable(false);
            conn1.close();
            counter = 2;

            //Checking that data was stored successfully.
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test160/writable2.txt",
                                                Connector.READ);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 3;
            if (true == conn1.canWrite())
                throw new TestFailedException("conn1.canWrite(): " + conn1.canWrite());
            InputStream stream = conn1.openInputStream();
            byte[] byteArray1 = new byte[1603];
            int retValue = stream.read(byteArray1);
            counter = 4;
            if (retValue != 1601)
                throw new TestFailedException("retValue: " + retValue);
            counter = 5;
            for (int i=0; i<retValue; ++i)
            {
                if (byteArray1[i] != 57)
                    throw new TestFailedException();
            }
            stream.close();
            conn1.close();
            assertTrue(" run_1610() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1610() counter: " + counter + "\n");
            assertTrue(" run_1610() FAILED!!! Counter: "+counter, false);
        }
    }//end run_1610()

}
