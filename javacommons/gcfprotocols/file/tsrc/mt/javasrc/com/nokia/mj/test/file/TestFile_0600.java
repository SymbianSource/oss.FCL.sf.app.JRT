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

public class TestFile_0600 extends FileTestBase
{
    public TestFile_0600()
    {
    }

    public TestFile_0600(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0600("run_0600", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0600) tc).run_0600();
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

    public void run_0600()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            //In this test a new data doesn't exceed EOF mark.
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test060/binaryFile.dat",
                                                Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 1;
            System.out.println(" run_0600: Counter 1");
            OutputStream outStream = conn1.openOutputStream(50);
            counter = 2;
            System.out.println(" run_0600: Counter 2");
            int addedNum = 57;
            for (int i=0; i < 30; ++i)
            {
                outStream.write(addedNum);
            }
            outStream.flush();
            outStream.close();
            conn1.close();
            counter = 3;
            System.out.println(" run_0600: Counter 3");

            //Read just added data.
            //Take a new connection to the 'binaryFile.dat' file.
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test060/binaryFile.dat",
                                                Connector.READ);
            if (conn1 == null)
                throw new TestFailedException();
            byte[] byteArray2 = new byte[200];
            counter = 4;
            System.out.println(" run_0600: Counter 4");
            DataInputStream inStream = conn1.openDataInputStream();
            System.out.println(" run_0600 Before Read");
            int retValue = inStream.read(byteArray2);
            System.out.println(" run_0600 After Read");
            counter = 5;
            if (retValue != 100)
                throw new TestFailedException("retValue: " + retValue);
            counter = 6;
            for (int i=0; i<retValue; ++i)
            {
                if (i < 50)
                {
                    if (byteArray2[i] != 56)
                        throw new TestFailedException("i: " + i + " byteArray2[i]: " +
                                                      + byteArray2[i]);
                }
                else if ((i > 49) && (i < 80))
                {
                    if (byteArray2[i] != 57)
                        throw new TestFailedException("i: " + i + " byteArray2[i]: " +
                                                      + byteArray2[i]);
                }
                else if ((i > 79) && (i < 100))
                {
                    if (byteArray2[i] != 56)
                        throw new TestFailedException("i: " + i + " byteArray2[i]: " +
                                                      + byteArray2[i]);
                }
                else
                    throw new TestFailedException("i: " + i + " byteArray2[i]: " +
                                                  + byteArray2[i]);
            }//end for
            counter = 7;
            conn1.close();
            inStream.close();

            //In this test a new data and offset mark exceed beyond of EOF mark.
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test060/binaryFile2.dat",
                                                Connector.READ_WRITE);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 8;
            outStream = conn1.openOutputStream(101);
            counter = 9;
            addedNum = 51;
            for (int i=0; i < 60; ++i)
            {
                outStream.write(addedNum);
            }
            outStream.flush();
            outStream.close();
            conn1.close();
            counter = 10;

            //Read just added data.
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test060/binaryFile2.dat",
                                                Connector.READ);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 11;
            byte[] byteArray3 = new byte[161];
            inStream = conn1.openDataInputStream();
            retValue = inStream.read(byteArray3);
            counter = 12;
            if (retValue != 160)
                throw new TestFailedException("retValue: " + retValue);
            counter = 13;
            for (int i=0; i<retValue; ++i)
            {
                if (i < 100)
                {
                    if (byteArray3[i] != 50)
                        throw new TestFailedException("i: " + i + " byteArray3[i]: " +
                                                      + byteArray3[i]);
                }
                else
                {
                    if (byteArray3[i] != 51)
                        throw new TestFailedException("i: " + i + " byteArray3[i]: " +
                                                      + byteArray3[i]);
                }
            }//end for
            counter = 14;
            retValue = inStream.read(byteArray3);
            counter = 15;
            if (retValue != -1)
                throw new TestFailedException("retValue: " + retValue);

            conn1.close();
            inStream.close();

            //Set offset to the end of the file.
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test060/binaryFile3.dat",
                                                Connector.READ_WRITE);
            counter = 16;
            if (conn1 == null)
                throw new TestFailedException();
            outStream = conn1.openOutputStream(98);
            counter = 17;
            addedNum = 57;
            for (int i=0; i < 2; ++i)
            {
                outStream.write(addedNum);
            }
            outStream.flush();
            outStream.close();
            conn1.close();
            counter = 18;

            //Read just added data.
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test060/binaryFile3.dat",
                                                Connector.READ);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 19;
            byteArray3 = new byte[161];
            inStream = conn1.openDataInputStream();
            retValue = inStream.read(byteArray3);
            counter = 20;
            if (retValue != 100)
                throw new TestFailedException("retValue: " + retValue);
            counter = 21;
            for (int i=0; i<retValue; ++i)
            {
                if (i < 98)
                {
                    if (byteArray3[i] != 50)
                        throw new TestFailedException("i: " + i + " byteArray3[i]: " +
                                                      + byteArray3[i]);
                }
                else
                {
                    if (byteArray3[i] != 57)
                        throw new TestFailedException("i: " + i + " byteArray3[i]: " +
                                                      + byteArray3[i]);
                }
            }//end for
            counter = 22;
            retValue = inStream.read(byteArray3);
            counter = 23;
            if (retValue != -1)
                throw new TestFailedException("retValue: " + retValue);
            conn1.close();
            inStream.close();

            //Set offset to the start of the file.
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test060/binaryFile4.dat",
                                                Connector.READ_WRITE);
            counter = 24;
            if (conn1 == null)
                throw new TestFailedException();
            outStream = conn1.openOutputStream(0);
            counter = 25;
            addedNum = 57;
            for (int i=0; i < 2; ++i)
            {
                outStream.write(addedNum);
            }
            outStream.flush();
            outStream.close();
            conn1.close();
            counter = 26;

            //Read just added data.
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test060/binaryFile4.dat",
                                                Connector.READ);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 27;
            byteArray3 = new byte[161];
            inStream = conn1.openDataInputStream();
            retValue = inStream.read(byteArray3);
            counter = 28;
            if (retValue != 100)
                throw new TestFailedException("retValue: " + retValue);
            counter = 29;
            for (int i=0; i<retValue; ++i)
            {
                if (i < 2)
                {
                    if (byteArray3[i] != 57)
                        throw new TestFailedException("i: " + i + " byteArray3[i]: " +
                                                      +byteArray3[i]);
                }
                else
                {
                    if (byteArray3[i] != 50)
                        throw new TestFailedException("i: " + i + " byteArray3[i]: " +
                                                      +byteArray3[i]);

                }
            }//end for
            counter = 30;
            retValue = inStream.read(byteArray3);
            counter = 31;
            if (retValue != -1)
                throw new TestFailedException("retValue: " + retValue);
            conn1.close();
            inStream.close();

            assertTrue(" run_0600() ok ", true);
            System.out.println(" run_0600() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0600() counter: " + counter + "\n");
            assertTrue(" run_0600() FAILED!!! Counter: "+counter, false);
        }
    }//end run_0600()

}