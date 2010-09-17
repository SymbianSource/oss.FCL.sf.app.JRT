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


package com.nokia.mj.impl.installer.jsrpluginnotifier;

import java.util.Hashtable;
import java.util.Vector;

import com.nokia.mj.impl.installer.integrityservice.IntegrityService;
import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.Uid;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * ApplicationRegistrator unit tests.
 */
public class JsrPluginNotifierTest extends TestCase implements InstallerMain
{
    IntegrityService iIntegrityService;
    InstallerExtensionInfo iInstallerExtensionInfo;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new JsrPluginNotifierTest("testInitializeNotifier", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JsrPluginNotifierTest)tc).testInitializeNotifier();
            }
        }));

        suite.addTest(new JsrPluginNotifierTest("testPluginCancelsInstallation", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JsrPluginNotifierTest)tc).testPluginCancelsInstallation();
            }
        }));

        suite.addTest(new JsrPluginNotifierTest("testNotifyPluginsInstall", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JsrPluginNotifierTest)tc).testNotifyPluginsInstall();
            }
        }));

        suite.addTest(new JsrPluginNotifierTest("testNotifyPluginInstallException", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JsrPluginNotifierTest)tc).testNotifyPluginInstallException();
            }
        }));

        suite.addTest(new JsrPluginNotifierTest("testNotifyPluginsUninstall", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JsrPluginNotifierTest)tc).testNotifyPluginsUninstall();
            }
        }));

        suite.addTest(new JsrPluginNotifierTest("testNotifyPluginUninstallException", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JsrPluginNotifierTest)tc).testNotifyPluginUninstallException();
            }
        }));

        suite.addTest(new JsrPluginNotifierTest("testPluginCancelsUninstallation", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JsrPluginNotifierTest)tc).testPluginCancelsUninstallation();
            }
        }));

        suite.addTest(new JsrPluginNotifierTest("testNoPluginsToNotify", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JsrPluginNotifierTest)tc).testNoPluginsToNotify();
            }
        }));

        suite.addTest(new JsrPluginNotifierTest("testRollbackInstallNotification", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JsrPluginNotifierTest)tc).testRollbackInstallNotification();
            }
        }));

        suite.addTest(new JsrPluginNotifierTest("testRollbackInstallNotificationException", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JsrPluginNotifierTest)tc).testRollbackInstallNotificationException();
            }
        }));

        suite.addTest(new JsrPluginNotifierTest("testRollbackUninstallNotification", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JsrPluginNotifierTest)tc).testRollbackUninstallNotification();
            }
        }));

        suite.addTest(new JsrPluginNotifierTest("testRollbackUninstallNotificationException", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JsrPluginNotifierTest)tc).testRollbackUninstallNotificationException();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public JsrPluginNotifierTest()
    {
    }

    public JsrPluginNotifierTest(String aTestName, TestMethod aTestMethod)
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
        // Must initialize integrity server because this notifier test case
        // will make a file copy using it.
        String isRoot = FileUtils.getIntegrityServiceRoot();
        // only one Integrity Service session at time is allowed
        iIntegrityService = IntegrityService.getInstance(isRoot);

        iInstallerExtensionInfo = new InstallerExtensionInfo();
    }

    protected void tearDown()
    {
        iIntegrityService.commit();
    }

    public void testInitializeNotifier()
    {
        // Ant deploy command has copied the configuration file
        // to Symbian Z: drive, creating JSR plugin notifier should
        // copy it to C: and read the content of the file.
        JsrPluginNotifier notif = new JsrPluginNotifier(iIntegrityService);

        assertTrue("Number of plugin class names was not 5, it is " +
                   String.valueOf(notif.iClassNames.size()), (notif.iClassNames.size() == 5));
        assertTrue("Number of initialized plugins was not 2, it is " +
                   String.valueOf(notif.iJsrPlugins.size()), (notif.iJsrPlugins.size() == 2));
    }

    public void testPluginCancelsInstallation()
    {
        JsrPluginNotifier notif = new JsrPluginNotifier(iIntegrityService);

        assertTrue("Number of plugin class names was not 5, it is " +
                   String.valueOf(notif.iClassNames.size()), (notif.iClassNames.size() == 5));
        assertTrue("Number of initialized plugins was not 2, it is " +
                   String.valueOf(notif.iJsrPlugins.size()), (notif.iJsrPlugins.size() == 2));

        // Initialize necessary values in ball for notification call
        Uid uid = Uid.createUid("[A0032000]");
        iInstallerExtensionInfo.iUid = uid;
        iInstallerExtensionInfo.iAttributes = new Hashtable();
        Attribute att = new Attribute("MIDlet-1", "3DSpaceShooter, /icon.png, Space3D");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-1", att);
        att = new Attribute("JSR-Plugin-Test-Cancel", "true");
        iInstallerExtensionInfo.iAttributes.put("JSR-Plugin-Test-Cancel", att);
        att = new Attribute("MIDlet-Jar-Size", "240229");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-Jar-Size", att);

        try
        {
            notif.notifyInstallation(iInstallerExtensionInfo);
            // Test plugin TestPlugin2 cancels installation if JAD/JAR attribute
            // JSR-Plugin-Test-Cancel exists.
            assertTrue("Test plugin did not cancel installation like should have.", false);
        }
        catch (InstallerException e)
        {
            // OK, installation is cancelled ok
        }
    }

    public void testNotifyPluginsInstall()
    {
        JsrPluginNotifier notif = new JsrPluginNotifier(iIntegrityService);

        // Initialize necessary values in ball for notification call
        Uid uid = Uid.createUid("[A0032001]");
        iInstallerExtensionInfo.iUid = uid;
        iInstallerExtensionInfo.iAttributes = new Hashtable();
        Attribute att = new Attribute("MIDlet-1", "3DSpaceShooter, /icon.png, Space3D");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-1", att);
        att = new Attribute("Nokia-MIDlet-On-Screen-Keypad", "navigationkeys");
        iInstallerExtensionInfo.iAttributes.put("Nokia-MIDlet-On-Screen-Keypad", att);
        att = new Attribute("MIDlet-Jar-Size", "240229");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-Jar-Size", att);

        try
        {
            notif.notifyInstallation(iInstallerExtensionInfo);
        }
        catch (InstallerException e)
        {
            // Installation must not be cancelled
            assertTrue("Error: Test plugin cancelled installation.", false);
        }

        String plugin2 = System.getProperty("TestPlugin2");
        assertTrue("TestPlugin2 was not called", plugin2 != null);
        assertTrue("TestPlugin2 correct method was not called", plugin2.equals("install"));
    }

    public void testNotifyPluginInstallException()
    {
        JsrPluginNotifier notif = new JsrPluginNotifier(iIntegrityService);

        assertTrue("Number of plugin class names was not 5, it is " +
                   String.valueOf(notif.iClassNames.size()), (notif.iClassNames.size() == 5));
        assertTrue("Number of initialized plugins was not 2, it is " +
                   String.valueOf(notif.iJsrPlugins.size()), (notif.iJsrPlugins.size() == 2));

        // Initialize necessary values in ball for notification call
        Uid uid = Uid.createUid("[A0032000]");
        iInstallerExtensionInfo.iUid = uid;
        iInstallerExtensionInfo.iAttributes = new Hashtable();
        Attribute att = new Attribute("MIDlet-1", "3DSpaceShooter, /icon.png, Space3D");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-1", att);
        att = new Attribute("JSR-Plugin-Test-Exception", "true");
        iInstallerExtensionInfo.iAttributes.put("JSR-Plugin-Test-Exception", att);
        att = new Attribute("MIDlet-Jar-Size", "240229");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-Jar-Size", att);

        try
        {
            notif.notifyInstallation(iInstallerExtensionInfo);
            // Test plugin TestPlugin2 throws exception if JAD/JAR attribute
            // JSR-Plugin-Test-Exception exists.
            assertTrue("Test plugin did not throw exception like should have.", false);
        }
        catch (InstallerException e)
        {
            // OK, installation is cancelled ok
        }
    }

    public void testNoPluginsToNotify()
    {
        JsrPluginNotifier notif = new JsrPluginNotifier(iIntegrityService);

        // Modify notifier so that there are no plugins
        notif.iJsrPlugins = new Vector(5,5);

        // Initialize necessary values in ball for notification call
        Uid uid = Uid.createUid("[A0032001]");
        iInstallerExtensionInfo.iUid = uid;
        iInstallerExtensionInfo.iAttributes = new Hashtable();
        Attribute att = new Attribute("MIDlet-1", "3DSpaceShooter, /icon.png, Space3D");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-1", att);
        att = new Attribute("Nokia-MIDlet-On-Screen-Keypad", "navigationkeys");
        iInstallerExtensionInfo.iAttributes.put("Nokia-MIDlet-On-Screen-Keypad", att);
        att = new Attribute("MIDlet-Jar-Size", "240229");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-Jar-Size", att);

        try
        {
            // No plugins but this call must still succeed
            notif.notifyInstallation(iInstallerExtensionInfo);
        }
        catch (InstallerException e)
        {
            // Installation must not be cancelled
            assertTrue("Error: Test plugin cancelled installation.", false);
        }
    }

    public void testNotifyPluginsUninstall()
    {
        JsrPluginNotifier notif = new JsrPluginNotifier(iIntegrityService);

        // Initialize necessary values in ball for notification call
        Uid uid = Uid.createUid("[A0032002]");
        iInstallerExtensionInfo.iUid = uid;
        iInstallerExtensionInfo.iAttributes = new Hashtable();
        Attribute att = new Attribute("MIDlet-1", "3DSpaceShooter, /icon.png, Space3D");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-1", att);
        att = new Attribute("Nokia-MIDlet-On-Screen-Keypad", "navigationkeys");
        iInstallerExtensionInfo.iAttributes.put("Nokia-MIDlet-On-Screen-Keypad", att);
        att = new Attribute("MIDlet-Jar-Size", "240229");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-Jar-Size", att);

        try
        {
            notif.notifyUninstallation(iInstallerExtensionInfo);
        }
        catch (InstallerException e)
        {
            // Unnstallation must not be cancelled
            assertTrue("Error: Test plugin cancelled uninstallation.", false);
        }

        String plugin2 = System.getProperty("TestPlugin2");
        assertTrue("TestPlugin2 was not called", plugin2 != null);
        assertTrue("TestPlugin2 correct method was not called", plugin2.equals("uninstall"));
    }

    public void testNotifyPluginUninstallException()
    {
        JsrPluginNotifier notif = new JsrPluginNotifier(iIntegrityService);

        // Initialize necessary values in ball for notification call
        Uid uid = Uid.createUid("[A0032003]");
        iInstallerExtensionInfo.iUid = uid;
        iInstallerExtensionInfo.iAttributes = new Hashtable();
        Attribute att = new Attribute("MIDlet-1", "3DSpaceShooter, /icon.png, Space3D");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-1", att);
        att = new Attribute("JSR-Plugin-Test-Exception", "true");
        iInstallerExtensionInfo.iAttributes.put("JSR-Plugin-Test-Exception", att);
        att = new Attribute("MIDlet-Jar-Size", "240229");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-Jar-Size", att);

        try
        {
            notif.notifyUninstallation(iInstallerExtensionInfo);
            // Test plugin TestPlugin2 throws exception if JAD/JAR attribute
            // JSR-Plugin-Test-Exception exists.
            assertTrue("Test plugin did not throw exception like it should have.", false);
        }
        catch (InstallerException e)
        {
            // OK, uninstallation is cancelled ok
        }
    }

    public void testPluginCancelsUninstallation()
    {
        JsrPluginNotifier notif = new JsrPluginNotifier(iIntegrityService);

        // Initialize necessary values in ball for notification call
        Uid uid = Uid.createUid("[A0032003]");
        iInstallerExtensionInfo.iUid = uid;
        iInstallerExtensionInfo.iAttributes = new Hashtable();
        Attribute att = new Attribute("MIDlet-1", "3DSpaceShooter, /icon.png, Space3D");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-1", att);
        att = new Attribute("JSR-Plugin-Test-Cancel", "true");
        iInstallerExtensionInfo.iAttributes.put("JSR-Plugin-Test-Cancel", att);
        att = new Attribute("MIDlet-Jar-Size", "240229");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-Jar-Size", att);

        try
        {
            notif.notifyUninstallation(iInstallerExtensionInfo);
            // Test plugin TestPlugin2 cancels installation if JAD/JAR attribute
            // JSR-Plugin-Test-Cancel exists.
            assertTrue("Test plugin did not cancel uninstallation like it should have.", false);
        }
        catch (InstallerException e)
        {
            // OK, uninstallation is cancelled ok
        }
    }

    public void testRollbackInstallNotification()
    {
        JsrPluginNotifier notif = new JsrPluginNotifier(iIntegrityService);

        // Initialize necessary values in ball for notification call
        Uid uid = Uid.createUid("[A0032001]");
        iInstallerExtensionInfo.iUid = uid;
        iInstallerExtensionInfo.iAttributes = new Hashtable();
        Attribute att = new Attribute("MIDlet-1", "3DSpaceShooter, /icon.png, Space3D");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-1", att);
        att = new Attribute("Nokia-MIDlet-On-Screen-Keypad", "navigationkeys");
        iInstallerExtensionInfo.iAttributes.put("Nokia-MIDlet-On-Screen-Keypad", att);
        att = new Attribute("MIDlet-Jar-Size", "240229");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-Jar-Size", att);

        notif.notifyRollbackInstall(iInstallerExtensionInfo);

        String plugin2 = System.getProperty("TestPlugin2");
        assertTrue("TestPlugin2 was not called", plugin2 != null);
        assertTrue("TestPlugin2 correct method was not called", plugin2.equals("rollbackInstall"));
    }

    public void testRollbackInstallNotificationException()
    {
        JsrPluginNotifier notif = new JsrPluginNotifier(iIntegrityService);

        // Initialize necessary values in ball for notification call
        Uid uid = Uid.createUid("[A0032001]");
        iInstallerExtensionInfo.iUid = uid;
        iInstallerExtensionInfo.iAttributes = new Hashtable();
        Attribute att = new Attribute("MIDlet-1", "3DSpaceShooter, /icon.png, Space3D");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-1", att);
        att = new Attribute("Nokia-MIDlet-On-Screen-Keypad", "navigationkeys");
        iInstallerExtensionInfo.iAttributes.put("Nokia-MIDlet-On-Screen-Keypad", att);
        att = new Attribute("JSR-Plugin-Test-Exception", "true");
        iInstallerExtensionInfo.iAttributes.put("JSR-Plugin-Test-Exception", att);
        att = new Attribute("MIDlet-Jar-Size", "240229");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-Jar-Size", att);

        notif.notifyRollbackInstall(iInstallerExtensionInfo);

        String plugin2 = System.getProperty("TestPlugin2");
        assertTrue("TestPlugin2 was not called", plugin2 != null);
        assertTrue("TestPlugin2 correct method was not called", plugin2.equals("rollbackInstall"));
    }

    public void testRollbackUninstallNotification()
    {
        JsrPluginNotifier notif = new JsrPluginNotifier(iIntegrityService);

        // Initialize necessary values in ball for notification call
        Uid uid = Uid.createUid("[A0032002]");
        iInstallerExtensionInfo.iUid = uid;
        iInstallerExtensionInfo.iAttributes = new Hashtable();
        Attribute att = new Attribute("MIDlet-1", "3DSpaceShooter, /icon.png, Space3D");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-1", att);
        att = new Attribute("Nokia-MIDlet-On-Screen-Keypad", "navigationkeys");
        iInstallerExtensionInfo.iAttributes.put("Nokia-MIDlet-On-Screen-Keypad", att);
        att = new Attribute("MIDlet-Jar-Size", "240229");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-Jar-Size", att);

        notif.notifyRollbackUninstall(iInstallerExtensionInfo);

        String plugin2 = System.getProperty("TestPlugin2");
        assertTrue("TestPlugin2 was not called", plugin2 != null);
        assertTrue("TestPlugin2 correct method was not called. Method was " + plugin2, plugin2.equals("rollbackUninstall"));
    }

    public void testRollbackUninstallNotificationException()
    {
        JsrPluginNotifier notif = new JsrPluginNotifier(iIntegrityService);

        // Initialize necessary values in ball for notification call
        Uid uid = Uid.createUid("[A0032002]");
        iInstallerExtensionInfo.iUid = uid;
        iInstallerExtensionInfo.iAttributes = new Hashtable();
        Attribute att = new Attribute("MIDlet-1", "3DSpaceShooter, /icon.png, Space3D");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-1", att);
        att = new Attribute("Nokia-MIDlet-On-Screen-Keypad", "navigationkeys");
        iInstallerExtensionInfo.iAttributes.put("Nokia-MIDlet-On-Screen-Keypad", att);
        att = new Attribute("JSR-Plugin-Test-Exception", "true");
        iInstallerExtensionInfo.iAttributes.put("JSR-Plugin-Test-Exception", att);
        att = new Attribute("MIDlet-Jar-Size", "240229");
        iInstallerExtensionInfo.iAttributes.put("MIDlet-Jar-Size", att);

        notif.notifyRollbackUninstall(iInstallerExtensionInfo);

        String plugin2 = System.getProperty("TestPlugin2");
        assertTrue("TestPlugin2 was not called", plugin2 != null);
        assertTrue("TestPlugin2 correct method was not called. Method was " + plugin2, plugin2.equals("rollbackUninstall"));
    }
}
