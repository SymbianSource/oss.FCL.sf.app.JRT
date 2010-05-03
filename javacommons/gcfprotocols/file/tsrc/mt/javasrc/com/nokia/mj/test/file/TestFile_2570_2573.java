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

public class TestFile_2570_2573 extends FileTestBase
{
    public TestFile_2570_2573()
    {
    }

    public TestFile_2570_2573(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2570_2573("run_2570", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2570_2573) tc).run_2570();
            }
        }));

        suite.addTest(new TestFile_2570_2573("run_2573", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2570_2573) tc).run_2573();
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

    public void run_2570()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Calling rename() operation when InputStream is opened to the
            // file by other connection.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "data/Images/myDir/myImages2/myImagesFile6.txt");
            counter = 1;
            conn1.create();
            counter = 2;
            InputStream stream = conn1.openInputStream();
            counter = 3;
            FileConnection conn2 = (FileConnection) Connector.open("file:///"
                                   + iRoot + "data/Images/myDir/myImages2/myImagesFile6.txt");
            counter = 4;
            try
            {
                conn2.rename("myImagesFile4.txt");
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 5;
            stream.close();
            conn2.rename("myImagesFile4.txt");
            counter = 6;
            conn2.delete();
            conn1.close();
            conn2.close();

            assertTrue(" run_2570() ok ", true);
            System.out.println(" run_2570() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2570() counter: " + counter + "\n");
            assertTrue(" run_2570() FAILED!!! Counter: "+counter, false);
        }
    }// end run_2570()

    //
    //
    //
    //
    public void run_2573()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // This test verifies that streams cannot be used after renaming.
            // Note: Throught a OutputStream has not been written anything
            // before
            // renaming.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "data/Images/myDir/myImages2/myImagesFile10.txt");
            counter = 1;
            conn1.create();
            counter = 2;
            OutputStream out = conn1.openOutputStream();
            counter = 3;
            conn1.rename("myImagesFile10New.txt");
            counter = 4;
            try
            {
                out.write(1);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 5;
            // out.close();
            OutputStream out2 = conn1.openOutputStream();
            counter = 6;
            out2.write(1);
            counter = 7;
            out2.flush();
            // Below are checked that byte was written to the file.
            InputStream in = conn1.openInputStream();
            counter = 8;
            byte[] byteArray1 = new byte[2];
            int retValue = in.read(byteArray1);
            counter = 9;
            if (retValue != 1)
                throw new TestFailedException("retValue: " + retValue);
            counter = 10;
            if (byteArray1[0] != 1)
                throw new TestFailedException("byteArray1[0]: " + byteArray1[0]);
            conn1.close();
            out2.close();
            in.close();
            counter = 11;

            // Same test as above to the InputStream.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "data/Images/myDir/myImages2/myImagesFile10New.txt");
            counter = 12;
            in = conn1.openInputStream();
            counter = 14;
            conn1.rename("myImagesFile20New.txt");
            counter = 15;
            try
            {
                in.read(byteArray1);
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            // in.close();
            counter = 16;
            InputStream in2 = conn1.openInputStream();
            counter = 17;
            byteArray1 = new byte[2];
            retValue = in2.read(byteArray1);
            counter = 18;
            if (retValue != 1)
                throw new TestFailedException("retValue: " + retValue);
            counter = 19;
            if (byteArray1[0] != 1)
                throw new TestFailedException("byteArray1[0]: " + byteArray1[0]);
            conn1.delete();
            in2.close();
            conn1.close();

            assertTrue(" run_2573() ok ", true);
            System.out.println(" run_2573() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2573() counter: " + counter + "\n");
            assertTrue(" run_2573() FAILED!!! Counter: "+counter, false);
        }
    }//end run_2573()

}
