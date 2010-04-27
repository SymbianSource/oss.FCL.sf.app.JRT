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
import com.nokia.mj.impl.fileutils.*;
import com.nokia.mj.test.fileutils.TestEnvironment;

/**
 * Checks cases related to Exists()
 */
public class TestContentId extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        if (TestEnvironment.Platform.equals("s60"))
        {
            suite.addTest(new TestContentId("testContentIdS60", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((TestContentId) tc).testContentIdS60();
                }
            }));
        }
        else
        {
            suite.addTest(new TestContentId("testContentIdLinux", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((TestContentId) tc).testContentIdLinux();
                }
            }));
        }

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestContentId()
    {
    }

    public TestContentId(String aTestName, TestMethod aTestMethod)
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
    }


    /**
     * Test ContentId.
     *
     * 1. Test non-existing path. Returns NULL.
     * 2. Test non-drm file. Returns NULL.
     * 3. Test drm file. Returns valid content ID.
     * 4. Test null file name.
     */
    public void testContentIdS60()
    {
        // 1. Test non-existing path. Returns NULL.
        assertTrue("DRM ContentId not null",
                   FileUtility.getContentId("Dummy") == null);

        // 2. Test non-drm file. Returns NULL.
        assertTrue("No DRM protection",
                   FileUtility.getContentId("c:\\HelloWorld1.jar") == null);


        // 3. Test drm file. Returns valid content ID.
        assertTrue("Check contentId",
                   FileUtility.getContentId("c:\\HelloWorld2.jar")
                   .equals("cid:01xoQr7DWRGKYamNJ7WM@wirelessfuture.com"));

        // 4. Test null file name.
        assertTrue("Path null",
                   FileUtility.getContentId(null) == null);
    }

    /**
     * Test ContentId.
     *
     * This is not supported on Linux platform so call returns always null.
     */
    public void testContentIdLinux()
    {
        System.out.println("Whoaa2");
        assertTrue("DRM ContentId not null",
                   FileUtility.getContentId("Dummy") == null);
    }
}
