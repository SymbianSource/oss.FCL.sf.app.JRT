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

public class TestFile_3300 extends FileTestBase
{
    public TestFile_3300()
    {
    }

    public TestFile_3300(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_3300("run_3300", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_3300) tc).run_3300();
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

    public void run_3300()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 =
                FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test330/readFile.txt",
                                            Connector.READ_WRITE);
            counter = 1;
            if (null == conn1)
                throw new TestFailedException();
            counter = 2;
            InputStream inStream = conn1.openInputStream();
            counter = 3;
            if (!inStream.markSupported())
                throw new TestFailedException();
            counter = 31;
            counter = 4;
            byte[] byteArray = new byte[15];
            inStream.mark(10);
            counter = 5;
            int retValue = inStream.read(byteArray,0,2);
            counter = 6;
            if (2 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            for (int i=0; i<retValue; ++i)
            {
                if (byteArray[i] != 75)
                    throw new TestFailedException("byteArray[i]: " + byteArray[i] +
                                                  " i: " + i);
            }
            counter = 7;
            //iThread.sleep(1000);
            //numOfBytes = inStream.available();
            //if(numOfBytes != 10)
            //  throw new TestFailedException("numOfBytes: " + numOfBytes);
            retValue = inStream.read(byteArray,0,8);
            if (8 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            for (int i=0; i<retValue; ++i)
            {
                if (byteArray[i] != 75)
                    throw new TestFailedException("byteArray[i]: " + byteArray[i] +
                                                  " i: " + i);
            }
            //numOfBytes = inStream.available();
            //if(numOfBytes != 0)
            //  throw new TestFailedException("numOfBytes: " + numOfBytes);
            counter = 8;
            inStream.reset();
            counter = 9;
            retValue = inStream.read(byteArray,0,2);
            counter = 10;
            if (2 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            for (int i=0; i<retValue; ++i)
            {
                if (byteArray[i] != 75)
                    throw new TestFailedException("byteArray[i]: " + byteArray[i] +
                                                  " i: " + i);
            }
            counter = 11;
            retValue = inStream.read(byteArray,0,8);
            counter = 12;
            if (8 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            for (int i=0; i<retValue; ++i)
            {
                if (byteArray[i] != 75)
                    throw new TestFailedException("byteArray[i]: " + byteArray[i] +
                                                  " i: " + i);
            }
            counter = 13;
            retValue = inStream.read(byteArray,0,10);
            counter = 14;
            if (10 != retValue)
                throw new TestFailedException("retValue: " + retValue);
            for (int i=0; i<retValue; ++i)
            {
                if (byteArray[i] != 76)
                    throw new TestFailedException("byteArray[i]: " + byteArray[i] +
                                                  " i: " + i);
            }
            counter = 15;
            inStream.close();
            conn1.close();

            assertTrue(" run_3300() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_3300() counter: " + counter + "\n");
            assertTrue(" run_3300() FAILED!!! Counter: "+counter, false);
        }
    }//end run_3300()

}
