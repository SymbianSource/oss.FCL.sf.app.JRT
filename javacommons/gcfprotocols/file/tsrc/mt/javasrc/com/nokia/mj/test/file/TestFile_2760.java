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

public class TestFile_2760 extends FileTestBase
{
    public TestFile_2760()
    {
    }

    public TestFile_2760(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2760("run_2760", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2760) tc).run_2760();
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

    public void run_2760()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // Checking that connection to the 'restricted' directory success
            // in the Manufacturer domain.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "data/videos/", Connector.READ_WRITE);
            counter = 1;
            conn1.setFileConnection("myDir/");
            counter = 111;
            if (0 != conn1.directorySize(true))
                throw new TestFailedException();
            conn1.close();
            counter = 2;

            // Checking that connection to just created directory works in the
            // 'two connection' case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test270/test276Dir/", Connector.READ_WRITE);
            counter = 3;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test270/test276Dir/", Connector.READ_WRITE);
            counter = 4;
            conn1.mkdir();
            counter = 5;
            conn1.close();
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test270/test276Dir/subDir/",
                                                    Connector.READ_WRITE);
            counter = 6;
            conn1.mkdir();
            counter = 7;
            conn2.setFileConnection("subDir/");
            counter = 8;
            if (0 != conn2.directorySize(true))
                throw new TestFailedException();
            conn1.close();
            conn2.close();
            counter = 9;

            // Checking that connection to just created file works in the
            // 'two connection' case.
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test270/test276Dir/newFile 276",
                                                    Connector.READ_WRITE);
            counter = 10;
            conn2 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test270/test276Dir/", Connector.READ_WRITE);
            counter = 11;
            conn1.create();
            counter = 12;
            conn1.close();
            counter = 13;
            conn2.setFileConnection("newFile 276");
            counter = 14;
            if (0 != conn2.fileSize())
                throw new TestFailedException();
            counter = 15;
            InputStream stream = conn2.openInputStream();
            conn2.close();
            stream.close();
            counter = 16;
            // MSA requires that setFileConnection does not
            // unescape file names so that connection to
            // file named "My%20Doc.txt" succeeds.
            // Note that this is NOT the same name as "My Doc.txt"
            // JcfFileAPI.exe has created the file
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test270/", Connector.READ_WRITE);
            counter = 17;
            conn1.setFileConnection("My%20Doc.txt");
            counter = 18;
            conn1.close();

            assertTrue(" run_2760() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2760() counter: " + counter + "\n");
            assertTrue(" run_2760() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2760()

}
