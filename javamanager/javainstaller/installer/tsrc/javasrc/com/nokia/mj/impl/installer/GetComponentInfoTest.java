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


package com.nokia.mj.impl.installer;

import com.nokia.mj.impl.comms.CommsEndpoint;
import com.nokia.mj.impl.comms.CommsServerEndpoint;
import com.nokia.mj.impl.comms.CommsListener;
import com.nokia.mj.impl.comms.CommsMessage;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.Platform;

import java.io.IOException;
import java.util.Vector;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * Test cases for GetComponentInfo.
 */
public class GetComponentInfoTest extends TestCase implements InstallerMain
{
    // Comms endpoint for the tests.
    private static final int TEST_ENDPOINT = 11000;

    private static String iInstalledApp = null;
    private CommsServerEndpoint iComms = null;
    private InstallerResultMessage iMsg = null;
    private InstallerTestUtils iItu = null;
    private Vector iDefaultOptions = null;

    // Begin j2meunit test framework setup

    public void installerMain(String[] aArgs)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new GetComponentInfoTest("testComponentInfoJadNotFound", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetComponentInfoTest)tc).testComponentInfoJadNotFound();
            }
        }));

        suite.addTest(new GetComponentInfoTest("testComponentInfoJarNotFound", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetComponentInfoTest)tc).testComponentInfoJarNotFound();
            }
        }));

        suite.addTest(new GetComponentInfoTest("testComponentInfoUidNotFound", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetComponentInfoTest)tc).testComponentInfoUidNotFound();
            }
        }));

        suite.addTest(new GetComponentInfoTest("testComponentInfoJadNoName", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetComponentInfoTest)tc).testComponentInfoJadNoName();
            }
        }));

        suite.addTest(new GetComponentInfoTest("testComponentInfoJadNoJar", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetComponentInfoTest)tc).testComponentInfoJadNoJar();
            }
        }));

        suite.addTest(new GetComponentInfoTest("testComponentInfoJad3Ok", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetComponentInfoTest)tc).testComponentInfoJad3Ok();
            }
        }));

        suite.addTest(new GetComponentInfoTest("testComponentInfoJadOk", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetComponentInfoTest)tc).testComponentInfoJadOk();
            }
        }));

        suite.addTest(new GetComponentInfoTest("testComponentInfoJarOk", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetComponentInfoTest)tc).testComponentInfoJarOk();
            }
        }));

        suite.addTest(new GetComponentInfoTest("testComponentInfoMmNok", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetComponentInfoTest)tc).testComponentInfoMmNok();
            }
        }));

        suite.addTest(new GetComponentInfoTest("testComponentInfoMmOk", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetComponentInfoTest)tc).testComponentInfoMmOk();
            }
        }));

        suite.addTest(new GetComponentInfoTest("testComponentInfoUidOk", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetComponentInfoTest)tc).testComponentInfoUidOk();
            }
        }));

        suite.addTest(new GetComponentInfoTest("testComponentInfoJadInstalledOk", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetComponentInfoTest)tc).testComponentInfoJadInstalledOk();
            }
        }));

        suite.addTest(new GetComponentInfoTest("testComponentInfoUidAfterUninstallation", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetComponentInfoTest)tc).testComponentInfoUidAfterUninstallation();
            }
        }));

        if (Platform.isS60())
        {
            suite.addTest(new GetComponentInfoTest("testComponentInfoJadInboxOk", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((GetComponentInfoTest)tc).testComponentInfoJadInboxOk();
                }
            }));

            suite.addTest(new GetComponentInfoTest("testComponentInfoJarInboxOk", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((GetComponentInfoTest)tc).testComponentInfoJarInboxOk();
                }
            }));

            suite.addTest(new GetComponentInfoTest("testComponentInfoMmInboxOk", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((GetComponentInfoTest)tc).testComponentInfoMmInboxOk();
                }
            }));
        }

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public GetComponentInfoTest()
    {
    }

    public GetComponentInfoTest(String aTestName, TestMethod aTestMethod)
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
        iComms = new CommsServerEndpoint();
        iComms.start(TEST_ENDPOINT);
        iMsg = null;
        iItu = new InstallerTestUtils();
        iItu.init();
    }

    protected void tearDown()
    {
        if (iItu != null)
        {
            iItu.destroy();
        }
        if (iComms != null)
        {
            iComms.stop();
            iComms.destroy();
            iComms = null;
        }
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

    public void testComponentInfoJadNotFound()
    {
        Log.log("GetComponentInfoTest.testComponentInfoJadNotFound begins");
        iComms.registerListener(0, new ResultListener());
        String[] args = new String[]
        {
            "componentinfo",
            "-commsresult=" + TEST_ENDPOINT,
            "-jad=" + iItu.getTestDir() + "nonexisting.jad",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NOT_FOUND);
        // Check result
        assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 2);
        assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_NOT_FOUND);
    }

    public void testComponentInfoJarNotFound()
    {
        Log.log("GetComponentInfoTest.testComponentInfoJarNotFound begins");
        iComms.registerListener(0, new ResultListener());
        String[] args = new String[]
        {
            "componentinfo",
            "-commsresult=" + TEST_ENDPOINT,
            "-jad=" + iItu.getTestDir() + "nonexisting.jar",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NOT_FOUND);
        // Check result
        assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 2);
        assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_NOT_FOUND);
    }

    public void testComponentInfoUidNotFound()
    {
        Log.log("GetComponentInfoTest.testComponentInfoUidNotFound begins");
        iComms.registerListener(0, new ResultListener());
        String[] args = new String[]
        {
            "componentinfo",
            "-commsresult=" + TEST_ENDPOINT,
            "-uid=" + (Platform.isS60()? "0xe0000000": "0"),
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NOT_FOUND);
        // Check result
        assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 2);
        assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_NOT_FOUND);
    }

    public void testComponentInfoJadNoName()
    {
        Log.log("GetComponentInfoTest.testComponentInfoJadNoName begins");
        iComms.registerListener(0, new ResultListener());
        String[] args = new String[]
        {
            "componentinfo",
            "-commsresult=" + TEST_ENDPOINT,
            "-jad=" + iItu.getTestDir() + "HelloWorld_NoName.jad",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        try
        {
            // Check result message
            assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 2);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_NONE);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_INSTALL_STATUS, GetComponentInfo.INVALID_PACKAGE);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_AUTHENTICITY);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_SUITE_NAME);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_VENDOR);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_VERSION);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_SUITE_GID);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_SUITE_UID);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_GID+1);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+1);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    public void testComponentInfoJadNoJar()
    {
        Log.log("GetComponentInfoTest.testComponentInfoJadNoJar begins");
        iComms.registerListener(0, new ResultListener());
        String[] args = new String[]
        {
            "componentinfo",
            "-commsresult=" + TEST_ENDPOINT,
            "-jad=" + iItu.getTestDir() + "HelloWorld_no_jar.jad",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        try
        {
            // Check result message
            assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 2);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_NONE);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_INSTALL_STATUS, GetComponentInfo.NEW_COMPONENT);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_AUTHENTICITY, 0);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_NAME, "HelloWorld_no_jar");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VENDOR, "Nokia");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VERSION, "1.0");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_GID, "Nokia/HelloWorld_no_jar");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_SUITE_UID);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_GID+1, "Nokia/HelloWorld_no_jar/HelloWorld");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+1, "HelloWorld");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_UID+1);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    public void testComponentInfoJadOk()
    {
        Log.log("GetComponentInfoTest.testComponentInfoJadOk begins");
        iComms.registerListener(0, new ResultListener());
        String[] args = new String[]
        {
            "componentinfo",
            "-commsresult=" + TEST_ENDPOINT,
            "-jad=" + iItu.getTestDir() + "HelloWorld.jad",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        try
        {
            // Check result message
            assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 2);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_NONE);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_INSTALL_STATUS, GetComponentInfo.NEW_COMPONENT);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_AUTHENTICITY, 1);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_NAME, "HelloWorld");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VENDOR, "Nokia");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VERSION, "1.0");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_GID, "Nokia/HelloWorld");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_GID+1, "Nokia/HelloWorld/HelloWorld");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+1, "HelloWorld");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_GID+2);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_UID+2);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+2);
            if (Platform.isS60())
            {
                assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_SUITE_UID);
                assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_UID+1);
            }
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    public void testComponentInfoJad3Ok()
    {
        Log.log("GetComponentInfoTest.testComponentInfoJad3Ok begins");
        iComms.registerListener(0, new ResultListener());
        String[] args = new String[]
        {
            "componentinfo",
            "-commsresult=" + TEST_ENDPOINT,
            "-jad=" + iItu.getTestDir() + "HelloWorld_3_midlets_no_ota_status.jad",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        try
        {
            // Check result message
            assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 2);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_NONE);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_INSTALL_STATUS, GetComponentInfo.NEW_COMPONENT);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_AUTHENTICITY, 0);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_NAME, "HelloWorld");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VENDOR, "Nokia");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VERSION, "1.0");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_GID, "Nokia/HelloWorld");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_SUITE_UID);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_GID+1, "Nokia/HelloWorld/HelloWorld");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+1, "HelloWorld");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_GID+2, "Nokia/HelloWorld/HelloWorld2");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+2, "HelloWorld2");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_UID+2);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_GID+3, "Nokia/HelloWorld/HelloWorld3");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+3, "HelloWorld3");
            if (Platform.isS60())
            {
                assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_UID+1, 0xefef0001);
                assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_UID+3, 0xefef0003);
            }
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    public void testComponentInfoJarOk()
    {
        Log.log("GetComponentInfoTest.testComponentInfoJarOk begins");
        iComms.registerListener(0, new ResultListener());
        String[] args = new String[]
        {
            "componentinfo",
            "-commsresult=" + TEST_ENDPOINT,
            "-jar=" + iItu.getTestDir() + "HelloWorld.jar",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        try
        {
            // Check result message
            assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 2);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_NONE);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_INSTALL_STATUS, GetComponentInfo.NEW_COMPONENT);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_AUTHENTICITY, 1);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_NAME, "HelloWorld");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VENDOR, "Nokia");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VERSION, "1.0");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_GID, "Nokia/HelloWorld");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_SUITE_UID);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_GID+1, "Nokia/HelloWorld/HelloWorld");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_UID+1);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+1, "HelloWorld");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_GID+2);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_UID+2);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+2);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    public void testComponentInfoMmNok()
    {
        Log.log("GetComponentInfoTest.testComponentInfoMmNok begins");
        iComms.registerListener(0, new ResultListener());
        String[] args = new String[]
        {
            "componentinfo",
            "-commsresult=" + TEST_ENDPOINT,
            "-jar=" + iItu.getTestDir() + "helloworldmm_invalid.jar",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_GENERAL);
        try
        {
            // Check result message
            assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 2);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_GENERAL);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_INSTALL_STATUS);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_AUTHENTICITY);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_SUITE_NAME);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_VENDOR);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_VERSION);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_SUITE_GID);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_SUITE_UID);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_GID+1);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_UID+1);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+1);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_GID+2);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_UID+2);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+2);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    public void testComponentInfoMmOk()
    {
        Log.log("GetComponentInfoTest.testComponentInfoMmOk begins");
        iComms.registerListener(0, new ResultListener());
        String[] args = new String[]
        {
            "componentinfo",
            "-commsresult=" + TEST_ENDPOINT,
            "-jar=" + iItu.getTestDir() + "helloworldmm.jar",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        try
        {
            // Check result message
            assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 2);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_NONE);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_INSTALL_STATUS, GetComponentInfo.NEW_COMPONENT);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_AUTHENTICITY, 1);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_NAME, "HelloWorld");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VENDOR, "Nokia");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VERSION, "1.0");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_GID, "Nokia/HelloWorld");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_SUITE_UID);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_GID+1, "Nokia/HelloWorld/HelloWorld");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_UID+1);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+1, "HelloWorld");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_GID+2);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_UID+2);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+2);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    public void testComponentInfoUidOk()
    {
        Log.log("GetComponentInfoTest.testComponentInfoUidOk begins");
        iComms.registerListener(0, new ResultListener());

        // Install test application.
        String[] args = new String[]
        {
            "install",
            "-silent",
            "-commsresult=" + TEST_ENDPOINT,
            "-jad=" + iItu.getTestDir() + "HelloWorld_3_midlets_no_ota_status.jad",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        // Save the uid of the installed application.
        iInstalledApp = Installer.iInstalledApps[0].toString();

        // Get component info for the installed application.
        args = new String[]
        {
            "componentinfo",
            "-commsresult=" + TEST_ENDPOINT,
            "-uid=" + iInstalledApp,
        };
        result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        try
        {
            // Check result message
            assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 2);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_NONE);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_INSTALL_STATUS, GetComponentInfo.ALREADY_INSTALLED);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_AUTHENTICITY, 0);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_NAME, "HelloWorld");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VENDOR, "Nokia");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VERSION, "1.0");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_GID, "Nokia/HelloWorld");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_GID+1, "Nokia/HelloWorld/HelloWorld");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+1, "HelloWorld");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_GID+2, "Nokia/HelloWorld/HelloWorld2");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+2, "HelloWorld2");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_GID+3, "Nokia/HelloWorld/HelloWorld3");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+3, "HelloWorld3");
            if (Platform.isS60())
            {
                assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_UID, 0x101b2bd2);
                assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_UID+1, 0xefef0001);
                assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_UID+2, 0x101631a4);
                assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_UID+3, 0xefef0003);
            }
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    public void testComponentInfoJadInstalledOk()
    {
        Log.log("GetComponentInfoTest.testComponentInfoJadInstalledOk begins");
        iComms.registerListener(0, new ResultListener());
        String[] args = new String[]
        {
            "componentinfo",
            "-commsresult=" + TEST_ENDPOINT,
            "-jad=" + iItu.getTestDir() + "HelloWorld.jad",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        try
        {
            // Check result message
            assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 2);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_NONE);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_INSTALL_STATUS, GetComponentInfo.ALREADY_INSTALLED);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_AUTHENTICITY, 1);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_NAME, "HelloWorld");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VENDOR, "Nokia");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VERSION, "1.0");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_GID, "Nokia/HelloWorld");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_GID+1, "Nokia/HelloWorld/HelloWorld");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+1, "HelloWorld");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_GID+2);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_UID+2);
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+2);
            if (Platform.isS60())
            {
                assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_UID, 0x101b2bd2);
                assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_UID+1, 0xefef0001);
            }
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    public void testComponentInfoUidAfterUninstallation()
    {
        Log.log("GetComponentInfoTest.testComponentInfoUidAfterUninstallation begins");
        iComms.registerListener(0, new ResultListener());

        // Uninstall test application.
        String[] args = new String[]
        {
            "uninstall",
            "-silent",
            "-iap=1",
            "-commsresult=" + TEST_ENDPOINT,
            "-uid=" + iInstalledApp,
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);

        // Get component info after uninstallation.
        args = new String[]
        {
            "componentinfo",
            "-commsresult=" + TEST_ENDPOINT,
            "-uid=" + iInstalledApp,
        };
        result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NOT_FOUND);
        // Check result
        assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 2);
        assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_NOT_FOUND);
    }

    public void testComponentInfoJadInboxOk()
    {
        Log.log("GetComponentInfoTest.testComponentInfoJadInboxOk begins");
        iComms.registerListener(0, new ResultListener());
        String startPath = "c:\\private\\1000484b\\Mail2\\00001001_S\\7\\00100007_F\\HelloWorld1.jad";
        String[] args = new String[]
        {
            "componentinfo",
            "-commsresult=" + TEST_ENDPOINT,
            "-jad=" + startPath,
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        try
        {
            // Check result message
            assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 2);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_NONE);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_INSTALL_STATUS, GetComponentInfo.NEW_COMPONENT);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_AUTHENTICITY, 0);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_NAME, "HelloWorld1");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VENDOR, "Unknown");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VERSION, "1.0");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_GID, "Unknown/HelloWorld1");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_SUITE_UID);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_GID+1, "Unknown/HelloWorld1/HelloWorld1");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+1, "HelloWorld1");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_UID+1);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    public void testComponentInfoJarInboxOk()
    {
        Log.log("GetComponentInfoTest.testComponentInfoJarInboxOk begins");
        iComms.registerListener(0, new ResultListener());
        String startPath = "c:\\private\\1000484b\\Mail2\\00001001_S\\d\\0010000d_F\\HelloWorld2.jar";
        String[] args = new String[]
        {
            "componentinfo",
            "-commsresult=" + TEST_ENDPOINT,
            "-jar=" + startPath,
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        try
        {
            // Check result message
            assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 2);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_NONE);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_INSTALL_STATUS, GetComponentInfo.NEW_COMPONENT);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_AUTHENTICITY, 0);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_NAME, "HelloWorld2");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VENDOR, "Unknown");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VERSION, "1.0");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_GID, "Unknown/HelloWorld2");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_SUITE_UID);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_GID+1, "Unknown/HelloWorld2/HelloWorld2");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+1, "HelloWorld2");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_UID+1);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    public void testComponentInfoMmInboxOk()
    {
        Log.log("GetComponentInfoTest.testComponentInfoMmInboxOk begins");
        iComms.registerListener(0, new ResultListener());
        String startPath = "c:\\private\\1000484b\\Mail2\\00001001_S\\1\\00100011_F\\hwmm.jar";
        String[] args = new String[]
        {
            "componentinfo",
            "-commsresult=" + TEST_ENDPOINT,
            "-jar=" + startPath,
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        try
        {
            // Check result message
            assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 2);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_NONE);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_INSTALL_STATUS, GetComponentInfo.NEW_COMPONENT);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_AUTHENTICITY, 0);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_NAME, "HelloWorldMM");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VENDOR, "Nokia");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_VERSION, "1.0");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_GID, "Nokia/HelloWorldMM");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_SUITE_UID);
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_GID+1, "Nokia/HelloWorldMM/HelloWorld");
            assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+1, "HelloWorld");
            assertResultMessageNoValue(iMsg, InstallerResultMessage.NAME_MIDLET_UID+1);
        }
        catch (Throwable t)
        {
            t.printStackTrace();
            assertTrue(t.toString(), false);
        }
    }

    private class ResultListener implements CommsListener
    {
        public void processMessage(CommsMessage aMessage)
        {
            try
            {
                iMsg = new InstallerResultMessage();
                iMsg.parse(aMessage);
                Log.log("ResultListener received " + iMsg);
                CommsMessage response = new CommsMessage();
                response.replyTo(aMessage);
                response.setMessageId(602);
                iComms.send(response);
                Log.log("ResultListener sent " + response);
            }
            catch (Throwable t)
            {
                t.printStackTrace();
            }
        }
    }

    private void assertResultMessage(InstallerResultMessage aMsg, String aName, int aValue)
    {
        assertTrue(aName + "!=" + aValue + " (" + aMsg.getIntValue(aName) + ")",
                   aMsg.getIntValue(aName) == aValue);
    }

    private void assertResultMessage(InstallerResultMessage aMsg, String aName, String aValue)
    {
        assertTrue(aName + "!=" + aValue + " (" + aMsg.getStringValue(aName) + ")",
                   aValue.equals(aMsg.getStringValue(aName)));
    }

    private void assertResultMessageNoValue(InstallerResultMessage aMsg, String aName)
    {
        try
        {
            String value = aMsg.getStringValue(aName);
            assertTrue("Unexpected value " + aName + "=" + value, value == null);
        }
        catch (Throwable t)
        {
            assertTrue("Unexpected exception for string value " + aName +
                       ": " + t, false);
        }

        try
        {
            int value = aMsg.getIntValue(aName);
            assertTrue("Unexpected value " + aName + "=" + value, false);
        }
        catch (IllegalArgumentException iae)
        {
            // Expected exception.
        }
        catch (Throwable t)
        {
            assertTrue("Incorrect exception for int value " + aName +
                       ": " + t, false);
        }
    }
}
