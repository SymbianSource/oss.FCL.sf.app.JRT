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

/**
 * Checks cases related to canRead()
 */
public class TestCanonicalPath extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestCanonicalPath("testCanonicalOnAppHome", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCanonicalPath) tc).testCanonicalOnAppHome();
            }
        }));

        suite.addTest(new TestCanonicalPath("testNonExistent", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCanonicalPath) tc).testNonExistent();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestCanonicalPath()
    {
    }

    public TestCanonicalPath(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    protected void setUp()
    {
        //path = TestEnvironment.Main_Test_Folder + "testcanread/";
    }

    public void testCanonicalOnAppHome()
    {
        try
        {
            // The path must be same as app's private directory
            ApplicationInfo appInfo = ApplicationInfo.getInstance();

            String appPrivateDir = appInfo.getRootPath();
            appPrivateDir = appPrivateDir.replace('\\', '/');

            System.out.println("testCanonicalOnAppHome: Input: "+appPrivateDir);
            System.out.println("testCanonicalOnAppHome: Returned: "+FileUtility.getCanonicalPath(appPrivateDir+"/"));
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }

    public void testNonExistent()
    {
        String path = "C:/data/absent/notatall";
        try
        {
            String ret = FileUtility.getCanonicalPath(path);
            assertTrue("Wrong Value returned: "+ret, ret.equals(path));

            path = "C:/data/absent/notatall/impossible";
            ret = FileUtility.getCanonicalPath(path);
            assertFalse("Wrong Value returned: "+ret, ret.equals(path));

        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }




}
