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
import com.nokia.mj.impl.fileutils.DriveListener;
import com.nokia.mj.impl.fileutils.DriveUtilities;
import com.nokia.mj.impl.fileutils.DriveInfo;


import com.nokia.mj.test.file.utils.TestFailedException;
import com.nokia.mj.test.file.utils.FileTestUtils;
import com.nokia.mj.test.file.utils.FileTestBase;
import com.nokia.mj.impl.installer.utils.InstallerMain;

/**
 * Tests written to check Drive utils that is part of FileUtils.
 * This is not part of main File API module tests.
 *
 */
public class TestFile_DriveUtils extends FileTestBase
{
    public TestFile_DriveUtils()
    {
    }

    public TestFile_DriveUtils(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_DriveUtils("getDriveInfoTest", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DriveUtils) tc).getDriveInfoTest();
            }
        }));

        suite.addTest(new TestFile_DriveUtils("getAllDriveInfoTest", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DriveUtils) tc).getAllDriveInfoTest();
            }
        }));

        suite.addTest(new TestFile_DriveUtils("registerUnregisterListener", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DriveUtils) tc).registerUnregisterListener();
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

    public void getDriveInfoTest()
    {
        DriveInfo[] drives = DriveUtilities.getAccessibleDrives();
        for (int index = 0; index < drives.length; index++)
        {
            System.out.println("Drive Info: ID: "+drives[index].iId);
            System.out.println("Drive Info: Local?: "+drives[index].iIsLocal);
            System.out.println("Drive Info: Present?: "+drives[index].iIsPresent);
            System.out.println("Drive Info: Removable?: "+drives[index].iIsRemovable);
            System.out.println("Drive Info: Root Path?: "+drives[index].iRootPath);
        }
    }

    public void getAllDriveInfoTest()
    {
        DriveInfo[] drives = DriveUtilities.getAllDrives();
        for (int index = 0; index < drives.length; index++)
        {
            System.out.println("Drive Info: ID: "+drives[index].iId);
            System.out.println("Drive Info: Local?: "+drives[index].iIsLocal);
            System.out.println("Drive Info: Present?: "+drives[index].iIsPresent);
            System.out.println("Drive Info: Removable?: "+drives[index].iIsRemovable);
            System.out.println("Drive Info: Root Path?: "+drives[index].iRootPath);
        }
    }

    public void registerUnregisterListener()
    {
        MyDriveListener myListener = new MyDriveListener();
        System.out.println("Registering Listener");
        DriveUtilities.registerListener(myListener);
        System.out.println("UnRegistering Listener");
        DriveUtilities.unregisterListener(myListener);
    }
}

class MyDriveListener implements DriveListener
{
    public void driveChanged(int aOperation, DriveInfo aDriveInfo)
    {
        if (aOperation == DriveListener.REMOVABLE_MEDIA_INSERTED)
        {
            System.out.println("Removable media inserted");
        }

        if (aOperation == DriveListener.REMOVABLE_MEDIA_REMOVED)
        {
            System.out.println("Removable media removed");
        }
    }
}