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


package com.nokia.mj.impl.security;

import java.util.Hashtable;
import java.util.Vector;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.security.midp.authentication.AuthenticationModule;
import com.nokia.mj.impl.security.midp.authorization.PermissionGranter;
import com.nokia.mj.impl.security.midp.common.AuthenticationCredentials;
import com.nokia.mj.impl.security.common.InstallerSecurityException;
import com.nokia.mj.impl.security.midp.common.SecurityAttributes;
import com.nokia.mj.impl.security.midp.common.MIDPPermission;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermissionImpl;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettingsImpl;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettings;
import com.nokia.mj.impl.security.midp.common.SigningCertificate;
import com.nokia.mj.impl.security.midp.common.ProtectionDomain;
import com.nokia.mj.impl.security.midp.common.AuthenticationAttribute;
import com.nokia.mj.impl.security.midp.common.PermissionAttribute;
import com.nokia.mj.impl.security.midp.authorization.AccessControllerImpl;
import com.nokia.mj.impl.security.midp.authorization.AccessControllerFactoryImpl;
import com.nokia.mj.impl.security.midp.authorization.PermissionResolver;
import com.nokia.mj.impl.security.midp.common.PermissionMappingTable;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.storage.*;
import com.nokia.mj.impl.security.midp.storage.SecurityStorage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.security.midp.authorization.PermissionGranterTests;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.security.midp.common.AuthenticationCredentials;
import com.nokia.mj.impl.security.midp.storage.AuthenticationStorageData;


import java.security.AccessControlException;
import javax.microedition.io.*;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * AuthenticationModule unit tests.
 */
public class OMJSecurityTests extends TestCase implements InstallerMain
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

    // class to be tested
    AuthenticationModule authenticationModule;
    PermissionGranter permissionGranter;
    StorageSession session;
    SecurityStorage storage;
    Uid appUID;
    Uid appUID2;
    Vector grantedPermissions;
    String[] blanketPermissions;
    SigningCertificate[] signingCerts;
    AccessControllerImpl ac;
    int assertTrace = 0;

    /**
     * Hashtable holding all the attributes. Tests populate this hashtable
     * prior to being executed
     */
    protected Hashtable allAttributes;

    // general-purpose constants
    protected static final String MIDP_PROFILE_ATTRIBUTE_NAME = "MicroEdition-Profile";
    protected static final String MIDP3 = "MIDP-3.0";
    protected static final String MIDP2 = "MIDP-2.0";
    private static final String COMMA = ",";

    // references to error messages
    protected InstallerErrorMessage errorMessage = new InstallerErrorMessage();
    protected InstallerDetailedErrorMessage detailedErrorMessage = new InstallerDetailedErrorMessage();

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new OMJSecurityTests("testAuthentication", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((OMJSecurityTests)tc).test();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public OMJSecurityTests()
    {
    }

    public OMJSecurityTests(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    private void assertWithTrace(boolean aCondition)
    {
        assertTrue("" + assertTrace, aCondition);
        assertTrace++;
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
        appUID2 = Uid.createUid("myUID2");
    }

    protected void tearDown()
    {
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
        StorageEntry appPackageEntry = new StorageEntry();
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageAttribute.ID,
                                         appUID.getStringValue()));
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
    }

    private void test()
    {
        // prepare the storage data
        allAttributes = new Hashtable();
        SecurityAttributes securityAttributes = new SecurityAttributes();
        String appName = "OMJSecurityTests";
        ac = AccessControllerFactoryImpl.getAccessController(appUID, appName);
        AuthenticationCredentials[] credentials;
        StorageEntry appPackageEntry;
        ProtectionDomain domain = null;
        AuthenticationStorageData authData = null;
        AuthenticationCredentials[] authCredentials = null;
        // Legacy signed MIDlet, no permissions requested -> any checkPermission must fail
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwA0wDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX0RldmljZV9NYW51ZmFjdHVyZXIwHhcNMDgwNjEyMDYxOTU3WhcNMTgwNjEwMDYxOTU3WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfRGV2aWNlX01hbnVmYWN0dXJlcjCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEApi3ipIPj8O33/rZre1euh7Itd9d6ZVf2mvQ/tLpHEsFJe5XLOsVViMvFskhhKUzmDsRbP4J03L6827/vEDIi/1i8kJGLaoFqJYaLGFWI2Zmxlj6aJV8cfZyOjmQPWJn1IDEe1ZAWbvMSp8xibWRsCLNEGKIkxQvAr/QDK/6iS+kCAwEAATANBgkqhkiG9w0BAQQFAAOBgQCDXt6klAs6yKvdTab3D2wkdoi3Lu4YFsMgLexJOu5HhAUJ/9VYO+Q2+mjd95MRsTa5rWQ2Jjvhn57Z3z/KBOwfHbymmNtMk6Gl14H0vQRoHa31jh3mTuAy5KotDVthaDp30aOizk473NU68wY1WdP4gFk5ZhrpNea9q3st13BxIQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "IcANmLKiOJQF8ABCNDj1PNNH/O8v9jfCVuiGBVm8enXDkM/gLwPjrC65sDKpOCHPqssUlHzjmVN5b9g8aRs4jxUOXNt2b732J7NSIPh97vw/WrP/KHdiooi/1KFUyklMyokK9ZrIv+GW1ttLCfKbuFupT9zmPAmWJQpnuD7J6sE="));
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, null);
        signingCerts = authenticationModule.getCertificatesDetails(session, appUID);
        assertWithTrace(signingCerts != null
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
        try
        {
            ac.checkPermission(new CommProtocolPermission("comm://"));
            assertWithTrace(false);
        }
        catch (AccessControlException e)
        {
            assertWithTrace(true);
        }
        // legacy signed MIDlet, some permissions requested -> checkPermission for not granted perms fails
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
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
        credentials = authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, null);
        signingCerts = authenticationModule.getCertificatesDetails(session, appUID);
        assertWithTrace(signingCerts != null
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
        try
        {
            ac.checkPermission(new HttpsProtocolPermission("https://url"));
            assertWithTrace(false);
        }
        catch (AccessControlException e)
        {
            assertWithTrace(true);
        }
        // 2 legacy signed MIDlets, some permissions requested
        storage.removeAuthenticationStorageData(appUID);
        storage.removeAuthenticationStorageData(appUID2);
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
        credentials = authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, null);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwA0wDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX0RldmljZV9NYW51ZmFjdHVyZXIwHhcNMDgwNjEyMDYxOTU3WhcNMTgwNjEwMDYxOTU3WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfRGV2aWNlX01hbnVmYWN0dXJlcjCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEApi3ipIPj8O33/rZre1euh7Itd9d6ZVf2mvQ/tLpHEsFJe5XLOsVViMvFskhhKUzmDsRbP4J03L6827/vEDIi/1i8kJGLaoFqJYaLGFWI2Zmxlj6aJV8cfZyOjmQPWJn1IDEe1ZAWbvMSp8xibWRsCLNEGKIkxQvAr/QDK/6iS+kCAwEAATANBgkqhkiG9w0BAQQFAAOBgQCDXt6klAs6yKvdTab3D2wkdoi3Lu4YFsMgLexJOu5HhAUJ/9VYO+Q2+mjd95MRsTa5rWQ2Jjvhn57Z3z/KBOwfHbymmNtMk6Gl14H0vQRoHa31jh3mTuAy5KotDVthaDp30aOizk473NU68wY1WdP4gFk5ZhrpNea9q3st13BxIQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "IcANmLKiOJQF8ABCNDj1PNNH/O8v9jfCVuiGBVm8enXDkM/gLwPjrC65sDKpOCHPqssUlHzjmVN5b9g8aRs4jxUOXNt2b732J7NSIPh97vw/WrP/KHdiooi/1KFUyklMyokK9ZrIv+GW1ttLCfKbuFupT9zmPAmWJQpnuD7J6sE="));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.microedition.io.Connector.http"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID2,null,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID2, null, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.microedition.io.Connector.http"));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID2,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID2, null);
        permissionGranter.grantJarPermissions( appUID2, null, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID2, null);
        // 2 legacy unsigned MIDlets, install from JAD -> checkPermission succeeds
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
        credentials = authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, null);
        signingCerts = authenticationModule.getCertificatesDetails(session, appUID);
        assertWithTrace(signingCerts == null);
        ac.checkPermission(new java.util.PropertyPermission("microedition.version", "read"));
        // legacy unsigned MIDlet, install from JAR -> checkPermission suceeds (with showing the prompts except for assigned perms)
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, null);
        signingCerts = authenticationModule.getCertificatesDetails(session, appUID);
        assertWithTrace(signingCerts == null);
        ac.checkPermission(new java.util.PropertyPermission("microedition.version", "read"));
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        // update from trusted to untrusted fails
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
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
        credentials = authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        storage.removeAuthenticationStorageData(appUID2);
        permissionGranter.removeSecurityData(session,appUID2);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        try
        {
            credentials = authenticationModule.authenticateJad(appUID2,appUID,securityAttributes.getAuthenticationAttributes());
            assertWithTrace(false);
        }
        catch (InstallerSecurityException e)
        {
            assertWithTrace(e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                            && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_AUTHORIZATION_ERR, null))
                            && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.DIFFERENT_SIGNERS, null)));
        }
        // update from untrusted to trusted fails for MIDP3 MIDlets
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, null);
        storage.removeAuthenticationStorageData(appUID2);
        permissionGranter.removeSecurityData(session,appUID2);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwA0wDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX0RldmljZV9NYW51ZmFjdHVyZXIwHhcNMDgwNjEyMDYxOTU3WhcNMTgwNjEwMDYxOTU3WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfRGV2aWNlX01hbnVmYWN0dXJlcjCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEApi3ipIPj8O33/rZre1euh7Itd9d6ZVf2mvQ/tLpHEsFJe5XLOsVViMvFskhhKUzmDsRbP4J03L6827/vEDIi/1i8kJGLaoFqJYaLGFWI2Zmxlj6aJV8cfZyOjmQPWJn1IDEe1ZAWbvMSp8xibWRsCLNEGKIkxQvAr/QDK/6iS+kCAwEAATANBgkqhkiG9w0BAQQFAAOBgQCDXt6klAs6yKvdTab3D2wkdoi3Lu4YFsMgLexJOu5HhAUJ/9VYO+Q2+mjd95MRsTa5rWQ2Jjvhn57Z3z/KBOwfHbymmNtMk6Gl14H0vQRoHa31jh3mTuAy5KotDVthaDp30aOizk473NU68wY1WdP4gFk5ZhrpNea9q3st13BxIQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "IcANmLKiOJQF8ABCNDj1PNNH/O8v9jfCVuiGBVm8enXDkM/gLwPjrC65sDKpOCHPqssUlHzjmVN5b9g8aRs4jxUOXNt2b732J7NSIPh97vw/WrP/KHdiooi/1KFUyklMyokK9ZrIv+GW1ttLCfKbuFupT9zmPAmWJQpnuD7J6sE="));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("","javax.microedition.io.SocketProtocolPermission socket://"));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "2",new Attribute("","javax.microedition.io.HttpProtocolPermission http://www.google.com"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        try
        {
            credentials = authenticationModule.authenticateJad(appUID2,appUID,securityAttributes.getAuthenticationAttributes());
            assertWithTrace(false);
        }
        catch (InstallerSecurityException e)
        {
            assertWithTrace(e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                            && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_AUTHORIZATION_ERR, null))
                            && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.DIFFERENT_SIGNERS, null)));
        }
        // update from untrusted to trusted succeeds for MIDP2 MIDlets
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
        credentials = authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, null);
        storage.removeAuthenticationStorageData(appUID2);
        permissionGranter.removeSecurityData(session,appUID2);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwA0wDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX0RldmljZV9NYW51ZmFjdHVyZXIwHhcNMDgwNjEyMDYxOTU3WhcNMTgwNjEwMDYxOTU3WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfRGV2aWNlX01hbnVmYWN0dXJlcjCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEApi3ipIPj8O33/rZre1euh7Itd9d6ZVf2mvQ/tLpHEsFJe5XLOsVViMvFskhhKUzmDsRbP4J03L6827/vEDIi/1i8kJGLaoFqJYaLGFWI2Zmxlj6aJV8cfZyOjmQPWJn1IDEe1ZAWbvMSp8xibWRsCLNEGKIkxQvAr/QDK/6iS+kCAwEAATANBgkqhkiG9w0BAQQFAAOBgQCDXt6klAs6yKvdTab3D2wkdoi3Lu4YFsMgLexJOu5HhAUJ/9VYO+Q2+mjd95MRsTa5rWQ2Jjvhn57Z3z/KBOwfHbymmNtMk6Gl14H0vQRoHa31jh3mTuAy5KotDVthaDp30aOizk473NU68wY1WdP4gFk5ZhrpNea9q3st13BxIQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "IcANmLKiOJQF8ABCNDj1PNNH/O8v9jfCVuiGBVm8enXDkM/gLwPjrC65sDKpOCHPqssUlHzjmVN5b9g8aRs4jxUOXNt2b732J7NSIPh97vw/WrP/KHdiooi/1KFUyklMyokK9ZrIv+GW1ttLCfKbuFupT9zmPAmWJQpnuD7J6sE="));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.microedition.io.Connector.http"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID2,appUID,securityAttributes.getAuthenticationAttributes());
        assertWithTrace(credentials != null && credentials.length == 1 && credentials[0].getProtectionDomainName().equals("Manufacturer"));
        // update from untrusted to untrusted succeeds
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
        credentials = authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, null);
        storage.removeAuthenticationStorageData(appUID2);
        permissionGranter.removeSecurityData(session,appUID2);
        PermissionResolver.testClearCache();
        appPackageEntry = new StorageEntry();
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageAttribute.ID,
                                         appUID2.getStringValue()));
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageNames.NAME,
                                         "MicroEdition-Profile"));
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageNames.VALUE,
                                         "MIDP-2.0"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID2,appUID,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID2, appUID);
        permissionGranter.grantJarPermissions( appUID2, null, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID2, null);
        // update from manufacturer to operator succeeds for legacy suites
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
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
        credentials = authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        storage.removeAuthenticationStorageData(appUID2);
        permissionGranter.removeSecurityData(session,appUID2);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICUDCCAbkCBEhQwIgwDQYJKoZIhvcNAQEEBQAwbzELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExGzAZBgNVBAMMEkpQX09wZXJhdG9yX0RvbWFpbjAeFw0wODA2MTIwNjIyMDBaFw0xODA2MTAwNjIyMDBaMG8xCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMRswGQYDVQQDDBJKUF9PcGVyYXRvcl9Eb21haW4wgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBAOrj7mg2cR2J0QKpdpOgEwSLoXnYed/fMS9JCYkcLuG0yfsZCYBMkfp+kiGhLi2RHrx2urKMBNDjlSbIlVxO8k/6wo48FSFiplRJoXRhi3hdGqA4SlommIRfPdpwInc6pC033BR9qEQZ6ZD/Mfxt9nIlwPT3jsjoh5s0QHUf+C3TAgMBAAEwDQYJKoZIhvcNAQEEBQADgYEAEHBGtEXorJ2meJm/lNwCw1NksPrFlCmlxnemBc5Whd+tv4e82A2ZoLQ0HiZVIhk6aV/2RyZkYRD/yhaJHmWadHXqxgScD9v0o1kVWzxTtUwE8YRaQw3X+FgYs3P+eiaI47JYswv+LnsCa+Z0IqbGHB8bdN6cspqiQxAZcc/ugRY="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "VkGjNLndcOGDcx6zIn4/cXBfqhaOGn9wTbXXG+DuBhF+KSydeeTU7tzYrIitifgmCQX9Ie6BqeHN7xQTSXkOBRv2GTmmujp1i45EmbptHuPBZ42bGKvK0blERGgQkJXpd0UGsty75pUW3uMZ16vgKYkS7I4a5R5+J9fQvTLt0ws="));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.microedition.io.Connector.http"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID2,appUID,securityAttributes.getAuthenticationAttributes());
        assertWithTrace(credentials != null && credentials.length == 1 && credentials[0].getProtectionDomainName().startsWith("Operator"));
        // update from identified third party to identified third party but with different organization for signing certificate succeeds for legacy MIDlets
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICyjCCAjOgAwIBAgIJAJ3QIjIE4oVvMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTExMDYxNVoXDTE1MDIwNDExMDYxNVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAM92j3+xb30PW1+/yHtSys24JPKJ0FxJp2K2vCtLnFh7HZnFQLnwNKKBZinU67qOTW4bdYdKxFt8FfYbX6Yx0Em9A8UYuyPGyzY8zmji5THyWu4bqjnAsNjwCwCAR+qgIp6bvQ9NYy+24GlLWnMagCwfIQXAhElQVzw35E/sIMTTVBMS1+syYAmgBn+wjR+OogYNPn8OGueIr+hwrMcCgWbcMPrByGBVJ8dU6/+zn4CXxg12O1u3RDoMmGjq+2iVUrfPks3ucUJ7A/9n+OwfpAwMMHCzpelvSSmrvM50GSiE/WnWYv2CUndjhOcShy9mu0wsz6I/On6tdUoXI9r05jUCAwEAATANBgkqhkiG9w0BAQUFAAOBgQAx+t+Z+tAqGVZGxyXJ956OqqHKkCMvCC79S/BYFPCayfIfEwB8BbrQlNpZrTdpuAnJ0kQAqI/FHJ/hfyzU9llLwMA1Jr2CjeY828ehR0nprQ/C3GJwDW2HWGKTmTarRFf1a1ehiP0fQN4Wvemv+2ntjBNMFmXV+3S4usooDcWMiQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "JZmGRu3+HfPrPfHV1aqbCC4LyTgWGNEhnZJDefnZPIzk8ijSSM/t9fPDZGvG2BILUtSuXl3ULAu+8xVQBEIAvQ0KH6nqauaNLx32j/3PQkYo1G2Pb3lDsyC9XHIsoGlsgAG0KO+j/wtjFgV/3MVyhH3KZBbaUdU0RjxsC6im5xal0r8mRKiuo3hPn4WzYxDy4TIgCcP9vcLyDOljGyWwkfbjgainAwwRG1AeItDjyRx0CAA4xHJlPcLMBXggJuhG+laMROE5qxOocsksauSLLvI9ucYgG9tIuSsr8JmbEXg462ynBUwetVzauaH4/w+przonPmGlo6Y3kUrvarGvDg=="));
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
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, null);
        appPackageEntry = new StorageEntry();
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageAttribute.ID,
                                         appUID.getStringValue()));
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageNames.NAME,
                                         "MIDlet-Certificate-1-1"));
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageNames.VALUE,
                                         "MIICyjCCAjOgAwIBAgIJAJ3QIjIE4oVvMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTExMDYxNVoXDTE1MDIwNDExMDYxNVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAM92j3+xb30PW1+/yHtSys24JPKJ0FxJp2K2vCtLnFh7HZnFQLnwNKKBZinU67qOTW4bdYdKxFt8FfYbX6Yx0Em9A8UYuyPGyzY8zmji5THyWu4bqjnAsNjwCwCAR+qgIp6bvQ9NYy+24GlLWnMagCwfIQXAhElQVzw35E/sIMTTVBMS1+syYAmgBn+wjR+OogYNPn8OGueIr+hwrMcCgWbcMPrByGBVJ8dU6/+zn4CXxg12O1u3RDoMmGjq+2iVUrfPks3ucUJ7A/9n+OwfpAwMMHCzpelvSSmrvM50GSiE/WnWYv2CUndjhOcShy9mu0wsz6I/On6tdUoXI9r05jUCAwEAATANBgkqhkiG9w0BAQUFAAOBgQAx+t+Z+tAqGVZGxyXJ956OqqHKkCMvCC79S/BYFPCayfIfEwB8BbrQlNpZrTdpuAnJ0kQAqI/FHJ/hfyzU9llLwMA1Jr2CjeY828ehR0nprQ/C3GJwDW2HWGKTmTarRFf1a1ehiP0fQN4Wvemv+2ntjBNMFmXV+3S4usooDcWMiQ=="));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
        storage.removeAuthenticationStorageData(appUID2);
        permissionGranter.removeSecurityData(session,appUID2);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIIC0DCCAjmgAwIBAgIJAI9pBHmi/8HTMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTExMTcxM1oXDTE1MDIwNDExMTcxM1owXTELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMSMwIQYDVQQKExpNeSBPdGhlciBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOxmRgiapQXW0j52cXwnGCMEk6Wsr/2iF6Ao0qlesxrFvPJB4eUgZ5/wNknwjyw0baTJgg/lYG4kT3wce7g8HU1CwnVxpmVNtQe0kuF8uP55LoqH24b4azy+WTnWXSfh0XGO2Z3yVWTmljzEvppLL95k7krqfn0UlwdWYCHZHVBSWILcRoIFu7uSaeJswhte4wnnhPh5+xEWnPZhCVbY6TI4kNhV7vje7Ugg6/LSSiHZ5NZYOhb/0M8noI5x5WWhMgJBw8UAGb0F1x9AqF4oZiTK9Qp01lDL37e4kV04TJg1QpvP5cMJNQWZWv9JW/OZbC+P9R6brIwX7lOXiPfZ8KUCAwEAATANBgkqhkiG9w0BAQUFAAOBgQA5pPk5+bkvQzldNfmedKLPdnInn4294t8QwAXfxvLI7+g7UqLO08c+iWI/IJdta2oE7tSUPqvVRuz37wnLCnWsOfmV3NliQsM7KcvrVXIN5onkyLUAQluake7HXq5Pu1U6y/3ggWxWalfnK1lKCKCfRGOt2qax13kWAKPSVJYHHg=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "PIROG3+RnzTAIliwyK2G52DY1QJZ1g//RNM/PvsHlT29UAAK00Nz52SAFHqPI8h1k5bgGHyxGYcrqMX2lxPuBX3NFCZTI95vE7Jdebwph2ktACx65LOS873KhsNgO6ObwUwru3u5VWkM+EAWvW3lj+SDZOEqGdzCziN3bwQhXnmbVEPmXcPxMvBsYXWxTGUeCnxItej9yYp+KA/fx5M4rvY6dm2aKLD1UoL15GEmeo5U8q2iZBo/N+NYM6CbP2oMDmOrxlo0b/Kf4C2sOYqhwE2O6pzodaquZLeuH3fzqfWoJKT8XTfidbhXv3nINJ0z3K/ARaY2o9tlqf3LcdKwgg=="));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.microedition.io.Connector.http"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID2,appUID,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID2, null, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.microedition.io.Connector.http"));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID2,appUID,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID2, appUID);
        permissionGranter.grantJarPermissions( appUID2, appUID, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID2, appUID);
        // update from identified third party to identified third party but with different organization for signing certificate fails for MIDP3 MIDlets
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICyjCCAjOgAwIBAgIJAJ3QIjIE4oVvMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTExMDYxNVoXDTE1MDIwNDExMDYxNVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAM92j3+xb30PW1+/yHtSys24JPKJ0FxJp2K2vCtLnFh7HZnFQLnwNKKBZinU67qOTW4bdYdKxFt8FfYbX6Yx0Em9A8UYuyPGyzY8zmji5THyWu4bqjnAsNjwCwCAR+qgIp6bvQ9NYy+24GlLWnMagCwfIQXAhElQVzw35E/sIMTTVBMS1+syYAmgBn+wjR+OogYNPn8OGueIr+hwrMcCgWbcMPrByGBVJ8dU6/+zn4CXxg12O1u3RDoMmGjq+2iVUrfPks3ucUJ7A/9n+OwfpAwMMHCzpelvSSmrvM50GSiE/WnWYv2CUndjhOcShy9mu0wsz6I/On6tdUoXI9r05jUCAwEAATANBgkqhkiG9w0BAQUFAAOBgQAx+t+Z+tAqGVZGxyXJ956OqqHKkCMvCC79S/BYFPCayfIfEwB8BbrQlNpZrTdpuAnJ0kQAqI/FHJ/hfyzU9llLwMA1Jr2CjeY828ehR0nprQ/C3GJwDW2HWGKTmTarRFf1a1ehiP0fQN4Wvemv+2ntjBNMFmXV+3S4usooDcWMiQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "JZmGRu3+HfPrPfHV1aqbCC4LyTgWGNEhnZJDefnZPIzk8ijSSM/t9fPDZGvG2BILUtSuXl3ULAu+8xVQBEIAvQ0KH6nqauaNLx32j/3PQkYo1G2Pb3lDsyC9XHIsoGlsgAG0KO+j/wtjFgV/3MVyhH3KZBbaUdU0RjxsC6im5xal0r8mRKiuo3hPn4WzYxDy4TIgCcP9vcLyDOljGyWwkfbjgainAwwRG1AeItDjyRx0CAA4xHJlPcLMBXggJuhG+laMROE5qxOocsksauSLLvI9ucYgG9tIuSsr8JmbEXg462ynBUwetVzauaH4/w+przonPmGlo6Y3kUrvarGvDg=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        appPackageEntry = new StorageEntry();
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageAttribute.ID,
                                         appUID.getStringValue()));
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageNames.NAME,
                                         "MIDlet-Certificate-1-1"));
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageNames.VALUE,
                                         "MIICyjCCAjOgAwIBAgIJAJ3QIjIE4oVvMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTExMDYxNVoXDTE1MDIwNDExMDYxNVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAM92j3+xb30PW1+/yHtSys24JPKJ0FxJp2K2vCtLnFh7HZnFQLnwNKKBZinU67qOTW4bdYdKxFt8FfYbX6Yx0Em9A8UYuyPGyzY8zmji5THyWu4bqjnAsNjwCwCAR+qgIp6bvQ9NYy+24GlLWnMagCwfIQXAhElQVzw35E/sIMTTVBMS1+syYAmgBn+wjR+OogYNPn8OGueIr+hwrMcCgWbcMPrByGBVJ8dU6/+zn4CXxg12O1u3RDoMmGjq+2iVUrfPks3ucUJ7A/9n+OwfpAwMMHCzpelvSSmrvM50GSiE/WnWYv2CUndjhOcShy9mu0wsz6I/On6tdUoXI9r05jUCAwEAATANBgkqhkiG9w0BAQUFAAOBgQAx+t+Z+tAqGVZGxyXJ956OqqHKkCMvCC79S/BYFPCayfIfEwB8BbrQlNpZrTdpuAnJ0kQAqI/FHJ/hfyzU9llLwMA1Jr2CjeY828ehR0nprQ/C3GJwDW2HWGKTmTarRFf1a1ehiP0fQN4Wvemv+2ntjBNMFmXV+3S4usooDcWMiQ=="));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
        storage.removeAuthenticationStorageData(appUID2);
        permissionGranter.removeSecurityData(session,appUID2);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIIC0DCCAjmgAwIBAgIJAI9pBHmi/8HTMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTExMTcxM1oXDTE1MDIwNDExMTcxM1owXTELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMSMwIQYDVQQKExpNeSBPdGhlciBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOxmRgiapQXW0j52cXwnGCMEk6Wsr/2iF6Ao0qlesxrFvPJB4eUgZ5/wNknwjyw0baTJgg/lYG4kT3wce7g8HU1CwnVxpmVNtQe0kuF8uP55LoqH24b4azy+WTnWXSfh0XGO2Z3yVWTmljzEvppLL95k7krqfn0UlwdWYCHZHVBSWILcRoIFu7uSaeJswhte4wnnhPh5+xEWnPZhCVbY6TI4kNhV7vje7Ugg6/LSSiHZ5NZYOhb/0M8noI5x5WWhMgJBw8UAGb0F1x9AqF4oZiTK9Qp01lDL37e4kV04TJg1QpvP5cMJNQWZWv9JW/OZbC+P9R6brIwX7lOXiPfZ8KUCAwEAATANBgkqhkiG9w0BAQUFAAOBgQA5pPk5+bkvQzldNfmedKLPdnInn4294t8QwAXfxvLI7+g7UqLO08c+iWI/IJdta2oE7tSUPqvVRuz37wnLCnWsOfmV3NliQsM7KcvrVXIN5onkyLUAQluake7HXq5Pu1U6y/3ggWxWalfnK1lKCKCfRGOt2qax13kWAKPSVJYHHg=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "PIROG3+RnzTAIliwyK2G52DY1QJZ1g//RNM/PvsHlT29UAAK00Nz52SAFHqPI8h1k5bgGHyxGYcrqMX2lxPuBX3NFCZTI95vE7Jdebwph2ktACx65LOS873KhsNgO6ObwUwru3u5VWkM+EAWvW3lj+SDZOEqGdzCziN3bwQhXnmbVEPmXcPxMvBsYXWxTGUeCnxItej9yYp+KA/fx5M4rvY6dm2aKLD1UoL15GEmeo5U8q2iZBo/N+NYM6CbP2oMDmOrxlo0b/Kf4C2sOYqhwE2O6pzodaquZLeuH3fzqfWoJKT8XTfidbhXv3nINJ0z3K/ARaY2o9tlqf3LcdKwgg=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        try
        {
            credentials = authenticationModule.authenticateJad(appUID2,appUID,securityAttributes.getAuthenticationAttributes());
            assertWithTrace(false);
        }
        catch (InstallerSecurityException e)
        {
            assertWithTrace(e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                            && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_AUTHORIZATION_ERR, null))
                            && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.DIFFERENT_SIGNERS, null)));
        }
        // update from identified third party to identified third party with same signer succeeds
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
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
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, null);
        appPackageEntry = new StorageEntry();
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
        storage.removeAuthenticationStorageData(appUID2);
        permissionGranter.removeSecurityData(session,appUID2);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwOgwDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX1RoaXJkX1BhcnR5X1RydXN0ZWQwHhcNMDgwNjEyMDYyMzM2WhcNMTgwNjEwMDYyMzM2WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfVGhpcmRfUGFydHlfVHJ1c3RlZDCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEArAesh03rswICRUjexWMoIH9ZZqUtlx+tfhZhuzuYhiL6cLdhxNlU8yuIdw4f17XP3yxVIB8rz1EEagCMd0Y5ZleNFZ992ReLcf9VH13RxaZ88PRxBLLROm1jcwTknYSS6AxGMup77DMwwFSN8XPnOHWt4aLGUrecMeUyI2c5UbsCAwEAATANBgkqhkiG9w0BAQQFAAOBgQAylJ+3aSq6a7AAL7UM8V8JhGZlLneErF7A/BJGDi7Vmr5gmlGyf6RCJJIg4qCp+J3Tv3eCRNNuphQaSRdFpaulwCmtfCzSZNtvZg0VhMOVsZqTwQgmP6NzKoIbqqzlgm5U+yUJw8FmX17vgdeSEdJ/oL/WT4AKrWFJYl8MdQ9ZnQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "F9/dB/WITCC1n886yppP9QTQBeQT6/8pV0UKwbTU9L8mP5qFuX2bjw7gPQZNxAC/4qNHYnmnMmwkfOPDjZzbOojdGJyEYXFyF0sBtLZKYYHylW8uXz4HDdtC1c2WHn8/Zl+a0Zoa8MjuxGRi5IVNY4XONlpPfzLxkOMVjna4KhA="));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.microedition.io.Connector.http"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID2,appUID,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID2, null, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.microedition.io.Connector.http"));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID2,appUID,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID2, appUID);
         permissionGranter.grantJarPermissions( appUID2, appUID, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID2, appUID);
        // update from identified third party to identified third party succeeds (V1 requests P1 (Net), P2 (Messaging), P3 (Messaging), V2 requests P2 (Messaging), P4 (UserData), P5 (Net)) -> check that P1 is removed, P2 is retained (with old settings), P3 is removed, P4 and P5 are added with brand new settings
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwOgwDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX1RoaXJkX1BhcnR5X1RydXN0ZWQwHhcNMDgwNjEyMDYyMzM2WhcNMTgwNjEwMDYyMzM2WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfVGhpcmRfUGFydHlfVHJ1c3RlZDCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEArAesh03rswICRUjexWMoIH9ZZqUtlx+tfhZhuzuYhiL6cLdhxNlU8yuIdw4f17XP3yxVIB8rz1EEagCMd0Y5ZleNFZ992ReLcf9VH13RxaZ88PRxBLLROm1jcwTknYSS6AxGMup77DMwwFSN8XPnOHWt4aLGUrecMeUyI2c5UbsCAwEAATANBgkqhkiG9w0BAQQFAAOBgQAylJ+3aSq6a7AAL7UM8V8JhGZlLneErF7A/BJGDi7Vmr5gmlGyf6RCJJIg4qCp+J3Tv3eCRNNuphQaSRdFpaulwCmtfCzSZNtvZg0VhMOVsZqTwQgmP6NzKoIbqqzlgm5U+yUJw8FmX17vgdeSEdJ/oL/WT4AKrWFJYl8MdQ9ZnQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "F9/dB/WITCC1n886yppP9QTQBeQT6/8pV0UKwbTU9L8mP5qFuX2bjw7gPQZNxAC/4qNHYnmnMmwkfOPDjZzbOojdGJyEYXFyF0sBtLZKYYHylW8uXz4HDdtC1c2WHn8/Zl+a0Zoa8MjuxGRi5IVNY4XONlpPfzLxkOMVjna4KhA="));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.wireless.messaging.sms.send,javax.wireless.messaging.mms.send"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.wireless.messaging.sms.send,javax.wireless.messaging.mms.send"));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, null);
        appPackageEntry = new StorageEntry();
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
        // change the settings of Messaging
        storage.writeUserSecuritySettings(appUID, "Messaging", UserSecuritySettings.BLANKET_INTERACTION_MODE);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwOgwDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX1RoaXJkX1BhcnR5X1RydXN0ZWQwHhcNMDgwNjEyMDYyMzM2WhcNMTgwNjEwMDYyMzM2WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfVGhpcmRfUGFydHlfVHJ1c3RlZDCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEArAesh03rswICRUjexWMoIH9ZZqUtlx+tfhZhuzuYhiL6cLdhxNlU8yuIdw4f17XP3yxVIB8rz1EEagCMd0Y5ZleNFZ992ReLcf9VH13RxaZ88PRxBLLROm1jcwTknYSS6AxGMup77DMwwFSN8XPnOHWt4aLGUrecMeUyI2c5UbsCAwEAATANBgkqhkiG9w0BAQQFAAOBgQAylJ+3aSq6a7AAL7UM8V8JhGZlLneErF7A/BJGDi7Vmr5gmlGyf6RCJJIg4qCp+J3Tv3eCRNNuphQaSRdFpaulwCmtfCzSZNtvZg0VhMOVsZqTwQgmP6NzKoIbqqzlgm5U+yUJw8FmX17vgdeSEdJ/oL/WT4AKrWFJYl8MdQ9ZnQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "F9/dB/WITCC1n886yppP9QTQBeQT6/8pV0UKwbTU9L8mP5qFuX2bjw7gPQZNxAC/4qNHYnmnMmwkfOPDjZzbOojdGJyEYXFyF0sBtLZKYYHylW8uXz4HDdtC1c2WHn8/Zl+a0Zoa8MjuxGRi5IVNY4XONlpPfzLxkOMVjna4KhA="));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.wireless.messaging.sms.send,javax.microedition.io.Connector.file.read,javax.microedition.io.Connector.http"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,appUID,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID, appUID, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.wireless.messaging.sms.send,javax.microedition.io.Connector.file.read,javax.microedition.io.Connector.http"));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID,appUID,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, appUID);
        permissionGranter.grantJarPermissions( appUID, appUID, securityAttributes.getPermissionAttributes(), credentials);
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, appUID);
        grantedPermissions = storage.readGrantedPermissions(appUID);
        MIDPPermission p1 = PermissionMappingTable.get("javax.wireless.messaging.sms.send");
        MIDPPermission p2 = PermissionMappingTable.get("javax.microedition.io.Connector.file.read");
        MIDPPermission p3 = PermissionMappingTable.get("javax.microedition.io.Connector.http");
        MIDPPermission p4 = PermissionMappingTable.get("javax.microedition.PropertyPermission");
        assertWithTrace(checkGrantedPermissions(grantedPermissions,
                                                new PolicyBasedPermissionImpl[]
                                                {
                                                    new PolicyBasedPermissionImpl(p1.getName(), p1.getTarget(), p1.getActionList(),
                                                                                  new UserSecuritySettingsImpl("Messaging",
                                                                                                               UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                                               new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.NO_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.SESSION_INTERACTION_MODE})),
                                                    new PolicyBasedPermissionImpl(p2.getName(), p2.getTarget(), p2.getActionList(),
                                                                                  new UserSecuritySettingsImpl("Read User Data Access",
                                                                                                               UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                                                               new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.NO_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.SESSION_INTERACTION_MODE})),
                                                    new PolicyBasedPermissionImpl(p3.getName(), p3.getTarget(), p3.getActionList(),
                                                                                  new UserSecuritySettingsImpl("Net Access",
                                                                                                               UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                                                               new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.NO_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.SESSION_INTERACTION_MODE})),
                                                    new PolicyBasedPermissionImpl("java.util.PropertyPermission", "microedition.*", "read", null),
                                                    new PolicyBasedPermissionImpl(p4.getName(), "mobinfo.publicinfo", "read", null),
                                                    new PolicyBasedPermissionImpl(p4.getName(), "mobinfo.cellid", "read",
                                                                                  new UserSecuritySettingsImpl("Location",
                                                                                                               UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                                                               new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.NO_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.SESSION_INTERACTION_MODE})),
                                                    new PolicyBasedPermissionImpl(p4.getName(), "mobinfo.countrycode", "read",
                                                                                  new UserSecuritySettingsImpl("Location",
                                                                                                               UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                                                               new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.NO_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.SESSION_INTERACTION_MODE})),
                                                    new PolicyBasedPermissionImpl(p4.getName(), "mobinfo.networkid", "read",
                                                                                  new UserSecuritySettingsImpl("Location",
                                                                                                               UserSecuritySettings.SESSION_INTERACTION_MODE,
                                                                                                               new int[]{UserSecuritySettings.BLANKET_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.NO_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.ONESHOT_INTERACTION_MODE,
                                                                                                                         UserSecuritySettings.SESSION_INTERACTION_MODE})),
                                                    new PolicyBasedPermissionImpl("com.nokia.ext2.internal.Ext2Perm", "ext2.target4", "ext2.action4", null),
                                                }));
        // getBlanketPermissions: while installing V1 the permissions are not put to blanket, while installing V2 there are some blanket permissions returned
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwOgwDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX1RoaXJkX1BhcnR5X1RydXN0ZWQwHhcNMDgwNjEyMDYyMzM2WhcNMTgwNjEwMDYyMzM2WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfVGhpcmRfUGFydHlfVHJ1c3RlZDCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEArAesh03rswICRUjexWMoIH9ZZqUtlx+tfhZhuzuYhiL6cLdhxNlU8yuIdw4f17XP3yxVIB8rz1EEagCMd0Y5ZleNFZ992ReLcf9VH13RxaZ88PRxBLLROm1jcwTknYSS6AxGMup77DMwwFSN8XPnOHWt4aLGUrecMeUyI2c5UbsCAwEAATANBgkqhkiG9w0BAQQFAAOBgQAylJ+3aSq6a7AAL7UM8V8JhGZlLneErF7A/BJGDi7Vmr5gmlGyf6RCJJIg4qCp+J3Tv3eCRNNuphQaSRdFpaulwCmtfCzSZNtvZg0VhMOVsZqTwQgmP6NzKoIbqqzlgm5U+yUJw8FmX17vgdeSEdJ/oL/WT4AKrWFJYl8MdQ9ZnQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "F9/dB/WITCC1n886yppP9QTQBeQT6/8pV0UKwbTU9L8mP5qFuX2bjw7gPQZNxAC/4qNHYnmnMmwkfOPDjZzbOojdGJyEYXFyF0sBtLZKYYHylW8uXz4HDdtC1c2WHn8/Zl+a0Zoa8MjuxGRi5IVNY4XONlpPfzLxkOMVjna4KhA="));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.wireless.messaging.sms.send,javax.wireless.messaging.mms.send"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.wireless.messaging.sms.send,javax.wireless.messaging.mms.send"));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        blanketPermissions = permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, null);
        assertWithTrace(checkStringArrays(blanketPermissions,
                                          new String[] {UserSecuritySettingsImpl.getLocalizedName(UserSecuritySettings.NET_ACCESS_SETTINGS),
                                                        UserSecuritySettingsImpl.getLocalizedName(UserSecuritySettings.MESSAGING_SETTINGS)
                                                       }));
        appPackageEntry = new StorageEntry();
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
        permissionGranter.cleanup();
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwOgwDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX1RoaXJkX1BhcnR5X1RydXN0ZWQwHhcNMDgwNjEyMDYyMzM2WhcNMTgwNjEwMDYyMzM2WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfVGhpcmRfUGFydHlfVHJ1c3RlZDCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEArAesh03rswICRUjexWMoIH9ZZqUtlx+tfhZhuzuYhiL6cLdhxNlU8yuIdw4f17XP3yxVIB8rz1EEagCMd0Y5ZleNFZ992ReLcf9VH13RxaZ88PRxBLLROm1jcwTknYSS6AxGMup77DMwwFSN8XPnOHWt4aLGUrecMeUyI2c5UbsCAwEAATANBgkqhkiG9w0BAQQFAAOBgQAylJ+3aSq6a7AAL7UM8V8JhGZlLneErF7A/BJGDi7Vmr5gmlGyf6RCJJIg4qCp+J3Tv3eCRNNuphQaSRdFpaulwCmtfCzSZNtvZg0VhMOVsZqTwQgmP6NzKoIbqqzlgm5U+yUJw8FmX17vgdeSEdJ/oL/WT4AKrWFJYl8MdQ9ZnQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "F9/dB/WITCC1n886yppP9QTQBeQT6/8pV0UKwbTU9L8mP5qFuX2bjw7gPQZNxAC/4qNHYnmnMmwkfOPDjZzbOojdGJyEYXFyF0sBtLZKYYHylW8uXz4HDdtC1c2WHn8/Zl+a0Zoa8MjuxGRi5IVNY4XONlpPfzLxkOMVjna4KhA="));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.wireless.messaging.sms.send,javax.microedition.io.Connector.file.read,javax.microedition.io.Connector.http"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,appUID,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID, appUID, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.wireless.messaging.sms.send,javax.microedition.io.Connector.file.read,javax.microedition.io.Connector.http"));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID,appUID,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, appUID);
        permissionGranter.grantJarPermissions( appUID, appUID, securityAttributes.getPermissionAttributes(), credentials);
        blanketPermissions = permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, appUID);
        assertWithTrace(checkStringArrays(blanketPermissions,
                                          new String[] {UserSecuritySettingsImpl.getLocalizedName(UserSecuritySettings.NET_ACCESS_SETTINGS),
                                                        UserSecuritySettingsImpl.getLocalizedName(UserSecuritySettings.MESSAGING_SETTINGS),
                                                        UserSecuritySettingsImpl.getLocalizedName(UserSecuritySettings.READ_USER_DATA_ACCESS_SETTINGS)
                                                       }));
        // getBlanketPermissions: while installing V1 the permissions are put to blanket, while installing V2  -> only the permissions which ould be put to Blanket are returned
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwOgwDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX1RoaXJkX1BhcnR5X1RydXN0ZWQwHhcNMDgwNjEyMDYyMzM2WhcNMTgwNjEwMDYyMzM2WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfVGhpcmRfUGFydHlfVHJ1c3RlZDCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEArAesh03rswICRUjexWMoIH9ZZqUtlx+tfhZhuzuYhiL6cLdhxNlU8yuIdw4f17XP3yxVIB8rz1EEagCMd0Y5ZleNFZ992ReLcf9VH13RxaZ88PRxBLLROm1jcwTknYSS6AxGMup77DMwwFSN8XPnOHWt4aLGUrecMeUyI2c5UbsCAwEAATANBgkqhkiG9w0BAQQFAAOBgQAylJ+3aSq6a7AAL7UM8V8JhGZlLneErF7A/BJGDi7Vmr5gmlGyf6RCJJIg4qCp+J3Tv3eCRNNuphQaSRdFpaulwCmtfCzSZNtvZg0VhMOVsZqTwQgmP6NzKoIbqqzlgm5U+yUJw8FmX17vgdeSEdJ/oL/WT4AKrWFJYl8MdQ9ZnQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "F9/dB/WITCC1n886yppP9QTQBeQT6/8pV0UKwbTU9L8mP5qFuX2bjw7gPQZNxAC/4qNHYnmnMmwkfOPDjZzbOojdGJyEYXFyF0sBtLZKYYHylW8uXz4HDdtC1c2WHn8/Zl+a0Zoa8MjuxGRi5IVNY4XONlpPfzLxkOMVjna4KhA="));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.wireless.messaging.sms.send,javax.wireless.messaging.mms.send"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.socket,javax.wireless.messaging.sms.send,javax.wireless.messaging.mms.send"));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), credentials);
        blanketPermissions = permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, null);
        assertWithTrace(checkStringArrays(blanketPermissions,
                                          new String[] {UserSecuritySettingsImpl.getLocalizedName(UserSecuritySettings.NET_ACCESS_SETTINGS),
                                                        UserSecuritySettingsImpl.getLocalizedName(UserSecuritySettings.MESSAGING_SETTINGS)
                                                       }));
        permissionGranter.setPermissionsToBlanket(session, appUID);
        appPackageEntry = new StorageEntry();
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
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwOgwDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX1RoaXJkX1BhcnR5X1RydXN0ZWQwHhcNMDgwNjEyMDYyMzM2WhcNMTgwNjEwMDYyMzM2WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfVGhpcmRfUGFydHlfVHJ1c3RlZDCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEArAesh03rswICRUjexWMoIH9ZZqUtlx+tfhZhuzuYhiL6cLdhxNlU8yuIdw4f17XP3yxVIB8rz1EEagCMd0Y5ZleNFZ992ReLcf9VH13RxaZ88PRxBLLROm1jcwTknYSS6AxGMup77DMwwFSN8XPnOHWt4aLGUrecMeUyI2c5UbsCAwEAATANBgkqhkiG9w0BAQQFAAOBgQAylJ+3aSq6a7AAL7UM8V8JhGZlLneErF7A/BJGDi7Vmr5gmlGyf6RCJJIg4qCp+J3Tv3eCRNNuphQaSRdFpaulwCmtfCzSZNtvZg0VhMOVsZqTwQgmP6NzKoIbqqzlgm5U+yUJw8FmX17vgdeSEdJ/oL/WT4AKrWFJYl8MdQ9ZnQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "F9/dB/WITCC1n886yppP9QTQBeQT6/8pV0UKwbTU9L8mP5qFuX2bjw7gPQZNxAC/4qNHYnmnMmwkfOPDjZzbOojdGJyEYXFyF0sBtLZKYYHylW8uXz4HDdtC1c2WHn8/Zl+a0Zoa8MjuxGRi5IVNY4XONlpPfzLxkOMVjna4KhA="));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.wireless.messaging.sms.send,javax.microedition.io.Connector.file.read,javax.microedition.io.Connector.http"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,appUID,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID, appUID, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.wireless.messaging.sms.send,javax.microedition.io.Connector.file.read,javax.microedition.io.Connector.http"));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID,appUID,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, appUID);
        permissionGranter.grantJarPermissions( appUID, appUID, securityAttributes.getPermissionAttributes(), credentials);
        blanketPermissions = permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, appUID);
        assertWithTrace(checkStringArrays(blanketPermissions,
                                          new String[] {UserSecuritySettingsImpl.getLocalizedName(UserSecuritySettings.NET_ACCESS_SETTINGS),
                                                        UserSecuritySettingsImpl.getLocalizedName(UserSecuritySettings.READ_USER_DATA_ACCESS_SETTINGS)
                                                       }));
        // put the permissions to blanket and re-install the same suite with same permissions -> check that there are no blanket permissions returned
        permissionGranter.setPermissionsToBlanket(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwOgwDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX1RoaXJkX1BhcnR5X1RydXN0ZWQwHhcNMDgwNjEyMDYyMzM2WhcNMTgwNjEwMDYyMzM2WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfVGhpcmRfUGFydHlfVHJ1c3RlZDCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEArAesh03rswICRUjexWMoIH9ZZqUtlx+tfhZhuzuYhiL6cLdhxNlU8yuIdw4f17XP3yxVIB8rz1EEagCMd0Y5ZleNFZ992ReLcf9VH13RxaZ88PRxBLLROm1jcwTknYSS6AxGMup77DMwwFSN8XPnOHWt4aLGUrecMeUyI2c5UbsCAwEAATANBgkqhkiG9w0BAQQFAAOBgQAylJ+3aSq6a7AAL7UM8V8JhGZlLneErF7A/BJGDi7Vmr5gmlGyf6RCJJIg4qCp+J3Tv3eCRNNuphQaSRdFpaulwCmtfCzSZNtvZg0VhMOVsZqTwQgmP6NzKoIbqqzlgm5U+yUJw8FmX17vgdeSEdJ/oL/WT4AKrWFJYl8MdQ9ZnQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "F9/dB/WITCC1n886yppP9QTQBeQT6/8pV0UKwbTU9L8mP5qFuX2bjw7gPQZNxAC/4qNHYnmnMmwkfOPDjZzbOojdGJyEYXFyF0sBtLZKYYHylW8uXz4HDdtC1c2WHn8/Zl+a0Zoa8MjuxGRi5IVNY4XONlpPfzLxkOMVjna4KhA="));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.wireless.messaging.sms.send,javax.microedition.io.Connector.file.read,javax.microedition.io.Connector.http"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,appUID,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID, appUID, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.wireless.messaging.sms.send,javax.microedition.io.Connector.file.read,javax.microedition.io.Connector.http"));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID,appUID,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, appUID);
        permissionGranter.grantJarPermissions( appUID, appUID, securityAttributes.getPermissionAttributes(), credentials);
        blanketPermissions = permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, appUID);
        assertWithTrace(blanketPermissions == null);
        // change some settings to e.g. Session and reinstall the same suite with same permissions -> check that only the settings which are not Blanket (due to being changed) are returned
        storage.writeUserSecuritySettings(appUID, "Messaging", UserSecuritySettings.SESSION_INTERACTION_MODE);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICWDCCAcECBEhQwOgwDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX1RoaXJkX1BhcnR5X1RydXN0ZWQwHhcNMDgwNjEyMDYyMzM2WhcNMTgwNjEwMDYyMzM2WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfVGhpcmRfUGFydHlfVHJ1c3RlZDCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEArAesh03rswICRUjexWMoIH9ZZqUtlx+tfhZhuzuYhiL6cLdhxNlU8yuIdw4f17XP3yxVIB8rz1EEagCMd0Y5ZleNFZ992ReLcf9VH13RxaZ88PRxBLLROm1jcwTknYSS6AxGMup77DMwwFSN8XPnOHWt4aLGUrecMeUyI2c5UbsCAwEAATANBgkqhkiG9w0BAQQFAAOBgQAylJ+3aSq6a7AAL7UM8V8JhGZlLneErF7A/BJGDi7Vmr5gmlGyf6RCJJIg4qCp+J3Tv3eCRNNuphQaSRdFpaulwCmtfCzSZNtvZg0VhMOVsZqTwQgmP6NzKoIbqqzlgm5U+yUJw8FmX17vgdeSEdJ/oL/WT4AKrWFJYl8MdQ9ZnQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME ,new Attribute("", "F9/dB/WITCC1n886yppP9QTQBeQT6/8pV0UKwbTU9L8mP5qFuX2bjw7gPQZNxAC/4qNHYnmnMmwkfOPDjZzbOojdGJyEYXFyF0sBtLZKYYHylW8uXz4HDdtC1c2WHn8/Zl+a0Zoa8MjuxGRi5IVNY4XONlpPfzLxkOMVjna4KhA="));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.wireless.messaging.sms.send,javax.microedition.io.Connector.file.read,javax.microedition.io.Connector.http"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,appUID,securityAttributes.getAuthenticationAttributes());
        permissionGranter.grantJadPermissions(appUID, appUID, securityAttributes.getPermissionAttributes(), credentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(com.nokia.mj.impl.security.midp.common.PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.wireless.messaging.sms.send,javax.microedition.io.Connector.file.read,javax.microedition.io.Connector.http"));
        securityAttributes.addManifestAttributes(allAttributes);
        credentials = authenticationModule.authenticateJar(appUID,appUID,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, appUID);
        permissionGranter.grantJarPermissions( appUID, appUID, securityAttributes.getPermissionAttributes(), credentials);
        blanketPermissions = permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, appUID);
        assertWithTrace(checkStringArrays(blanketPermissions,
                                          new String[] {UserSecuritySettingsImpl.getLocalizedName(UserSecuritySettings.MESSAGING_SETTINGS)}));
        // upgrade tests for legacy suites
        upgradeTests(true);
        // upgrade tests for MIDP3 suites
        upgradeTests(false);
        // getBlanketPermissionsDetails - null values
        assertTrue(permissionGranter.getBlanketPermissionsDetails(null) == null);
        // getBlanketPermissionsDetails - unknown uid
        permissionGranter.removeSecurityData(session, appUID);
        storage.removeAuthenticationStorageData(appUID);
        assertTrue(permissionGranter.getBlanketPermissionsDetails(appUID) == null);
        // getBlanketPermissionsDetails for Messaging, Net Access -> no details
        permissionGranter.removeSecurityData(session, appUID);
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("IdentifiedThirdParty", "ITDP", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.http,javax.wireless.messaging.sms.send"));
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("IdentifiedThirdParty", "ITDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.io.Connector.http,javax.wireless.messaging.sms.send"));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        permissionGranter.getBlanketPermissions(appUID);
        assertTrue(permissionGranter.getBlanketPermissionsDetails(appUID) == null);
        permissionGranter.addSecurityData(session, appUID, null);
        // getBlanketPermissionsDetails for Multimedia Read User Data -> no details
        permissionGranter.removeSecurityData(session, appUID);
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("IdentifiedThirdParty", "ITDP", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.media.control.RecordControl,javax.microedition.io.Connector.file.read"));
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("IdentifiedThirdParty", "ITDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.media.control.RecordControl,javax.microedition.io.Connector.file.read"));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, null);
        assertTrue(permissionGranter.getBlanketPermissionsDetails(appUID) == null);
        // getBlanketPermissionsDetails for Multimedia, Local Connectivity -> privacy details
        permissionGranter.removeSecurityData(session, appUID);
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("IdentifiedThirdParty", "ITDP", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.media.control.RecordControl,javax.microedition.io.Connector.bluetooth.client"));
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("IdentifiedThirdParty", "ITDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.media.control.RecordControl,javax.microedition.io.Connector.bluetooth.client"));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, null);
        String blanketDetails = permissionGranter.getBlanketPermissionsDetails(appUID);
        assertTrue("qtn_java_settings_inst_query_perm_sec".equals(blanketDetails)
            || "Allowing these permissions may result in compromised privacy".equals(
            blanketDetails));
        // getBlanketPermissionsDetails for Messaging, Multimedia -> net details
        permissionGranter.removeSecurityData(session, appUID);
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("IdentifiedThirdParty", "ITDP", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.media.control.RecordControl,javax.wireless.messaging.sms.send"));
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("IdentifiedThirdParty", "ITDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","javax.microedition.media.control.RecordControl,javax.wireless.messaging.sms.send"));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions( appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, null);
        blanketDetails = permissionGranter.getBlanketPermissionsDetails(appUID);
        assertTrue("qtn_java_settings_inst_query_perm_net".equals(blanketDetails)
            || "Allowing these permissions may result in compromised privacy or increased network usage costs".equals(
            blanketDetails));
    }

    private void upgradeTests(boolean legacySuites)
    {
        boolean expectToFail = !legacySuites;
        // upgrade from untrusted to all the other domains
        upgradeDomain(ProtectionDomain.getUnidentifiedThirdPartyDomain(), ProtectionDomain.getUnidentifiedThirdPartyDomain(), false, legacySuites);
        upgradeDomain(ProtectionDomain.getUnidentifiedThirdPartyDomain(), ProtectionDomain.getOperatorDomain(), expectToFail, legacySuites);
        upgradeDomain(ProtectionDomain.getUnidentifiedThirdPartyDomain(), ProtectionDomain.getIdentifiedThirdPartyDomain(), expectToFail, legacySuites);
        upgradeDomain(ProtectionDomain.getUnidentifiedThirdPartyDomain(), ProtectionDomain.getManufacturerDomain(), expectToFail, legacySuites);

        // upgrade from operator to all the other domains
        upgradeDomain(ProtectionDomain.getOperatorDomain(), ProtectionDomain.getUnidentifiedThirdPartyDomain(), true, legacySuites);
        upgradeDomain(ProtectionDomain.getOperatorDomain(), ProtectionDomain.getOperatorDomain(), false, legacySuites);
        upgradeDomain(ProtectionDomain.getOperatorDomain(), ProtectionDomain.getIdentifiedThirdPartyDomain(), expectToFail, legacySuites);
        upgradeDomain(ProtectionDomain.getOperatorDomain(), ProtectionDomain.getManufacturerDomain(), expectToFail, legacySuites);

        // upgrade from identified third party to all the other domains
        upgradeDomain(ProtectionDomain.getIdentifiedThirdPartyDomain(), ProtectionDomain.getUnidentifiedThirdPartyDomain(), true, legacySuites);
        upgradeDomain(ProtectionDomain.getIdentifiedThirdPartyDomain(), ProtectionDomain.getOperatorDomain(), expectToFail, legacySuites);
        upgradeDomain(ProtectionDomain.getIdentifiedThirdPartyDomain(), ProtectionDomain.getIdentifiedThirdPartyDomain(), false, legacySuites);
        upgradeDomain(ProtectionDomain.getIdentifiedThirdPartyDomain(), ProtectionDomain.getManufacturerDomain(), expectToFail, legacySuites);

        // upgrade from manufacturer to all the other domains
        upgradeDomain(ProtectionDomain.getManufacturerDomain(), ProtectionDomain.getUnidentifiedThirdPartyDomain(), true, legacySuites);
        upgradeDomain(ProtectionDomain.getManufacturerDomain(), ProtectionDomain.getOperatorDomain(), expectToFail, legacySuites);
        upgradeDomain(ProtectionDomain.getManufacturerDomain(), ProtectionDomain.getIdentifiedThirdPartyDomain(), expectToFail, legacySuites);
        upgradeDomain(ProtectionDomain.getManufacturerDomain(), ProtectionDomain.getManufacturerDomain(), false, legacySuites);
    }

    private void upgradeDomain(ProtectionDomain domain1, ProtectionDomain domain2, boolean expectToFail, boolean legacySuite)
    {
        AuthenticationCredentials[] credentials;
        boolean success1 = false;
        boolean success2 = false;
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
        authenticationModule.cleanup();
        StorageEntry appPackageEntry = new StorageEntry();
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageAttribute.ID,
                                         appUID.getStringValue()));
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
        appPackageEntry = new StorageEntry();
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageAttribute.ID,
                                         appUID.getStringValue()));
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageNames.NAME,
                                         "MicroEdition-Profile"));
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageNames.VALUE,
                                         (legacySuite ? "MIDP-2.0" : "MIDP-3.0")));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
        credentials = authenticationModule.authenticateJar(appUID, null, domain1, TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar");
        authenticationModule.addSecurityData(session, appUID, null);
        permissionGranter.grantJarPermissions(appUID, null, domain1);
        blanketPermissions = permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, null);
        signingCerts = authenticationModule.getCertificatesDetails(session, appUID);
        success1 = (blanketPermissions == null && signingCerts == null);
        ac.checkPermission(new java.util.PropertyPermission("microedition.version", "read"));
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertWithTrace(PermissionGranterTests.checkGrantedPermissions(grantedPermissions, PermissionGranterTests.getPolicyPermissions(domain1.getName()), getExtPermissions(domain1.getCategory())));
        String domainCategory = authenticationModule.getProtectionDomainCategory(session, appUID);
        assertWithTrace(domainCategory.equals(domain1.getCategory()));
        try
        {
            credentials = authenticationModule.authenticateJar(appUID, appUID, domain2, TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar");
            authenticationModule.addSecurityData(session, appUID, appUID);
            assertWithTrace(!expectToFail);
        }
        catch (InstallerSecurityException e)
        {
            assertWithTrace(expectToFail);
            assertWithTrace(e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                            && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_AUTHORIZATION_ERR, null))
                            && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.DIFFERENT_SIGNERS, null)));
            return;
        }
        permissionGranter.grantJarPermissions(appUID,appUID, domain2);
        blanketPermissions = permissionGranter.getBlanketPermissions(appUID);
        permissionGranter.addSecurityData(session, appUID, appUID);
        signingCerts = authenticationModule.getCertificatesDetails(session, appUID);
        success2 = (blanketPermissions == null && signingCerts == null);
        assertWithTrace(success1 == true && success2 == true);
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertWithTrace(PermissionGranterTests.checkGrantedPermissions(grantedPermissions, PermissionGranterTests.getPolicyPermissions(domain2.getName()), getExtPermissions(domain2.getCategory())));
        domainCategory = authenticationModule.getProtectionDomainCategory(session, appUID);
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session,appUID);
        PermissionResolver.testClearCache();
        assertWithTrace(domainCategory.equals(domain2.getCategory()));
    }

    private boolean checkGrantedPermissions(Vector grantedPermissions, PolicyBasedPermissionImpl[] expectedPermissions)
    {
        if (grantedPermissions.size() != expectedPermissions.length)
        {
            return false;
        }
        boolean[] grantedPermissionsChecked =
            new boolean[grantedPermissions.size()];
        initBoolArray(grantedPermissionsChecked);
        for (int i=0; i<grantedPermissions.size(); i++)
        {
            PolicyBasedPermissionImpl grantedPermission = (PolicyBasedPermissionImpl)grantedPermissions.elementAt(i);
            int index = findPermission(grantedPermission,
                                       expectedPermissions);
            if (index == -1)
            {
                return false;
            }
            if (!grantedPermissionsChecked[i])
            {
                grantedPermissionsChecked[i] = true;
            }
            else
            {
                return false;
            }
        }
        return checkBoolArray(grantedPermissionsChecked);
    }
    public static MIDPPermission[] getExtPermissions(String domain)
    {
        if (domain.equalsIgnoreCase(ApplicationInfo.IDENTIFIED_THIRD_PARTY_DOMAIN))
        {
            return new MIDPPermission[]
                   {
                       new MIDPPermission("com.nokia.ext1.public.Ext1Perm", "ext1.target1", "ext1.action1"),
                       new MIDPPermission("com.nokia.ext2.internal.Ext2Perm", "ext2.target1", "ext2.action1"),
                       new MIDPPermission("com.nokia.ext2.internal.Ext2Perm", "ext2.target2", "ext2.action2"),
                       new MIDPPermission("com.nokia.ext2.internal.Ext2Perm", "ext2.target3", "ext2.action3"),
                       new MIDPPermission("com.nokia.ext2.internal.Ext2Perm", "ext2.target4", "ext2.action4"),
                       new MIDPPermission("com.nokia.ext2.internal.Ext2Perm", "ext2.target5", "ext2.action5"),
                   };
        }
        else if (domain.equalsIgnoreCase(ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN))
        {
            return new MIDPPermission[]
                   {
                       new MIDPPermission("com.nokia.ext1.internal.Ext1Perm", "ext1.target1", "ext1.action1"),
                       new MIDPPermission("com.nokia.mj.impl.security.midp.authorization.HttpProtocolPermissionImpl", "http://", null),
                       new MIDPPermission("com.nokia.mj.impl.security.midp.authorization.FileProtocolPermissionImpl", "PUBLIC_DIRS", "read"),
                       new MIDPPermission("com.nokia.ext1.internal.Ext1Perm", "ext1.target2", "ext1.action2"),
                       new MIDPPermission("com.nokia.ext1.internal.Ext1Perm", "ext1.target3", "ext1.action3"),
                   };
        }
        return new MIDPPermission[0];
    }

    private void initBoolArray(boolean[] boolArray)
    {
        for (int i=0; i<boolArray.length; i++)
        {
            boolArray[i] = false;
        }
    }

    private boolean checkBoolArray(boolean[] boolArray)
    {
        for (int i=0; i<boolArray.length; i++)
        {
            if (boolArray[i] == false)
            {
                return false;
            }
        }
        return true;
    }

    private int findPermission(PolicyBasedPermissionImpl permission, PolicyBasedPermissionImpl[] permissions)
    {
        for (int i=0; i<permissions.length; i++)
        {
            if (permissions[i].equals(permission))
            {
                return i;
            }
        }
        return -1;
    }

    private boolean checkStringArrays(String[] arr1, String[] arr2)
    {
        if (arr1 != null && arr2 != null && arr1.length == arr2.length)
        {
            boolean[] stringsChecked =
                new boolean[arr1.length];
            initBoolArray(stringsChecked);
            for (int i=0; i<arr1.length; i++)
            {
                for (int j=0; i<arr2.length; j++)
                {
                    if (arr1[i].equals(arr2[j]))
                    {
                        stringsChecked[i] = true;
                        break;
                    }
                }
            }
            return checkBoolArray(stringsChecked);
        }
        return false;
    }
}
