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

public class TestFile_2530 extends FileTestBase
{
    public TestFile_2530()
    {
    }

    public TestFile_2530(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2530("run_2530", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2530) tc).run_2530();
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

    public void run_2530()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test250/file253.txt", Connector.READ_WRITE);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            OutputStream outStream = conn1.openOutputStream();
            byte[] byteArray1 = new byte[100];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 52;
            }
            counter = 2;
            outStream.write(byteArray1);
            counter = 3;
            try
            {
                conn1.rename("?newFile253");
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 4;
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 52;
            }
            counter = 5;
            outStream.write(byteArray1);
            counter = 51;
            outStream.flush();
            counter = 52;
            outStream.close();
            counter = 53;
            conn1.close();
            counter = 6;

            // Re-open a file to check that data was stored successfully.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test250/file253.txt", Connector.READ_WRITE);
            counter = 7;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 8;
            InputStream stream = conn1.openInputStream();
            counter = 9;
            byte[] byteArray2 = new byte[513];
            int retValue = stream.read(byteArray2);
            if (retValue != 200)
                throw new TestFailedException("retValue: " + retValue);
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray2[i] != 52)
                    throw new TestFailedException();
            }
            counter = 10;
            stream.close();
            conn1.close();
            counter = 11;

            // Test below verifies that InputStream works after failed renaming.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test250/file253b.txt", Connector.READ_WRITE);
            counter = 12;
            if (conn1 == null)
                throw new TestFailedException();
            stream = conn1.openInputStream();
            counter = 13;
            byte[] byteArray3 = new byte[512];
            retValue = stream.read(byteArray3);
            if (retValue != 512)
                throw new TestFailedException("retValue: " + retValue);
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray3[i] != 77)
                    throw new TestFailedException();
            }
            try
            {
                conn1.rename("?newFile253");
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 15;
            retValue = stream.read(byteArray3);
            if (retValue != 188)
                throw new TestFailedException("retValue: " + retValue);
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray3[i] != 77)
                    throw new TestFailedException();
            }
            stream.close();
            conn1.close();

            assertTrue(" run_2530() ok ", true);
            System.out.println(" run_2530() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2530() counter: " + counter + "\n");
            assertTrue(" run_2530() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2530()

}
