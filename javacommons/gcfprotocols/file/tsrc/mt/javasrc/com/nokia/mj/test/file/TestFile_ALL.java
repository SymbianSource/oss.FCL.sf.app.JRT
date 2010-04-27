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

import java.io.IOException;
import javax.microedition.io.file.*;
import javax.microedition.io.*;

import com.nokia.mj.test.file.utils.FileTestUtils;
import com.nokia.mj.test.file.utils.FileTestBase;
import com.nokia.mj.impl.installer.utils.InstallerMain;

public class TestFile_ALL extends FileTestBase
{
    public TestFile_ALL()
    {
    }

    public TestFile_ALL(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        TestFile_SystemProperties.registerTests(suite);
        TestFile_FileSystemRegistry.registerTests(suite);
        TestFile_Security_Manufacturer.registerTests(suite);

        TestFile_0010.registerTests(suite);
        TestFile_0020.registerTests(suite);
        TestFile_0030.registerTests(suite);
        TestFile_0040.registerTests(suite);
        TestFile_0060.registerTests(suite);

        //rem TestFile_0081 localization stuff not needed

        TestFile_0100_0110.registerTests(suite);
        TestFile_0200.registerTests(suite);
        // TestFile_0220.registerTests(suite);
        TestFile_0230.registerTests(suite);
        TestFile_0240.registerTests(suite);
        TestFile_0250.registerTests(suite);

        //rem TestFile_0270  bad

        TestFile_0300.registerTests(suite);
        TestFile_0310.registerTests(suite);
        TestFile_0320.registerTests(suite);
        TestFile_0400.registerTests(suite);
        TestFile_0410.registerTests(suite);
        TestFile_0420.registerTests(suite);
        // TestFile_0430.registerTests(suite); // Bad Test case
        // TestFile_0440.registerTests(suite); // Bad test case
        TestFile_0500.registerTests(suite);
        TestFile_0510.registerTests(suite);
        TestFile_0600.registerTests(suite);
        TestFile_0610.registerTests(suite);
        TestFile_0620.registerTests(suite);
        TestFile_0700.registerTests(suite);
        TestFile_0710.registerTests(suite);
        TestFile_0800.registerTests(suite);
        TestFile_0810.registerTests(suite);

        TestFile_1000.registerTests(suite);
        TestFile_1010.registerTests(suite);
        TestFile_1020_1030.registerTests(suite);
        TestFile_1040.registerTests(suite);
        TestFile_1050.registerTests(suite);
        TestFile_1100.registerTests(suite);
        TestFile_1110.registerTests(suite);
        TestFile_1130.registerTests(suite);
        TestFile_1200_1210.registerTests(suite);
        TestFile_1300.registerTests(suite);
        TestFile_1310.registerTests(suite);
        TestFile_1320.registerTests(suite);
        TestFile_1400.registerTests(suite);
        TestFile_1410_1420.registerTests(suite);
        TestFile_1500.registerTests(suite);
        TestFile_1600.registerTests(suite);
        TestFile_1610.registerTests(suite);
        TestFile_1620.registerTests(suite);
        TestFile_1630.registerTests(suite);
        TestFile_1640.registerTests(suite);
        TestFile_1660.registerTests(suite);
        TestFile_1700.registerTests(suite);
        TestFile_1710.registerTests(suite); //Strange! Works when run alone
        TestFile_1720.registerTests(suite);
        TestFile_1730.registerTests(suite);
        TestFile_1740.registerTests(suite);
        TestFile_1800.registerTests(suite);
        //TestFile_1810.registerTests(suite);
        TestFile_1820.registerTests(suite);
        TestFile_1850.registerTests(suite);
        TestFile_1900_1910.registerTests(suite);
        TestFile_1920_1930.registerTests(suite);
        //TestFile_1962.registerTests(suite); Test case problem

        TestFile_2000.registerTests(suite);
        TestFile_2010.registerTests(suite);
        TestFile_2020.registerTests(suite);
        TestFile_2100.registerTests(suite);
        TestFile_2110.registerTests(suite);
        TestFile_2200.registerTests(suite);
        TestFile_2210.registerTests(suite);
        TestFile_2220.registerTests(suite);
        TestFile_2300.registerTests(suite);
        TestFile_2310.registerTests(suite);
        TestFile_2330_2400.registerTests(suite);
        TestFile_2410.registerTests(suite);
        TestFile_2420.registerTests(suite);
        TestFile_2430.registerTests(suite);
        TestFile_2440.registerTests(suite);
        //TestFile_2500.registerTests(suite); //Some problem with conversion!
        TestFile_2510.registerTests(suite);
        TestFile_2520.registerTests(suite);
        TestFile_2530.registerTests(suite);
        TestFile_2540.registerTests(suite);
        //TestFile_2600.registerTests(suite);
        TestFile_2610.registerTests(suite);
        TestFile_2620.registerTests(suite);
        TestFile_2630.registerTests(suite);
        //TestFile_2650.registerTests(suite);
        TestFile_2700.registerTests(suite);
        TestFile_2710.registerTests(suite);
        TestFile_2720.registerTests(suite);
        TestFile_2730.registerTests(suite);
        TestFile_2760.registerTests(suite);
        TestFile_2790_2792.registerTests(suite);
        TestFile_2800_2801.registerTests(suite); //Conversion problem
        TestFile_2900.registerTests(suite);
        TestFile_2910_2920.registerTests(suite);
        TestFile_2930.registerTests(suite);

//        TestFile_3000.registerTests(suite); //VFS STUFF
        //TestFile_3001.registerTests(suite);
        TestFile_3300.registerTests(suite);
        TestFile_3310.registerTests(suite);
        TestFile_3320.registerTests(suite);
        TestFile_3330.registerTests(suite);
        TestFile_3340.registerTests(suite);
        TestFile_3350.registerTests(suite);
        TestFile_3360_3370.registerTests(suite);
        TestFile_3380.registerTests(suite);
        TestFile_3390.registerTests(suite);
        TestFile_3400.registerTests(suite);
        TestFile_3410.registerTests(suite);
        TestFile_3420.registerTests(suite);
        TestFile_3430.registerTests(suite);
        TestFile_3440.registerTests(suite);
        TestFile_3450.registerTests(suite);
        TestFile_3460.registerTests(suite);
        //TestFile_3461.registerTests(suite);

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }
}