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


package com.nokia.mj.impl.security.midp.common;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.security.midp.authentication.AuthenticationModule;
import com.nokia.mj.impl.security.midp.authorization.PermissionGranter;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.storage.*;
import com.nokia.mj.impl.security.midp.storage.SecurityStorage;
import com.nokia.mj.impl.security.midp.authorization.PermissionResolver;
import com.nokia.mj.impl.security.midp.common.PermissionAttribute;
import com.nokia.mj.impl.security.midp.common.SecurityAttributes;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermissionImpl;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.ErrorMessageBase;
import com.nokia.mj.impl.utils.OtaStatusCode;
import java.util.Vector;
import java.util.Hashtable;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * SecurityExtensions unit tests.
 */
public class SecurityExtensionsTests extends TestCase implements InstallerMain
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

    protected InstallerErrorMessage errorMessage = new InstallerErrorMessage();
    protected InstallerDetailedErrorMessage detailedErrorMessage = new InstallerDetailedErrorMessage();
    Hashtable allAttributes = new Hashtable();
    AuthenticationModule authenticationModule;
    PermissionGranter permissionGranter;
    StorageSession session;
    SecurityStorage storage;
    Uid appUID;
    protected static final String MIDP_PROFILE_ATTRIBUTE_NAME = "MicroEdition-Profile";
    protected static final String MIDP3 = "MIDP-3.0";
    protected static final String MIDP2 = "MIDP-2.0";

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new SecurityExtensionsTests("test", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SecurityExtensionsTests)tc).test();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public SecurityExtensionsTests()
    {
    }

    public SecurityExtensionsTests(String aTestName, TestMethod aTestMethod)
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
        session = StorageFactory.createSession();
        session.open();
        storage = new SecurityStorage();
        appUID = Uid.createUid("myUID");
    }

    protected void tearDown()
    {
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
    }

    protected void test()
    {
        SecurityAttributes securityAttributes;
        AuthenticationCredentials[] credentials;
        // package names
        assertTrue(find(SecurityExtensionsReader.getExtProtectedPackages(), new String[] {"com.nokia.ext1.internal.", "com.nokia.ext2.internal."})
                   && !find(SecurityExtensionsReader.getExtProtectedPackages(), new String[] {"com.nokia.ext1.public."})
                   && !find(SecurityExtensionsReader.getExtProtectedPackages(), new String[] {"com.nokia.ext2.public."})
                   && find(SecurityExtensionsReader.getExtRestrictedPackages(), new String[] {"com.nokia.ext1.public.", "com.nokia.ext2.public."})
                   && !find(SecurityExtensionsReader.getExtRestrictedPackages(), new String[] {"com.nokia.ext1.internal."})
                   && !find(SecurityExtensionsReader.getExtRestrictedPackages(), new String[] {"com.nokia.ext2.internal."}));
        // mappings
        MIDPPermission perm = SecurityExtensionsReader.getExtPermission("com.nokia.ext1.public.Ext1Perm");
        assertTrue(perm != null && perm.getName() != null && perm.getName().equals("com.nokia.ext1.internal.Ext1Perm") && perm.getTarget() != null && perm.getTarget().equals("*")&& perm.getActionList() == null);
        assertTrue(SecurityExtensionsReader.getExtPermission("com.nokia.mj.impl.gcf.protocol.socket.SocketPermissionImpl") == null);
        perm = SecurityExtensionsReader.getExtPermission("com.nokia.ext2.public.Ext2Perm");
        assertTrue(perm != null && perm.getName() != null && perm.getName().equals("com.nokia.ext2.internal.Ext2Perm") && perm.getTarget() != null && perm.getTarget().equals("*")&& perm.getActionList() == null);
        // policies (unsigned suite, check that ext1 perms are granted and the base permissions were not altered)
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes.addManifestAttributes(allAttributes);
        authenticationModule.authenticateJar(session, appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        assertTrue(checkGrantedPermissions(storage.readGrantedPermissions(appUID),
                                           new PolicyBasedPermissionImpl[]
                                           {
                                               new PolicyBasedPermissionImpl("com.nokia.ext1.internal.Ext1Perm", "ext1.target1", "ext1.action1", new UserSecuritySettingsImpl("Ext1", UserSecuritySettings.ONESHOT_INTERACTION_MODE, new int[] {UserSecuritySettings.ONESHOT_INTERACTION_MODE, UserSecuritySettings.SESSION_INTERACTION_MODE, UserSecuritySettings.BLANKET_INTERACTION_MODE, UserSecuritySettings.NO_INTERACTION_MODE})),
                                               new PolicyBasedPermissionImpl("com.nokia.ext1.internal.Ext1Perm", "ext1.target2", "ext1.action2", new UserSecuritySettingsImpl("Net Access", UserSecuritySettings.SESSION_INTERACTION_MODE, new int[] {UserSecuritySettings.ONESHOT_INTERACTION_MODE, UserSecuritySettings.SESSION_INTERACTION_MODE, UserSecuritySettings.NO_INTERACTION_MODE})),
                                               new PolicyBasedPermissionImpl("com.nokia.ext1.internal.Ext1Perm", "ext1.target3", "ext1.action3", null),
                                               new PolicyBasedPermissionImpl("com.nokia.mj.impl.gcf.protocol.http.HttpPermissionImpl", "http://*", null, new UserSecuritySettingsImpl("Net Access", UserSecuritySettings.SESSION_INTERACTION_MODE, new int[] {UserSecuritySettings.ONESHOT_INTERACTION_MODE, UserSecuritySettings.SESSION_INTERACTION_MODE, UserSecuritySettings.NO_INTERACTION_MODE})),
                                               new PolicyBasedPermissionImpl("com.nokia.mj.impl.file.FileInternalPermission", "*", "read", new UserSecuritySettingsImpl("Read User Data Access", UserSecuritySettings.ONESHOT_INTERACTION_MODE, new int[] {UserSecuritySettings.ONESHOT_INTERACTION_MODE, UserSecuritySettings.SESSION_INTERACTION_MODE, UserSecuritySettings.NO_INTERACTION_MODE})),
                                               new PolicyBasedPermissionImpl("com.nokia.mj.impl.mms.MMSPermissionImpl", "mms://*", "send", new UserSecuritySettingsImpl("Messaging", UserSecuritySettings.ONESHOT_INTERACTION_MODE, new int[] {UserSecuritySettings.ONESHOT_INTERACTION_MODE, UserSecuritySettings.SESSION_INTERACTION_MODE, UserSecuritySettings.NO_INTERACTION_MODE}))
                                           }));
        // policies (trusted third party suite which requests ext1 public perms fails
        try
        {
            storage.removeAuthenticationStorageData(appUID);
            permissionGranter.removeSecurityData(session,appUID);
            PermissionResolver.testClearCache();
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwA0wDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX0RldmljZV9NYW51ZmFjdHVyZXIwHhcNMDgwNjEyMDYxOTU3WhcNMTgwNjEwMDYxOTU3WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfRGV2aWNlX01hbnVmYWN0dXJlcjCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEApi3ipIPj8O33/rZre1euh7Itd9d6ZVf2mvQ/tLpHEsFJe5XLOsVViMvFskhhKUzmDsRbP4J03L6827/vEDIi/1i8kJGLaoFqJYaLGFWI2Zmxlj6aJV8cfZyOjmQPWJn1IDEe1ZAWbvMSp8xibWRsCLNEGKIkxQvAr/QDK/6iS+kCAwEAATANBgkqhkiG9w0BAQQFAAOBgQCDXt6klAs6yKvdTab3D2wkdoi3Lu4YFsMgLexJOu5HhAUJ/9VYO+Q2+mjd95MRsTa5rWQ2Jjvhn57Z3z/KBOwfHbymmNtMk6Gl14H0vQRoHa31jh3mTuAy5KotDVthaDp30aOizk473NU68wY1WdP4gFk5ZhrpNea9q3st13BxIQ=="));
            allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "IcANmLKiOJQF8ABCNDj1PNNH/O8v9jfCVuiGBVm8enXDkM/gLwPjrC65sDKpOCHPqssUlHzjmVN5b9g8aRs4jxUOXNt2b732J7NSIPh97vw/WrP/KHdiooi/1KFUyklMyokK9ZrIv+GW1ttLCfKbuFupT9zmPAmWJQpnuD7J6sE="));
            allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","com.nokia.ext1.public.Ext1Perm"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            credentials = authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
            permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), credentials);
            assertTrue(false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(
                e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                                                 new String[] {PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME})));
        }
        // policies (trusted third party suite which requests ext1 internal perms fails
        try
        {
            storage.removeAuthenticationStorageData(appUID);
            permissionGranter.removeSecurityData(session,appUID);
            PermissionResolver.testClearCache();
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwA0wDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX0RldmljZV9NYW51ZmFjdHVyZXIwHhcNMDgwNjEyMDYxOTU3WhcNMTgwNjEwMDYxOTU3WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfRGV2aWNlX01hbnVmYWN0dXJlcjCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEApi3ipIPj8O33/rZre1euh7Itd9d6ZVf2mvQ/tLpHEsFJe5XLOsVViMvFskhhKUzmDsRbP4J03L6827/vEDIi/1i8kJGLaoFqJYaLGFWI2Zmxlj6aJV8cfZyOjmQPWJn1IDEe1ZAWbvMSp8xibWRsCLNEGKIkxQvAr/QDK/6iS+kCAwEAATANBgkqhkiG9w0BAQQFAAOBgQCDXt6klAs6yKvdTab3D2wkdoi3Lu4YFsMgLexJOu5HhAUJ/9VYO+Q2+mjd95MRsTa5rWQ2Jjvhn57Z3z/KBOwfHbymmNtMk6Gl14H0vQRoHa31jh3mTuAy5KotDVthaDp30aOizk473NU68wY1WdP4gFk5ZhrpNea9q3st13BxIQ=="));
            allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "IcANmLKiOJQF8ABCNDj1PNNH/O8v9jfCVuiGBVm8enXDkM/gLwPjrC65sDKpOCHPqssUlHzjmVN5b9g8aRs4jxUOXNt2b732J7NSIPh97vw/WrP/KHdiooi/1KFUyklMyokK9ZrIv+GW1ttLCfKbuFupT9zmPAmWJQpnuD7J6sE="));
            allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","com.nokia.ext1.internal.Ext1Perm"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            credentials = authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
            permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), credentials);
            assertTrue(false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(
                e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                                                 new String[] {PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME})));
        }
        // policies (trusted third party suite which requests ext2 internal perms
        try
        {
            storage.removeAuthenticationStorageData(appUID);
            permissionGranter.removeSecurityData(session,appUID);
            PermissionResolver.testClearCache();
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICyjCCAjOgAwIBAgIJANglmaqnbnYRMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTEzMTMzN1oXDTE1MDIwNDEzMTMzN1owVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMkJNh9P28KvJCgoJ3rwlpl0mVSpEVrl9UikxvBCqccQCJNuvcVKy8N06l8CGvwNVDhFb3eLyg5aUA8BsdcsTNfCxkItrbdyj7rNvvrPRztt42gFY1ZBSJKzkAMzXe1+d8SsjzwUxwA0xqhmA8PGVEhCruLtgAL3OPy3Y382SGmaQgiSBlYl/lkS8o8w6yNHXUUs/NC+1fQ7FRpb9VBueDT4neOhZopNZH+9VFH1vGT4oZPIn+fJY+Ue69WwDXXnHSNrrr5pDx+H882Fh7Z1Lswq3FVPzQCN69QqUpzGwaGF4NaMGywJYHnD6t3DHtGZwHnVZoCJx/L/P58JOWmNqz0CAwEAATANBgkqhkiG9w0BAQUFAAOBgQBgbsN4vL4Vh/NVmAACVqcIKrbYp6SiRA72a9P4cusyShq+ogkTweas9DMyX/DkEtbnjSEkRPnvcKydsux7fARBg+fzk/AJ6Q8vH9RMSxwbwygoqieHID68ahGnx0/E1KUuw78Og2zQScvoE7xn9jCiWoTzA5qfOKZJ445wonS0dw=="));
            allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "dfDsGKZLIj8Bx96d9C1OsHyCGiupL0+sXxElCb2clanbgipDTOEGFlyo2uY1YjmVsZ0HC0Hms1ZJIGaf9WHMeuesMUncSqJyN0Euj/k7I8eBD8kHhoR0qqBDRzqLZhPelqqcsWCGyS0whZiBreOujiKoKAPMVbFYQugZ+JiSS7qJ++rs7HmLd6/OQkH+XIoQO7ffcBLKTIG2ezGGtQgELkDZIs7lHOiH6NraqxQ2EHz8dgdOyVtfW7aHOVlq0yZLN8Gai8w5JXV7O/FSvkLxV+i40jPs/SPVTc1dZm4UWms/gBCCH1bIZfEdq9lQFdVLeEmlYluCW8QCfhZ7dGSQow=="));
            allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","com.nokia.ext2.internal.Ext2Perm"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            credentials = authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
            permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), credentials);
            assertTrue(false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(
                e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                                                 new String[] {PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME})));
        }
        // policies (trusted third party suite which requests ext2 public perms
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICyjCCAjOgAwIBAgIJANglmaqnbnYRMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTEzMTMzN1oXDTE1MDIwNDEzMTMzN1owVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMkJNh9P28KvJCgoJ3rwlpl0mVSpEVrl9UikxvBCqccQCJNuvcVKy8N06l8CGvwNVDhFb3eLyg5aUA8BsdcsTNfCxkItrbdyj7rNvvrPRztt42gFY1ZBSJKzkAMzXe1+d8SsjzwUxwA0xqhmA8PGVEhCruLtgAL3OPy3Y382SGmaQgiSBlYl/lkS8o8w6yNHXUUs/NC+1fQ7FRpb9VBueDT4neOhZopNZH+9VFH1vGT4oZPIn+fJY+Ue69WwDXXnHSNrrr5pDx+H882Fh7Z1Lswq3FVPzQCN69QqUpzGwaGF4NaMGywJYHnD6t3DHtGZwHnVZoCJx/L/P58JOWmNqz0CAwEAATANBgkqhkiG9w0BAQUFAAOBgQBgbsN4vL4Vh/NVmAACVqcIKrbYp6SiRA72a9P4cusyShq+ogkTweas9DMyX/DkEtbnjSEkRPnvcKydsux7fARBg+fzk/AJ6Q8vH9RMSxwbwygoqieHID68ahGnx0/E1KUuw78Og2zQScvoE7xn9jCiWoTzA5qfOKZJ445wonS0dw=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "dfDsGKZLIj8Bx96d9C1OsHyCGiupL0+sXxElCb2clanbgipDTOEGFlyo2uY1YjmVsZ0HC0Hms1ZJIGaf9WHMeuesMUncSqJyN0Euj/k7I8eBD8kHhoR0qqBDRzqLZhPelqqcsWCGyS0whZiBreOujiKoKAPMVbFYQugZ+JiSS7qJ++rs7HmLd6/OQkH+XIoQO7ffcBLKTIG2ezGGtQgELkDZIs7lHOiH6NraqxQ2EHz8dgdOyVtfW7aHOVlq0yZLN8Gai8w5JXV7O/FSvkLxV+i40jPs/SPVTc1dZm4UWms/gBCCH1bIZfEdq9lQFdVLeEmlYluCW8QCfhZ7dGSQow=="));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","com.nokia.ext2.public.Ext2Perm"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes.addManifestAttributes(allAttributes);
        authenticationModule.authenticateJar(session, appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        assertTrue(checkGrantedPermissions(storage.readGrantedPermissions(appUID),
                                           new PolicyBasedPermissionImpl[]
                                           {
                                               new PolicyBasedPermissionImpl("com.nokia.ext2.internal.Ext2Perm", "ext2.target1", "ext2.action1", new UserSecuritySettingsImpl("Ext1", UserSecuritySettings.BLANKET_INTERACTION_MODE, new int[] {UserSecuritySettings.BLANKET_INTERACTION_MODE, UserSecuritySettings.NO_INTERACTION_MODE})),
                                               new PolicyBasedPermissionImpl("com.nokia.ext2.internal.Ext2Perm", "ext2.target2", "ext2.action2", new UserSecuritySettingsImpl("Ext2", UserSecuritySettings.ONESHOT_INTERACTION_MODE, new int[] {UserSecuritySettings.ONESHOT_INTERACTION_MODE, UserSecuritySettings.NO_INTERACTION_MODE})),
                                               new PolicyBasedPermissionImpl("com.nokia.ext2.internal.Ext2Perm", "ext2.target3", "ext2.action3", null),
                                               new PolicyBasedPermissionImpl("com.nokia.ext2.internal.Ext2Perm", "ext2.target4", "ext2.action4", null),
                                               new PolicyBasedPermissionImpl("com.nokia.ext2.internal.Ext2Perm", "ext2.target5", "ext2.action5", new UserSecuritySettingsImpl("Messaging", UserSecuritySettings.SESSION_INTERACTION_MODE, new int[] {UserSecuritySettings.ONESHOT_INTERACTION_MODE, UserSecuritySettings.NO_INTERACTION_MODE, UserSecuritySettings.BLANKET_INTERACTION_MODE, UserSecuritySettings.SESSION_INTERACTION_MODE}))
                                           }));
    }

    private static boolean checkGrantedPermissions(Vector grantedPermissions, PolicyBasedPermissionImpl[] expectedPerms)
    {
        if (grantedPermissions == null)
        {
            return false;
        }
        boolean[] grantedPermissionsChecked =
            new boolean[expectedPerms.length];
        com.nokia.mj.impl.security.midp.authorization.PermissionGranterTests.initBoolArray(grantedPermissionsChecked);
        for (int i=0; i<grantedPermissions.size(); i++)
        {
            int index = -1;
            PolicyBasedPermissionImpl grantedPermission = (PolicyBasedPermissionImpl)grantedPermissions.elementAt(i);
            for (int j=0; j<expectedPerms.length; j++)
            {
                if (expectedPerms[j].equals(grantedPermission))
                {
                    index = j;
                    break;
                }
            }
            if (index >= 0)
            {
                grantedPermissionsChecked[index] = true;
            }
        }
        return com.nokia.mj.impl.security.midp.authorization.PermissionGranterTests.checkBoolArray(grantedPermissionsChecked);
    }

    private static boolean find(Vector vStr, String[] expectedStrs)
    {
        if (vStr == null)
        {
            return false;
        }
        boolean[] strsChecked =
            new boolean[expectedStrs.length];
        com.nokia.mj.impl.security.midp.authorization.PermissionGranterTests.initBoolArray(strsChecked);
        for (int i=0; i<vStr.size(); i++)
        {
            int index = -1;
            String str = (String)vStr.elementAt(i);
            for (int j=0; j<expectedStrs.length; j++)
            {
                if (expectedStrs[j].equals(str))
                {
                    index = j;
                    break;
                }
            }
            if (index >= 0)
            {
                strsChecked[index] = true;
            }
        }
        return com.nokia.mj.impl.security.midp.authorization.PermissionGranterTests.checkBoolArray(strsChecked);
    }

}
