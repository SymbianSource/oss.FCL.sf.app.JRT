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

public class TestFile_2520 extends FileTestBase
{
    public TestFile_2520()
    {
    }

    public TestFile_2520(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2520("run_2520", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2520) tc).run_2520();
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

    public void run_2520()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            //"Connection closed" error case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test100/",
                                   Connector.READ_WRITE);
            counter = 1;
            if (!conn1.exists())
                throw new TestFailedException();
            conn1.close();
            try
            {
                conn1.rename("newTest100");
                throw new TestFailedException();
            }
            catch (ConnectionClosedException ex) {}
            counter = 2;
            conn1.close();
            counter = 3;

            //"Connection has been opened in the READ mode" error case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test100/",
                                   Connector.READ);
            counter = 4;
            if (!conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.rename("newTest2100");
                throw new TestFailedException();
            }
            catch (IllegalModeException ex) {}
            counter = 5;
            conn1.close();

            //"Null pointer argument" error case.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test100/",
                                   Connector.READ_WRITE);
            counter = 6;
            if (!conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.rename(null);
                throw new TestFailedException();
            }
            catch (NullPointerException ex) {}
            counter = 7;
            conn1.close();

            //"IllegalArgumentException" error cases.
            //Directory cases.
            counter = 8;
            FileTestUtils.illegalArgumentFromRenaming("test/newDir",
                    "file:///"+iRoot+"mainTestDir/test100/");
            counter = 9;
            FileTestUtils.illegalArgumentFromRenaming("test%2FnewDir",
                    "file:///"+iRoot+"mainTestDir/test100/");
            counter = 10;
            FileTestUtils.illegalArgumentFromRenaming("testnewDir/%2F",
                    "file:///"+iRoot+"mainTestDir/test100/");
            counter = 11;
            FileTestUtils.illegalArgumentFromRenaming("test\\newDir",
                    "file:///"+iRoot+"mainTestDir/test100/");
            //File cases.
            counter = 12;
            FileTestUtils.illegalArgumentFromRenaming("newFile03/",
                    "file:///"+iRoot+"mainTestDir/test100/file100.txt");
            counter = 13;
            FileTestUtils.illegalArgumentFromRenaming("newFile03%2F",
                    "file:///"+iRoot+"mainTestDir/test100/file100.txt");
            counter = 14;
            FileTestUtils.illegalArgumentFromRenaming("newFile03\\",
                    "file:///"+iRoot+"mainTestDir/test100/file100.txt");
            counter = 15;
            FileTestUtils.illegalArgumentFromRenaming("aNewFil\\e",
                    "file:///"+iRoot+"mainTestDir/test100/file100.txt");

            //"IOException" error cases.
            //Directory cases.
            counter = 16;
            FileTestUtils.IOExceptionFromRenaming("?newDir",
                                                  "file:///"+iRoot+"mainTestDir/test100/");
            counter = 17;
            FileTestUtils.IOExceptionFromRenaming("newDir<",
                                                  "file:///"+iRoot+"mainTestDir/test100/");
            counter = 18;
            FileTestUtils.IOExceptionFromRenaming("newD|ir",
                                                  "file:///"+iRoot+"mainTestDir/test100/");
            //A new directory already exists.
            counter = 19;
            FileTestUtils.IOExceptionFromRenaming("test101",
                                                  "file:///"+iRoot+"mainTestDir/test100/");
            //File cases.
            counter = 20;
            FileTestUtils.IOExceptionFromRenaming("?newFile",
                                                  "file:///"+iRoot+"mainTestDir/test100/file100.txt");
            counter = 21;
            FileTestUtils.IOExceptionFromRenaming("newFile<",
                                                  "file:///"+iRoot+"mainTestDir/test100/file100.txt");
            counter = 22;
            FileTestUtils.IOExceptionFromRenaming("newFi|le",
                                                  "file:///"+iRoot+"mainTestDir/test100/file100.txt");
            //A new file already exists.
            counter = 23;
            FileTestUtils.IOExceptionFromRenaming("file101b.txt",
                                                  "file:///"+iRoot+"mainTestDir/test101/file101.txt");

            //Directory does not exists.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test100sfgf/",
                                   Connector.READ_WRITE);
            counter = 24;
            if (conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.rename("newTest2100");
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 25;
            conn1.close();

            //File does not exists.
            conn1 = (FileConnection)
                    Connector.open("file:///"+iRoot+"mainTestDir/test100/gdfgggr",
                                   Connector.READ_WRITE);
            counter = 26;
            if (conn1.exists())
                throw new TestFailedException();
            try
            {
                conn1.rename("newTest2100");
                throw new TestFailedException();
            }
            catch (IOException ex) {}
            counter = 27;
            conn1.close();

            assertTrue(" run_2520() ok ", true);
            System.out.println(" run_2520() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2520() counter: " + counter + "\n");
            assertTrue(" run_2520() FAILED!!! Counter: "+counter, false);
        }
    }//end run_2520()


}
