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

public class TestFile_2010 extends FileTestBase
{
    public TestFile_2010()
    {
    }

    public TestFile_2010(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2010("run_2010", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2010) tc).run_2010();
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

    public void run_2010()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            //"Connection points to the existing directory" error case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test100/",
                                   Connector.READ_WRITE);
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 1;
            try
            {
                conn1.create();
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 2;
            conn1.close();

            //"A new URI ends to the trailing slash'/'" error case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test200/newFile10/",
                                   Connector.READ_WRITE);
            counter = 4;
            if (conn1.exists())
                throw new TestFailedException();
            counter = 41;
            try
            {
                conn1.create();
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 5;
            conn1.close();

            //"File already exists" error case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test000/file007.txt",
                                   Connector.READ_WRITE);
            counter = 7;
            InputStream stream = conn1.openInputStream();
            if (!conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.create();
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 8;
            conn1.close();
            stream.close();

            //"Connection has been opened in the READ mode" error case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test200/newfile5.txt",
                                   Connector.READ);
            counter = 10;
            if (conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.create();
                throw new TestFailedException();
            }
            catch (IllegalModeException ex) {}
            counter = 11;
            conn1.close();

            //"Connection closed" error case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test200/newfile15.txt",
                                   Connector.READ);
            counter = 12;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.close();
            try
            {
                conn1.create();
                throw new TestFailedException();
            }
            catch (ConnectionClosedException ex) {}
            counter = 13;
            conn1.close();

            //URI contains more than one non-created directory.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test200/newDir/newFile",
                                   Connector.READ_WRITE);
            counter = 15;
            if (conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.create();
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 16;
            conn1.close();

            //READ_WRITE connection established to the read-only file.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test020/binaryFile2.dat",
                                   Connector.READ_WRITE);
            counter = 18;
            try
            {
                conn1.create();
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 19;
            conn1.close();

            assertTrue(" run_2010() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2010() counter: " + counter + "\n");
            assertTrue(" run_2010() FAILED!!! Counter: "+counter, false);
        }
    }//end run_2010()

}
