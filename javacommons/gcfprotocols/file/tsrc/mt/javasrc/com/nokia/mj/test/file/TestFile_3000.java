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

public class TestFile_3000 extends FileTestBase
{
    public TestFile_3000()
    {
    }

    public TestFile_3000(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_3000("run_3000", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_3000) tc).run_3000();
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

    public void run_3000()
    {
        FileSystemRegistry registry = null;
        int counter = 0;
        try
        {
            Enumeration roots = registry.listRoots();
            counter = 1;
            if (false == roots.hasMoreElements())
                throw new TestFailedException();
            counter = 2;
            int counterRoot = 0;
            while (roots.hasMoreElements())
            {
                String obj = (String) roots.nextElement();
                // append(obj + "\n");
                obj = obj.toLowerCase();
                if (obj.equals("" + iRoot + ""))
                {
                    ++counterRoot;
                    continue;
                }
                if (obj.equals("internal/"))
                {
                    ++counterRoot;
                    continue;
                }
                if (obj.equals("d:/"))
                {
                    ++counterRoot;
                    continue;
                }
                if (obj.equals("temporary/"))
                {
                    ++counterRoot;
                    continue;
                }
                if (obj.equals("e:/"))
                {
                    ++counterRoot;
                    continue;
                }
                if (obj.equals("memory card/"))
                {
                    ++counterRoot;
                    continue;
                }
                if (obj.equals("z:/"))
                {
                    ++counterRoot;
                    continue;
                }
                if (obj.equals("rom/"))
                {
                    ++counterRoot;
                    continue;
                }
                ++counterRoot;
            }// end while
            counter = 3;
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

            assertTrue(" run_3000() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_3000() counter: " + counter + "\n");
            assertTrue(" run_3000() FAILED!!! Counter: "+counter, false);
        }
    }//end run_3000()

}
