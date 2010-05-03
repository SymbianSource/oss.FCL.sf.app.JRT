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
public class TestMkdirs extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestMkdirs("testMkdirs", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMkdirs) tc).testMkdirs();
            }
        }));

        suite.addTest(new TestMkdirs("testMkdirsIllegalArgment",
                                     new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestMkdirs) tc).testMkdirsIllegalArgment();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestMkdirs()
    {
    }

    public TestMkdirs(String aTestName, TestMethod aTestMethod)
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
        path = TestEnvironment.Main_Test_Folder + "testmkdirs/";
    }

    public void testMkdirsIllegalArgment()
    {
        String paths = path + "fol<der1/folder2/folder3/folder4/";
        try
        {
            FileUtility utility = new FileUtility(paths);
            assertFalse("Created directories", utility.mkdirs());
            assertFalse("Files exist", utility.exists());
        }
        catch (IllegalArgumentException ex)
        {
            // Good
        }
        catch (Exception e)
        {
            assertTrue("Unable to create directories" + e, false);
        }
    }

    public void testMkdirs()
    {
        String paths = path + "folder1/folder2/folder3/folder4/";
        try
        {
            FileUtility utility = new FileUtility(paths);
            assertTrue("Failed to create directories", utility.mkdirs());
            assertTrue("Files did not exist", utility.exists());
        }
        catch (Exception e)
        {
            assertTrue("Unable to create directories" + e, false);
        }
    }
}
