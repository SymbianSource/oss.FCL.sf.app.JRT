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

public class TestFile_2510 extends FileTestBase
{
    public TestFile_2510()
    {
    }

    public TestFile_2510(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2510("run_2510", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2510) tc).run_2510();
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

    public void run_2510()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test250/normalFile.txt",
                                                Connector.READ_WRITE);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            OutputStream outStream = conn1.openOutputStream();
            byte[] byteArray1 = new byte[100];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 52;
            }
            outStream.write(byteArray1);
            counter = 2;
            // Open a InputStream.
            InputStream inStream = conn1.openInputStream();
            counter = 3;
            byte[] byteArray12 = new byte[1];
            int retValue = inStream.read(byteArray12);
            if (retValue != 1)
                throw new TestFailedException("retValue: " + retValue);
            counter = 4;
            for (int i = 0; i < retValue; ++i)
            {
                // NOTE: Test case used to check for 54. As it was present before.
                // OutputStream has not called flush. So, old data was expected.
                // Now since we write immediately, it has to be 52.
                if (byteArray12[i] != 52)
                    throw new TestFailedException("i: " + "byteArray12[i]: "
                                                  + byteArray12[i]);
            }
            conn1.rename("new file\n");
            String name = conn1.getName();
            counter = 5;
            if (false == name.equals("new file"))
                throw new TestFailedException();
            counter = 6;
            for (int i = 0; i < byteArray12.length; ++i)
            {
                byteArray12[i] = 52;
            }
            // Writing to the stream have to fail after renaming a file.
            try
            {
                outStream.write(byteArray12);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 7;
            // Reading from the stream have to fail after renaming a file.
            try
            {
                inStream.read(byteArray12);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 8;
            inStream.close();
            outStream.close();

            // Re-opening an InputStream.
            counter = 9;
            inStream = conn1.openInputStream();
            byte[] byteArray123 = new byte[101];
            retValue = inStream.read(byteArray123);
            if (retValue != 100)
                throw new TestFailedException("retValue: " + retValue);
            counter = 10;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray123[i] != 52)
                    throw new TestFailedException("i: " + i
                                                  + " byteArray123[i]: " + byteArray123[i]);
            }// end for
            inStream.close();
            counter = 11;

            // Re-opening an OutputStream.
            outStream = conn1.openOutputStream();
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 53;
            }
            outStream.write(byteArray1);
            outStream.close();
            conn1.close();
            counter = 12;

            // Open a re-named file to check that data was stored successfully.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test250/new file", Connector.READ_WRITE);
            counter = 13;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 14;
            InputStream stream = conn1.openInputStream();
            counter = 15;
            byte[] byteArray2 = new byte[513];
            retValue = stream.read(byteArray2);
            if (retValue != 100)
                throw new TestFailedException("retValue: " + retValue);
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray2[i] != 53)
                    throw new TestFailedException("i: " + i
                                                  + " byteArray2[i]: " + byteArray2[i]);
            }
            stream.close();
            conn1.close();
            counter = 16;

            // Opening a 'new file' file for renaming with DataOutputStream.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test250/new file", Connector.READ_WRITE);
            counter = 17;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 18;
            DataOutputStream dataOutStream = conn1.openDataOutputStream();
            counter = 19;
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 65;
            }
            counter = 20;
            dataOutStream.write(byteArray1);
            counter = 21;
            // Open a InputStream.
            DataInputStream dataInStream = conn1.openDataInputStream();
            retValue = dataInStream.read(byteArray123);
            if (retValue != 100)
                throw new TestFailedException("retValue: " + retValue);
            for (int i = 0; i < retValue; ++i)
            {
                // NOTE: Test case used to check for 53. As it was present before.
                // OutputStream has not called flush. So, old data was expected.
                // Now since we write immediately, it has to be 65.
                if (byteArray123[i] != 65)
                    throw new TestFailedException("i: " + i
                                                  + " byteArray123[i]: " + byteArray123[i]);
            }
            // Renaming a file using escaped file name. File name: aa%bb
            counter = 22;
            conn1.rename("%61%61%25%62%62");
            counter = 23;
            conn1.rename("%61%61%25%62%63");
            try
            {
                dataOutStream.write(byteArray12);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 24;
            // Reading from the stream have to fail after renaming a file.
            try
            {
                dataInStream.read(byteArray12);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 25;
            dataInStream = conn1.openDataInputStream();
            byte[] byteArray4 = new byte[101];
            retValue = dataInStream.read(byteArray4);
            if (retValue != 100)
                throw new TestFailedException("retValue: " + retValue);
            counter = 26;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray4[i] != 65)
                    throw new TestFailedException("i: " + i
                                                  + " byteArray4[i]: " + byteArray4[i]);
            }// end for
            dataInStream.close();
            counter = 27;

            // Re-opening an OutputStream.
            dataOutStream = conn1.openDataOutputStream();
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 66;
            }
            dataOutStream.write(byteArray1);
            dataOutStream.close();
            conn1.close();
            counter = 28;

            // Re-opening a file to check that renaming was done successfully.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test250/aa%25bc", Connector.READ_WRITE);
            counter = 29;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 30;
            dataInStream = conn1.openDataInputStream();
            counter = 31;
            byte[] byteArray5 = new byte[513];
            retValue = dataInStream.read(byteArray5);
            if (retValue != 100)
                throw new TestFailedException();
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray5[i] != 66)
                    throw new TestFailedException("i: " + i
                                                  + " byteArray5[i]: " + byteArray5[i]);
            }
            dataInStream.close();
            conn1.close();
            assertTrue(" run_2510() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2510() counter: " + counter + "\n");
            assertTrue(" run_2510() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2510()

}
