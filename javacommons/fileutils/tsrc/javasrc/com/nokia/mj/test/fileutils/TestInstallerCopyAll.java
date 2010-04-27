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
public class TestInstallerCopyAll extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestInstallerCopyAll("testInstallerMethod", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestInstallerCopyAll) tc).testInstallerMethod();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestInstallerCopyAll()
    {
    }

    public TestInstallerCopyAll(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    protected void setUp()
    {
        path = TestEnvironment.Main_Test_Folder + "testfilecopyall/";
    }

    static boolean copyAll(String aFrom, String aTo)
    {
        System.out.println("FileOps.copyAll: " + aFrom + " -> " + aTo);
        //return copyAll(new File(aFrom), new File(aTo));

        boolean result = false;
        try
        {
            FileUtility fromFile = new FileUtility(aFrom);
            if (fromFile.isDirectory())
            {
                System.out.println("Calling copy all");
                result = FileOperations.copyAll(aFrom, aTo, true);
            }
            else
            {
                result = FileOperations.copy(aFrom, aTo, true);
            }
            if (!result)
            {
                System.out.println("FileOps.copyAll failed: " + aFrom + " -> " + aTo);
            }
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            System.out.println("FileOps.copyAll exception: " + aFrom + " -> " + aTo+ "EX: "+ ex);
        }
        return result;
    }


    public void testInstallerMethod()
    {
        try
        {
            String srcPath = path.replace('/', '\\');
            //srcPath += "source";
            //FileUtility util = new FileUtility(path);
            copyAll(srcPath+"source", srcPath+"destination");
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }

}
