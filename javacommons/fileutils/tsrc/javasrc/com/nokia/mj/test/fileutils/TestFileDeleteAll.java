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
import java.io.OutputStream;
import java.io.InputStream;
import java.util.Vector;

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
public class TestFileDeleteAll extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestFileDeleteAll("testFileDeleteAll", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileDeleteAll) tc).testFileDeleteAll();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestFileDeleteAll()
    {
    }

    public TestFileDeleteAll(String aTestName, TestMethod aTestMethod)
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
        path = TestEnvironment.Main_Test_Folder + "testfiledeleteall/";
    }

    private void matchEnumWithVector(java.util.Enumeration source,
                                     Vector expectedList)
    {
        while (source.hasMoreElements())
        {
            String file = (String) source.nextElement();
            if (expectedList.contains(file))
            {
                expectedList.remove(file);
            }
            else
            {
                assertTrue("List contents wrong", false);
            }
        }
        assertTrue("All elements not returned", 0 == expectedList.size());
    }

    private void testFileDeleteAll()
    {
        try
        {
            String target = path+"source/";
            FileUtility utility = new FileUtility(target);

            assertTrue("Target Directory not present", utility.exists());

            assertTrue("Delete All Failed", FileOperations.deleteAll(target));
            assertFalse("Destination exists", utility.exists());
        }
        catch (Exception e)
        {
            assertTrue("Unexpected exception: "+e, false);
        }
    }
}
