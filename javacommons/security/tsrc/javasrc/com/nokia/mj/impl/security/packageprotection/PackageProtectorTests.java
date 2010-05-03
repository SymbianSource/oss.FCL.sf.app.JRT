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


package com.nokia.mj.impl.security.packageprotection;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.security.packageprotection.PackageProtector;
import com.nokia.mj.impl.security.common.InstallerSecurityException;
import java.io.FileInputStream;
import java.io.File;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * PackageProtector unit tests.
 */
public class PackageProtectorTests extends TestCase implements InstallerMain
{

    private static String TEST_DATA_DIR;
    private static String IMPLEMENTATION_CLASSES;

    static
    {
        String platform = System.getProperty("os.name");
        if (platform != null && platform.equalsIgnoreCase("linux"))
        {
            TEST_DATA_DIR = System.getProperty("JAVA_BIN_ROOT") + "/securitytestdata/";
            IMPLEMENTATION_CLASSES = System.getProperty("JAVA_BIN_ROOT") + "/jsr/classes/";
        }
        else
        {
            TEST_DATA_DIR = "C:\\java\\securitytestdata\\";
            IMPLEMENTATION_CLASSES = System.getProperty("JAVA_RES_ROOT") + "\\jvm\\lib\\common\\";
        }
    }

    // class to be tested
    PackageProtector packageProtector;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new PackageProtectorTests("testJARs", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((PackageProtectorTests)tc).testJARs();
            }
        }));

        suite.addTest(new PackageProtectorTests("testScanChecks", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((PackageProtectorTests)tc).testScanChecks();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public PackageProtectorTests()
    {
    }

    public PackageProtectorTests(String aTestName, TestMethod aTestMethod)
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
        packageProtector = PackageProtector.getInstance();
    }

    protected void tearDown()
    {
    }

    private void testJARs()
    {
        // check selected valid jars -> the scanning should succeed
        checkJARs(TEST_DATA_DIR + "packageprotection" + File.separator + "validjars" + File.separator , true);

        // check selected invalid jars -> the scanning should fail
        checkJARs(TEST_DATA_DIR + "packageprotection" + File.separator + File.separator + "invalidjars" + File.separator , false);

        // checking all the implementation classes, check that it does not break the package protector
        checkJARs(IMPLEMENTATION_CLASSES, true, false);

        // robustness testing -> for now it just logs the cases which should have failed
        checkJARs(TEST_DATA_DIR + "packageprotection" + File.separator + File.separator + "robustness" + File.separator, true, true);

    }

    private void checkJARs(String pathToJARs, boolean checkRobustness, boolean expectToLetGo)
    {
        File[] testJARs = new File(pathToJARs).listFiles();
        if (testJARs!= null && testJARs.length > 0)
        {
            for (int i=0; i<testJARs.length; i++)
            {
                if (testJARs[i].isFile())
                {
                    try
                    {
                        packageProtector.scanApplication(testJARs[i].getPath());
                        if (!checkRobustness && !expectToLetGo)
                        {
                            assertTrue("Scanning " + testJARs[i].getPath() + " should have failed",false);
                        }
                        if (checkRobustness && expectToLetGo)
                        {
                            // signal the failure and let go
                            logError("scanning " + testJARs[i].getPath() + " should have failed");
                        }
                    }
                    catch (InstallerSecurityException e)
                    {
                        if (!checkRobustness && expectToLetGo)
                        {
                            assertTrue("Scanning " + testJARs[i].getPath() + " should have succeeded",false);
                        }
                    }
                }
            }
        }
    }

    private void checkJARs(String pathToJARs, boolean expectToLetGo)
    {
        checkJARs(pathToJARs, false /*boolean checkRobustness*/, expectToLetGo);
    }

    private void testScanChecks()
    {
        String jarPath = TEST_DATA_DIR + "security_tmp" + File.separator + "HelloTouch.jar";
        try
        {
            ScanCheck.BaseClassCheck[] baseClassChecks =
                new ScanCheck.BaseClassCheck[]
            {
                new ScanCheck.BaseClassCheck("javax/microedition/lcdui/Canvas"),
                new ScanCheck.BaseClassCheck("javax/microedition/lcdui/game/GameCanvas"),
                new ScanCheck.BaseClassCheck("com/nokia/mid/ui/FullCanvas"),
            };
            ScanCheck.MethodCheck[] methodChecks = new ScanCheck.MethodCheck[]
            {
                new ScanCheck.MethodCheck("pointerDragged", "(II)V"),
                new ScanCheck.MethodCheck("pointerPressed", "(II)V"),
                new ScanCheck.MethodCheck("pointerReleased", "(II)V"),
            };
            ScanCheck scanChecks = new ScanCheck(baseClassChecks, methodChecks);
            packageProtector.scanApplication(jarPath, scanChecks);
            // Check that ScanCheck results are correct.
            for (int i = 0; i < scanChecks.iBaseClassChecks.length; i++)
            {
                ScanCheck.BaseClassCheck check = scanChecks.iBaseClassChecks[i];
                String msg = "Unexpected ScanCheck.iBaseClassCheck result " +
                             i + ": " + check.toString();
                switch (i)
                {
                case 0:
                    assertTrue(msg, check.iClassNames.length == 3);
                    assertTrue(msg, "hellotouch/FromCanvasEmptyMethods".equals(check.iClassNames[0]));
                    assertTrue(msg, "hellotouch/FromTmpCanvas".equals(check.iClassNames[1]));
                    assertTrue(msg, "hellotouch/TmpCanvas".equals(check.iClassNames[2]));
                    break;
                case 1:
                    assertTrue(msg, check.iClassNames.length == 1);
                    assertTrue(msg, "hellotouch/FromGameCanvas".equals(check.iClassNames[0]));
                    break;
                case 2:
                    assertTrue(msg, check.iClassNames.length == 1);
                    assertTrue(msg, "hellotouch/FromFullCanvas".equals(check.iClassNames[0]));
                    break;
                default:
                    assertTrue(
                        "Incorrect number of scanChecks.iBaseClassChecks: " +
                        scanChecks.iBaseClassChecks.length, false);
                }
            }
            for (int i = 0; i < scanChecks.iMethodChecks.length; i++)
            {
                ScanCheck.MethodCheck check = scanChecks.iMethodChecks[i];
                ScanCheck.MethodCheckResult[] results = check.iResults;
                String msg = "Unexpected ScanCheck.iMethodCheck result " +
                             i + ": " + check.toString();
                switch (i)
                {
                case 0:
                case 1:
                    assertTrue(msg, results.length == 4);
                    assertTrue(msg, "hellotouch/HelloTouch".equals(results[0].iClassName));
                    assertTrue(msg, results[0].iCodeLength == 35);
                    assertTrue(msg, "hellotouch/FromCanvasEmptyMethods".equals(results[1].iClassName));
                    assertTrue(msg, results[1].iCodeLength == 1);
                    assertTrue(msg, "hellotouch/FromTmpCanvas".equals(results[2].iClassName));
                    assertTrue(msg, results[2].iCodeLength == 1);
                    assertTrue(msg, "hellotouch/FromGameCanvas".equals(results[3].iClassName));
                    assertTrue(msg, results[3].iCodeLength == 35);
                    break;
                case 2:
                    assertTrue(msg, results.length == 4);
                    assertTrue(msg, "hellotouch/HelloTouch".equals(results[0].iClassName));
                    assertTrue(msg, results[0].iCodeLength == 35);
                    assertTrue(msg, "hellotouch/FromCanvasEmptyMethods".equals(results[1].iClassName));
                    assertTrue(msg, results[1].iCodeLength == 1);
                    assertTrue(msg, "hellotouch/FromTmpCanvas".equals(results[2].iClassName));
                    assertTrue(msg, results[2].iCodeLength == 35);
                    assertTrue(msg, "hellotouch/FromGameCanvas".equals(results[3].iClassName));
                    assertTrue(msg, results[3].iCodeLength == 1);
                    break;
                default:
                    assertTrue(
                        "Incorrect number of scanChecks.iMethodChecks: " +
                        scanChecks.iMethodChecks.length, false);
                }
            }
        }
        catch (InstallerSecurityException e)
        {
            assertTrue("Scanning " + jarPath + " should have succeeded", false);
        }
    }

    private void log(String msg)
    {
        com.nokia.mj.impl.utils.Logger.LOG(
            com.nokia.mj.impl.utils.Logger.EJavaSecurity,
            com.nokia.mj.impl.utils.Logger.EInfo,
            "SecurityPackageProtectorUnitTests: " + msg);
    }

    private void logError(String error)
    {
        com.nokia.mj.impl.utils.Logger.LOG(
            com.nokia.mj.impl.utils.Logger.EJavaSecurity,
            com.nokia.mj.impl.utils.Logger.EError,
            "SecurityPackageProtectorUnitTests: " + error);
    }
}
