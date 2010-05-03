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

/**
 * Checks cases related to Exists()
 */
public class TestResolveDrive extends TestCase implements InstallerMain
{

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestResolveDrive("testExisting", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestResolveDrive) tc).testExisting();
            }
        }));

        suite.addTest(new TestResolveDrive("testNonExisting", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestResolveDrive) tc).testNonExisting();
            }
        }));

        suite.addTest(new TestResolveDrive("testExistingDrive", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestResolveDrive) tc).testExistingDrive();
            }
        }));

        suite.addTest(new TestResolveDrive("testExistingOnZ", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestResolveDrive) tc).testExistingOnZ();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestResolveDrive()
    {
    }

    public TestResolveDrive(String aTestName, TestMethod aTestMethod)
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

    public void testNonExisting()
    {
        String fileName = "\\data\\images\\nonExistentTestExistsFile";
        String realFile = "C:\\Data\\Images\\nonExistentTestExistsFile";

        try
        {
            FileUtility check = new FileUtility(realFile);
            if (check.exists())
            {
                check.delete();
            }
            check = null;

            String resolved =  FileUtility.resolveDrive(fileName);
            System.out.println("Given drive: "+resolved);

            assertTrue(" Value returned false. "+resolved, resolved == null);
        }
        catch (Exception e)
        {
            assertTrue(" testNonExisting failed " + e, false);
        }
    }

    public void testExisting()
    {
        String fileName = "\\data\\images\\testExistsFile";
        String realFile = "C:\\Data\\Images\\testExistsFile";

        try
        {
            FileUtility check = new FileUtility(realFile);
            if (!check.exists())
            {
                check.createNewFile();
            }
            check = null;

            String resolved =  FileUtility.resolveDrive(fileName);
            System.out.println("Given drive: "+resolved);

            assertTrue(" Value returned false. ", resolved.equalsIgnoreCase((realFile)));
        }
        catch (Exception e)
        {
            assertTrue(" testExisting failed " + e, false);
        }
    }

    public void testExistingDrive()
    {
        String fileName = "\\data\\images\\testExistsFile";
        String realFile = "C:\\Data\\Images\\testExistsFile";

        try
        {
            FileUtility check = new FileUtility(realFile);
            if (!check.exists())
            {
                check.mkdir();
            }
            check = null;

            String resolved =  FileUtility.resolveDrive(fileName);
            System.out.println("Given drive: "+resolved);

            assertTrue(" Value returned false. ", resolved.equalsIgnoreCase((realFile)));
        }
        catch (Exception e)
        {
            assertTrue(" testExisting failed " + e, false);
        }
    }


    public void testExistingOnZ()
    {
        String fileName = "\\private\\testExistsFile";
        String realFile = "Z:\\private\\testExistsFile";

        try
        {
//            FileUtility check = new FileUtility(realFile);
//            if(!check.exists())
//            {
//                check.createNewFile();
//            }
//            check = null;
//
            String resolved =  FileUtility.resolveDrive(fileName);
            System.out.println("Given drive: "+resolved);

            assertTrue(" Value returned is null. ", resolved!=null);
        }
        catch (Exception e)
        {
            assertTrue(" testExisting failed " + e, false);
        }
    }
}
