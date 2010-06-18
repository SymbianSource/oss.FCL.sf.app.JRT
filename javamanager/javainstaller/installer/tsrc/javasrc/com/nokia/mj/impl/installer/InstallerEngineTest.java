/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.Platform;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Uid;

import java.io.IOException;
import java.util.Vector;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * Test cases for InstallerEngine.
 */
public class InstallerEngineTest extends TestCase implements InstallerMain
{
    private InstallerTestUtils iItu = null;
    private Vector iDefaultOptions = null;

    // Begin j2meunit test framework setup

    public void installerMain(String[] aArgs)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        if (Platform.isLinux())
        {
            // In S60 this would be interactive test, so disable it there.
            suite.addTest(new InstallerEngineTest("testTrustedJadNonSilentOk", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((InstallerEngineTest)tc).testTrustedJadNonSilentOk();
                }
            }));
        }

        // Run 1
        suite.addTest(new InstallerEngineTest("testMidletMessageOk", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testMidletMessageOk();
            }
        }));

        suite.addTest(new InstallerEngineTest("testMidletMessageNok", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testMidletMessageNok();
            }
        }));

        suite.addTest(new InstallerEngineTest("testTrustedJadOk", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testTrustedJadOk();
            }
        }));

        suite.addTest(new InstallerEngineTest("testTrustedJarOk", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testTrustedJarOk();
            }
        }));

        suite.addTest(new InstallerEngineTest("test3AppsOk", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).test3AppsOk();
            }
        }));

        suite.addTest(new InstallerEngineTest("testDownloadJadOk", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testDownloadJadOk();
            }
        }));

        // Run 2
        suite.addTest(new InstallerEngineTest("testDownloadJarOk", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testDownloadJarOk();
            }
        }));

        suite.addTest(new InstallerEngineTest("testDownloadJadHttpRedirectOk", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testDownloadJadHttpRedirectOk();
            }
        }));

        suite.addTest(new InstallerEngineTest("testDownloadJarHttpRedirectOk", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testDownloadJarHttpRedirectOk();
            }
        }));

        suite.addTest(new InstallerEngineTest("testDownloadJadHttpRedirectCountExceeded", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testDownloadJadHttpRedirectCountExceeded();
            }
        }));

        suite.addTest(new InstallerEngineTest("testDownloadJadHttpAuthNok", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testDownloadJadHttpAuthNok();
            }
        }));

        suite.addTest(new InstallerEngineTest("testDownloadJadHttpAuthOk", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testDownloadJadHttpAuthOk();
            }
        }));

        suite.addTest(new InstallerEngineTest("testDownloadJadCancel", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testDownloadJadCancel();
            }
        }));

        suite.addTest(new InstallerEngineTest("testJadNoName", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testJadNoName();
            }
        }));

        suite.addTest(new InstallerEngineTest("testInvalidJar", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testInvalidJar();
            }
        }));

        suite.addTest(new InstallerEngineTest("testInvalidPush", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testInvalidPush();
            }
        }));

        suite.addTest(new InstallerEngineTest("testNoJad", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testNoJad();
            }
        }));

        suite.addTest(new InstallerEngineTest("testNoJar", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testNoJar();
            }
        }));

        suite.addTest(new InstallerEngineTest("testNoJavaBinRoot", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testNoJavaBinRoot();
            }
        }));

        suite.addTest(new InstallerEngineTest("testBlockUninstall", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testBlockUninstall();
            }
        }));

        suite.addTest(new InstallerEngineTest("testOutputCommands", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testOutputCommands();
            }
        }));

        suite.addTest(new InstallerEngineTest("testApplicationInfo", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testApplicationInfo();
            }
        }));

        // Run 3
        suite.addTest(new InstallerEngineTest("testInstallOptions", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testInstallOptions();
            }
        }));

        suite.addTest(new InstallerEngineTest("testInstallAuthAndBig", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerEngineTest)tc).testInstallAuthAndBig();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public InstallerEngineTest()
    {
    }

    public InstallerEngineTest(String aTestName, TestMethod aTestMethod)
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
        iItu = new InstallerTestUtils();
        iItu.init();
        iDefaultOptions = new Vector();
        iDefaultOptions.addElement("-silent");
        iDefaultOptions.addElement("-iap=1");
    }

    protected void tearDown()
    {
        iItu.destroy();
    }

    public void assertResult(int aResult, int aExpectedResult)
    {
        assertTrue("Installer returned " + aResult +
                   ", expected " + aExpectedResult,
                   aResult == aExpectedResult);
    }

    public void assertResult(String aMsg, int aResult, int aExpectedResult)
    {
        assertTrue(aMsg + ": Installer returned " + aResult +
                   ", expected " + aExpectedResult,
                   aResult == aExpectedResult);
    }

    public void assertNoIsDir()
    {
        assertTrue("IntegrityService root dir exists",
                   !FileUtils.exists(FileUtils.getIntegrityServiceRoot()));
    }

    /**
     * Makes installation, update and uninstallation for given application.
     * Does the following steps:
     * install with cancellation and check that app is not installed,
     * install and check that app is installed,
     * update with cancellation and check that app is installed,
     * update and check that app is installed,
     * update again and check that app is installed,
     * uninstall with cancellation and check that app is still installed,
     * uninstall and check that app is no longer installed.
     */
    public void doInstallUninstall(String aFilename, boolean aIsJad, Vector aInstallOptions)
    {
        SuiteInfo suite = null;
        try
        {
            suite = iItu.getNameVendor(aFilename, aIsJad);
            assertTrue("getNameVendor returns null for " + aFilename,
                       suite != null);
        }
        catch (IOException ioe)
        {
            ioe.printStackTrace();
            assertTrue("getNameVendor throws exception for " + aFilename +
                       ": " + ioe, false);
        }
        doInstallUninstall(suite, aFilename, aIsJad, aInstallOptions);
    }

    /**
     * Makes installation, update and uninstallation for given application.
     * Does the following steps:
     * install with cancellation and check that app is not installed,
     * install and check that app is installed,
     * update with cancellation and check that app is installed,
     * update and check that app is installed,
     * uninstall with cancellation and check that app is still installed,
     * uninstall and check that app is no longer installed.
     */
    public void doInstallUninstall(SuiteInfo aSuite, String aFilename, boolean aIsJad, Vector aInstallOptions)
    {
        // Construct install command arguments.
        Vector installOptions = new Vector();
        installOptions.addElement("install");
        if (aIsJad)
        {
            installOptions.addElement("-jad=" + aFilename);
        }
        else
        {
            installOptions.addElement("-jar=" + aFilename);
        }
        if (aInstallOptions != null)
        {
            for (int i = 0; i < aInstallOptions.size(); i++)
            {
                installOptions.addElement(aInstallOptions.elementAt(i));
            }
        }
        String[] args = new String[installOptions.size()];
        installOptions.copyInto(args);
        installOptions.addElement("-forcecancel");
        String[] cancelArgs = new String[installOptions.size()];
        installOptions.copyInto(cancelArgs);

        // Check that suite is not installed.
        assertFalse("exists before installation: " + aFilename, iItu.exists(aSuite));

        // Install with cancellation.
        Log.logOut("InstallerEngineTest.doInstallUninstall: install with cancellation " + aFilename);
        int result = Installer.mainWithResult(cancelArgs);
        // Check result code.
        assertResult(result, Installer.ERR_GENERAL);
        // Check exception.
        assertTrue("cancelled installation returns incorrect exception " +
                   Installer.getExecuteException(),
                   iItu.isExceptionReason
                   (Installer.getExecuteException(),
                    InstallerErrorMessage.INST_UNEXPECTED_ERR,
                    InstallerDetailedErrorMessage.INTERNAL_ERROR,
                    OtaStatusCode.INTERNAL_ERROR));
        // Check that suite is not installed.
        assertFalse("exists after cancelled install: " + aFilename, iItu.exists(aSuite));
        // Check that IntegrityService root dir does not exist.
        assertNoIsDir();

        // Install.
        Log.logOut("InstallerEngineTest.doInstallUninstall: install " + aFilename);
        result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        // Check exception.
        assertTrue("installation returns exception",
                   Installer.getExecuteException() == null);
        assertTrue("installed app uid not found after installation", Installer.iInstalledApps.length > 0);
        // Check that suite is installed.
        assertTrue("does not exist after installation: " + aFilename, iItu.exists(aSuite));
        // Check that IntegrityService root dir does not exist.
        assertNoIsDir();

        // List.
        Log.logOut("InstallerEngineTest.doInstallUninstall: list -v");
        result = Installer.mainWithResult(new String[] { "list", "-v" });
        assertResult(result, Installer.ERR_NONE);
        Log.logOut("InstallerEngineTest.doInstallUninstall: list -otastatus");
        result = Installer.mainWithResult(new String[] { "list", "-otastatus" });
        assertResult(result, Installer.ERR_NONE);

        // Update with cancellation.
        Log.logOut("InstallerEngineTest.doInstallUninstall: update with cancellation " + aFilename);
        result = Installer.mainWithResult(cancelArgs);
        // Check result code.
        assertResult(result, Installer.ERR_GENERAL);
        // Check exception.
        assertTrue("cancelled update returns incorrect exception " +
                   Installer.getExecuteException(),
                   iItu.isExceptionReason
                   (Installer.getExecuteException(),
                    InstallerErrorMessage.INST_UNEXPECTED_ERR,
                    InstallerDetailedErrorMessage.INTERNAL_ERROR,
                    OtaStatusCode.INTERNAL_ERROR));
        // Check that suite is installed.
        assertTrue("does not exists after cancelled update: " + aFilename, iItu.exists(aSuite));
        // Check that IntegrityService root dir does not exist.
        assertNoIsDir();

        // Update.
        Log.logOut("InstallerEngineTest.doInstallUninstall: update " + aFilename);
        result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        // Check exception.
        assertTrue("update returns exception",
                   Installer.getExecuteException() == null);
        assertTrue("installed app uid not found after update", Installer.iInstalledApps.length > 0);
        // Check that suite is installed.
        assertTrue("does not exist after update: " + aFilename, iItu.exists(aSuite));
        // Check that IntegrityService root dir does not exist.
        assertNoIsDir();

        // Update again.
        Log.logOut("InstallerEngineTest.doInstallUninstall: 2nd update " + aFilename);
        result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        // Check exception.
        assertTrue("2nd update returns exception",
                   Installer.getExecuteException() == null);
        assertTrue("installed app uid not found after 2nd update", Installer.iInstalledApps.length > 0);
        // Check that suite is installed.
        assertTrue("does not exist after 2nd update: " + aFilename, iItu.exists(aSuite));
        // Check that IntegrityService root dir does not exist.
        assertNoIsDir();

        // Construct uninstall command arguments.
        Vector uninstallOptions = new Vector();
        uninstallOptions.addElement("uninstall");
        uninstallOptions.addElement("-uid=" + Installer.iInstalledApps[0]);
        if (aInstallOptions != null)
        {
            for (int i = 0; i < aInstallOptions.size(); i++)
            {
                uninstallOptions.addElement(aInstallOptions.elementAt(i));
            }
        }
        args = new String[uninstallOptions.size()];
        uninstallOptions.copyInto(args);
        uninstallOptions.addElement("-forcecancel");
        cancelArgs = new String[uninstallOptions.size()];
        uninstallOptions.copyInto(cancelArgs);

        // Uninstall with cancellation.
        Log.logOut("InstallerEngineTest.doInstallUninstall: uninstall with cancellation " + aFilename);
        result = Installer.mainWithResult(cancelArgs);
        // Check result code.
        assertResult(result, Installer.ERR_GENERAL);
        // Check exception.
        assertTrue("cancelled uninstallation returns incorrect exception " +
                   Installer.getExecuteException(),
                   iItu.isExceptionReason
                   (Installer.getExecuteException(),
                    InstallerErrorMessage.UNINST_UNEXPECTED_ERR,
                    InstallerDetailedErrorMessage.INTERNAL_ERROR,
                    OtaStatusCode.INTERNAL_ERROR));
        // Check that suite is still installed.
        assertTrue("does not exist after cancelled uninstall: " + aFilename, iItu.exists(aSuite));
        // Check that IntegrityService root dir does not exist.
        assertNoIsDir();

        // Uninstall.
        Log.logOut("InstallerEngineTest.doInstallUninstall: uninstall " + aFilename);
        result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        // Check exception.
        assertTrue("installation returns exception",
                   Installer.getExecuteException() == null);
        // Check that suite is no longer installed.
        assertTrue("exists after uninstallation: " + aFilename, iItu.doesNotExist(aSuite));
        // Check that IntegrityService root dir does not exist.
        assertNoIsDir();
    }

    public void testTrustedJadNonSilentOk()
    {
        Log.logOut("InstallerEngineTest.testTrustedJadNonSilentOk begins");
        doInstallUninstall(iItu.getTestDir() + "HelloWorld.jad", true, null);
    }

    public void testMidletMessageOk()
    {
        Log.logOut("InstallerEngineTest.testMidletMessageOk begins");
        doInstallUninstall(new SuiteInfo("HelloWorld", "Nokia"),
                           iItu.getTestDir() + "helloworldmm.jar",
                           false, iDefaultOptions);
    }

    public void testMidletMessageNok()
    {
        Log.logOut("InstallerEngineTest.testMidletMessageNok begins");
        String[] args = new String[]
        {
            "install",
            "-silent",
            "-jar=" + iItu.getTestDir() + "helloworldmm_invalid.jar",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_GENERAL);
        // Check exception
        assertTrue("failed installation returns incorrect exception " +
                   Installer.getExecuteException(),
                   iItu.isExceptionReason
                   (Installer.getExecuteException(),
                    InstallerErrorMessage.INST_CORRUPT_PKG,
                    InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                    OtaStatusCode.INVALID_JAR));
        assertTrue("installed app uid should not exist", Installer.iInstalledApps.length == 0);
    }

    public void testTrustedJadOk()
    {
        Log.logOut("InstallerEngineTest.testTrustedJadOk begins");
        doInstallUninstall(iItu.getTestDir() + "HelloWorld.jad", true, iDefaultOptions);
    }

    public void testTrustedJarOk()
    {
        Log.logOut("InstallerEngineTest.testTrustedJarOk begins");
        doInstallUninstall(iItu.getTestDir() + "HelloWorld.jar", false, iDefaultOptions);
    }

    public void test3AppsOk()
    {
        Log.logOut("InstallerEngineTest.test3AppsOk begins");
        doInstallUninstall(iItu.getTestDir() + "HelloWorld_3_midlets.jad", true, iDefaultOptions);
    }

    public void testDownloadJadOk()
    {
        Log.logOut("InstallerEngineTest.testDownloadJadOk begins");
        SuiteInfo suite = new SuiteInfo("JBenchmark2", "Kishonti LP");
        doInstallUninstall(suite, "http://195.134.231.83:7070/omjserver/resources/omj/T00000900_JBenchmark2_gcf.jsp", true, iDefaultOptions);
    }

    public void testDownloadJarOk()
    {
        Log.logOut("InstallerEngineTest.testDownloadJarOk begins");
        SuiteInfo suite = new SuiteInfo("JBenchmark2", "Kishonti LP");
        doInstallUninstall(suite, "http://195.134.231.83:7070/omjserver/resources/omj/JBenchmark2.jar", false, iDefaultOptions);
    }

    public void testDownloadJadHttpRedirectOk()
    {
        Log.logOut("InstallerEngineTest.testDownloadJadHttpRedirectOk begins");
        SuiteInfo suite = new SuiteInfo("JBenchmark2", "Kishonti LP");
        doInstallUninstall(suite, "http://195.134.231.83:7070/RedirectServlet/redirect?name=JBenchmark2.jad&count=5", true, iDefaultOptions);
    }

    public void testDownloadJarHttpRedirectOk()
    {
        Log.logOut("InstallerEngineTest.testDownloadJarHttpRedirectOk begins");
        SuiteInfo suite = new SuiteInfo("JBenchmark2", "Kishonti LP");
        doInstallUninstall(suite, "http://195.134.231.83:7070/RedirectServlet/redirect?name=JBenchmark2.jar", false, iDefaultOptions);
    }

    public void testDownloadJadHttpAuthOk()
    {
        Log.logOut("InstallerEngineTest.testDownloadJadHttpAuthOk begins");
        SuiteInfo suite = new SuiteInfo("DS_Snow_http_auth", "DS_Effects");
        iDefaultOptions.addElement("-username=guest");
        iDefaultOptions.addElement("-password=guest");
        doInstallUninstall(suite, "http://195.134.231.83:7070/httpauth/DS_Snow_http_auth.jad", true, iDefaultOptions);
    }

    public void testDownloadJadHttpAuthNok()
    {
        Log.logOut("InstallerEngineTest.testDownloadJadHttpAuthNok begins");
        String[] args = new String[]
        {
            "install",
            "-silent",
            "-jad=http://195.134.231.83:7070/httpauth/DS_Snow_http_auth.jad",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_CANCEL);
        // Check exception
        assertTrue("failed installation returns incorrect exception " +
                   Installer.getExecuteException(),
                   iItu.isExceptionReason
                   (Installer.getExecuteException(),
                    InstallerErrorMessage.INST_CANCEL,
                    InstallerDetailedErrorMessage.NO_MSG,
                    OtaStatusCode.USER_CANCELLED));
        assertTrue("installed app uid should not exist", Installer.iInstalledApps.length == 0);
    }

    public void testDownloadJadHttpRedirectCountExceeded()
    {
        Log.logOut("InstallerEngineTest.testDownloadJadHttpRedirectCountExceeded begins");
        String[] args = new String[]
        {
            "install",
            "-silent",
            "-jad=http://195.134.231.83:7070/RedirectServlet/redirect?name=JBenchmark2.jad&count=6",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_GENERAL);
        // Check exception
        assertTrue("failed installation returns incorrect exception " +
                   Installer.getExecuteException(),
                   iItu.isExceptionReason
                   (Installer.getExecuteException(),
                    InstallerErrorMessage.INST_NO_NET,
                    InstallerDetailedErrorMessage.NO_NET,
                    OtaStatusCode.LOSS_OF_SERVICE));
        assertTrue("installed app uid should not exist", Installer.iInstalledApps.length == 0);
    }

    public void testDownloadJadCancel()
    {
        Log.logOut("InstallerEngineTest.testDownloadJadCancel begins");
        Log.log("InstallerEngineTest.testDownloadJadCancel: start");
        // Create a thread which sleeps a moment and then cancels installation.
        new Thread(new Runnable()
        {
            public void run()
            {
                Log.log("InstallerEngineTest.testDownloadJadCancel: canceler thread starts to wait");
                try
                {
                    Thread.sleep(12000);
                }
                catch (InterruptedException ie)
                {
                }
                Log.log("InstallerEngineTest.testDownloadJadCancel: canceler thread calls cancel");
                Installer.cancel();
            }
        }).start();

        // Start installation which downloads a large file.
        String[] args = new String[]
        {
            "install",
            "-silent",
            "-jad=http://195.134.231.83:7070/omjserver/resources/omj/T00000900_JBenchmark2_8mb_gcf.jsp",
        };
        Log.log("InstallerEngineTest.testDownloadJadCancel: installation starts");
        int result = Installer.mainWithResult(args);
        Log.log("InstallerEngineTest.testDownloadJadCancel: installation returned");
        // Check result code.
        assertResult(result, Installer.ERR_CANCEL);
        // Check exception
        assertTrue("failed installation returns incorrect exception " +
                   Installer.getExecuteException(),
                   iItu.isExceptionReason
                   (Installer.getExecuteException(),
                    InstallerErrorMessage.INST_CANCEL,
                    InstallerDetailedErrorMessage.NO_MSG,
                    OtaStatusCode.USER_CANCELLED));
        assertTrue("installed app uid should not exist", Installer.iInstalledApps.length == 0);
        Log.log("InstallerEngineTest.testDownloadJadCancel: end");
    }

    public void testNoJad()
    {
        Log.logOut("InstallerEngineTest.testNoJad begins");
        String[] args = new String[]
        {
            "install",
            "-silent",
            "-jad=" + iItu.getTestDir() + "HelloWorld_nonexisting.jad",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_GENERAL);
        // Check exception
        assertTrue("failed installation returns incorrect exception " +
                   Installer.getExecuteException(),
                   iItu.isExceptionReason
                   (Installer.getExecuteException(),
                    InstallerErrorMessage.INST_CORRUPT_PKG,
                    InstallerDetailedErrorMessage.INTERNAL_ERROR,
                    OtaStatusCode.INVALID_DESCRIPTOR));
        assertTrue("installed app uid should not exist", Installer.iInstalledApps.length == 0);
    }

    public void testNoJar()
    {
        Log.logOut("InstallerEngineTest.testNoJar begins");
        String[] args = new String[]
        {
            "install",
            "-silent",
            "-jar=" + iItu.getTestDir() + "HelloWorld_nonexisting.jar",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_GENERAL);
        // Check exception
        assertTrue("failed installation returns incorrect exception " +
                   Installer.getExecuteException(),
                   iItu.isExceptionReason
                   (Installer.getExecuteException(),
                    InstallerErrorMessage.INST_CORRUPT_PKG,
                    InstallerDetailedErrorMessage.INTERNAL_ERROR,
                    OtaStatusCode.INVALID_JAR));
        assertTrue("installed app uid should not exist", Installer.iInstalledApps.length == 0);
    }

    public void testJadNoName()
    {
        Log.logOut("InstallerEngineTest.testJadNoName begins");
        String[] args = new String[]
        {
            "install",
            "-silent",
            "-jad=" + iItu.getTestDir() + "HelloWorld_NoName.jad",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_GENERAL);
        // Check exception
        assertTrue("failed installation returns incorrect exception " +
                   Installer.getExecuteException(),
                   iItu.isExceptionReason
                   (Installer.getExecuteException(),
                    InstallerErrorMessage.INST_CORRUPT_PKG,
                    InstallerDetailedErrorMessage.ATTR_MISSING,
                    OtaStatusCode.INVALID_DESCRIPTOR));
        assertTrue("installed app uid should not exist", Installer.iInstalledApps.length == 0);
    }

    public void testInvalidJar()
    {
        Log.logOut("InstallerEngineTest.testInvalidJar begins");
        String[] args = new String[]
        {
            "install",
            "-silent",
            "-jar=" + iItu.getTestDir() + "HelloWorld_NoName.jad",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_GENERAL);
        // Check exception
        assertTrue("failed installation returns incorrect exception " +
                   Installer.getExecuteException(),
                   iItu.isExceptionReason
                   (Installer.getExecuteException(),
                    InstallerErrorMessage.INST_CORRUPT_PKG,
                    InstallerDetailedErrorMessage.INTERNAL_ERROR,
                    OtaStatusCode.INVALID_JAR));
        assertTrue("installed app uid should not exist", Installer.iInstalledApps.length == 0);
    }

    public void testInvalidPush()
    {
        Log.logOut("InstallerEngineTest.testInvalidPush begins");
        String[] args = new String[]
        {
            "install",
            "-silent",
            "-jad=" + iItu.getTestDir() + "HelloWorld_invalid_push.jad",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_GENERAL);
        // Check exception
        assertTrue("failed installation returns incorrect exception " +
                   Installer.getExecuteException(),
                   iItu.isExceptionReason
                   (Installer.getExecuteException(),
                    InstallerErrorMessage.INST_PUSH_REG_ERR,
                    InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                    OtaStatusCode.PUSH_REGISTRATION_FAILURE));
        assertTrue("installed app uid should not exist", Installer.iInstalledApps.length == 0);
    }

    public void testNoJavaBinRoot()
    {
        Log.logOut("InstallerEngineTest.testNoJavaBinRoot begins");
        String[] args = new String[]
        {
            "install",
            "-silent",
            "-jad=" + iItu.getTestDir() + "HelloWorld_nonexisting.jad",
        };
        String javaBinRootName = "JAVA_BIN_ROOT";
        String oldJavaBinRoot = System.getProperty(javaBinRootName);
        System.setProperty(javaBinRootName, "");
        int result = Installer.mainWithResult(args);
        System.setProperty(javaBinRootName, oldJavaBinRoot);
        // Check result code.
        assertResult(result, Installer.ERR_NOT_FOUND);
        // Check exception
        assertTrue("installation returned exception " +
                   Installer.getExecuteException(),
                   Installer.getExecuteException() == null);
        assertTrue("installed app uid should not exist", Installer.iInstalledApps.length == 0);
    }

    public void testBlockUninstall()
    {
        Log.logOut("InstallerEngineTest.testBlockUninstall begins");
        // Get suite info for block uninstall test application.
        String filename = iItu.getTestDir() + "HelloWorld_block_uninstall.jad";
        SuiteInfo suite = null;
        try
        {
            suite = iItu.getNameVendor(filename, true);
            assertTrue("getNameVendor returns null for " + filename,
                       suite != null);
        }
        catch (IOException ioe)
        {
            ioe.printStackTrace();
            assertTrue("getNameVendor throws exception for " + filename +
                       ": " + ioe, false);
        }

        Log.logOut("InstallerEngineTest.testBlockUninstall: Install application whose uninstallation is blocked.");
        String[] args = new String[]
        {
            "install", "-silent",
            "-jad=" + filename,
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult("Installing block uninstall app failed",
                     result, Installer.ERR_NONE);
        // Check exception.
        assertTrue("installation returns exception",
                   Installer.getExecuteException() == null);
        assertTrue("installed app uid not found after installation", Installer.iInstalledApps.length > 0);
        // Check that suite is installed.
        assertTrue("does not exist after installation", iItu.exists(suite));
        // Check that IntegrityService root dir does not exist.
        assertNoIsDir();

        // Save installed application uid.
        Uid appUid = Installer.iInstalledApps[0];

        // List application info.
        Log.logOut("InstallerEngineTest.testBlockUninstall: list -v");
        result = Installer.mainWithResult(new String[] { "list", "-v" });
        assertResult(result, Installer.ERR_NONE);

        Log.logOut("InstallerEngineTest.testBlockUninstall: Try to uninstall application whose uninstallation is blocked.");
        String[] uninstallArgs = new String[]
        {
            "uninstall", "-silent", "-uid=" + appUid,
        };
        result = Installer.mainWithResult(uninstallArgs);
        // Check result code.
        assertResult("Uninstalling app whose uninstallation was blocked did not fail",
                     result, Installer.ERR_GENERAL);
        // Check exception
        assertTrue("blocked uninstallation returns incorrect exception " +
                   Installer.getExecuteException(),
                   iItu.isExceptionReason
                   (Installer.getExecuteException(),
                    InstallerErrorMessage.UNINST_NOT_ALLOWED,
                    InstallerDetailedErrorMessage.UNINST_BLOCKED,
                    OtaStatusCode.USER_CANCELLED));
        assertTrue("installed app uid should not exist", Installer.iInstalledApps.length == 0);
        // Check that suite is installed.
        assertTrue("does not exist after blocked uninstallation", iItu.exists(suite));
        // Check that IntegrityService root dir does not exist.
        assertNoIsDir();

        Log.logOut("InstallerEngineTest.testBlockUninstall: Upgrade application with a version whose uninstallation is not blocked.");
        args = new String[]
        {
            "install", "-silent",
            "-jad=" + iItu.getTestDir() + "HelloWorld_unblock_uninstall.jad",
        };
        result = Installer.mainWithResult(args);
        // Check result code.
        assertResult("Upgrading app whose uninstallation was blocked failed",
                     result, Installer.ERR_NONE);
        // Check exception.
        assertTrue("installation returns exception",
                   Installer.getExecuteException() == null);
        assertTrue("installed app uid not found after installation", Installer.iInstalledApps.length > 0);
        // Check that suite is installed.
        assertTrue("does not exist after installation", iItu.exists(suite));
        // Check that IntegrityService root dir does not exist.
        assertNoIsDir();
        // Check that uid is the same after upgrade.
        assertTrue("appUid differs after upgrade, old=" + appUid +
                   ", new=" + Installer.iInstalledApps[0],
                   appUid.equals(Installer.iInstalledApps[0]));

        // List application info.
        Log.logOut("InstallerEngineTest.testBlockUninstall: list -v");
        result = Installer.mainWithResult(new String[] { "list", "-v" });
        assertResult(result, Installer.ERR_NONE);

        Log.logOut("InstallerEngineTest.testBlockUninstall: Uninstall application whose uninstallation is no longer blocked.");
        result = Installer.mainWithResult(uninstallArgs);
        // Check result code.
        assertResult("Uninstalling app whose uninstallation is no longer blocked failed",
                     result, Installer.ERR_NONE);
        // Check exception
        assertTrue("blocked uninstallation returns exception",
                   Installer.getExecuteException() == null);
        assertTrue("installed app uid should not exist", Installer.iInstalledApps.length == 0);
        // Check that suite is uninstalled.
        assertTrue("exists after unblocked uninstallation", !iItu.exists(suite));
        // Check that IntegrityService root dir does not exist.
        assertNoIsDir();

        // List application info.
        Log.logOut("InstallerEngineTest.testBlockUninstall: list -v");
        result = Installer.mainWithResult(new String[] { "list", "-v" });
        assertResult(result, Installer.ERR_NONE);

    }

    public void testOutputCommands()
    {
        Log.logOut("InstallerEngineTest.testOutputCommands begins");
        // Some parameter combinations which cause an error to be returned.
        int result = Installer.mainWithResult(new String[] { });
        assertResult(result, Installer.ERR_NOT_FOUND);
        result = Installer.mainWithResult(new String[] { "list", "-nosystemexit" });
        assertResult(result, Installer.RET_NO_SYSTEM_EXIT);
        result = Installer.mainWithResult(new String[] { "list", "-storage=APPLICATION_PACKAGE_TABLE" });
        assertResult(result, Installer.ERR_GENERAL);

        // Parameter combinations that can be executed successfully.
        callInstallerOk(new String[] { "unknowncommand" });
        callInstallerOk(new String[] { "version" });
        callInstallerOk(new String[] { "help" });
        callInstallerOk(new String[] { "help", "unknowncommand" });
        callInstallerOk(new String[] { "help", "help" });
        callInstallerOk(new String[] { "help", "version" });
        callInstallerOk(new String[] { "help", "install" });
        callInstallerOk(new String[] { "help", "uninstall" });
        callInstallerOk(new String[] { "help", "uninstallall" });
        callInstallerOk(new String[] { "help", "list" });
        callInstallerOk(new String[] { "help", "launch" });
        callInstallerOk(new String[] { "help", "poll" });
        callInstallerOk(new String[] { "help", "rollback" });
        callInstallerOk(new String[] { "help", "test" });
        callInstallerOk(new String[] { "list" });
        callInstallerOk(new String[] { "list", "-v" });
        callInstallerOk(new String[] { "list", "-storage" });
        callInstallerOk(new String[] { "list", "-otastatus" });
        callInstallerOk(new String[] { "list", "-systemproperties" });
        callInstallerOk(new String[] { "launch", "-nouid" });
        callInstallerOk(new String[] { "launch", "-uid=invaliduid" });
        callInstallerOk(new String[] { "uninstallall", "-silent" });
        callInstallerOk(new String[] { "unregister" });
        callInstallerOk(new String[] { "test", "-nomainclass" });
    }

    public void testApplicationInfo()
    {
        Log.logOut("InstallerEngineTest.testApplicationInfo begins");
        ApplicationInfo applicationInfo = ApplicationInfo.getInstance();
        assertTrue("ApplicationInfo.getName returns incorrect value " +
                   applicationInfo.getName(),
                   "JavaInstaller".equals(applicationInfo.getName()));
        assertTrue("ApplicationInfo.getSuiteName returns incorrect value " +
                   applicationInfo.getSuiteName(),
                   "JavaInstaller".equals(applicationInfo.getSuiteName()));
        assertTrue("ApplicationInfo.getVendor returns incorrect value " +
                   applicationInfo.getVendor(),
                   "Nokia".equals(applicationInfo.getVendor()));
        assertTrue("ApplicationInfo.getRuntimeType returns incorrect value " +
                   applicationInfo.getRuntimeType(),
                   "INSTALLER".equals(applicationInfo.getRuntimeType()));
        assertTrue("ApplicationInfo.getProtectionDomain returns incorrect value " +
                   applicationInfo.getProtectionDomain(),
                   ApplicationInfo.MANUFACTURER_DOMAIN.equals
                   (applicationInfo.getProtectionDomain()));
        assertTrue("ApplicationInfo.getUid returns incorrect value " +
                   applicationInfo.getUid(),
                   PlatformUid.createUid("0x2001843A").equals
                   (applicationInfo.getUid()));
        assertTrue("ApplicationInfo.getSuiteUid returns incorrect value " +
                   applicationInfo.getSuiteUid(),
                   applicationInfo.getUid().equals
                   (applicationInfo.getSuiteUid()));
        assertTrue("ApplicationInfo.getAttribute returns incorrect value " +
                   applicationInfo.getAttribute(null),
                   applicationInfo.getAttribute(null) == null);

        try
        {
            String mainClass = applicationInfo.getMainClass();
            assertTrue("ApplicationInfo.getMainClass did not throw exception", false);
        }
        catch (RuntimeException rte)
        {
            // expected exception
        }
        catch (Exception ex)
        {
            assertTrue("ApplicationInfo.getMainClass threw unexpected exception: " + ex, false);
        }
    }

    public void testInstallOptions()
    {
        Log.logOut("InstallerEngineTest.testInstallOptions begins");
        // Install with all possible options.
        String[] args = new String[]
        {
            "install",
            "-silent",
            "-forcecancel",
            "-jad=" + iItu.getTestDir() + "HelloWorld.jad",
            "-sourceurl=http://nohost/HelloWorld.jad",
            "-charset=UTF-8",
            "-iap=1",
            "-drive=C",
            "-upgrade=yes",
            "-upgrade_data=no",
            "-overwrite=yes",
            "-untrusted=yes",
            "-ocsp=yes",
            "-ignore_ocsp_warnings=no",
            "-download=no",
            "-username=username",
            "-password=password",
            "-proxyhost=proxyhost",
            "-proxyport=proxyport",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_GENERAL);
        // Check exception
        assertTrue("failed installation returns incorrect exception " +
                   Installer.getExecuteException(),
                   iItu.isExceptionReason
                   (Installer.getExecuteException(),
                    InstallerErrorMessage.INST_UNEXPECTED_ERR,
                    InstallerDetailedErrorMessage.INTERNAL_ERROR,
                    OtaStatusCode.INTERNAL_ERROR));
        assertTrue("installed app uid should not exist", Installer.iInstalledApps.length == 0);

        // Install with both jad and jar options should fail.
        args = new String[]
        {
            "install",
            "-silent",
            "-jad=" + iItu.getTestDir() + "HelloWorld.jad",
            "-jar=" + iItu.getTestDir() + "HelloWorld.jar",
        };
        result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_GENERAL);
        // Check exception
        assertTrue("failed installation returns incorrect exception " +
                   Installer.getExecuteException(),
                   iItu.isExceptionReason
                   (Installer.getExecuteException(),
                    InstallerErrorMessage.INST_UNEXPECTED_ERR,
                    InstallerDetailedErrorMessage.INTERNAL_ERROR,
                    OtaStatusCode.INTERNAL_ERROR));
        assertTrue("installed app uid should not exist", Installer.iInstalledApps.length == 0);
    }

    /**
     * Tests subsequent application installations. First installs an application
     * which requires authentication and then downloads a big application.
     */
    public void testInstallAuthAndBig()
    {
        Log.logOut("InstallerEngineTest.testInstallAuthAndBig begins");
        // Do first installation with doInstallUninstall helper
        SuiteInfo suite = new SuiteInfo("DS_Snow_http_auth", "DS_Effects");
        iDefaultOptions.addElement("-username=guest");
        iDefaultOptions.addElement("-password=guest");
        doInstallUninstall(suite, "http://195.134.231.83:7070/httpauth/DS_Snow_http_auth.jad", true, iDefaultOptions);

        // Then start installation which downloads a large file.
        Log.log("InstallerEngineTest.testInstallAuthAndBig begin to install big app");
        String[] args = new String[]
        {
            "install",
            "-silent",
            "-jad=http://195.134.231.83:7070/omjserver/resources/omj/T00000900_JBenchmark2_8mb_gcf.jsp",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        // Check exception
        assertTrue("installation returned exception " +
                   Installer.getExecuteException(),
                   Installer.getExecuteException() == null);
        assertTrue("installed app uid should exist", Installer.iInstalledApps.length == 1);
    }

    private int callInstallerOk(String[] aArgs)
    {
        int result = Installer.mainWithResult(aArgs);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        // Check exception
        assertTrue("installation returned exception " +
                   Installer.getExecuteException(),
                   Installer.getExecuteException() == null);
        assertTrue("installed app uid should not exist", Installer.iInstalledApps.length == 0);
        return result;
    }
}
