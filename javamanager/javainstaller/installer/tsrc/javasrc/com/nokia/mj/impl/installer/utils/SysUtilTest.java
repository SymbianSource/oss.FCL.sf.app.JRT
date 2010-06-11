/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.utils.Uid;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * SysUtil unit tests.
 */
public class SysUtilTest extends TestCase implements InstallerMain
{
    // Begin j2meunit test framework setup

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new SysUtilTest("testGetPropertyValue", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SysUtilTest)tc).testGetPropertyValue();
            }
        }));

        suite.addTest(new SysUtilTest("testGetUndefinedPropertyValue", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SysUtilTest)tc).testGetUndefinedPropertyValue();
            }
        }));

        suite.addTest(new SysUtilTest("testSetPropertyValue", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SysUtilTest)tc).testSetPropertyValue();
            }
        }));

        suite.addTest(new SysUtilTest("testSetUndefinedPropertyValue", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SysUtilTest)tc).testSetUndefinedPropertyValue();
            }
        }));

        suite.addTest(new SysUtilTest("testDefineProperty", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SysUtilTest)tc).testDefineProperty();
            }
        }));

        suite.addTest(new SysUtilTest("testGetRepositoryValue", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SysUtilTest)tc).testGetRepositoryValue();
            }
        }));

        suite.addTest(new SysUtilTest("testGetUndefinedRepositoryValue", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SysUtilTest)tc).testGetUndefinedRepositoryValue();
            }
        }));

        suite.addTest(new SysUtilTest("testGetRepositoryStringValue", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SysUtilTest)tc).testGetRepositoryStringValue();
            }
        }));

        suite.addTest(new SysUtilTest("testGetUndefinedRepositoryStringValue", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SysUtilTest)tc).testGetUndefinedRepositoryStringValue();
            }
        }));

        suite.addTest(new SysUtilTest("testSetRepositoryValue", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SysUtilTest)tc).testSetRepositoryValue();
            }
        }));

        suite.addTest(new SysUtilTest("testSetUndefinedRepositoryValue", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SysUtilTest)tc).testSetUndefinedRepositoryValue();
            }
        }));

        suite.addTest(new SysUtilTest("testSetRepositoryStringValue", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SysUtilTest)tc).testSetRepositoryStringValue();
            }
        }));

        suite.addTest(new SysUtilTest("testSetUndefinedRepositoryStringValue", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SysUtilTest)tc).testSetUndefinedRepositoryStringValue();
            }
        }));

        suite.addTest(new SysUtilTest("testGetDriveUniqId", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SysUtilTest)tc).testGetDriveUniqId();
            }
        }));

        suite.addTest(new SysUtilTest("testIsDrivePresent", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SysUtilTest)tc).testIsDrivePresent();
            }
        }));

        suite.addTest(new SysUtilTest("testIsDiskSpaceBelowCriticalLevel", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SysUtilTest)tc).testIsDiskSpaceBelowCriticalLevel();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public SysUtilTest()
    {
    }

    public SysUtilTest(String aTestName, TestMethod aTestMethod)
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

    public void testGetPropertyValue()
    {
        try
        {
            // Test PS keys defined in ScreensaverInternalPSKeys.h
            Uid uid = PlatformUid.createUid("0x101F8771");  // KPSUidScreenSaver
            int key = 0x00000001; // KScreenSaverPreviewMode
            int value = SysUtil.getPropertyValue(uid, key);
            assertTrue("KScreenSaverPreviewMode(!=0): " + value, value == 0);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
        }
    }

    public void testGetUndefinedPropertyValue()
    {
        try
        {
            // Test PS keys defined in ScreensaverInternalPSKeys.h
            Uid uid = PlatformUid.createUid("0x101F8771");  // KPSUidScreenSaver
            int key = 0x00000010; // Undefined key
            int value = SysUtil.getPropertyValue(uid, key);
            assertTrue("Getting undefined property value did not fail", false);
        }
        catch (InstallerException ie)
        {
            // OK, expected exception.
        }
    }

    public void testSetPropertyValue()
    {
        try
        {
            // Test PS keys defined in ScreensaverInternalPSKeys.h
            Uid uid = PlatformUid.createUid("0x101F8771");  // KPSUidScreenSaver
            int key = 0x00000001; // KScreenSaverPreviewMode
            // Set screensaver preview mode on (stays on for 10 secs).
            SysUtil.setPropertyValue(uid, key, 1);
            int value = SysUtil.getPropertyValue(uid, key);
            assertTrue("KScreenSaverPreviewMode(!=1): " + value, value == 1);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
        }
    }

    public void testSetUndefinedPropertyValue()
    {
        try
        {
            // Test PS keys defined in ScreensaverInternalPSKeys.h
            Uid uid = PlatformUid.createUid("0x101F8771");  // KPSUidScreenSaver
            int key = 0x00000010; // Undefined key
            SysUtil.setPropertyValue(uid, key, 1);
            assertTrue("Setting undefined property value did not fail", false);
        }
        catch (InstallerException ie)
        {
            // OK, expected exception.
        }
    }

    public void testDefineProperty()
    {
        Uid uid = SysUtil.PROP_CATEGORY_SYSTEM;
        long key = SysUtil.PROP_KEY_JAVA_LATEST_INSTALLATION;

        // Delete property.
        try
        {
            SysUtil.deleteProperty(uid, key);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception from deleteProperty: " + ie, false);
        }

        // Try to get undefined property.
        try
        {
            int value = SysUtil.getPropertyValue(uid, key);
            assertTrue("Getting undefined property value did not fail", false);
        }
        catch (InstallerException ie)
        {
            // OK, expected exception.
        }

        // Try to set undefined property.
        try
        {
            SysUtil.setPropertyValue(uid, key, 1);
            assertTrue("Setting undefined property value did not fail", false);
        }
        catch (InstallerException ie)
        {
            // OK, expected exception.
        }

        // Define the property.
        try
        {
            SysUtil.defineProperty(uid, key);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception from defineProperty: " + ie, false);
        }

        // Set property after it is defined.
        try
        {
            SysUtil.setPropertyValue(uid, key, 1);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception from setPropertyValue: " + ie, false);
        }

        // Get property after it is defined.
        try
        {
            int value = SysUtil.getPropertyValue(uid, key);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception from getPropertyValue: " + ie, false);
        }

    }

    public void testGetRepositoryValue()
    {
        try
        {
            // Test CenRep keys defined in ScreensaverInternalCRKeys.h
            Uid uid = PlatformUid.createUid("0x101F8770"); // KCRUidScreenSaver
            long key = 0x00000004; // KScreenSaverInvertedColors
            int value = SysUtil.getRepositoryValue(uid, key);
            assertTrue("KScreenSaverInvertedColors(!=0): " + value, value == 0);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
        }
    }

    public void testGetUndefinedRepositoryValue()
    {
        try
        {
            // P&S keys are not valid CenRep keys, test with them.
            Uid uid = SysUtil.PROP_CATEGORY_SYSTEM;
            long key = SysUtil.PROP_KEY_JAVA_LATEST_INSTALLATION;
            int value = SysUtil.getRepositoryValue(uid, key);
            assertTrue("Getting undefined repository value did not fail", false);
        }
        catch (InstallerException ie)
        {
            // OK, expected exception.
        }
    }

    public void testGetRepositoryStringValue()
    {
        try
        {
            Uid uid = SysUtil.REPO_ID_JAVA_INST_VARIATION;
            long key = SysUtil.REPO_KEY_JAVA_INST_SHELL_FOLDER_VALUE;
            String value = SysUtil.getRepositoryStringValue(uid, key);
            assertTrue("Unexpected value: " + value, "Games,Applications".equals(value));
            //assertTrue("Unexpected value: " + value, "".equals(value));
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
        }
    }

    public void testGetUndefinedRepositoryStringValue()
    {
        try
        {
            // P&S keys are not valid CenRep keys, test with them.
            Uid uid = SysUtil.PROP_CATEGORY_SYSTEM;
            long key = SysUtil.PROP_KEY_JAVA_LATEST_INSTALLATION;
            String value = SysUtil.getRepositoryStringValue(uid, key);
            assertTrue("Getting undefined repository string value did not fail", false);
        }
        catch (InstallerException ie)
        {
            // OK, expected exception.
        }
    }

    public void testSetRepositoryValue()
    {
        try
        {
            // Test CenRep keys defined in ScreensaverInternalCRKeys.h
            Uid uid = PlatformUid.createUid("0x101F8770"); // KCRUidScreenSaver
            long key = 0x00000004; // KScreenSaverInvertedColors
            int value = 0;
            SysUtil.setRepositoryValue(uid, key, value);
        }
        catch (InstallerException ie)
        {
            // Setting KScreenSaverInvertedColors throws exception.
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue("Unexpected exception: " + t, false);
        }
    }

    public void testSetUndefinedRepositoryValue()
    {
        try
        {
            Uid uid = SysUtil.REPO_ID_JAVA_INST_VARIATION;
            long key = 100; // Undefined key.
            int value = 0;
            SysUtil.setRepositoryValue(uid, key, value);
        }
        catch (InstallerException ie)
        {
            // Expected exception.
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue("Unexpected exception: " + t, false);
        }
    }

    public void testSetRepositoryStringValue()
    {
        try
        {
            Uid uid = SysUtil.REPO_ID_JAVA_INST_VARIATION;
            long key = SysUtil.REPO_KEY_JAVA_INST_DEF_INST_DRIVE;
            String valueIn = "C";
            SysUtil.setRepositoryStringValue(uid, key, valueIn);
            String valueOut = SysUtil.getRepositoryStringValue(uid, key);
            assertTrue("Unexpected value: " + valueOut, valueIn.equals(valueOut));
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue("Unexpected exception: " + t, false);
        }
    }

    public void testSetUndefinedRepositoryStringValue()
    {
        try
        {
            Uid uid = SysUtil.REPO_ID_JAVA_INST_VARIATION;
            long key = 100; // Undefined key.
            String value = "foo";
            SysUtil.setRepositoryStringValue(uid, key, value);
        }
        catch (InstallerException ie)
        {
            // Expected exception.
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue("Unexpected exception: " + t, false);
        }
    }

    public void testGetDriveUniqId()
    {
        try
        {
            int uniqId = SysUtil.getDriveUniqId(2);
            assertTrue("Expected 2(C)-drive uniq id to be not zero", uniqId != 0);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
        }

        try
        {
            int uniqId = SysUtil.getDriveUniqId(10);
            System.out.println("\n10(K)-drive unique id: " + uniqId + "\n");
            assertTrue("Getting uniq id for nonexistent drive 10 did not fail", false);
        }
        catch (InstallerException ie)
        {
            // OK, expected exception.
        }

        try
        {
            int uniqId = SysUtil.getDriveUniqId(30);
            System.out.println("\n30-drive unique id: " + uniqId + "\n");
            assertTrue("Getting uniq id for nonexistent drive 30 did not fail", false);
        }
        catch (InstallerException ie)
        {
            // OK, expected exception.
        }
    }

    public void testIsDrivePresent()
    {
        try
        {
            boolean present = SysUtil.isDrivePresent(2);
            assertTrue("Expected 2(C)-drive to be present", present);
            present = SysUtil.isDrivePresent(10);
            assertTrue("Expected 10(K)-drive not to be present", !present);
            present = SysUtil.isDrivePresent(30);
            assertTrue("Expected 30-drive not to be present", !present);
            present = SysUtil.isDrivePresent(-1);
            assertTrue("Expected -1-drive not to be present", !present);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
        }
    }

    public void testIsDiskSpaceBelowCriticalLevel()
    {
        try
        {
            boolean belowCriticalLevel =
                SysUtil.isDiskSpaceBelowCriticalLevel(0, 2);
            assertTrue("Expected 2(C)-drive to have free disk space",
                       !belowCriticalLevel);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
        }

        try
        {
            boolean belowCriticalLevel =
                SysUtil.isDiskSpaceBelowCriticalLevel(0, 10);
            assertTrue("Checking disk space for nonexistent drive 10 did not fail", false);
        }
        catch (InstallerException ie)
        {
            // OK, expected exception.
        }

        try
        {
            boolean belowCriticalLevel =
                SysUtil.isDiskSpaceBelowCriticalLevel(0, 30);
            assertTrue("Checking disk space for nonexistent drive 30 did not fail", false);
        }
        catch (InstallerException ie)
        {
            // OK, expected exception.
        }
    }
}
