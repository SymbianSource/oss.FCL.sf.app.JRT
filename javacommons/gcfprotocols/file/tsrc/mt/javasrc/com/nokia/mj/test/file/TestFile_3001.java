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

public class TestFile_3001 extends FileTestBase
{
    public TestFile_3001()
    {
    }

    public TestFile_3001(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_3001("run_3001", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_3001) tc).run_3001();
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

    public void run_3001()
    {
        FileSystemRegistry registry = null;
        int counter = 0;
        try
        {
            // Returns the localized names of the root dirs in the
            // same order than listRoots
            String rootNames = System.getProperty("fileconn.dir.roots.names");
            counter = 1;
            Enumeration roots = registry.listRoots();
            counter = 2;
            if (false == roots.hasMoreElements())
                throw new TestFailedException();
            counter = 3;
            if (null == rootNames)
                throw new TestFailedException(
                    "No system property fileconn.dir.roots.names");
            rootNames = rootNames.toLowerCase();
            System.out.println(" File Property returned: "+rootNames);
            counter = 4;
            int counterRoot = 0;
            int indSeparator;
            String name;
            while (roots.hasMoreElements())
            {
                String obj = (String) roots.nextElement();
                System.out.println(" run3001: Name: "+obj);
                obj = obj.toLowerCase();
                if (obj.equals("" + iRoot + ""))
                {
                    ++counterRoot;
                    if (!rootNames.startsWith("internal"))
                        throw new TestFailedException(
                            "Wrong value of fileconn.dir.roots.names 1: "
                            + rootNames);
                    indSeparator = rootNames.indexOf(';');
                    rootNames = rootNames.substring(indSeparator + 1);
                    continue;
                }
                if (obj.equals("internal/"))
                {
                    ++counterRoot;
                    if (!rootNames.startsWith("internal"))
                        throw new TestFailedException(
                            "Wrong value of fileconn.dir.roots.names 2: "
                            + rootNames);
                    indSeparator = rootNames.indexOf(';');
                    rootNames = rootNames.substring(indSeparator + 1);
                    continue;
                }
                if (obj.equals("d:/"))
                {
                    ++counterRoot;
                    if (!rootNames.startsWith("temporary"))
                        throw new TestFailedException(
                            "Wrong value of fileconn.dir.roots.names 3: "
                            + rootNames);
                    indSeparator = rootNames.indexOf(';');
                    rootNames = rootNames.substring(indSeparator + 1);
                    continue;
                }
                if (obj.equals("temporary/"))
                {
                    ++counterRoot;
                    if (!rootNames.startsWith("temporary"))
                        throw new TestFailedException(
                            "Wrong value of fileconn.dir.roots.names 4: "
                            + rootNames);
                    indSeparator = rootNames.indexOf(';');
                    rootNames = rootNames.substring(indSeparator + 1);
                    continue;
                }
                if (obj.equals("e:/"))
                {
                    ++counterRoot;
                    if (!rootNames.startsWith("memory card"))
                        throw new TestFailedException(
                            "Wrong value of fileconn.dir.roots.names 5: "
                            + rootNames);
                    indSeparator = rootNames.indexOf(';');
                    rootNames = rootNames.substring(indSeparator + 1);
                    continue;
                }
                if (obj.equals("memory card/"))
                {
                    ++counterRoot;
                    if (!rootNames.startsWith("memory card"))
                        throw new TestFailedException(
                            "Wrong value of fileconn.dir.roots.names 6: "
                            + rootNames);
                    indSeparator = rootNames.indexOf(';');
                    rootNames = rootNames.substring(indSeparator + 1);
                    continue;
                }
                if (obj.equals("z:/"))
                {
                    ++counterRoot;
                    if (!rootNames.startsWith("rom"))
                        throw new TestFailedException(
                            "Wrong value of fileconn.dir.roots.names 7: "
                            + rootNames);
                    indSeparator = rootNames.indexOf(';');
                    rootNames = rootNames.substring(indSeparator + 1);

                    continue;
                }
                if (obj.equals("rom/"))
                {
                    ++counterRoot;
                    if (!rootNames.startsWith("rom"))
                        throw new TestFailedException(
                            "Wrong value of fileconn.dir.roots.names 8: "
                            + rootNames);
                    continue;
                }

                if (iMultiDriveSupportTests)
                {
                    if (obj.equals("g:/"))
                    {
                        ++counterRoot;
                        if (!rootNames.startsWith("internal g"))
                            throw new TestFailedException(
                                "Wrong value of fileconn.dir.roots.names 9: "
                                + rootNames);
                        indSeparator = rootNames.indexOf(';');
                        rootNames = rootNames.substring(indSeparator + 1);
                        continue;
                    }
                    if (obj.equals("internal g/"))
                    {
                        ++counterRoot;
                        if (!rootNames.startsWith("internal g"))
                            throw new TestFailedException(
                                "Wrong value of fileconn.dir.roots.names 10: "
                                + rootNames);
                        indSeparator = rootNames.indexOf(';');
                        rootNames = rootNames.substring(indSeparator + 1);
                        continue;
                    }
                }

                ++counterRoot;
            }//end while
            counter = 5;
            if (iMultiDriveSupportTests)
            {
                if (10 != counterRoot)
                    throw new TestFailedException("counterRoot: " + counterRoot);
            }
            else
            {
                if (8 != counterRoot)
                    throw new TestFailedException("counterRoot: " + counterRoot);
            }

            assertTrue(" run_3001() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_3001() counter: " + counter + "\n");
            assertTrue(" run_3001() FAILED!!! Counter: " + counter, false);
        }
    }//end run_3001()

}
