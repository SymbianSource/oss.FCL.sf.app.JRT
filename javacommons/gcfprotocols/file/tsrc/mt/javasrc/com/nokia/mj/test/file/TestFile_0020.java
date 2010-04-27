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

public class TestFile_0020 extends FileTestBase
{
    public TestFile_0020()
    {
    }

    public TestFile_0020(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0020("run_0020", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0020) tc).run_0020();
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

    public void run_0020()
    {
        int counter = 0;
        try
        {
            FileTestUtils.createObject("file:///" + iRoot
                                       + "mainTestDir/test000/mytmp.txt", true);
            counter = 1;
            FileTestUtils.endpointExists("file:///" + iRoot
                                         + "mainTestDir/test000/mytmp.txt");
            counter = 2;

            FileTestUtils.endpointExists("file:///" + iRoot + "");
            counter = 3;

            /*
             * CSB: Problem with jniutils. Isnt converting unicode strings
             * properly //Name of the existing file in the human-readable format
             * is 'p��.txt'.
             * endpointExists("file:///"+iRoot+"mainTestDir/test000/p%C3%A4%C3%A4.txt");
             * counter = 4;
             *
             * endpointExists("file:///"+iRoot+"mainTestDir/test000/p��.txt");
             * counter = 5;
             *
             * //URI contains Unicode character what has not been escaped. #2
             * createObject("file:///"+iRoot+"mainTestDir/test000/p��3.txt",true);
             * counter = 6;
             * endpointExists("file:///"+iRoot+"mainTestDir/test000/p��3.txt");
             * counter = 7;
             */
            // //Too long URI before unescaping and stripping.
            // FileTestUtils.createObject(
            // "file:///"+iRoot+"mainTestDir/test000/%31%C3%A434567890123456789012345678901234567890
            // " +
            // "1234567890123456789012345678901234567890 " +
            // "1234567890123456789012345678901234567890 " +
            // "1234567890123456789012345678901234567890 " +
            // "1234567890123456789012345678901234567890 " +
            // "1234567890 12",true);
            //
            // //64 + 41 + 41 + 41 + 41 + 11 = 239
            // FileTestUtils.createObject(
            // "file:///"+iRoot+"mainTestDir/test000/%31%C3%A434567890123456789012345678901234567890
            // " +
            // "1234567890123456789012345678901234567890 " +
            // "1234567890123456789012345678901234567890 " +
            // "1234567890123456789012345678901234567890 " +
            // "1234567890123456789012345678901234567890 " +
            // "12345678901",
            // true);
            // counter = 8;
            // FileTestUtils.endpointExists(
            // "file:///"+iRoot+"mainTestDir/test000/%31%C3%A434567890123456789012345678901234567890
            // " +
            // "1234567890123456789012345678901234567890 " +
            // "1234567890123456789012345678901234567890 " +
            // "1234567890123456789012345678901234567890 " +
            // "1234567890123456789012345678901234567890 " +
            // "12345678901");
            // counter = 9;
            // //Note: This is maximum file name length what can be used with
            // //emulator in 3.0. In device max file name length is 255 but in
            // //emulator also the Windows path preceding the emulator path
            // //is counted (e.g. 'Y:/epoc32/WINSCW/'
            // //64 + 41 + 41 + 41 + 41 + 9 = 237
            // FileTestUtils.createObject(
            // "file:///"+iRoot+"mainTestDir/test000/%31%C3%A434567890123456789012345678901234567890
            // " +
            // "1234567890123456789012345678901234567890 " +
            // "1234567890123456789012345678901234567890 " +
            // "1234567890123456789012345678901234567890 " +
            // "1234567890123456789012345678901234567890 " +
            // "123456789",
            // true);
            // counter = 8;
            // FileTestUtils.endpointExists(
            // "file:///"+iRoot+"mainTestDir/test000/%31%C3%A434567890123456789012345678901234567890
            // " +
            // "1234567890123456789012345678901234567890 " +
            // "1234567890123456789012345678901234567890 " +
            // "1234567890123456789012345678901234567890 " +
            // "1234567890123456789012345678901234567890 " +
            // "123456789");
            counter = 9;

            // Colon replaced with '|' character.
            FileTestUtils.createObject("file:///c|/mainTestDir/temp/", false);
            counter = 10;
            FileTestUtils.endpointExists("file:///c|/mainTestDir/temp/");
            counter = 11;

            // Escaped '|' character.
            FileTestUtils.createObject(
                "file:///c%7C/mainTestDir/test000/temp.txt", true);
            counter = 12;
            FileTestUtils
            .endpointExists("file:///c%7C/mainTestDir/test000/temp.txt");
            counter = 13;

            // Escaped '|' character.
            FileTestUtils.createObject(
                "file:///c%7c/mainTestDir/test000/temp2.txt", true);
            counter = 14;
            FileTestUtils
            .endpointExists("file:///c%7c/mainTestDir/test000/temp2.txt");
            counter = 15;

            // Escaped URI. URI in the unescaped format:
            // file:///C|/mainTestDir/test000/B.txt
            FileTestUtils.createObject(
                "file:///%43%7c/%6D%61%69%6E%54%65%73%74%44%69%72/%74%65%73%74%30%30%30/"
                + "%42%2E%74%78%74", true);
            counter = 16;
            FileTestUtils
            .endpointExists("file:///%43%7c/%6D%61%69%6E%54%65%73%74%44%69%72/%74%65%73%74%30%30%30/"
                            + "%42%2E%74%78%74");
            counter = 17;

            // Take a connection to the dir without trailing slash "/".
            FileTestUtils.endpointExists("file:///" + iRoot
                                         + "mainTestDir/test000/dir 003");
            counter = 18;

            //Take a connection to the file with trailing slash "/".
            FileTestUtils.endpointExists("file:///" + iRoot
                                         + "mainTestDir/test000/file 002.txt/");
            counter = 19;

            //Creating a directory.
            FileTestUtils.createObject("file:///" + iRoot
                                       + "mainTestDir/test000/dir 010", false);
            counter = 20;
            FileTestUtils.endpointExists("file:///" + iRoot
                                         + "mainTestDir/test000/dir 010");
            counter = 21;

            assertTrue(" run_0020() ok", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_0020(): \n counter: " + counter + "\n");
            assertTrue(" run_0020() FAILED!!! Counter: " + counter, false);
        }
    }//end run_0020()

}