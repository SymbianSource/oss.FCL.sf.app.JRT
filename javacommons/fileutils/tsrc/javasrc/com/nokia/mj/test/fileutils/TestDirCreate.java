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

import java.io.InputStreamReader;
import java.io.ByteArrayInputStream;
import java.io.IOException;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.fileutils.*;
import com.nokia.mj.test.fileutils.TestEnvironment;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * FileURL unit tests.
 */
public class TestDirCreate extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestDirCreate("testDirCreate", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestDirCreate) tc).testDirCreate();
            }
        }));

        suite.addTest(new TestDirCreate("testDirCreateFailure",
                                        new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestDirCreate) tc).testDirCreateFailure();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestDirCreate()
    {
    }

    public TestDirCreate(String aTestName, TestMethod aTestMethod)
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
        path = TestEnvironment.Main_Test_Folder + "testdircreate/";
    }

    protected void tearDown()
    {
    }

    public void testDirCreate()
    {
        String name = "FileUtilityCreateTestDir0/";

        FileUtility target = new FileUtility(path + name);
        boolean ret = false;
        try
        {
            ret = target.mkdir();
        }
        catch (Exception e)
        {
            System.out.println("Unable to create file: " + path + name);
            assertTrue("Unable to create dir: " + e, false);
        }

        assertTrue("Mkdir returned " + ret, ret);
        assertTrue("File did not exist: " + path + name, target.exists());

        // Unicode name
        name = "pääDir1/";
        target = new FileUtility(path + name);
        ret = false;
        try
        {
            ret = target.mkdir();
        }
        catch (Exception e)
        {
            System.out.println("Unable to create file: " + path + name);
            assertTrue("Unable to create dir: " + e, false);
        }
        assertTrue("Mkdir returned " + ret, ret);
        assertTrue("File did not exist: " + path + name, target.exists());
    }

    private void checkCreateOnExisting(String path, String name)
    {
        FileUtility target = new FileUtility(path + name);
        boolean ret = false;

        try
        {
            ret = target.mkdir();
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
            ret = target.mkdir();
        }
        catch (Exception e)
        {
            assertTrue("Unable to create: " + path + name + " Exception: "
                       + e.toString(), false);
        }

        assertFalse("Test failed on create: " + path + name, ret);
    }

    public void testDirCreateFailure()
    {
        String separator = System.getProperty("/");
        checkCreateOnExisting(path, "FileCreateFailureTestDir0" + separator);

        // non-ascii name
        checkCreateOnExisting(path, "pääDirTest" + separator);

        checkCreateOnExistingDir(path, "FileCreateFailureOnDirTest" + separator);
    }
}
