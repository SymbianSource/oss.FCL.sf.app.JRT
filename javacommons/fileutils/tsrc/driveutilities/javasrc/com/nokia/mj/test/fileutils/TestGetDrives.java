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
import com.nokia.mj.impl.fileutils.DriveInfo;

public class TestGetDrives extends TestCase
{

    public TestGetDrives()
    {
    }

    public TestGetDrives(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new TestGetDrives("testGetAllDrives", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestGetDrives) tc).testGetAllDrives();
            }
        }));

        aSuite.addTest(new TestGetDrives("testGetAccessibleDrives", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestGetDrives) tc).testGetAccessibleDrives();
            }
        }));

        return aSuite;
    }

    public void testGetAllDrives()
    {
        System.out.println("TestGetDrives.testGetAllDrives()");
        DriveInfo[] dis = DriveUtilities.getAllDrives();
        assertNotNull(dis);
        assertTrue(dis.length > 0);
    }

    public void testGetAccessibleDrives()
    {
        System.out.println("TestGetDrives.testGetAccessibleDrives()");
        DriveInfo[] dis = DriveUtilities.getAccessibleDrives();
        assertNotNull(dis);
        assertTrue(dis.length > 0);
    }
}
