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

import com.nokia.mj.impl.comms.CommsEndpoint;
import com.nokia.mj.impl.comms.CommsServerEndpoint;
import com.nokia.mj.impl.comms.CommsListener;
import com.nokia.mj.impl.comms.CommsMessage;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.installer.utils.Log;

import java.io.IOException;
import java.util.Vector;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * Test cases for InstallerResultMessage.
 */
public class InstallerResultMessageTest extends TestCase implements InstallerMain
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

        suite.addTest(new InstallerResultMessageTest("testInvalidEndpoint", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerResultMessageTest)tc).testInvalidEndpoint();
            }
        }));
        suite.addTest(new InstallerResultMessageTest("testInstallResultOk", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerResultMessageTest)tc).testInstallResultOk();
            }
        }));
        suite.addTest(new InstallerResultMessageTest("testInstallResultNok", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerResultMessageTest)tc).testInstallResultNok();
            }
        }));
        suite.addTest(new InstallerResultMessageTest("testUninstallResultNok", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerResultMessageTest)tc).testUninstallResultNok();
            }
        }));
        suite.addTest(new InstallerResultMessageTest("testUninstallResultOk", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InstallerResultMessageTest)tc).testUninstallResultOk();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public InstallerResultMessageTest()
    {
    }

    public InstallerResultMessageTest(String aTestName, TestMethod aTestMethod)
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

    public void testInvalidEndpoint()
    {
        Log.log("InstallerResultMessageTest.testInvalidEndpoint begins");
        iComms.registerListener(0, new ResultListener());
        String[] args = new String[]
        {
            "install",
            "-silent",
            "-commsresult=invalidendpoint",
            "-jad=" + iItu.getTestDir() + "HelloWorld.jad",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_GENERAL);
    }

    public void testInstallResultOk()
    {
        Log.log("InstallerResultMessageTest.testInstallResultOk begins");
        iComms.registerListener(0, new ResultListener());
        String[] args = new String[]
        {
            "install",
            "-silent",
            "-commsresult=" + TEST_ENDPOINT,
            "-jad=" + iItu.getTestDir() + "HelloWorld.jad",
        };
        int result = Installer.mainWithResult(args);
        // Save the uid of the installed app so that it can be
        // uninstalled in the next test case.
        iInstalledApp = Installer.iInstalledApps[0].toString();
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        // Check result message
        assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 0);
        assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, 0);
        assertResultMessage(iMsg, InstallerResultMessage.NAME_AUTHENTICITY, 1);
        assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_NAME, "HelloWorld");
        assertResultMessage(iMsg, InstallerResultMessage.NAME_VENDOR, "Nokia");
        assertResultMessage(iMsg, InstallerResultMessage.NAME_VERSION, "1.0");
        assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_GID, "Nokia/HelloWorld");
        assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_GID+1, "Nokia/HelloWorld/HelloWorld");
        assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+1, "HelloWorld");
    }

    public void testInstallResultNok()
    {
        Log.log("InstallerResultMessageTest.testInstallResultNok begins");
        iComms.registerListener(0, new ResultListener());
        String[] args = new String[]
        {
            "install",
            "-silent",
            "-commsresult=" + TEST_ENDPOINT,
            "-jad=" + iItu.getTestDir() + "HelloWorld.jad",
            "-forcecancel",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_GENERAL);
        // Check result message
        assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 0);
        assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_GENERAL);
    }

    public void testUninstallResultOk()
    {
        Log.log("InstallerResultMessageTest.testUninstallResultOk begins");
        iComms.registerListener(0, new ResultListener());
        String[] args = new String[]
        {
            "uninstall",
            "-silent",
            "-commsresult=" + TEST_ENDPOINT,
            "-uid=" + iInstalledApp,
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_NONE);
        // Check result message
        assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 1);
        assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, 0);
        assertResultMessage(iMsg, InstallerResultMessage.NAME_AUTHENTICITY, 1);
        assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_NAME, "HelloWorld");
        assertResultMessage(iMsg, InstallerResultMessage.NAME_VENDOR, "Nokia");
        assertResultMessage(iMsg, InstallerResultMessage.NAME_VERSION, "1.0");
        assertResultMessage(iMsg, InstallerResultMessage.NAME_SUITE_GID, "Nokia/HelloWorld");
        assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_GID+1, "Nokia/HelloWorld/HelloWorld");
        assertResultMessage(iMsg, InstallerResultMessage.NAME_MIDLET_NAME+1, "HelloWorld");
    }

    public void testUninstallResultNok()
    {
        Log.log("InstallerResultMessageTest.testUninstallResultNok begins");
        iComms.registerListener(0, new ResultListener());
        String[] args = new String[]
        {
            "uninstall",
            "-silent",
            "-commsresult=" + TEST_ENDPOINT,
            "-uid=" + iInstalledApp,
            "-forcecancel",
        };
        int result = Installer.mainWithResult(args);
        // Check result code.
        assertResult(result, Installer.ERR_GENERAL);
        // Check result message
        assertResultMessage(iMsg, InstallerResultMessage.NAME_OPERATION, 1);
        assertResultMessage(iMsg, InstallerResultMessage.NAME_RESULT, Installer.ERR_GENERAL);
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
}
