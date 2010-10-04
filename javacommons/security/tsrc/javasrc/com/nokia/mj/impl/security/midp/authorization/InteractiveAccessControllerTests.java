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
import com.nokia.mj.impl.utils.exception.ExceptionBase;
import com.nokia.mj.impl.security.midp.authentication.AuthenticationModule;
import com.nokia.mj.impl.security.midp.authorization.PermissionGranter;
import com.nokia.mj.impl.security.midp.common.AuthenticationCredentials;
import com.nokia.mj.impl.security.midp.common.SecurityAttributes;
import com.nokia.mj.impl.security.midp.common.SigningCertificate;
import com.nokia.mj.impl.security.midp.common.AuthenticationAttribute;
import com.nokia.mj.impl.security.midp.common.MIDPPermission;
import com.nokia.mj.impl.security.midp.common.PermissionMappingTable;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.security.utils.*;
import java.security.*;
//import javax.microedition.io.*;
import com.nokia.mj.impl.security.midp.storage.SecurityStorage;
import com.nokia.mj.impl.storage.*;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermission;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermissionImpl;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettings;
import com.nokia.mj.impl.security.midp.common.GeneralSecuritySettings;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettingsImpl;
import com.nokia.mj.impl.rt.ui.RuntimeUiFactory;
import com.nokia.mj.impl.rt.ui.RuntimeUi;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import java.util.Vector;

/**
 * Interactive AccessController unit tests.
 */
public class InteractiveAccessControllerTests extends TestCase implements InstallerMain
{

    private static String TEST_DATA_DIR;

    static
    {
        String platform = System.getProperty("os.name");
        if (platform != null && platform.equalsIgnoreCase("linux"))
        {
            TEST_DATA_DIR = System.getProperty("JAVA_BIN_ROOT") + "/securitytestdata/";
        }
        else
        {
            TEST_DATA_DIR = "C:\\java\\securitytestdata\\";
        }
    }

    /**
     * Hashtable holding all the attributes. Tests populate this hashtable
     * prior to being executed
     */
    protected Hashtable allAttributes;

    AuthenticationModule authenticationModule;
    PermissionGranter permissionGranter;

    // general-purpose constants
    protected static final String MIDP_PROFILE_ATTRIBUTE_NAME = "MicroEdition-Profile";
    protected static final String MIDP3 = "MIDP-3.0";
    protected static final String MIDP2 = "MIDP-2.0";
    private static final String COMMA = ",";

    /**
     * The implementation's mapping between (MIDP2 and MIDP3 permissions) and the
     * internal class permissions
     */
    private Hashtable IMPL_PERMISSION_MAPPING_TABLE;

    /**
     * The mapping between (MIDP2 and MIDP3 permissions) and the internal
     * class permissions
     */
    //private Hashtable TEST_PERMISSION_MAPPING_TABLE;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new InteractiveAccessControllerTests("testSecurityPrompts", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InteractiveAccessControllerTests)tc).testSecurityPrompts();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public InteractiveAccessControllerTests()
    {
    }

    public InteractiveAccessControllerTests(String aTestName, TestMethod aTestMethod)
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
        authenticationModule = authenticationModule.getInstance();
        permissionGranter = permissionGranter.getInstance();
    }

    protected void tearDown()
    {
    }

    private void testSecurityPrompts()
    {
        System.setProperty("com.nokia.mj.impl.rt.ui", "com.nokia.mj.impl.rt.ui.avkon.RuntimeUiAvkon");
        try
        {
            Class.forName("com.nokia.mj.impl.rt.ui.qt.RuntimeUiQt");
            System.setProperty("com.nokia.mj.impl.rt.ui", "com.nokia.mj.impl.rt.ui.qt.RuntimeUiQt");
        }catch(Throwable t)
        {
        }
        runTests();
    }

    private void runTests()
    {
        // init the new mapping & policies
        //initMappingTable();
        IMPL_PERMISSION_MAPPING_TABLE = PermissionMappingTable.setMappingTable(TestPermissionMappingTable.getMappingTable());
        SecurityPolicyModule.policiesDir = TEST_DATA_DIR;
        SecurityPolicyModule.policiesFileNamePrefix = "test_";
        allAttributes = new Hashtable();
        SecurityAttributes securityAttributes;
        SigningCertificate[] signingCerts;
        AuthenticationCredentials[] credentials;
        Uid appUID = Uid.createUid("myAppUID");
        String appName = "InteractiveSecurityTests";
        SecurityStorage storage = new SecurityStorage();
        StorageSession session = StorageFactory.createSession();
        session.open();
        Vector grantedPermissions;
        storage.removeGrantedPermissions(appUID);
        AccessControllerImpl ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        StorageEntry entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.SECURITY_WARNINGS,
                               "" + GeneralSecuritySettings.DEFAULT_SECURITY_MODE));
        session.write(StorageNames.MIDP_PACKAGE_TABLE, entry);
        // prompts for composite actions
        GeneralSecuritySettings.setSecurityWarningsMode(session, appUID, GeneralSecuritySettings.USER_SECURITY_MODE);
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.PermissionWithCompositeActionsImpl",
                                          "uri://",
                                          "action1",
                                          new UserSecuritySettingsImpl("Function Group",
                                                                       UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.NO_INTERACTION_MODE
                                                                                 })));
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.PermissionWithCompositeActionsImpl",
                                          "uri://",
                                          "action2",
                                          new UserSecuritySettingsImpl("Function Group",
                                                                       UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.NO_INTERACTION_MODE
                                                                                 })));
        storage.removeGrantedPermissions(appUID);
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        com.nokia.mj.impl.security.midp.authentication.InteractiveAuthenticationModuleTests.showGuidelines("Check that each of the following three prompts contains two identical texts");
        ac.checkPermission(new PermissionWithCompositeActionsImpl("uri://", "\nPrompt for action1", "action1"));
        ac.checkPermission(new PermissionWithCompositeActionsImpl("uri://", "\nPrompt for action2", "action2"));
        ac.checkPermission(new PermissionWithCompositeActionsImpl("uri://", "\nPrompt for action1 and action2", "action1,action2"));
        ac.checkPermission(new PermissionWithCompositeActionsImpl("uri://", "\nPrompt for action1 and action2", "action2,action1"));
        try
        {
            ac.checkPermission(new PermissionWithCompositeActionsImpl("uri://", "\n(IF YOU SEE THIS PROMPT THE TEST FAILED)", "action2,action1,action3"));
            assertTrue(false);
        }
        catch (AccessControlException e)
        {
            assertTrue(true);
        }
        try
        {
            ac.checkPermission(new PermissionWithCompositeActionsImpl("uri://", "\n(IF YOU SEE THIS PROMPT THE TEST FAILED)", "action2,action3"));
            assertTrue(false);
        }
        catch (AccessControlException e)
        {
            assertTrue(true);
        }
        assertTrue(true);
        // for NORMAL security mode, in case of identical prompts, only show one (even though they are part of different function groups)
        GeneralSecuritySettings.setSecurityWarningsMode(session, appUID, GeneralSecuritySettings.DEFAULT_SECURITY_MODE);
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
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.HttpProtocolPermissionImpl",
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
        ac.checkPermission(new HttpProtocolPermissionImpl("http://", "\n(choose ALLOW and make sure this text is shown only once)"));
        ac.checkPermission(new CommProtocolPermissionImpl("comm://", "\n(choose ALLOW and make sure this text is shown only once)"));
        // for USER security mode, in case of identical prompts, show them both
        GeneralSecuritySettings.setSecurityWarningsMode(session, appUID, GeneralSecuritySettings.USER_SECURITY_MODE);
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
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
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.HttpProtocolPermissionImpl",
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
        ac.checkPermission(new HttpProtocolPermissionImpl("http://", "\n(choose ALLOW and make sure this text is shown twice)"));
        ac.checkPermission(new CommProtocolPermissionImpl("comm://", "\n(choose ALLOW and make sure this text is shown twice)"));
        // check user permission with oneshot interaction mode -> multiple prompts and deny is oneshot deny
        GeneralSecuritySettings.setSecurityWarningsMode(session, appUID, GeneralSecuritySettings.USER_SECURITY_MODE);
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
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
        ac.checkPermission(new CommProtocolPermissionImpl("comm://", "\n(choose ALLOW)"));
        try
        {
            ac.checkPermission(new CommProtocolPermissionImpl("comm://", "\n(choose DENY)"));
            assertTrue(false);
        }
        catch (AccessControlException e)
        {
            assertTrue(true);
        }
        try
        {
            ac.checkPermission(new CommProtocolPermissionImpl("comm://", "\n(choose DENY)"));
            assertTrue(false);
        }
        catch (AccessControlException e)
        {
            assertTrue(true);
        }
        storage.removeGrantedPermissions(appUID);
        // check user permission with session interaction mode -> only one user prompt
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        GeneralSecuritySettings.setSecurityWarningsMode(session, appUID, GeneralSecuritySettings.DEFAULT_SECURITY_MODE);
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.HttpProtocolPermissionImpl",
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
        ac.checkPermission(new HttpProtocolPermissionImpl("http://somethingelse", "\n(choose ALLOW)"));
        ac.checkPermission(new HttpProtocolPermissionImpl("http://somethingelse", "\n(IF YOU SEE THIS PROMPT THE TEST FAILED)"));
        assertTrue(ac.checkPermission("javax.microedition.io.Connector.http") == 1);
        storage.removeGrantedPermissions(appUID);
        assertTrue(true);
        // isPermissionAllowed
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        GeneralSecuritySettings.setSecurityWarningsMode(session, appUID, GeneralSecuritySettings.USER_SECURITY_MODE);
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.HttpProtocolPermissionImpl",
                                          "http://",
                                          null,
                                          new UserSecuritySettingsImpl("Net Access",
                                                                       UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.NO_INTERACTION_MODE
                                                                                 })));
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.HttpsProtocolPermissionImpl",
                                          "https://",
                                          null,
                                          new UserSecuritySettingsImpl("Net Access",
                                                                       UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.NO_INTERACTION_MODE
                                                                                 })));
        storage.removeGrantedPermissions(appUID);
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        assertTrue(ac.checkPermission("javax.microedition.io.Connector.http") == -1);
        assertTrue(ac.checkPermission("javax.microedition.io.Connector.https") == -1);
        assertTrue(ac.checkPermission("javax.microedition.securityservice.CMSMessageSignatureService") == 0);
        // show note -> choose yes
        com.nokia.mj.impl.security.midp.authentication.InteractiveAuthenticationModuleTests.showGuidelines("Choose Yes for each of the following two prompts");
        assertTrue(ac.isPermissionAllowed("javax.microedition.io.Connector.http"));
        assertTrue(ac.isPermissionAllowed("javax.microedition.io.Connector.https"));
        assertTrue(ac.isPermissionAllowed("javax.microedition.media.control.VideoControl.getSnapshot") == false);
        // show note choose no
        com.nokia.mj.impl.security.midp.authentication.InteractiveAuthenticationModuleTests.showGuidelines("Choose No for each of the following two prompts");
        assertTrue(ac.isPermissionAllowed("javax.microedition.io.Connector.http") == false);
        assertTrue(ac.isPermissionAllowed("javax.microedition.io.Connector.https") == false);
        assertTrue(ac.isPermissionAllowed("javax.microedition.io.Connector.rtsp") == false);
        assertTrue(ac.isPermissionAllowed("javax.microedition.media.control.VideoControl.getSnapshot") == false);
        storage.removeGrantedPermissions(appUID);
        assertTrue(true);
        // check user permission with session interaction mode, user says no -> second prompt is not shown in normal security mode
        GeneralSecuritySettings.setSecurityWarningsMode(session, appUID, GeneralSecuritySettings.DEFAULT_SECURITY_MODE);
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.HttpProtocolPermissionImpl",
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
        try
        {
            ac.checkPermission(new HttpProtocolPermissionImpl("http://somethingelse","\n(choose DENY)"));
            assertTrue(false);
        }
        catch (AccessControlException e)
        {
            assertTrue(true);
        }
        try
        {
            ac.checkPermission(new HttpProtocolPermissionImpl("http://somethingelse", "\n(IF YOU SEE THIS PROMPT THE TEST FAILED)"));
            assertTrue(false);
        }
        catch (AccessControlException e)
        {
            assertTrue(true);
        }
        storage.removeGrantedPermissions(appUID);
        assertTrue(true);

        // check user permission with session interaction mode, user says no -> second prompt is not shown in user security mode
        GeneralSecuritySettings.setSecurityWarningsMode(session, appUID, GeneralSecuritySettings.USER_SECURITY_MODE);
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.HttpProtocolPermissionImpl",
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
        try
        {
            ac.checkPermission(new HttpProtocolPermissionImpl("http://somethingelse","\n(choose DENY)"));
            assertTrue(false);
        }
        catch (AccessControlException e)
        {
            assertTrue(true);
        }
        try
        {
            ac.checkPermission(new HttpProtocolPermissionImpl("http://somethingelse", "\n(IF YOU SEE THIS PROMPT THE TEST FAILED)"));
            assertTrue(false);
        }
        catch (AccessControlException e)
        {
            assertTrue(true);
        }
        storage.removeGrantedPermissions(appUID);
        assertTrue(true);
        // check user permission with blanket interaction mode, user says no -> second prompt is not shown
        GeneralSecuritySettings.setSecurityWarningsMode(session, appUID, GeneralSecuritySettings.DEFAULT_SECURITY_MODE);
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.FileProtocolPermissionImpl",
                                          "file://*",
                                          "read",
                                          new UserSecuritySettingsImpl("File Access",
                                                                       UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                                   UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                   UserSecuritySettings.NO_INTERACTION_MODE
                                                                                 })));
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.HttpProtocolPermissionImpl",
                                          "http://",
                                          null,
                                          new UserSecuritySettingsImpl("NetAccess",
                                                                       UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.NO_INTERACTION_MODE})));
        storage.removeGrantedPermissions(appUID);
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        try
        {
            ac.checkPermission(new FileProtocolPermissionImpl("file://myfile1.txt","read", "\n(choose DENY)"));
            assertTrue(false);
        }
        catch (AccessControlException e)
        {
            assertTrue(true);
        }
        try
        {
            ac.checkPermission(new FileProtocolPermissionImpl("file://myfile2.txt","read", "\n(IF YOU SEE THIS PROMPT THE TEST FAILED)"));
            assertTrue(false);
        }
        catch (AccessControlException e)
        {
            assertTrue(true);
        }
        storage.removeGrantedPermissions(appUID);
        assertTrue(true);
        storage.removeGrantedPermissions(appUID);

        // check user permission with blanket interaction mode, user says no -> second prompt is not shown
        GeneralSecuritySettings.setSecurityWarningsMode(session, appUID, GeneralSecuritySettings.USER_SECURITY_MODE);
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.FileProtocolPermissionImpl",
                                          "file://*",
                                          "read",
                                          new UserSecuritySettingsImpl("File Access",
                                                                       UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                                   UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                   UserSecuritySettings.NO_INTERACTION_MODE
                                                                                 })));
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.HttpProtocolPermissionImpl",
                                          "http://",
                                          null,
                                          new UserSecuritySettingsImpl("NetAccess",
                                                                       UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.NO_INTERACTION_MODE})));
        storage.removeGrantedPermissions(appUID);
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        try
        {
            ac.checkPermission(new FileProtocolPermissionImpl("file://myfile1.txt","read", "\n(choose DENY)"));
            assertTrue(false);
        }
        catch (AccessControlException e)
        {
            assertTrue(true);
        }
        try
        {
            ac.checkPermission(new FileProtocolPermissionImpl("file://myfile2.txt","read", "\n(IF YOU SEE THIS PROMPT THE TEST FAILED)"));
            assertTrue(false);
        }
        catch (AccessControlException e)
        {
            assertTrue(true);
        }
        storage.removeGrantedPermissions(appUID);
        assertTrue(true);
        storage.removeGrantedPermissions(appUID);
        // check user permission with prompt not shown in blanket interaction mode -> one user prompt only
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        GeneralSecuritySettings.setSecurityWarningsMode(session, appUID, GeneralSecuritySettings.DEFAULT_SECURITY_MODE);
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.FileProtocolPermissionImpl",
                                          "file://*",
                                          "read",
                                          new UserSecuritySettingsImpl("File Access",
                                                                       UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                                   UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                   UserSecuritySettings.NO_INTERACTION_MODE
                                                                                 })));
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.HttpProtocolPermissionImpl",
                                          "http://",
                                          null,
                                          new UserSecuritySettingsImpl("NetAccess",
                                                                       UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.NO_INTERACTION_MODE})));
        storage.removeGrantedPermissions(appUID);
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        ac.checkPermission(new FileProtocolPermissionImpl("file://myfile1.txt","read", "\n(choose ALLOW)"));
        ac.checkPermission(new FileProtocolPermissionImpl("file://myfile2.txt","read", "\n(IF YOU SEE THIS PROMPT THE TEST FAILED)"));
        ac.checkPermission(new HttpProtocolPermissionImpl("http://", "\n(choose ALLOW)"));
        ac.checkPermission(new HttpProtocolPermissionImpl("http://", "\n(IF YOU SEE THIS PROMPT THE TEST FAILED)"));
        storage.removeGrantedPermissions(appUID);
        assertTrue(true);
        storage.removeGrantedPermissions(appUID);
        // user permission with blanket interaction mode, show prompt, do an update, check if the promtps is still shown
        GeneralSecuritySettings.setSecurityWarningsMode(session, appUID, GeneralSecuritySettings.DEFAULT_SECURITY_MODE);
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.cleanup();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwOgwDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX1RoaXJkX1BhcnR5X1RydXN0ZWQwHhcNMDgwNjEyMDYyMzM2WhcNMTgwNjEwMDYyMzM2WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfVGhpcmRfUGFydHlfVHJ1c3RlZDCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEArAesh03rswICRUjexWMoIH9ZZqUtlx+tfhZhuzuYhiL6cLdhxNlU8yuIdw4f17XP3yxVIB8rz1EEagCMd0Y5ZleNFZ992ReLcf9VH13RxaZ88PRxBLLROm1jcwTknYSS6AxGMup77DMwwFSN8XPnOHWt4aLGUrecMeUyI2c5UbsCAwEAATANBgkqhkiG9w0BAQQFAAOBgQAylJ+3aSq6a7AAL7UM8V8JhGZlLneErF7A/BJGDi7Vmr5gmlGyf6RCJJIg4qCp+J3Tv3eCRNNuphQaSRdFpaulwCmtfCzSZNtvZg0VhMOVsZqTwQgmP6NzKoIbqqzlgm5U+yUJw8FmX17vgdeSEdJ/oL/WT4AKrWFJYl8MdQ9ZnQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "F9/dB/WITCC1n886yppP9QTQBeQT6/8pV0UKwbTU9L8mP5qFuX2bjw7gPQZNxAC/4qNHYnmnMmwkfOPDjZzbOojdGJyEYXFyF0sBtLZKYYHylW8uXz4HDdtC1c2WHn8/Zl+a0Zoa8MjuxGRi5IVNY4XONlpPfzLxkOMVjna4KhA="));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.microedition.io.Connector.http"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.microedition.io.Connector.http"));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.addSecurityData(session, appUID, null);
        permissionGranter.getBlanketPermissions(appUID);
        ac.checkPermission(new HttpProtocolPermissionImpl("http://", "\n(choose ALLOW)"));
        ac.checkPermission(new HttpProtocolPermissionImpl("http://", "\n(IF YOU SEE THIS PROMPT THE TEST FAILED)"));
        boolean blanketPromptShown = storage.readUserSecuritySettingsPromptFlag(appUID, "Net Access");
        assertTrue(blanketPromptShown);
        StorageEntry appPackageEntry = new StorageEntry();
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageAttribute.ID,
                                         appUID.getStringValue()));
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageNames.NAME,
                                         "MIDlet-Certificate-1-1"));
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageNames.VALUE,
                                         "MIICWDCCAcECBEhQwOgwDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX1RoaXJkX1BhcnR5X1RydXN0ZWQwHhcNMDgwNjEyMDYyMzM2WhcNMTgwNjEwMDYyMzM2WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfVGhpcmRfUGFydHlfVHJ1c3RlZDCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEArAesh03rswICRUjexWMoIH9ZZqUtlx+tfhZhuzuYhiL6cLdhxNlU8yuIdw4f17XP3yxVIB8rz1EEagCMd0Y5ZleNFZ992ReLcf9VH13RxaZ88PRxBLLROm1jcwTknYSS6AxGMup77DMwwFSN8XPnOHWt4aLGUrecMeUyI2c5UbsCAwEAATANBgkqhkiG9w0BAQQFAAOBgQAylJ+3aSq6a7AAL7UM8V8JhGZlLneErF7A/BJGDi7Vmr5gmlGyf6RCJJIg4qCp+J3Tv3eCRNNuphQaSRdFpaulwCmtfCzSZNtvZg0VhMOVsZqTwQgmP6NzKoIbqqzlgm5U+yUJw8FmX17vgdeSEdJ/oL/WT4AKrWFJYl8MdQ9ZnQ=="));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
        PermissionResolver.cleanup();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwOgwDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX1RoaXJkX1BhcnR5X1RydXN0ZWQwHhcNMDgwNjEyMDYyMzM2WhcNMTgwNjEwMDYyMzM2WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfVGhpcmRfUGFydHlfVHJ1c3RlZDCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEArAesh03rswICRUjexWMoIH9ZZqUtlx+tfhZhuzuYhiL6cLdhxNlU8yuIdw4f17XP3yxVIB8rz1EEagCMd0Y5ZleNFZ992ReLcf9VH13RxaZ88PRxBLLROm1jcwTknYSS6AxGMup77DMwwFSN8XPnOHWt4aLGUrecMeUyI2c5UbsCAwEAATANBgkqhkiG9w0BAQQFAAOBgQAylJ+3aSq6a7AAL7UM8V8JhGZlLneErF7A/BJGDi7Vmr5gmlGyf6RCJJIg4qCp+J3Tv3eCRNNuphQaSRdFpaulwCmtfCzSZNtvZg0VhMOVsZqTwQgmP6NzKoIbqqzlgm5U+yUJw8FmX17vgdeSEdJ/oL/WT4AKrWFJYl8MdQ9ZnQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "F9/dB/WITCC1n886yppP9QTQBeQT6/8pV0UKwbTU9L8mP5qFuX2bjw7gPQZNxAC/4qNHYnmnMmwkfOPDjZzbOojdGJyEYXFyF0sBtLZKYYHylW8uXz4HDdtC1c2WHn8/Zl+a0Zoa8MjuxGRi5IVNY4XONlpPfzLxkOMVjna4KhA="));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.microedition.io.Connector.http"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,appUID,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.microedition.io.Connector.http"));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID,appUID,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, appUID);
        permissionGranter.grantJarPermissions( appUID, appUID, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.addSecurityData(session, appUID, appUID);
        blanketPromptShown = storage.readUserSecuritySettingsPromptFlag(appUID, "Net Access");
        ac.checkPermission(new HttpProtocolPermissionImpl("http://", "\n(IF YOU SEE THIS PROMPT THE TEST FAILED)"));
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.cleanup();
        assertTrue(blanketPromptShown);
        // Add check user permission with prompt shown in blanket interaction mode -> no user prompts
        // legacy signed MIDlet, some permissions requested -> checkPermission for the granted perms succeeds and checkPermission for not granted perms fails
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        storage.removeAuthenticationStorageData(appUID);
        storage.removeGrantedPermissions(appUID);
        PermissionResolver.cleanup();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwA0wDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX0RldmljZV9NYW51ZmFjdHVyZXIwHhcNMDgwNjEyMDYxOTU3WhcNMTgwNjEwMDYxOTU3WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfRGV2aWNlX01hbnVmYWN0dXJlcjCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEApi3ipIPj8O33/rZre1euh7Itd9d6ZVf2mvQ/tLpHEsFJe5XLOsVViMvFskhhKUzmDsRbP4J03L6827/vEDIi/1i8kJGLaoFqJYaLGFWI2Zmxlj6aJV8cfZyOjmQPWJn1IDEe1ZAWbvMSp8xibWRsCLNEGKIkxQvAr/QDK/6iS+kCAwEAATANBgkqhkiG9w0BAQQFAAOBgQCDXt6klAs6yKvdTab3D2wkdoi3Lu4YFsMgLexJOu5HhAUJ/9VYO+Q2+mjd95MRsTa5rWQ2Jjvhn57Z3z/KBOwfHbymmNtMk6Gl14H0vQRoHa31jh3mTuAy5KotDVthaDp30aOizk473NU68wY1WdP4gFk5ZhrpNea9q3st13BxIQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "IcANmLKiOJQF8ABCNDj1PNNH/O8v9jfCVuiGBVm8enXDkM/gLwPjrC65sDKpOCHPqssUlHzjmVN5b9g8aRs4jxUOXNt2b732J7NSIPh97vw/WrP/KHdiooi/1KFUyklMyokK9ZrIv+GW1ttLCfKbuFupT9zmPAmWJQpnuD7J6sE="));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.microedition.io.Connector.http"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.microedition.io.Connector.http"));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" + System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.addSecurityData(session, appUID, null);
        signingCerts = authenticationModule.getCertificatesDetails(session, appUID);
        assertTrue(signingCerts != null
                   && signingCerts.length == 1
                   && signingCerts[0].getIssuer() != null
                   && signingCerts[0].getDateNotAfter() != null
                   && signingCerts[0].getDateNotBefore() != null
                   && signingCerts[0].getValidFrom() != null
                   && signingCerts[0].getValidUntil() != null
                   && signingCerts[0].getSerialNumber() != null
                   && signingCerts[0].getFingerprint() != null
                   && signingCerts[0].getFormattedFingerprint() != null
                   && signingCerts[0].getSubject() != null
                   && signingCerts[0].getOrganization() != null
                   && signingCerts[0].getProtectionDomain() != null
                   && signingCerts[0].getProtectionDomain().getName() != null
                   && signingCerts[0].getProtectionDomain().getCategory() != null);
        ac.checkPermission(new SocketProtocolPermissionImpl("socket://100", "\n(choose ALLOW)"));
        //ac.checkPermission(new HttpProtocolPermission("http://url"));
        ac.checkPermission(new SocketProtocolPermissionImpl("socket://100", "\n(IF YOU SEE THIS PROMPT THE TEST FAILED"));
        ac.checkPermission(new HttpProtocolPermissionImpl("http://url", "\n(IF YOU SEE THIS PROMPT THE TEST FAILED"));
        // 2 legacy unsigned MIDlets, install from JAD -> checkPermission succeeds with multiple prompts
        GeneralSecuritySettings.setSecurityWarningsMode(session, appUID, GeneralSecuritySettings.DEFAULT_SECURITY_MODE);
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        storage.removeAuthenticationStorageData(appUID);
        storage.removeGrantedPermissions(appUID);
        PermissionResolver.cleanup();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" + System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.addSecurityData(session, appUID, null);
        signingCerts = authenticationModule.getCertificatesDetails(session, appUID);
        assertTrue(signingCerts == null);
        ac.checkPermission(new HttpProtocolPermissionImpl("http://url", "\n(choose ALLOW)"));
        ac.checkPermission(new HttpsProtocolPermissionImpl("https://url1", "\n(IF YOU SEE THIS PROMPT THE TEST FAILED"));
        GeneralSecuritySettings.setSecurityWarningsMode(session, appUID, GeneralSecuritySettings.USER_SECURITY_MODE);
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        ac.checkPermission(new FileProtocolPermissionImpl("file://myfile1.txt","read", "\n(choose ALLOW)"));
        ac.checkPermission(new FileProtocolPermissionImpl("file://myfile1.txt","read", "\n(choose ALLOW)"));
        try
        {
            ac.checkPermission(new FileProtocolPermissionImpl("file://myfile1.txt","read", "\n(choose DENY)"));
            assertTrue(false);
        }
        catch (AccessControlException e)
        {
            assertTrue(true);
        }
        storage.removeGrantedPermissions(appUID);
        /* Uncomment if the mutual exclusive rules are to be applied in runtime security prompts
                // 2 check mutual exclusive rules: auto invocation is already set to blanket, then Net Access is set to Blanket -> check that auto invocation is changed to max allowed (exclusing blanket)
                GeneralSecuritySettings.setSecurityWarningsMode(session, appUID, GeneralSecuritySettings.DEFAULT_SECURITY_MODE);
                AccessControllerFactoryImpl.destroyAccessController(appUID);
                ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
                PermissionResolver.cleanup();
                grantedPermissions = new Vector();
                grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                    "something",
                    "*",
                    "autoinvocation",
                    new UserSecuritySettingsImpl("Application Auto Invocation",
                        UserSecuritySettings.BLANKET_INTERACTION_MODE,
                        new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                            UserSecuritySettings.SESSION_INTERACTION_MODE,
                            UserSecuritySettings.BLANKET_INTERACTION_MODE,
                            UserSecuritySettings.NO_INTERACTION_MODE})));
                grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                    "com.nokia.mj.impl.security.midp.authorization.HttpProtocolPermissionImpl",
                    "http://",
                    null,
                    new UserSecuritySettingsImpl("Net Access",
                        UserSecuritySettings.SESSION_INTERACTION_MODE,
                        new int[] { UserSecuritySettings.BLANKET_INTERACTION_MODE,
                            UserSecuritySettings.NO_INTERACTION_MODE})));
                storage.removeGrantedPermissions(appUID);
                storage.writeGrantedPermissions(appUID, null, grantedPermissions);
                ac.checkPermission(new HttpProtocolPermissionImpl("http://", "\n(choose ALLOW)"));
                Vector readPermissions = storage.readGrantedPermissions(appUID);
                boolean success = false;
                for(int i=0; i<readPermissions.size(); i++)
                {
                    UserSecuritySettings settings =
                        ((PolicyBasedPermission)readPermissions.elementAt(i))
                        .getUserSecuritySettings();
                    if (settings != null && settings.getName()
                        .equalsIgnoreCase("Application Auto Invocation"))
                    {
                        success = (settings.getCurrentInteractionMode()
                            == UserSecuritySettings.SESSION_INTERACTION_MODE);
                    }
                }
                storage.removeGrantedPermissions(appUID);
                assertTrue(success);*/
        // test parralel instances of the ui: show a security prompt, show an error, destroy the ui for error, destroy the ui for the security prompt
        GeneralSecuritySettings.setSecurityWarningsMode(session, appUID, GeneralSecuritySettings.DEFAULT_SECURITY_MODE);
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.CommProtocolPermissionImpl",
                                          "comm://",
                                          null,
                                          new UserSecuritySettingsImpl("Low Level Net Access",
                                                                       UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.NO_INTERACTION_MODE
                                                                                 })));
        storage.removeGrantedPermissions(appUID);
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        ac.checkPermission(new CommProtocolPermissionImpl("comm://", "\n(choose ALLOW)"));
        RuntimeUi ui = RuntimeUiFactory.getRuntimeUi();
        ExceptionBase exc = new ExceptionBase(
            new SecurityErrorMessage(),
            SecurityErrorMessage.JAR_TAMPERED,
            null,
            new SecurityDetailedErrorMessage(),
            SecurityDetailedErrorMessage.JAR_TAMPERED,
            null);
        ui.error(appName, exc);
        ui.destroy();
        // check prompt for blanket
        GeneralSecuritySettings.setSecurityWarningsMode(session, appUID, GeneralSecuritySettings.DEFAULT_SECURITY_MODE);
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        PermissionResolver.cleanup();
        grantedPermissions = new Vector();
        grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                                          "com.nokia.mj.impl.security.midp.authorization.FileProtocolPermissionImpl",
                                          "file://*",
                                          "read",
                                          new UserSecuritySettingsImpl("File Access",
                                                                       UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                       new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                   UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                                   UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                   UserSecuritySettings.NO_INTERACTION_MODE
                                                                                 })));
        storage.removeGrantedPermissions(appUID);
        storage.writeGrantedPermissions(appUID, null, grantedPermissions);
        ac.checkPermission(new FileProtocolPermissionImpl("file://blahblah","read","\n(choose ALLOW)"));
        ac.checkPermission(new FileProtocolPermissionImpl("file://myfile1.txt","read", "\n(IF YOU SEE THIS PROMPT THE TEST FAILED\")"));
        ac.checkPermission(new FileProtocolPermissionImpl("file://blahblah","read", "\n(IF YOU SEE THIS PROMPT THE TEST FAILED"));
        storage.removeGrantedPermissions(appUID);
        assertTrue(true);
        AccessControllerFactoryImpl.destroyAccessController(appUID);
        /* Uncomment this from time to time and check if QT still fails: create QT thread, destroy QT thread, create QT thread -> this fails
                // test sequential instances of the ui: show an error, destroy the ui for the error, show security prompt, destroy ui for security prompt
                ui = RuntimeUiFactory.getRuntimeUi();
                exc = new ExceptionBase(
                    new SecurityErrorMessage(),
                    SecurityErrorMessage.CERT_NOT_AVAILABLE,
                    null,
                    new SecurityDetailedErrorMessage(),
                    SecurityDetailedErrorMessage.CERT_DISABLED,
                    null);
                ui.error("myMIDlet", exc);
                ui.destroy();
                ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
                PermissionResolver.cleanup();
                grantedPermissions = new Vector();
                grantedPermissions.addElement(new PolicyBasedPermissionImpl(
                    "com.nokia.mj.impl.security.midp.authorization.FileProtocolPermissionImpl",
                    "file://*",
                    "read",
                    new UserSecuritySettingsImpl("Low Level Net Access",
                        UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                        new int[] { UserSecuritySettings.ONESHOT_INTERACTION_MODE})));
                storage.removeGrantedPermissions(appUID);
                storage.writeGrantedPermissions(appUID, null, grantedPermissions);
                ac.checkPermission(new FileProtocolPermissionImpl("file://somepath/somesub-path/somefile.someextension", "\n(choose ALLOW)"));
                AccessControllerFactoryImpl.destroyAccessController(appUID);*/
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, appUID);
        PermissionMappingTable.setMappingTable(IMPL_PERMISSION_MAPPING_TABLE);
        SecurityPolicyModule.policiesDir = TEST_DATA_DIR + "securitypolicies"  + System.getProperty("file.separator");
    }
}
