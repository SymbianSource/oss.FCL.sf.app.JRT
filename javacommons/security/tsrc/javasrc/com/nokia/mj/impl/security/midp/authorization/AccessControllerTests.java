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


package com.nokia.mj.impl.security.midp.authorization;

import java.util.Hashtable;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.security.midp.authorization.AccessControllerImpl;
import com.nokia.mj.impl.security.midp.authorization.PermissionResolver;
import com.nokia.mj.impl.security.midp.authorization.AccessControllerFactoryImpl;
import java.security.*;
import javax.microedition.io.*;
import com.nokia.mj.impl.security.midp.storage.SecurityStorage;
import com.nokia.mj.impl.storage.StorageSession;
import com.nokia.mj.impl.storage.StorageFactory;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermission;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermissionImpl;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettings;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettingsImpl;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import java.util.Vector;

/**
 * AccessController unit tests.
 */
public class AccessControllerTests extends TestCase implements InstallerMain
{

    int assertTrace = 0;
    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        /* Uncomment this from time to time and check if QT still fails: create QT thread, destroy QT thread, create QT thread -> this fails
                suite.addTest(new AccessControllerTests("testAccessControllerFactory", new TestMethod()
                    { public void run(TestCase tc) {((AccessControllerTests)tc).testAccessControllerFactory();} }));
        */
        suite.addTest(new AccessControllerTests("testAccessController", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AccessControllerTests)tc).testAccessController();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public AccessControllerTests()
    {
    }

    public AccessControllerTests(String aTestName, TestMethod aTestMethod)
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

    private void assertWithTrace(boolean aCondition)
    {
        assertTrue("" + assertTrace, aCondition);
        assertTrace++;
    }

    protected void testAccessControllerFactory()
    {
        Uid appUID1 = Uid.createUid("appUID1");
        Uid appUID2 = Uid.createUid("appUID2");
        AccessControllerImpl ac1;
        AccessControllerImpl ac2;
        String appName = "AccessControllerTests";
        // check null values
        ac1 = AccessControllerFactoryImpl.getAccessController(null, null);
        assertWithTrace(ac1 == null);
        AccessControllerFactoryImpl.destroyAccessController(null);
        ac1 = AccessControllerFactoryImpl.getAccessController(null, "appName");
        assertWithTrace(ac1 == null);
        AccessControllerFactoryImpl.destroyAccessController(null);
        // check duplicates
        AccessControllerFactoryImpl.destroyAccessController(appUID1);
        ac1 = AccessControllerFactoryImpl.getAccessController(appUID1, appName);
        ac2 = AccessControllerFactoryImpl.getAccessController(appUID1, appName);
        AccessControllerFactoryImpl.destroyAccessController(appUID1);
        assertWithTrace(ac1 == ac2);
        // check destroy
        ac1 = AccessControllerFactoryImpl.getAccessController(appUID1, appName);
        AccessControllerFactoryImpl.destroyAccessController(appUID1);
        ac2 = AccessControllerFactoryImpl.getAccessController(appUID1, appName);
        // call destroy two times (or calling it on a UID which does not have an instance)
        AccessControllerFactoryImpl.destroyAccessController(appUID1);
        AccessControllerFactoryImpl.destroyAccessController(appUID1);
        assertWithTrace(ac1 != ac2);
    }

    protected void testAccessController()
    {
        Uid appUID = Uid.createUid("myAppUID");
        String appName = "AccessControllerTests";
        AccessControllerImpl ac;
        SecurityStorage storage = new SecurityStorage();
        Vector grantedPermissions;
        storage.removeGrantedPermissions(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        // check null values
        PermissionResolver.cleanup();
        try
        {
            ac.checkPermission((Permission)null);
            assertWithTrace(false);
        }
        catch (AccessControlException e)
        {
            assertWithTrace(true);
        }
        assertWithTrace(ac.checkPermission((String)null) == 0);
        // check permission which is not granted
        PermissionResolver.cleanup();
        try
        {
            ac.checkPermission(new CommProtocolPermission("comm://"));
            assertWithTrace(false);
        }
        catch (AccessControlException e)
        {
            assertWithTrace(true);
        }
        try
        {
            ac.checkPermission(new CommProtocolPermission("comm://"));
            assertWithTrace(false);
        }
        catch (AccessControlException e)
        {
            assertWithTrace(true);
        }
        // check allowed permission which is granted
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "javax.microedition.io.CommProtocolPermission",
                                          "comm://",
                                          null,
                                          PolicyBasedPermission.ALLOWED_TYPE));
        storage.removeGrantedPermissions(appUID);
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        ac.checkPermission(new CommProtocolPermission("comm://"));
        try
        {
            ac.checkPermission(new FileProtocolPermission("file://myFile.txt", "read"));
            assertWithTrace(false);
        }
        catch (AccessControlException e)
        {
            assertWithTrace(true);
        }
        storage.removeGrantedPermissions(appUID);
        // check permission not allowed
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.CommProtocolPermissionImpl",
                                          "comm://",
                                          null,
                                          new UserSecuritySettingsImpl("Low Level Net Access",
                                                                       UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                                   UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                   UserSecuritySettings.NO_INTERACTION_MODE
                                                                                 })));
        storage.removeGrantedPermissions(appUID);
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        try
        {
            ac.checkPermission(new SocketProtocolPermission("socket://:100"));
            assertWithTrace(false);
        }
        catch (AccessControlException e)
        {
            assertWithTrace(true);
        }
        storage.removeGrantedPermissions(appUID);
        // check user permission with NO as interaction mode -> security exception thrown
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.HttpProtocolPermissionImpl",
                                          "http://",
                                          null,
                                          new UserSecuritySettingsImpl("Net Access",
                                                                       UserSecuritySettings.NO_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                                   UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                   UserSecuritySettings.NO_INTERACTION_MODE
                                                                                 })));
        storage.removeGrantedPermissions(appUID);
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        try
        {
            ac.checkPermission(new HttpProtocolPermissionImpl("http://"));
            assertWithTrace(false);
        }
        catch (AccessControlException e)
        {
            assertWithTrace(true);
        }
        try
        {
            ac.checkPermission(new HttpProtocolPermission("http://"));
            assertWithTrace(false);
        }
        catch (AccessControlException e)
        {
            assertWithTrace(true);
        }
        storage.removeGrantedPermissions(appUID);
        // check the named permissions: unknown permission
        assertWithTrace(ac.checkPermission("unknownPermission") == 0);
        // check the named permissions: allowed permission
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.gcf.protocol.socket.SocketPermissionImpl",
                                          "socket://*",
                                          null,
                                          null));
        storage.removeGrantedPermissions(appUID);
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        assertWithTrace(ac.checkPermission("javax.microedition.io.Connector.socket") == 1);
        // check the named permissions: user permission with NO interaction mode
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.gcf.protocol.http.HttpPermissionImpl",
                                          "http://",
                                          null,
                                          new UserSecuritySettingsImpl("Net Access",
                                                                       UserSecuritySettings.NO_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                                   UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                   UserSecuritySettings.NO_INTERACTION_MODE
                                                                                 })));
        storage.removeGrantedPermissions(appUID);
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        assertWithTrace(ac.checkPermission("javax.microedition.io.Connector.http") == 0);
        // check the named permissions: user permission with ONESHOT interaction mode
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.gcf.protocol.http.HttpPermissionImpl",
                                          "http://",
                                          null,
                                          new UserSecuritySettingsImpl("Net Access",
                                                                       UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                                   UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                   UserSecuritySettings.NO_INTERACTION_MODE
                                                                                 })));
        storage.removeGrantedPermissions(appUID);
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        assertWithTrace(ac.checkPermission("javax.microedition.io.Connector.http") == -1);
        // check the named permissions: user permission with BLANKET interaction mode (blanket prompt not shown yet)
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.gcf.protocol.http.HttpPermissionImpl",
                                          "http://",
                                          null,
                                          new UserSecuritySettingsImpl("Net Access",
                                                                       UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                                   UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                   UserSecuritySettings.NO_INTERACTION_MODE
                                                                                 })));
        storage.removeGrantedPermissions(appUID);
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        assertWithTrace(ac.checkPermission("javax.microedition.io.Connector.http") == -1);
        // check the named permissions: user permission with BLANKET interaction mode (blanket prompt shown already)
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.gcf.protocol.http.HttpPermissionImpl",
                                          "http://",
                                          null,
                                          new UserSecuritySettingsImpl("Net Access",
                                                                       UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                                   UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                   UserSecuritySettings.NO_INTERACTION_MODE
                                                                                 })));
        storage.removeGrantedPermissions(appUID);
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        storage.writeUserSecuritySettingsPromptFlag(appUID, "Net Access", true);
        assertWithTrace(ac.checkPermission("javax.microedition.io.Connector.http") == 1);
        // check the named permissions: user permission with SESSION interaction mode (session prompt not shown already)
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.gcf.protocol.http.HttpPermissionImpl",
                                          "http://",
                                          null,
                                          new UserSecuritySettingsImpl("Net Access",
                                                                       UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                                   UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                   UserSecuritySettings.NO_INTERACTION_MODE
                                                                                 })));
        storage.removeGrantedPermissions(appUID);
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        storage.writeUserSecuritySettingsPromptFlag(appUID, "Net Access", true);
        assertWithTrace(ac.checkPermission("javax.microedition.io.Connector.http") == -1);
        // check the named permissions: denied permission
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.gcf.protocol.datagram.DatagramPermissionImpl",
                                          "datagram://",
                                          null,
                                          null));
        storage.removeGrantedPermissions(appUID);
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        assertWithTrace(ac.checkPermission("javax.wireless.messaging.mms.send") == 0);
        // cleanup
        storage.removeGrantedPermissions(appUID);
        AccessControllerFactoryImpl.destroyAccessController(appUID);
    }
}
