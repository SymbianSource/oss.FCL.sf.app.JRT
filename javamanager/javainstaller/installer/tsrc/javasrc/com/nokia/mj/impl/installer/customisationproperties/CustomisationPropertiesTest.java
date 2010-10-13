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


package com.nokia.mj.impl.installer.customisationproperties;

import java.util.Hashtable;
import java.util.Vector;

import com.nokia.mj.impl.installer.utils.DriveInfo;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.installer.utils.Platform;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * ApplicationRegistrator unit tests.
 */
public class CustomisationPropertiesTest extends TestCase implements InstallerMain
{


    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new CustomisationPropertiesTest("testSplitCommaSeparatedString", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((CustomisationPropertiesTest)tc).testSplitCommaSeparatedString();
            }
        }));

        if (Platform.isS60())
        {
            suite.addTest(new CustomisationPropertiesTest("testDefaultInstallationDrive", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((CustomisationPropertiesTest)tc).testDefaultInstallationDrive();
                }
            }));

            suite.addTest(new CustomisationPropertiesTest("testPossibleInstallationDrives", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((CustomisationPropertiesTest)tc).testPossibleInstallationDrives();
                }
            }));

            suite.addTest(new CustomisationPropertiesTest("testTypesOfPossibleInstallationDrives", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((CustomisationPropertiesTest)tc).testTypesOfPossibleInstallationDrives();
                }
            }));

            suite.addTest(new CustomisationPropertiesTest("testPreinstalledDefaultDrive", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((CustomisationPropertiesTest)tc).testPreinstalledDefaultDrive();
                }
            }));

            suite.addTest(new CustomisationPropertiesTest("testDefaultInstallationFolder", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((CustomisationPropertiesTest)tc).testDefaultInstallationFolder();
                }
            }));

            suite.addTest(new CustomisationPropertiesTest("testPossibleInstallationFolders", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((CustomisationPropertiesTest)tc).testPossibleInstallationFolders();
                }
            }));

            suite.addTest(new CustomisationPropertiesTest("testShellFolderAttributeNames", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((CustomisationPropertiesTest)tc).testShellFolderAttributeNames();
                }
            }));
        }

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public CustomisationPropertiesTest()
    {
    }

    public CustomisationPropertiesTest(String aTestName, TestMethod aTestMethod)
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

    public void testSplitCommaSeparatedString()
    {
        CustomisationProperties cust = new CustomisationProperties();

        Vector value;

        value = cust.splitCommaSeparatedString(null);
        assertTrue("Splitting null did not result empty vector", value.size() == 0);

        value = cust.splitCommaSeparatedString("");
        assertTrue("Splitting empty did not result empty vector", value.size() == 0);


        value = cust.splitCommaSeparatedString(",");
        assertTrue("Splitting comma did not result empty vector", value.size() == 0);

        value = cust.splitCommaSeparatedString(" ,,    ,   , ,, ");
        assertTrue("Splitting \" ,,    ,   , ,, \" did not result empty vector", value.size() == 0);

        value = cust.splitCommaSeparatedString(",Games,Applications,");
        assertTrue("Splitting ,Games,Applications, did not result vector with 2 objects", value.size() == 2);
        assertTrue("Splitting ,Games,Applications, did not result Games", ((String)value.get(0)).equals("Games"));
        assertTrue("Splitting ,Games,Applications, did not result Applications", ((String)value.get(1)).equals("Applications"));


        value = cust.splitCommaSeparatedString(" ,Games ,, , Applications,  ");
        assertTrue("Splitting \" ,Games ,, , Applications,  \" did not result vector with 2 objects", value.size() == 2);
        assertTrue("Splitting \" ,Games ,, , Applications,  \" did not result Games", ((String)value.get(0)).equals("Games"));
        assertTrue("Splitting \" ,Games ,, , Applications,  \" did not result Applications", ((String)value.get(1)).equals("Applications"));
    }


    public void testDefaultInstallationDrive()
    {
        CustomisationProperties cust = new CustomisationProperties();

        Vector value = null;

        // Test DefaultInstallationDrive
        try
        {
            value = cust.getProperty(CustomisationProperties.DefaultInstallationDrive);
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected throwable " + t.toString(), false);
        }
        assertTrue("DefaultInstallationDrive value is null", (value != null));
        assertTrue("DefaultInstallationDrive value does not contain any strings", (value.size() > 0));

        String drive = (String)value.firstElement();
        assertTrue("DefaultInstallationDrive is null", (drive != null));
        assertTrue("DefaultInstallationDrive is C:", drive.equals("C:"));


        value = cust.getIntegerProperty(CustomisationProperties.DefaultInstallationDrive);
        assertTrue("DefaultInstallationDrive value is null", (value != null));
        assertTrue("DefaultInstallationDrive value does not contain any integers", (value.size() > 0));

        Integer idrive = (Integer)value.firstElement();
        assertTrue("DefaultInstallationDrive is null", (idrive != null));
        assertTrue("DefaultInstallationDrive is not C:", idrive.intValue() == 2);
    }

    public void testPossibleInstallationDrives()
    {
        CustomisationProperties cust = new CustomisationProperties();

        Vector value = null;
        // Test PossibleInstallationDrives
        value = cust.getProperty(CustomisationProperties.PossibleInstallationDrives);
        assertTrue("PossibleInstallationDrives value is null", (value != null));
        assertTrue("PossibleInstallationDrives value does not contain any strings", (value.size() > 0));

        String drive = (String)value.firstElement();
        assertTrue("PossibleInstallationDrives is null", (drive != null));
        assertTrue("First PossibleInstallationDrive is not C:", drive.equals("C:"));
        // second possible drive
        drive = (String)value.get(1);
        assertTrue("PossibleInstallationDrives is null", (drive != null));
        assertTrue("Second PossibleInstallationDrive is not E:", drive.equals("E:"));

        value = cust.getIntegerProperty(CustomisationProperties.PossibleInstallationDrives);
        assertTrue("PossibleInstallationDrives value is null", (value != null));
        assertTrue("PossibleInstallationDrives value does not contain any integers", (value.size() > 0));

        Integer idrive = (Integer)value.get(1);
        assertTrue("Second PossibleInstallationDrive is E:", idrive.intValue() == 4);
    }

    public void testTypesOfPossibleInstallationDrives()
    {
        CustomisationProperties cust = new CustomisationProperties();

        Vector value = null;
        // Test TypesOfPossibleInstallationDrives
        value = cust.getIntegerProperty(CustomisationProperties.TypesOfPossibleInstallationDrives);
        assertTrue("TypesOfPossibleInstallationDrives value is null", (value != null));
        assertTrue("TypesOfPossibleInstallationDrives value does not contain any integers", (value.size() > 0));

        Integer type = (Integer)value.firstElement();
        assertTrue("TypesOfPossibleInstallationDrives is null", (type != null));
        assertTrue("Type of first drive is not PHONE_MEMORY, it is " + type.toString(),
                   (type.intValue() == DriveInfo.PHONE_MEMORY));
        // second possible drive is E:
        type = (Integer)value.get(1);
        assertTrue("Type of second drive is null", (type != null));
        assertTrue("Type of second drive is not MEMORY_CARD, it is " + type.toString(),
                   (type.intValue() == DriveInfo.MEMORY_CARD));
    }

    public void testPreinstalledDefaultDrive()
    {
        CustomisationProperties cust = new CustomisationProperties();

        Vector value = null;
        // Test PreinstalledDefaultDrive
        value = cust.getProperty(CustomisationProperties.PreinstalledDefaultDrive);
        assertTrue("PreinstalledDefaultDrive value is null", (value != null));
        assertTrue("PreinstalledDefaultDrive value does not contain any strings", (value.size() > 0));

        String drive = (String)value.firstElement();
        assertTrue("PreinstalledDefaultDrive is null", (drive != null));
        assertTrue("PreinstalledDefaultDrive is not C:", drive.equals("C:"));
    }

    public void testDefaultInstallationFolder()
    {
        CustomisationProperties cust = new CustomisationProperties();

        Vector value = null;
        // Test DefaultInstallationFolder
        value = cust.getProperty(CustomisationProperties.DefaultInstallationFolder);
        assertTrue("DefaultInstallationFolder value is null", (value != null));
        assertTrue("DefaultInstallationFolder value does not contain any strings", (value.size() > 0));

        String folder = (String)value.firstElement();
        assertTrue("DefaultInstallationFolder is null", (folder != null));
        assertTrue("DefaultInstallationFolder is not empty string", folder.equals(""));
    }

    public void testPossibleInstallationFolders()
    {
        CustomisationProperties cust = new CustomisationProperties();

        Vector valueDef = cust.getProperty(CustomisationProperties.DefaultInstallationFolder);
        Vector value = cust.getProperty(CustomisationProperties.PossibleInstallationFolders);

        assertTrue("DefaultInstallationFolder value is null", (valueDef != null));
        assertTrue("DefaultInstallationFolder value does not contain any strings", (valueDef.size() > 0));
        String defFolder = (String)valueDef.firstElement();

        assertTrue("PossibleInstallationFolders value is null", (value != null));
        assertTrue("PossibleInstallationFolders value does not contain any strings", (value.size() > 0));

        // default installation folder is one of the possible folders
        assertTrue("Default folder is not one of possible installation folders", value.contains(defFolder));
    }

    public void testShellFolderAttributeNames()
    {
        CustomisationProperties cust = new CustomisationProperties();

        Vector value = cust.getProperty(CustomisationProperties.ShellFolderAttributeNames);

        assertTrue("ShellFolderAttributeNames value is null", (value != null));
        assertTrue("ShellFolderAttributeNames value does not contain any strings", (value.size() > 0));
        String firstName = (String)value.firstElement();

        // 'Nokia-MIDlet-Category' is one of the attribute names
        assertTrue("Nokia-MIDlet-Category is not allowed attribute name", value.contains("Nokia-MIDlet-Category"));
    }

}
