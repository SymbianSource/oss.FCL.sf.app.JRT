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

import com.nokia.mj.test.file.utils.TestFailedException;
import com.nokia.mj.test.file.utils.FileTestUtils;
import com.nokia.mj.test.file.utils.FileTestBase;
import com.nokia.mj.impl.installer.utils.InstallerMain;

/**
 * Not to be run currently. Implementation for DRM file access is currently not
 * done. So all tests will fail.
 *
 */
public class TestFile_SystemProperties extends FileTestBase
{
    public TestFile_SystemProperties()
    {
    }

    public TestFile_SystemProperties(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_SystemProperties("printSystemProperties", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_SystemProperties) tc).printSystemProperties();
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

    public void printSystemProperties()
    {
        try
        {
            String[] propKeys = { "fileconn.dir.graphics",
                                  "fileconn.dir.graphics.name", "fileconn.dir.memorycard",
                                  "fileconn.dir.memorycard.name", "fileconn.dir.music",
                                  "fileconn.dir.music.name", "fileconn.dir.photos",
                                  "fileconn.dir.photos.name", "fileconn.dir.private",
                                  "fileconn.dir.private.name", "fileconn.dir.recordings",
                                  "fileconn.dir.recordings.name", "fileconn.dir.roots.names",
                                  "fileconn.dir.tones", "fileconn.dir.tones.name",
                                  "fileconn.dir.videos", "fileconn.dir.videos.name"
                                };

            String propVal = null;
            for (int index = 0; index < propKeys.length; index++)
            {
                propVal = System.getProperty(propKeys[index]);
                System.out
                .println("Key: " + propKeys[index] + " Value: " + propVal);

                assertTrue("Property: " + propKeys[index] + " failed",
                           propVal != null);
            }
        }
        catch (Exception ex)
        {
            assertTrue("System properties failed: "+ex, false);
        }
    }

}