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


package com.nokia.mj.test.fileutils;

import java.io.IOException;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.fileutils.*;
import com.nokia.mj.test.fileutils.TestEnvironment;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * Tests FileUtility.createNewFile(). Ensure that you run after exists. Test
 * case assumes that exists() works fine.
 */
public class TestFileCreate extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestFileCreate("testFileCreate", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileCreate) tc).testFileCreate();
            }
        }));

        suite.addTest(new TestFileCreate("testFileCreateFailure",
                                         new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileCreate) tc).testFileCreateFailure();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestFileCreate()
    {
    }

    public TestFileCreate(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    // End j2meunit test framework setup

    protected void setUp()
    {
        path = TestEnvironment.Main_Test_Folder + "testcreate/";
    }

    public void testFileCreate()
    {
        String name = "FileUtilityCreateTest0";
        FileUtility target = new FileUtility(path + name);
        boolean ret = false;
        try
        {
            ret = target.createNewFile();
        }
        catch (Exception e)
        {
            assertTrue("Unable to create file: " + path + name, false);
        }

        assertTrue("Unable to create file: " + path + name, ret);
        assertTrue("File did not exist: " + path + name, target.exists());

        // Unicode name
        name = "pää1.txt";
        target = new FileUtility(path + name);
        ret = false;
        try
        {
            ret = target.createNewFile();
        }
        catch (Exception e)
        {
            assertTrue("Unable to create file: " + e, false);
        }
        assertTrue("Unable to create file: " + path + name, ret);
        assertTrue("File did not exist: " + path + name, target.exists());
    }

    private void checkCreateOnExisting(String path, String name)
    {
        FileUtility target = new FileUtility(path + name);
        boolean ret = false;

        try
        {
            ret = target.createNewFile();
        }
        catch (Exception e)
        {
            assertTrue("Unable to create: " + path + name + " Exception: "
                       + e.toString(), false);
        }

        assertFalse("Wrong return value on create: " + path + name, ret);
    }

    private void checkCreateOnExistingDir(String path, String name)
    {
        FileUtility target = new FileUtility(path + name);
        boolean ret = false;

        try
        {
            ret = target.createNewFile();
            System.out.println(" check create on existing dir Path: ret: "
                               + ret);
        }
        catch (IOException ex)
        {
            // Good test passed
            ret = true;
        }
        catch (Exception e)
        {
            assertTrue("Unable to create: Exception: " + e.toString(), false);
        }

        if (!ret)
            System.out.println(" check create on existing dir Path: " + path
                               + name);
        assertTrue("Test failed on create: " + path + name, ret);
    }

    public void testFileCreateFailure()
    {
        checkCreateOnExistingDir(path, "FileCreateFailureOnDirTest");
        checkCreateOnExisting(path, "FileCreateFailureTest0");

        // non-ascii name
        checkCreateOnExisting(path, "pää.txt");

    }
}
