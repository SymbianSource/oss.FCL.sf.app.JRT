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

import java.io.IOException;
import javax.microedition.io.file.*;
import javax.microedition.io.*;

import com.nokia.mj.test.file.utils.FileTestUtils;
import com.nokia.mj.test.file.utils.FileTestBase;
import com.nokia.mj.impl.installer.utils.InstallerMain;

public class TestFile_0010 extends FileTestBase
{
    public TestFile_0010()
    {
    }

    public TestFile_0010(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0010("run_0010", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0010) tc).run_0010();
            }
        }));
    }

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        registerTests(suite);

        // suite.addTest(new TestFile_0010("run_0010", new TestMethod() {
        // public void run(TestCase tc)
        // {
        // ((TestFile_0010) tc).run_0010();
        // }
        // }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public void run_0010()
    {
        // First is passed correct URI.
        int counter = 0;
        try
        {
            FileTestUtils.IllegalArgumentExceptionCheck("file:///" + iRoot
                    + "mytmp.txt", true);
            counter = 1;

            FileTestUtils.IllegalArgumentExceptionCheck("file://localhost/"
                    + iRoot + "", true);
            counter = 111;

            // Too short URI.
            FileTestUtils.IllegalArgumentExceptionCheck("file:///c:", false);
            counter = 2;

            // Too long URI. CSB: Do we have any such limit now?
            FileTestUtils.IllegalArgumentExceptionCheck("file:///" + iRoot
                    + "1234567890123456789012345678901234567890"
                    + "1234567890123456789012345678901234567890"
                    + "1234567890123456789012345678901234567890"
                    + "1234567890123456789012345678901234567890"
                    + "1234567890123456789012345678901234567890"
                    + "1234567890123456789012345678901234567890"
                    + "12345678901234", false);

            counter = 3;

            // '%' character without hexadecimal number.
            FileTestUtils.IllegalArgumentExceptionCheck("file:///" + iRoot
                    + "mainTestDir/tem%p.txt", false);
            counter = 4;

            // URI contains host.
            FileTestUtils.IllegalArgumentExceptionCheck("file://localhost/"
                    + iRoot + "mainTestDir/temp.txt", true);
            counter = 5;

            // URI contains both escaped and non-escaped unicode characters.
            // CSB: we do not have any such constraint now. Do we need it?
            // FileTestUtils.IllegalArgumentExceptionCheck("file:///"+iRoot+"mainTestDir/test000/p%C3%A4�.txt",false);
            counter = 6;

            // URI contains two '/' character in the middle of the URI.
            FileTestUtils.IllegalArgumentExceptionCheck("file:///" + iRoot
                    + "mainTestDir//test000/", false);
            counter = 7;

            // URI contains two '/' character in the end of the URI.
            // IOException is thorwn is this case.
            FileTestUtils.IllegalArgumentExceptionCheck("file:///" + iRoot
                    + "mainTestDir/test000//", false);
            counter = 8;

            // Non-created directory in the middle of the uri.
            FileTestUtils.IoExceptionCheck("file:///" + iRoot
                                           + "fsgfg/mainTestDir/test000/", Connector.READ_WRITE, true);
            counter = 9;

            // Relative path case.
            FileTestUtils.IllegalArgumentExceptionCheck("file:///" + iRoot
                    + "mainTestDir/../tmp.txt", false);
            counter = 10;

            FileTestUtils.IllegalArgumentExceptionCheck("file:///" + iRoot
                    + "mainTestDir/.", false);
            counter = 11;

            // Only one charater after '%'
            FileTestUtils.IllegalArgumentExceptionCheck("file:///" + iRoot
                    + "mainTestDir/%%", false);
            counter = 12;

            assertTrue(" run_0010() ok", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_0010(): counter: " + counter + "\n");
            assertTrue(" run_0010() FAILED!!! Counter: " + counter, false);
        }
    }//end run_0010()
}