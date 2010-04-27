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

public class TestFile_2110 extends FileTestBase
{
    public TestFile_2110()
    {
    }

    public TestFile_2110(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2110("run_2110", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2110) tc).run_2110();
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

    public void run_2110()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            //"Connection points to the existing file" error case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test100/file100.txt",
                                   Connector.READ_WRITE);
            counter = 1;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 111;
            try
            {
                conn1.mkdir();
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 2;
            conn1.close();

            //"Connection points to the existing file" error case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test100/file100.txt",
                                   Connector.READ_WRITE);
            counter = 21;
            InputStream stream = conn1.openInputStream();
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 2111;
            try
            {
                conn1.mkdir();
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 22;
            conn1.close();
            stream.close();

            //"Connection has been invoked to the file" error case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test000/file007.txt",
                                   Connector.READ_WRITE);
            if (!conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.mkdir();
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 3;
            conn1.close();

            //"Connection has been opened in the READ mode" error case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test200/newfile5.txt",
                                   Connector.READ);
            if (conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.mkdir();
                throw new TestFailedException();
            }
            catch (IllegalModeException ex) {}
            counter = 4;
            conn1.close();

            //"Connection closed" error case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test200/newfile15.txt",
                                   Connector.READ);
            if (conn1.exists())
                throw new TestFailedException();
            conn1.close();
            try
            {
                conn1.mkdir();
                throw new TestFailedException();
            }
            catch (ConnectionClosedException ex) {}
            counter = 5;
            conn1.close();

            //URI contains more than one non-created directory.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test200/newDir/newDir2/",
                                   Connector.READ_WRITE);
            counter = 6;
            if (conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.mkdir();
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 7;
            conn1.close();

            //READ_WRITE connection established to the read-only directory.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test200/readonlyDir/",
                                   Connector.READ_WRITE);
            counter = 9;
            try
            {
                conn1.mkdir();
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 20;
            conn1.close();

            //Calling mkdir() operation when InputStream is opened to the
            //file by other connection.
            conn1 = (FileConnection)Connector.open
                    ("file:///"+iRoot+"data/Images/myDir/myImages2/myImagesFile.txt");
            counter = 21;
            stream = conn1.openInputStream();
            counter = 22;
            FileConnection conn2 = (FileConnection)Connector.open
                                   ("file:///"+iRoot+"data/Images/myDir/myImages2/myImagesFile.txt");
            counter = 23;
            try
            {
                conn2.mkdir();
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            stream.close();
            counter = 24;
            conn2.close();
            conn1.close();

            assertTrue(" run_2110() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2110() counter: " + counter + "\n");
            assertTrue(" run_2110() FAILED!!! Counter: "+counter, false);
        }
    }//end run_2110()
}
