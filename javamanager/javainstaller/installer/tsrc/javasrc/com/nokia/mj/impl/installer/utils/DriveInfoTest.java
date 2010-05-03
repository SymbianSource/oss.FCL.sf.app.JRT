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


package com.nokia.mj.impl.installer.utils;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * Unit tests for DriveInfo class.
 */
public class DriveInfoTest extends TestCase implements InstallerMain
{
    // Begin j2meunit test framework setup

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new DriveInfoTest("testDriveInfo", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((DriveInfoTest)tc).testDriveInfo();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public DriveInfoTest()
    {
    }

    public DriveInfoTest(String aTestName, TestMethod aTestMethod)
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

    protected void tearDown()
    {
    }

    public void testDriveInfo()
    {
        checkDriveInfo(0, 2, DriveInfo.UNKNOWN_DRIVE);
        checkDriveInfo(2, 1, DriveInfo.PHONE_MEMORY);
        checkDriveInfo(4, 0x402, DriveInfo.MEMORY_CARD);
        checkDriveInfo(6, 0x401, DriveInfo.INTERNAL_MASS_STORAGE);
    }

    private void checkDriveInfo(int aDrive, int aState, int aType)
    {
        DriveInfo driveInfo = new DriveInfo(aDrive, aState, 10000000);
        Log.log("checkDriveInfo: " + driveInfo.toString());
        assertTrue("DriveInfo returns incorrect drive, got " +
                   driveInfo.getNumber() + ", expected " + aDrive,
                   driveInfo.getNumber() == aDrive);
        assertTrue("DriveInfo returns incorrect type, got " +
                   driveInfo.getDriveType() + ", expected " + aType,
                   driveInfo.getDriveType() == aType);
    }
}
