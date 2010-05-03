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

public class TestFile_2500 extends FileTestBase
{
    public TestFile_2500()
    {
    }

    public TestFile_2500(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2500("run_2500", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2500) tc).run_2500();
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

    public void run_2500()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test250/temp/", Connector.READ_WRITE);
            counter = 1;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 2;
            conn1.rename("new dir");
            counter = 3;
            String name = conn1.getName();
            if (false == name.equals("new dir/"))
                throw new TestFailedException();
            counter = 4;
            conn1.close();

            // Re-open a directory to check that directory was really re-named
            // on the
            // filesystem.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test250/new dir/", Connector.READ_WRITE);
            counter = 5;
            if (conn1 == null)
                throw new TestFailedException();
            // Re-naming a directory using trailing slash "/" in the end
            // of the file name.
            counter = 6;
            conn1.rename("new dir2/\t");
            name = conn1.getName();
            counter = 7;
            if (false == name.equals("new dir2/"))
                throw new TestFailedException();
            conn1.close();
            counter = 8;

            // Re-open a directory to check that directory was really re-named
            // on the
            // filesystem.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test250/new dir2/", Connector.READ_WRITE);
            counter = 9;
            if (conn1 == null)
                throw new TestFailedException();
            // Re-naming a directory using escaped name. Name is: 'new �ir3/'
            counter = 10;
            conn1.rename("%6E%65%77%20%c3%b6%69%72%33/");
            counter = 11;
            name = conn1.getName();
            if (false == name.equals("new �ir3/"))
                throw new TestFailedException("Name returned: "+name);
            counter = 12;
            conn1.close();

            // Re-open a directory to check that directory was really re-named
            // on the
            // filesystem.
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test250/new �ir3/", Connector.READ_WRITE);
            counter = 13;
            if (conn1 == null)
                throw new TestFailedException();
            conn1.close();
            assertTrue(" run_2500() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2500() counter: " + counter + "\n");
            assertTrue(" run_2500() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2500()

}
