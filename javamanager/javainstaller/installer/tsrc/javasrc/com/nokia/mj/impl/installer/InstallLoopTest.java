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


package com.nokia.mj.impl.installer;

import com.nokia.mj.impl.installer.utils.InstallerMain;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * Tests for making several installations and uninstallations in a row.
 */
public class InstallLoopTest extends TestCase implements InstallerMain
{
    private static String[] iArgs = null;

    // Begin j2meunit test framework setup

    public void installerMain(String[] aArgs)
    {
        iArgs = aArgs;

        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new InstallLoopTest("testInstallLoop", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallLoopTest)tc).testInstallLoop();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public InstallLoopTest()
    {
    }

    public InstallLoopTest(String aTestName, TestMethod aTestMethod)
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

    public void testInstallLoop()
    {
        int loopCount = 0;
        try
        {
            loopCount = Integer.parseInt(iArgs[2]);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Loop count not specified", false);
        }
        if (iArgs.length < 4)
        {
            assertTrue("Installer options not specified", false);
        }
        String[] options = new String[iArgs.length-1];
        options[0] = "install";
        options[1] = "-silent";
        for (int i = 3; i < iArgs.length; i++)
        {
            options[i-1] = iArgs[i];
        }
        for (int i = 0; i < loopCount; i++)
        {
            installUninstall(options, i);
        }
    }

    private void installUninstall(String[] aOptions, int aIndex)
    {
        System.out.println("Installation count: " + aIndex);
        int result = Installer.mainWithResult(aOptions);
        assertTrue("Installation failed, result=" + result, result == 0);
        assertTrue("Installer.iInstalledApps == null",
                   Installer.iInstalledApps != null);

        System.out.println("Uninstallation count: " + aIndex);
        String uid = Installer.iInstalledApps[0].toString();
        result = Installer.mainWithResult
                 (new String[]
                  {
                      "uninstall", "-silent", "-uid=" + uid,
                  });
        assertTrue("Uninstallation failed, result=" +
                   result + ", uid=" + uid, result == 0);
    }
}
