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

public class TestFile_2800_2801 extends FileTestBase
{
    public TestFile_2800_2801()
    {
    }

    public TestFile_2800_2801(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_2800_2801("run_2800", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_2800_2801) tc).run_2800();
            }
        }));

//        suite.addTest(new TestFile_2800_2801("run_2801", new TestMethod() {
//            public void run(TestCase tc)
//            {
//                ((TestFile_2800_2801) tc).run_2801();
//            }
//        }));
    }


    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());
        registerTests(suite);
        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    private void testGetConnectionMethods(String aEndpoint, String aName,
                                          String aPath, String aUri) throws IOException
    {
        FileConnection conn1 = null;
        conn1 = (FileConnection) Connector.open(aEndpoint, Connector.READ);
        String name = conn1.getName();
        if (false == name.equals(aName))
            throw new TestFailedException("Name: " + name + " Expected: "
                                          + aName);
        String path = conn1.getPath();
        if (false == path.equals(aPath))
            throw new TestFailedException("path: " + path + " Expected: "
                                          + aPath);
        String uri = conn1.getURL();
        if (false == uri.equals(aUri))
            throw new TestFailedException("uri: " + uri + " Expected: " + aUri);
        conn1.close();
    }

    //
    //
    //
    //
    public void run_2800()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            counter = 1;
            testGetConnectionMethods("file:///" + iRoot + "", "", "/" + iRoot
                                     + "", "file:///" + iRoot + "");
            counter = 2;
            testGetConnectionMethods("file:///" + iRoot + "mainTestDir/tmp",
                                     "tmp", "/" + iRoot + "mainTestDir/", "file:///" + iRoot
                                     + "mainTestDir/tmp");
//            counter = 3;
//            testGetConnectionMethods("file:///d:/system/temp", "temp/",
//                    "/d:/system/", "file:///d:/system/temp/");

            counter = 4;
            testGetConnectionMethods("file:///" + iRoot
                                     + "mainTestDir/test280/new dir/file.txt", "file.txt", "/"
                                     + iRoot + "mainTestDir/test280/new dir/", "file:///"
                                     + iRoot + "mainTestDir/test280/new%20dir/file.txt");
            counter = 5;
            testGetConnectionMethods("file:///" + iRoot
                                     + "mainTestDir/test280/te%20mp/some file", "some file", "/"
                                     + iRoot + "mainTestDir/test280/te mp/", "file:///" + iRoot
                                     + "mainTestDir/test280/te%20mp/some%20file");
            counter = 6;
            testGetConnectionMethods("file:///" + iRoot
                                     + "mainTestDir/test280/te mp/tääfile", "tääfile",
                                     "/" + iRoot + "mainTestDir/test280/te mp/", "file:///"
                                     + iRoot
                                     + "mainTestDir/test280/te%20mp/t%c3%a4%c3%a4file");
            counter = 7;
            testGetConnectionMethods("file:///" + iRoot
                                     + "mainTestDir/test280/te mp/täödir/", "täödir/",
                                     "/" + iRoot + "mainTestDir/test280/te mp/", "file:///"
                                     + iRoot
                                     + "mainTestDir/test280/te%20mp/t%c3%a4%c3%b6dir/");
            counter = 8;

            assertTrue(" run_2800() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2800() counter: " + counter + "\n");
            assertTrue(" run_2800() FAILED!!! Counter: "+counter, false);
        }
    }// end run_2800()

    //
    //
    //
    //
    public void run_2801()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // 1
            testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecDisplayCount50.dcf?drm=dec+display",
                "drmfile024DecDisplayCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecDisplayCount50.dcf?drm=dec+display");
            counter = 1;
            testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPlayCount50.dcf?drm=dec+play",
                "drmfile024DecPlayCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPlayCount50.dcf?drm=dec+play");
            counter = 2;
            testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPrintCount50.dcf?drm=dec+print",
                "drmfile024DecPrintCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPrintCount50.dcf?drm=dec+print");
            counter = 3;
            testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecExecuteCount50.dcf?drm=dec+execute",
                "drmfile024DecExecuteCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecExecuteCount50.dcf?drm=dec+execute");
            counter = 4;
            testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecDisplayCount50.dcf?drm=dec+display+preview",
                "drmfile024DecDisplayCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecDisplayCount50.dcf?drm=dec+display+preview");
            counter = 5;
            testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPlayCount50.dcf?drm=dec+play+preview",
                "drmfile024DecPlayCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPlayCount50.dcf?drm=dec+play+preview");
            counter = 6;
            testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPrintCount50.dcf?drm=dec+print+preview",
                "drmfile024DecPrintCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPrintCount50.dcf?drm=dec+print+preview");
            counter = 7;
            testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecExecuteCount50.dcf?drm=dec+execute+preview",
                "drmfile024DecExecuteCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecExecuteCount50.dcf?drm=dec+execute+preview");
            counter = 8;
            testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPlayCount50.dcf?drm=enc",
                "drmfile024DecPlayCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPlayCount50.dcf?drm=enc");
            counter = 9;
            testGetConnectionMethods(
                "file:///Internal/Videos/drmFiles/drmfile024DecPrintCount50.dcf?drm=dec+prInt ",
                "drmfile024DecPrintCount50.dcf",
                "/Internal/Videos/drmFiles/",
                "file:///Internal/Videos/drmFiles/drmfile024DecPrintCount50.dcf?drm=dec+prInt%20");
            counter = 11;
            testGetConnectionMethods(
                "file:///Internal/Videos/drmFiles/drmfile024DecPrintCount50.dcf?foo=bar;drm=dec+prInt ",
                "drmfile024DecPrintCount50.dcf",
                "/Internal/Videos/drmFiles/",
                "file:///Internal/Videos/drmFiles/drmfile024DecPrintCount50.dcf?foo=bar;drm=dec+prInt%20");
            counter = 12;
            testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPrintCount50.dcf?foo = bar;drm=dec+prInt;foo = bar",
                "drmfile024DecPrintCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPrintCount50.dcf?foo%20=%20bar;drm=dec+prInt;foo%20=%20bar");
            counter = 13;
            testGetConnectionMethods(
                "file:///Memory card/drmfile025DecPrintCount50.dcf?foo = bar;drm=dec+prInt;foo = bar",
                "drmfile025DecPrintCount50.dcf",
                "/Memory card/",
                "file:///Memory%20card/drmfile025DecPrintCount50.dcf?foo%20=%20bar;drm=dec+prInt;foo%20=%20bar");
            counter = 14;
            testGetConnectionMethods(
                "file:///e:/drmfile025DecPrintCount50.dcf?drm=dEc+prInt+preview;foo = bar",
                "drmfile025DecPrintCount50.dcf", "/e:/",
                "file:///e:/drmfile025DecPrintCount50.dcf?drm=dEc+prInt+preview;foo%20=%20bar");
            counter = 15;

            assertTrue(" run_2801() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2801() counter: " + counter + "\n");
            assertTrue(" run_2801() FAILED!!! Counter: "+counter, false);
        }
    }//end run_2801()

}
