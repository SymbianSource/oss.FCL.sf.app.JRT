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
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

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
public class TestDelete extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestDelete("testDeleteOnFile", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestDelete) tc).testDeleteOnFile();
            }
        }));

        suite.addTest(new TestDelete("testDeleteOnDir", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestDelete) tc).testDeleteOnDir();
            }
        }));

        suite.addTest(new TestDelete("testDeleteOnAbsent", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestDelete) tc).testDeleteOnAbsent();
            }
        }));

        suite.addTest(new TestDelete("testDeleteWithInputStream",
                                     new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestDelete) tc).testDeleteWithInputStream();
            }
        }));

        suite.addTest(new TestDelete("testDeleteWithOutputStream",
                                     new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestDelete) tc).testDeleteWithOutputStream();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestDelete()
    {
    }

    public TestDelete(String aTestName, TestMethod aTestMethod)
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
        path = TestEnvironment.Main_Test_Folder + "testdelete/";
    }

    protected void tearDown()
    {
    }

    public void testDeleteOnFile()
    {
        String name = "inputFile.txt";
        try
        {
            FileUtility utility = new FileUtility(path + name);
            assertTrue("Delete failed", utility.delete());
            assertFalse("Exists after delete", utility.exists());
        }
        catch (Exception e)
        {
            assertTrue("Test Failed. Unexpected Exception: " + e, false);
        }
    }

    public void testDeleteOnDir()
    {
        String name = "folder1/";
        try
        {
            FileUtility utility = new FileUtility(path + name);
            assertTrue("Delete failed", utility.delete());
            assertFalse("Exists after delete", utility.exists());
        }
        catch (Exception e)
        {
            assertTrue("Test Failed. Unexpected Exception: " + e, false);
        }
    }

    public void testDeleteOnAbsent()
    {
        String name = "absentFile";
        try
        {
            FileUtility utility = new FileUtility(path + name);
            assertFalse("Delete failed", utility.delete());
        }
        catch (Exception e)
        {
            assertTrue("Test Failed. Unexpected Exception: " + e, false);
        }
    }

    public void testDeleteWithInputStream()
    {
        String name = "inputStreamFile.txt";
        try
        {
            FileUtility utility = new FileUtility(path + name);
            InputStream fis = utility.openInputStream();
            assertTrue("Delete failed", utility.delete());

            try
            {
                fis.read();
                assertTrue("Read did not throw exception", false);
            }
            catch (IOException ex)
            {
                // Expected
            }
        }
        catch (Exception e)
        {
            assertTrue("Test Failed. Unexpected Exception: " + e, false);
        }
    }

    public void testDeleteWithOutputStream()
    {
        String name = "outputStreamFile.txt";
        try
        {
            FileUtility utility = new FileUtility(path + name);
            OutputStream fis = utility.openOutputStream();
            assertTrue("Delete failed", utility.delete());

            try
            {
                fis.write(10);
                assertTrue("Write did not throw exception", false);
            }
            catch (IOException ex)
            {
                // Expected
            }
        }
        catch (Exception e)
        {
            assertTrue("Test Failed. Unexpected Exception: " + e, false);
        }
    }
}
