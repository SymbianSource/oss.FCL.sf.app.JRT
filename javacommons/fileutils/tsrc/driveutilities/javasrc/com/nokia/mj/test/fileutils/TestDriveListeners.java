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
import j2meunit.framework.TestSuite;
import j2meunit.framework.TestMethod;

import com.nokia.mj.impl.fileutils.DriveUtilities;
import com.nokia.mj.impl.fileutils.DriveListener;
import com.nokia.mj.impl.fileutils.DriveInfo;

public class TestDriveListeners extends TestCase
{
    public TestDriveListeners()
    {
    }

    public TestDriveListeners(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new TestDriveListeners("testListeners", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestDriveListeners) tc).testListeners();
            }
        }));

        return aSuite;
    }

    class TestListener implements DriveListener
    {
        public void driveChanged(final int aOperation, final DriveInfo aDriveInfo)
        {
            System.out.println("driveChanged aOperation=" + aOperation);
            System.out.println("driveChanged aDriveInfo.iRootPath="     + aDriveInfo.iRootPath);
            System.out.println("driveChanged aDriveInfo.iIsPresent="    + aDriveInfo.iIsPresent);
            System.out.println("driveChanged aDriveInfo.iIsRemovable="  + aDriveInfo.iIsRemovable);
            System.out.println("driveChanged aDriveInfo.iIsLocal="      + aDriveInfo.iIsLocal);
            System.out.println("driveChanged aDriveInfo.iIsReadOnly="   + aDriveInfo.iIsReadOnly);
            System.out.println("driveChanged aDriveInfo.iId="           + aDriveInfo.iId);
        }
    }

    public void testListeners()
    {
        System.out.println("TestDriveListeners.testListeners()");
        TestListener listener = new TestListener();
        assertTrue(DriveUtilities.registerListener(listener));

        // Not a very professional way, but should work.
        // During the sleep generate MMC change events
        try
        {
            Thread.sleep(10 * 1000);
        }
        catch (InterruptedException e)
        {
        }

        assertTrue(DriveUtilities.unregisterListener(listener));
    }
}
