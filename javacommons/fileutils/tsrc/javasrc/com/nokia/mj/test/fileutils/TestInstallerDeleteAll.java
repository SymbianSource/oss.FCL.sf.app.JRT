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

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.fileutils.*;
import com.nokia.mj.test.fileutils.TestEnvironment;

/**
 * Checks cases related to canRead()
 */
public class TestInstallerDeleteAll extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestInstallerDeleteAll("testInstallerMethod", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestInstallerDeleteAll) tc).testInstallerMethod();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestInstallerDeleteAll()
    {
    }

    public TestInstallerDeleteAll(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    protected void setUp()
    {
        path = TestEnvironment.Main_Test_Folder + "testfiledeleteall";
    }

    static boolean deleteAll(FileUtility aFile) throws java.io.IOException
    {
        boolean result = true;
        if (aFile == null)
        {
            result = false;
        }
        if (result && aFile.isDirectory())
        {
            FileUtility[] files = aFile.listFiles();
            if (files != null)
            {
                for (int i = 0; i < files.length && result; i++)
                {
                    System.out.println("File Returned by list: "+files[i]);
                    result = deleteAll(files[i]);
                }
            }
        }
        if (result && aFile.exists())
        {
            result = aFile.delete();
        }
        if (!result)
        {
            System.out.println("FileOps.deleteAll failed: [" + aFile + "]");
        }
        return result;
    }


    public void testInstallerMethod()
    {
        try
        {
            FileUtility util = new FileUtility(path);
            deleteAll(util);
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }

}
