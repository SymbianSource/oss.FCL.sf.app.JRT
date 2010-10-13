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
import com.nokia.mj.impl.storage.StorageSession;
import com.nokia.mj.impl.storage.StorageFactory;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.security.midp.authorization.PermissionGranter;
import com.nokia.mj.impl.security.midp.common.MIDPPermission;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettings;
import com.nokia.mj.impl.security.midp.common.UserSecuritySettingsImpl;
import com.nokia.mj.impl.security.midp.common.PermissionAttribute;
import com.nokia.mj.impl.security.midp.common.PolicyBasedPermission;
import com.nokia.mj.impl.security.midp.common.SecurityAttributes;
import com.nokia.mj.impl.security.midp.common.AuthenticationAttribute;
import com.nokia.mj.impl.security.midp.common.AuthenticationCredentials;
import com.nokia.mj.impl.security.midp.common.ProtectionDomain;
import com.nokia.mj.impl.security.midp.storage.AuthenticationStorageData;
import com.nokia.mj.impl.security.midp.storage.SecurityStorage;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.ErrorMessageBase;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.security.midp.common.PermissionMappingTable;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import java.util.Vector;

/**
 * PermissionGranter unit tests.
 */
public class PermissionGranterTests extends TestCase implements InstallerMain
{

    private static String TEST_DATA_DIR;

    static
    {
        String platform = System.getProperty("os.name");
        if (platform != null && platform.equalsIgnoreCase("linux"))
        {
            TEST_DATA_DIR = System.getProperty("JAVA_BIN_ROOT");
            if (TEST_DATA_DIR != null
                    && !TEST_DATA_DIR.endsWith("/")
                    && !TEST_DATA_DIR.endsWith("\\"))
            {
                TEST_DATA_DIR = TEST_DATA_DIR + "/";
            }
            TEST_DATA_DIR = TEST_DATA_DIR + "securitytestdata/";
        }
        else
        {
            TEST_DATA_DIR = "C:\\java\\securitytestdata\\";
        }
    }

    // general-purpose constants
    private static final String MIDP_PROFILE_ATTRIBUTE_NAME = "MicroEdition-Profile";
    private static final String MIDP3 = "MIDP-3.0";
    private static final String MIDP2 = "MIDP-2.0";
    private static final String COMMA = ",";

    // constants for the error messages to be displayed in case of failures
    protected String UNKNOWN_PERMISSION_MSG = "Requesting for an unknown permission threw InvalidAttributeException with following status code and message: ";
    protected String INVALID_PERMISSION_VALUE_MSG = "Placing a permission attribute with invalid value threw InvalidAttributeException with following status code and message: ";

    // constants for permissions
    private static final String MIDP2_SOCKET_PERMISSION = "javax.microedition.io.Connector.socket";
    private static final String MIDP2_SERVER_SOCKET_PERMISSION = "javax.microedition.io.Connector.serversocket";
    private static final String MIDP3_SOCKET_PERMISSION = "javax.microedition.io.SocketProtocolPermission";
    private static final String MIDP2_HTTP_PERMISSION = "javax.microedition.io.Connector.http";
    private static final String MIDP2_HTTPS_PERMISSION = "javax.microedition.io.Connector.https";
    private static final String MIDP3_HTTP_PERMISSION = "javax.microedition.io.HttpProtocolPermission";
    private static final String MIDP3_HTTPS_PERMISSION = "javax.microedition.io.HttpsProtocolPermission";
    private static final String MIDP2_SSL_PERMISSION = "javax.microedition.io.Connector.ssl";
    private static final String MIDP3_SSL_PERMISSION = "javax.microedition.io.SSLProtocolPermission";
    private static final String MIDP2_DATAGRAM_PERMISSION = "javax.microedition.io.Connector.datagram";
    private static final String MIDP2_DATAGRAM_RECEIVER_PERMISSION = "javax.microedition.io.Connector.datagramreceiver";
    private static final String MIDP3_DATAGRAM_PERMISSION = "javax.microedition.io.DatagramProtocolPermission";
    private static final String MIDP2_COMM_PERMISSION = "javax.microedition.io.Connector.comm";
    private static final String MIDP3_COMM_PERMISSION = "javax.microedition.io.CommProtocolPermission";
    private static final String MIDP2_PUSH_REGISTRY_PERMISSION = "javax.microedition.io.PushRegistry";
    private static final String MIDP3_PUSH_REGISTRY_PERMISSION = "javax.microedition.io.PushRegistryPermission";
    private static final String MIDP3_RUNTIME_PERMISSION = "java.lang.RuntimePermission";
    private static final String MIDP3_PROPERTY_PERMISSION = "java.util.PropertyPermission";
    private static final String MIDP3_EVENT_PERMISSION = "javax.microedition.event.EventPermission";
    private static final String MIDP3_PLAYER_PERMISSION = "javax.microedition.media.PlayerPermission";
    private static final String MIDP3_ACTIONS_DENIED_PERMISSION = "javax.microedition.midlet.ActionsDeniedPermission";
    private static final String MIDP3_AUTOSTART_PERMISSION = "javax.microedition.midlet.AutoStartPermission";
    private static final String MIDP3_FILE_PERMISSION = "javax.microedition.io.FileProtocolPermission";
    private static final String MIDP2_SMS_SEND_PERMISSION = "javax.wireless.messaging.sms.send";
    private static final String MIDP2_MMS_OPEN_PERMISSION = "javax.microedition.io.Connector.mms";
    // internal permissions
    // Change the names of the real internal permissions once they are available
    private static final String INTERNAL_SOCKET_PERMISSION = "com.nokia.mj.impl.security.midp.authorization.SocketProtocolPermissionImpl";
    private static final String INTERNAL_HTTP_PERMISSION = "com.nokia.mj.impl.security.midp.authorization.HttpProtocolPermissionImpl";
    private static final String INTERNAL_HTTPS_PERMISSION = "com.nokia.mj.impl.security.midp.authorization.HttpsProtocolPermissionImpl";
    private static final String INTERNAL_SSL_PERMISSION = "com.nokia.mj.impl.security.midp.authorization.SSLProtocolPermissionImpl";
    private static final String INTERNAL_DATAGRAM_PERMISSION = "com.nokia.mj.impl.security.midp.authorization.DatagramProtocolPermissionImpl";
    private static final String INTERNAL_COMM_PERMISSION = "com.nokia.mj.impl.security.midp.authorization.CommProtocolPermissionImpl";
    private static final String INTERNAL_PUSH_REGISTRY_PERMISSION = "javax.microedition.io.PushRegistryPermission";
    private static final String INTERNAL_RUNTIME_PERMISSION = "java.lang.RuntimePermission";
    private static final String INTERNAL_PROPERTY_PERMISSION = "java.util.PropertyPermission";
    private static final String INTERNAL_EVENT_PERMISSION = "javax.microedition.event.EventPermission";
    private static final String INTERNAL_PLAYER_PERMISSION = "javax.microedition.media.PlayerPermission";
    private static final String INTERNAL_ACTIONS_DENIED_PERMISSION = "javax.microedition.midlet.ActionsDeniedPermission";
    private static final String INTERNAL_AUTOSTART_PERMISSION = "javax.microedition.midlet.AutoStartPermission";
    private static final String INTERNAL_FILE_PERMISSION = "com.nokia.mj.impl.security.midp.authorization.FileProtocolPermissionImpl";

    /**
     * Hashtable containing the mapping between MIDP3 permissions and their
     * default targets
     */
    private static Hashtable midp2PermissionTargets = new Hashtable();
    static
    {
        midp2PermissionTargets = new Hashtable();
        midp2PermissionTargets.put(MIDP3_SOCKET_PERMISSION, "socket://*");
        midp2PermissionTargets.put(MIDP3_HTTP_PERMISSION, "http://*");
        midp2PermissionTargets.put(MIDP3_HTTPS_PERMISSION, "https://*");
        midp2PermissionTargets.put(MIDP3_SSL_PERMISSION, "ssl://*");
        midp2PermissionTargets.put(MIDP3_DATAGRAM_PERMISSION,"datagram://*");
        midp2PermissionTargets.put(MIDP3_COMM_PERMISSION, "comm://*");
        midp2PermissionTargets.put(MIDP3_PUSH_REGISTRY_PERMISSION, "*");
    }

    /**
     * The implementation's mapping between (MIDP2 and MIDP3 permissions) and the
     * internal class permissions
     */
    private Hashtable IMPL_PERMISSION_MAPPING_TABLE;

    // references to error messages
    protected InstallerErrorMessage errorMessage = new InstallerErrorMessage();
    protected InstallerDetailedErrorMessage detailedErrorMessage = new InstallerDetailedErrorMessage();
    private SecurityAttributes securityAttributes;
    private StorageSession session;
    private SecurityStorage storage;
    private Uid appUID;
    private PermissionGranter permissionGranter;
    private Vector grantedPermissions;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new PermissionGranterTests("testGrantPermissions", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((PermissionGranterTests)tc).testGrantPermissions();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public PermissionGranterTests()
    {
    }

    public PermissionGranterTests(String aTestName, TestMethod aTestMethod)
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
        securityAttributes = new SecurityAttributes();
        storage = new SecurityStorage();
        session = StorageFactory.createSession();
        session.open();
        appUID = Uid.createUid("myUID");
        permissionGranter = PermissionGranter.getInstance();
    }

    protected void tearDown()
    {
        permissionGranter.removeSecurityData(session, appUID);
        storage.removeAuthenticationStorageData(appUID);
    }

    protected void testGrantPermissions()
    {
        // Remove following two instr once the real internal permissions are in place
        IMPL_PERMISSION_MAPPING_TABLE = PermissionMappingTable.setMappingTable(TestPermissionMappingTable.getMappingTable());
        SecurityPolicyModule.policiesDir = TEST_DATA_DIR;
        SecurityPolicyModule.policiesFileNamePrefix = "test_";
        String[] blanketPermissions = null;
        // data structures used in tests
        AuthenticationStorageData authData = null;
        AuthenticationCredentials[] authCredentials = null;
        Hashtable allAttributes = new Hashtable();
        MIDPPermission[] jadPermissions;
        MIDPPermission[] jarPermissions;
        MIDPPermission[] allPermissions;
        assertTrue(permissionGranter != null);
        // test the null values
        permissionGranter.removeSecurityData(session, appUID);
        permissionGranter.grantJarPermissions(null, null, null, (PermissionAttribute[])null);
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(grantedPermissions == null);
        // populate the storage
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        AuthenticationStorageData authStorageData = new AuthenticationStorageData("protDomain", "category", "jarHash", "certHash", null, null);
        storage.writeAuthenticationStorageData(appUID, authStorageData);
        // 1. Legacy, signed : permissions requested in JAD & permissions requested in JAR -> grant the requested permissions filled in with right
        //    targets/actions from the policy plus the assigned permissions from the policy
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("Manufacturer", "MFD", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("", MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_DATAGRAM_PERMISSION + COMMA + "                  " + MIDP2_COMM_PERMISSION));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_SOCKET_PERMISSION + COMMA + MIDP2_HTTP_PERMISSION));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("Manufacturer", "MFD");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_DATAGRAM_PERMISSION + COMMA + "                  " + MIDP2_COMM_PERMISSION));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_SOCKET_PERMISSION + COMMA + MIDP2_HTTP_PERMISSION));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(checkGrantedPermissions(grantedPermissions, getPolicyPermissions("Manufacturer", new String[]
                                           {
                                               INTERNAL_DATAGRAM_PERMISSION, INTERNAL_COMM_PERMISSION, INTERNAL_SOCKET_PERMISSION,INTERNAL_HTTP_PERMISSION,"javax.microedition.PropertyPermission","javax.microedition.midlet.AutoStartPermission"
                                           }),
                                           getDefaultPermissions()));
        // 1.1 Legacy, signed: P1, P2 requested in the JAD, P3, P4 requested in the JAR -> grant the requested permissions filled in with right targets/actions from the policy
        permissionGranter.removeSecurityData(session, appUID);
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("IdentifiedThirdParty", "ITPD", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_DATAGRAM_PERMISSION + COMMA + "                  " + MIDP2_COMM_PERMISSION));
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("IdentifiedThirdParty", "ITDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_DATAGRAM_PERMISSION + COMMA + "                  " + MIDP2_COMM_PERMISSION));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_SOCKET_PERMISSION + COMMA + MIDP2_HTTP_PERMISSION));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(checkGrantedPermissions(grantedPermissions, getPolicyPermissions("IdentifiedThirdParty",
                                           new String[] {INTERNAL_DATAGRAM_PERMISSION, INTERNAL_COMM_PERMISSION, INTERNAL_SOCKET_PERMISSION,INTERNAL_HTTP_PERMISSION}),
                                           getAssignedPermissions("IdentifiedThirdParty")));
        // 1.2 One of the requested mandatory permission is not available in the domain -> fail
        try
        {
            storage.removeAuthenticationStorageData(appUID);
            permissionGranter.removeSecurityData(session, appUID);
            authData = new AuthenticationStorageData("IdentifiedThirdParty", "ITPD", null, null, null, null);
            storage.writeAuthenticationStorageData(appUID, authData);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","signature"));
            allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1", new Attribute("",MIDP3_DATAGRAM_PERMISSION));
            securityAttributes.addDescriptorAttributes(allAttributes);
            authCredentials = new AuthenticationCredentials[1];
            authCredentials[0] = new AuthenticationCredentials("IdentifiedThirdParty", "ITDP");
            permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1", new Attribute("",MIDP3_DATAGRAM_PERMISSION));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1", new Attribute("",MIDP3_RUNTIME_PERMISSION + " name"));
            securityAttributes.addManifestAttributes(allAttributes);
            permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
            assertTrue(INVALID_PERMISSION_VALUE_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_PERMISSION_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                                                        new String[] {PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1"})));
        }
        // 1.3 One of the requested optional permission is not available in the domain -> succeed, only the assigned permissions are granted
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("IdentifiedThirdParty", "ITPD", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","signature"));
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("IdentifiedThirdParty", "ITDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1", new Attribute("",MIDP3_RUNTIME_PERMISSION + " name"));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(checkGrantedPermissions(grantedPermissions, getAssignedPermissions("IdentifiedThirdParty"), true));
        // 2. Legacy, signed : permissions NOT requested in JAD & permissions requested in JAR -> grant the requested permissions filled in with right
        // targets/actions from the policy plus the assigned ones
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("Operator", "OPD", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("Operator", "OPD");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_SOCKET_PERMISSION + COMMA + MIDP2_HTTP_PERMISSION));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(checkGrantedPermissions(grantedPermissions, getPolicyPermissions("Operator", new String[]
                                           {
                                               INTERNAL_SOCKET_PERMISSION, INTERNAL_HTTP_PERMISSION,"javax.microedition.PropertyPermission","javax.microedition.midlet.AutoStartPermission"
                                           }),
                                           getDefaultPermissions()));
        // 3. Legacy, signed : permissions NOT requested in JAD & permissions NOT requested in JAR -> grant only the "assigned" permissions
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("IdentifiedThirdParty", "ITPD", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","signature"));
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("IdentifiedThirdParty", "ITDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(grantedPermissions != null && checkGrantedPermissions(grantedPermissions, getAssignedPermissions("IdentifiedThirdParty"), true));
        // same for a domain which has one assigned permissions -> only the assigned + default permissions are granted
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("Operator", "OPD", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","signature"));
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("Operator", "OPD");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(checkGrantedPermissions(grantedPermissions, getPolicyPermissions("Operator", new String[] {"javax.microedition.PropertyPermission","javax.microedition.midlet.AutoStartPermission"}),
                                           getDefaultPermissions()));
        // 4. Legacy, unsigned : permissions requested in JAD & permissions requested in JAR -> ignore requested permissions and
        //    grant all the permissions available in the policy for untrusted MIDlets
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("UnidentifiedThirdParty", "UTPD", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME,
                          new Attribute("","MyOptionalPermission" + COMMA + MIDP2_DATAGRAM_PERMISSION + COMMA + "                  " + MIDP2_COMM_PERMISSION));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","MyMandatoryPermission" + COMMA + MIDP2_SOCKET_PERMISSION + COMMA + MIDP2_HTTP_PERMISSION));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("UnidentifiedThirdParty", "ITDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME, new Attribute("","MyOwnOptionalPermission"));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","MyOwnMandatoryPermission"));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(checkGrantedPermissions(grantedPermissions, getPolicyPermissions("UnidentifiedThirdParty")));
        // 5. Legacy, unsigned : permissions requested in JAD & permissions NOT requested in JAR -> ignore requested permissions and,
        //    grant all the permissions available in the policy for untrusted MIDlets
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("UnidentifiedThirdParty", "UTPD", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME, new Attribute("","MyOptionalPermission" + COMMA + MIDP2_DATAGRAM_PERMISSION + COMMA + "                  " + MIDP2_COMM_PERMISSION));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","MyMandatoryPermission" + COMMA + MIDP2_SOCKET_PERMISSION + COMMA + MIDP2_HTTP_PERMISSION));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("UnidentifiedThirdParty", "ITDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(checkGrantedPermissions(grantedPermissions, getPolicyPermissions("UnidentifiedThirdParty")));
        // 6. Legacy, unsigned : permissions NOT requested in JAD & permissions requested in JAR -> ignore requested permissions and
        //    grant all the permissions available in the policy for untrusted MIDlets
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("UnidentifiedThirdParty", "UTDP", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("UnidentifiedThirdParty", "UITP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME, new Attribute("","MyOwnOptionalPermission"));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","MyOwnMandatoryPermission"));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(checkGrantedPermissions(grantedPermissions, getPolicyPermissions("UnidentifiedThirdParty")));
        // 7. Legacy, unsigned : permissions NOT requested in JAD & permissions NOT requested in JAR -> grant all the permissions available in the policy for untrusted MIDlets
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("UnidentifiedThirdParty", "UTDP", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("UnidentifiedThirdParty", "UIDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(checkGrantedPermissions(grantedPermissions, getPolicyPermissions("UnidentifiedThirdParty")));
        // 8. MIDP3 unsigned, unknown mandatory permission in JAD file -> failure
        permissionGranter.removeSecurityData(session, appUID);
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("","MyMandatoryClass MyMandatoryTarget MyMandatoryAction1,MyMandatoryAction2,MyMandatoryAction3"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            authCredentials = new AuthenticationCredentials[1];
            authCredentials[0] = new AuthenticationCredentials("UnidentifiedThirdParty", "UIDP");
            permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
            assertTrue(UNKNOWN_PERMISSION_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(UNKNOWN_PERMISSION_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                                                        new String[] {PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1"})));
        }
        // 8.1. MIDP3 unsigned, unknown mandatory permission in JAR file -> failure
        permissionGranter.removeSecurityData(session, appUID);
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("","MyMandatoryClass MyMandatoryTarget MyMandatoryAction1,MyMandatoryAction2,MyMandatoryAction3"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addManifestAttributes(allAttributes);
            permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
            assertTrue(UNKNOWN_PERMISSION_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(UNKNOWN_PERMISSION_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                                                        new String[] {PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1"})));
        }
        // 9. MIDP3 unsigned, unknown optional permission in JAD file -> ignored, grant only the assigned permissions
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("UnidentifiedThirdParty", "UTDP", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1",new Attribute("","MyOptionalClass MyOptionalTarget MyOptionalAction"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("UnidentifiedThirdParty", "UIDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(grantedPermissions != null && checkGrantedPermissions(grantedPermissions, getAssignedPermissions("UnidentifiedThirdParty"), true));
        // 9.1. MIDP3 unsigned, P1, unknown optional P2 requested in JAD -> unknown permission is ignored, grant only P1 plus the assigned permissions
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("UnidentifiedThirdParty", "UTDP", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("",MIDP3_SOCKET_PERMISSION + " socket://"));
        allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1",new Attribute("","MyOptionalClass MyOptionalTarget MyOptionalAction"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("UnidentifiedThirdParty", "UIDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(grantedPermissions != null && checkGrantedPermissions(grantedPermissions,
                   new MIDPPermission[] {new MIDPPermission(INTERNAL_SOCKET_PERMISSION,"socket://")},getAssignedPermissions("UnidentifiedThirdParty")));
        // 10. MIDP3 unsigned MIDlet, unknown mandatory permission in JAR file -> failure
        permissionGranter.removeSecurityData(session, appUID);
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("","MyMandatoryClass MyMandatoryTarget MyMandatoryAction1,MyMandatoryAction2,MyMandatoryAction3"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addManifestAttributes(allAttributes);
            permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
            assertTrue(UNKNOWN_PERMISSION_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(UNKNOWN_PERMISSION_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                                                        new String[] {PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1"})));
        }
        // 11. MIDP3 unsigned MIDlet (only JAR available and it contains authentication info), unknown mandatory permission in JAR file -> failure
        permissionGranter.removeSecurityData(session, appUID);
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("","MyMandatoryClass MyMandatoryTarget MyMandatoryAction1,MyMandatoryAction2,MyMandatoryAction3"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addManifestAttributes(allAttributes);
            permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
            assertTrue(UNKNOWN_PERMISSION_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(UNKNOWN_PERMISSION_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                                                        new String[] {PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1"})));
        }
        // 12. MIDP3 signed MIDlet, unknown mandatory permission in JAR file -> failure
        permissionGranter.removeSecurityData(session, appUID);
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("","MyMandatoryClass MyMandatoryTarget MyMandatoryAction1,MyMandatoryAction2,MyMandatoryAction3"));
            securityAttributes.addManifestAttributes(allAttributes);
            permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
            assertTrue(UNKNOWN_PERMISSION_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(UNKNOWN_PERMISSION_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                                                        new String[] {PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1"})));
        }
        // 13. MIDP3 signed MIDlet, unknown mandatory permission in JAR file -> failure
        permissionGranter.removeSecurityData(session, appUID);
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("","MyMandatoryClass MyMandatoryTarget MyMandatoryAction1,MyMandatoryAction2,MyMandatoryAction3"));
            securityAttributes.addManifestAttributes(allAttributes);
            permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
            assertTrue(UNKNOWN_PERMISSION_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(UNKNOWN_PERMISSION_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                                                        new String[] {PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1"})));
        }
        // 16. Most frequent use-case: unsigned MIDP3 MIDlet, requesting permissions in JAD only -> grant requested plus assigned permissions
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("UnidentifiedThirdParty", "UTDP", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("",MIDP3_SOCKET_PERMISSION + " socket://50"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "2",new Attribute("",MIDP3_HTTP_PERMISSION + " http://www.google.com"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "3",new Attribute("",MIDP3_HTTPS_PERMISSION + " https://www.myurl.com/mypage"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "4",new Attribute("",MIDP3_SSL_PERMISSION + " ssl://*.sun.com"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "5",new Attribute("",MIDP3_DATAGRAM_PERMISSION + " datagram://12345"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "6",new Attribute("",MIDP3_COMM_PERMISSION + " comm:123"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("UnidentifiedThirdParty", "UIDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(grantedPermissions != null && checkGrantedPermissions(grantedPermissions,
                   new MIDPPermission[]
                   {
                       new MIDPPermission(INTERNAL_SOCKET_PERMISSION,"socket://50"),
                       new MIDPPermission(INTERNAL_HTTP_PERMISSION,"http://www.google.com"),
                       new MIDPPermission(INTERNAL_HTTPS_PERMISSION,"https://www.myurl.com/mypage"),
                       new MIDPPermission(INTERNAL_SSL_PERMISSION,"ssl://*.sun.com"),
                       new MIDPPermission(INTERNAL_DATAGRAM_PERMISSION,"datagram://12345"),
                       new MIDPPermission(INTERNAL_COMM_PERMISSION, "comm:123")
                   },
                   getAssignedPermissions("IdentifiedThirdParty")));
        // 16.1 Requesting (among other permissions) permissions which are not available in the domain -> failure
        try
        {
            storage.removeAuthenticationStorageData(appUID);
            permissionGranter.removeSecurityData(session, appUID);
            authData = new AuthenticationStorageData("UnidentifiedThirdParty", "UTDP", null, null, null, null);
            storage.writeAuthenticationStorageData(appUID, authData);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("",MIDP3_SOCKET_PERMISSION + " socket://50"));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "2",new Attribute("",MIDP3_HTTP_PERMISSION + " http://www.google.com"));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "3",new Attribute("",MIDP3_HTTPS_PERMISSION + " https://www.myurl.com/mypage"));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "4",new Attribute("",MIDP3_SSL_PERMISSION + " ssl://*.sun.com"));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "5",new Attribute("",MIDP3_DATAGRAM_PERMISSION + " datagram://12345"));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "6",new Attribute("",MIDP3_COMM_PERMISSION + " comm:123"));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "7",new Attribute("",MIDP3_RUNTIME_PERMISSION + " exitVM"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            authCredentials = new AuthenticationCredentials[1];
            authCredentials[0] = new AuthenticationCredentials("UnidentifiedThirdParty", "UIDP");
            permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
            assertTrue(INVALID_PERMISSION_VALUE_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_PERMISSION_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                                                        new String[] {PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "7"})));
        }
        // 17. Most frequent use-case: signed MIDP3 MIDlet, requesting permissions in JAD&JAR only -> grant requested plus assigned permissions (if any)
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("Operator", "OPD", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("",MIDP3_SOCKET_PERMISSION + " socket://50"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "2",new Attribute("",MIDP3_HTTP_PERMISSION + " http://www.google.com"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "3",new Attribute("",MIDP3_HTTPS_PERMISSION + " https://www.myurl.com/mypage"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "4",new Attribute("",MIDP3_SSL_PERMISSION + " ssl://*.sun.com"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "5",new Attribute("",MIDP3_DATAGRAM_PERMISSION + " datagram://12345"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "6",new Attribute("",MIDP3_COMM_PERMISSION + " comm:123"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","signature"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("Operator", "OPD");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("",MIDP3_SOCKET_PERMISSION + " socket://50"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "2",new Attribute("",MIDP3_HTTP_PERMISSION + " http://www.google.com"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "3",new Attribute("",MIDP3_HTTPS_PERMISSION + " https://www.myurl.com/mypage"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "4",new Attribute("",MIDP3_SSL_PERMISSION + " ssl://*.sun.com"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "5",new Attribute("",MIDP3_DATAGRAM_PERMISSION + " datagram://12345"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "6",new Attribute("",MIDP3_COMM_PERMISSION + " comm:123"));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(grantedPermissions != null && checkGrantedPermissions(grantedPermissions,
                   new MIDPPermission[]
                   {
                       new MIDPPermission(INTERNAL_SOCKET_PERMISSION,"socket://50"),
                       new MIDPPermission(INTERNAL_HTTP_PERMISSION,"http://www.google.com"),
                       new MIDPPermission(INTERNAL_HTTPS_PERMISSION,"https://www.myurl.com/mypage"),
                       new MIDPPermission(INTERNAL_SSL_PERMISSION,"ssl://*.sun.com"),
                       new MIDPPermission(INTERNAL_DATAGRAM_PERMISSION,"datagram://12345"),
                       new MIDPPermission(INTERNAL_COMM_PERMISSION, "comm:123"),
                       new MIDPPermission("javax.microedition.PropertyPermission", "mobinfo.msisdn", "read"),
                       new MIDPPermission("javax.microedition.midlet.AutoStartPermission", null)
                   },
                   getDefaultPermissions()));
        // 18. identified third party (some assigned permissions exist)
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("IdentifiedThirdParty", "ITDP", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("",MIDP3_SOCKET_PERMISSION + " socket://50"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "2",new Attribute("",MIDP3_HTTP_PERMISSION + " http://www.google.com"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "3",new Attribute("",MIDP3_HTTPS_PERMISSION + " https://www.myurl.com/mypage"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "4",new Attribute("",MIDP3_SSL_PERMISSION + " ssl://*.sun.com"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "5",new Attribute("",MIDP3_DATAGRAM_PERMISSION + " datagram://12345"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "6",new Attribute("",MIDP3_COMM_PERMISSION + " comm:123"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","signature"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("IdentifiedThirdParty", "IDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("",MIDP3_SOCKET_PERMISSION + " socket://50"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "2",new Attribute("",MIDP3_HTTP_PERMISSION + " http://www.google.com"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "3",new Attribute("",MIDP3_HTTPS_PERMISSION + " https://www.myurl.com/mypage"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "4",new Attribute("",MIDP3_SSL_PERMISSION + " ssl://*.sun.com"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "5",new Attribute("",MIDP3_DATAGRAM_PERMISSION + " datagram://12345"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "6",new Attribute("",MIDP3_COMM_PERMISSION + " comm:123"));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        grantedPermissions = storage.readGrantedPermissions(appUID);
        assertTrue(grantedPermissions != null && checkGrantedPermissions(grantedPermissions,
                   new MIDPPermission[]
                   {
                       new MIDPPermission(INTERNAL_SOCKET_PERMISSION,"socket://50"),
                       new MIDPPermission(INTERNAL_HTTP_PERMISSION,"http://www.google.com"),
                       new MIDPPermission(INTERNAL_HTTPS_PERMISSION,"https://www.myurl.com/mypage"),
                       new MIDPPermission(INTERNAL_SSL_PERMISSION,"ssl://*.sun.com"),
                       new MIDPPermission(INTERNAL_DATAGRAM_PERMISSION,"datagram://12345"),
                       new MIDPPermission(INTERNAL_COMM_PERMISSION, "comm:123")
                   },
                   getAssignedPermissions("IdentifiedThirdParty")));
        // 19. getBlanketPermissions - null appUID
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        blanketPermissions = permissionGranter.getBlanketPermissions(null);
        assertTrue(blanketPermissions == null);
        // 20. getBlanketPermissions - unknown appUID
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        blanketPermissions = permissionGranter.getBlanketPermissions(appUID);
        assertTrue(blanketPermissions == null);
        // 21. getBlanketPermissions - unsigned suite
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("UnidentifiedThirdParty", "UTDP", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("UnidentifiedThirdParty", "UIDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        blanketPermissions = permissionGranter.getBlanketPermissions(appUID);
        assertTrue(blanketPermissions == null);
        // 22. getBlanketPermissions - one of the requested permissions does not allow Blanket
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
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_DATAGRAM_PERMISSION + COMMA + "                  " + MIDP2_SMS_SEND_PERMISSION));
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("IdentifiedThirdParty", "ITDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_DATAGRAM_PERMISSION + COMMA + "                  " + MIDP2_SMS_SEND_PERMISSION));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_SOCKET_PERMISSION + COMMA + MIDP2_HTTP_PERMISSION));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        blanketPermissions = permissionGranter.getBlanketPermissions(appUID);
        assertTrue(blanketPermissions == null);
        // 23. getBlanketPermissions - request one permissions which is already in Blanket
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
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_HTTP_PERMISSION));
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("IdentifiedThirdParty", "ITDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_HTTP_PERMISSION));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        blanketPermissions = permissionGranter.getBlanketPermissions(appUID);
        assertTrue(blanketPermissions == null);
        // 24. getBlanketPermissions - request Auto Invocation and Net Access -> mutually exclusive permissions
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
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_PUSH_REGISTRY_PERMISSION + COMMA + MIDP2_HTTP_PERMISSION));
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("IdentifiedThirdParty", "ITDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_PUSH_REGISTRY_PERMISSION + COMMA + MIDP2_HTTP_PERMISSION));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        blanketPermissions = permissionGranter.getBlanketPermissions(appUID);
        assertTrue(blanketPermissions == null);
        // 25. getBlanketPermissions - manufacturer signed MIDlet
        permissionGranter.removeSecurityData(session, appUID);
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        authData = new AuthenticationStorageData("Manufacturer", "MFD", null, null, null, null);
        storage.writeAuthenticationStorageData(appUID, authData);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_HTTP_PERMISSION + COMMA + MIDP2_SMS_SEND_PERMISSION + COMMA + MIDP2_MMS_OPEN_PERMISSION + COMMA + MIDP2_COMM_PERMISSION));
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("Manufacturer", "MFD");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_HTTP_PERMISSION + COMMA + MIDP2_SMS_SEND_PERMISSION + COMMA + MIDP2_MMS_OPEN_PERMISSION + COMMA + MIDP2_COMM_PERMISSION));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        blanketPermissions = permissionGranter.getBlanketPermissions(appUID);
        assertTrue(blanketPermissions == null);
        // 26. getBlanketPermissions - request Messaging, Net Access and Local connectivity. Since Messaging and Net Access have the current interaction mode set to Blanket, only Local connectivity is returned
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
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_HTTP_PERMISSION + COMMA + MIDP2_SMS_SEND_PERMISSION + COMMA + MIDP2_MMS_OPEN_PERMISSION + COMMA + MIDP2_COMM_PERMISSION));
        securityAttributes.addDescriptorAttributes(allAttributes);
        authCredentials = new AuthenticationCredentials[1];
        authCredentials[0] = new AuthenticationCredentials("IdentifiedThirdParty", "ITDP");
        permissionGranter.grantJadPermissions(appUID, null, securityAttributes.getPermissionAttributes(), authCredentials);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("",MIDP2_HTTP_PERMISSION + COMMA + MIDP2_SMS_SEND_PERMISSION + COMMA + MIDP2_MMS_OPEN_PERMISSION + COMMA + MIDP2_COMM_PERMISSION));
        securityAttributes.addManifestAttributes(allAttributes);
        permissionGranter.grantJarPermissions(session, appUID, null, securityAttributes.getPermissionAttributes());
        blanketPermissions = permissionGranter.getBlanketPermissions(appUID);
        assertTrue(blanketPermissions != null && blanketPermissions.length == 1 && blanketPermissions[0].equals(UserSecuritySettingsImpl.getLocalizedName(UserSecuritySettings.LOCAL_CONNECTIVITY_SETTINGS)));
        permissionGranter.setPermissionsToBlanket(session, appUID);
        grantedPermissions = storage.readGrantedPermissions(appUID);
        for (int i=0; i<grantedPermissions.size(); i++)
        {
            PolicyBasedPermission permission =
                ((PolicyBasedPermission)grantedPermissions.elementAt(i));
            UserSecuritySettings settings =
                permission.getUserSecuritySettings();
            if (settings != null && settings.getName() == UserSecuritySettings.LOCAL_CONNECTIVITY_SETTINGS)
            {
                assertTrue(settings.getCurrentInteractionMode() == UserSecuritySettings.BLANKET_INTERACTION_MODE);
            }
        }
        // grantAllPermissions - null values
        permissionGranter.removeSecurityData(session, appUID);
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.grantJarPermissions(session, null, null, (ProtectionDomain)null);
        permissionGranter.removeSecurityData(session, appUID);
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.grantJarPermissions(session, appUID, null, (ProtectionDomain)null);
        grantedPermissions = storage.readGrantedPermissions(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        storage.removeAuthenticationStorageData(appUID);
        assertTrue(grantedPermissions == null);
        // grantAllPermissions - operator domain
        grantAllPermissions(ProtectionDomain.getOperatorDomain());
        // grantAllPermissions - manufacturer domain
        grantAllPermissions(ProtectionDomain.getManufacturerDomain());
        // grantAllPermissions - identified third party domain
        grantAllPermissions(ProtectionDomain.getIdentifiedThirdPartyDomain());
        // grantAllPermissions - unidentified third party domain
        grantAllPermissions(ProtectionDomain.getUnidentifiedThirdPartyDomain());
        // Remove following two instr once the real internal permissions are in place
        PermissionMappingTable.setMappingTable(IMPL_PERMISSION_MAPPING_TABLE);
        SecurityPolicyModule.policiesDir = TEST_DATA_DIR + "securitypolicies" + System.getProperty("file.separator");
    }

    private void grantAllPermissions(ProtectionDomain domain)
    {
        permissionGranter.removeSecurityData(session, appUID);
        storage.removeAuthenticationStorageData(appUID);
        permissionGranter.grantJarPermissions(session, appUID, null, domain);
        grantedPermissions = storage.readGrantedPermissions(appUID);
        permissionGranter.removeSecurityData(session, appUID);
        storage.removeAuthenticationStorageData(appUID);
        assertTrue(checkGrantedPermissions(grantedPermissions, getPolicyPermissions(domain.getName())));
    }

    private static MIDPPermission[] getPolicyPermissions(String policyName, String[] permissionNameFilter, String permissionTypeFilter)
    {
        Vector vPermissions = new Vector();
        SecurityPolicyPermission[] policyPerms = null;
        if (policyName.equalsIgnoreCase("manufacturer"))
        {
            policyPerms = SecurityPolicyModuleTests.MANUFACTURER_PERMS;
        }
        else if (policyName.equalsIgnoreCase("operator"))
        {
            policyPerms = SecurityPolicyModuleTests.OPERATOR_PERMS;
        }
        else if (policyName.equalsIgnoreCase("IdentifiedThirdParty"))
        {
            policyPerms = SecurityPolicyModuleTests.IDENTIFIED_THIRD_PARTY_PERMS;
        }
        else if (policyName.equalsIgnoreCase("UnidentifiedThirdParty"))
        {
            policyPerms = SecurityPolicyModuleTests.UNIDENTIFIED_THIRD_PARTY_PERMS;
        }
        for (int i=0; i<policyPerms.length; i++)
        {
            if (((permissionNameFilter != null
                    && findString(policyPerms[i].getName(), permissionNameFilter) != -1)
                    || permissionNameFilter == null) && (permissionTypeFilter == null
                                                         || (permissionTypeFilter.equals("assigned")
                                                             && (policyPerms[i].getType() == PolicyBasedPermission.ASSIGNED_TYPE 
                                                             || policyPerms[i].getType() == PolicyBasedPermission.USER_ASSIGNED_TYPE))))
            {
                vPermissions.addElement(new MIDPPermission(policyPerms[i].getName(), policyPerms[i].getTarget(), policyPerms[i].getActionList()));
            }
        }
        if (policyName.equalsIgnoreCase("IdentifiedThirdParty") && (permissionNameFilter == null && permissionTypeFilter != null && permissionTypeFilter.equals("assigned")))
        {
            vPermissions.addElement(new MIDPPermission("com.nokia.ext2.internal.Ext2Perm", "ext2.target4", "ext2.action4"));
        }
        if (policyName.equalsIgnoreCase("UnidentifiedThirdParty") && permissionNameFilter == null && permissionTypeFilter == null)
        {
            MIDPPermission[] extPermissions = new MIDPPermission[]
            {
                new MIDPPermission("com.nokia.ext1.internal.Ext1Perm", "ext1.target1", "ext1.action1"),
                new MIDPPermission("com.nokia.mj.impl.gcf.protocol.http.HttpPermissionImpl", "http://", null),
                new MIDPPermission("com.nokia.ext1.internal.Ext1Perm", "ext1.target2", "ext1.action2"),
                new MIDPPermission("com.nokia.ext1.internal.Ext1Perm", "ext1.target3", "ext1.action3"),
            };
            for (int i=0; i<extPermissions.length; i++)
            {
                vPermissions.addElement(extPermissions[i]);
            }
        }
        if (policyName.equalsIgnoreCase("IdentifiedThirdParty") && permissionNameFilter == null && permissionTypeFilter == null)
        {
            MIDPPermission[] extPermissions = new MIDPPermission[]
            {
                new MIDPPermission("com.nokia.ext1.public.Ext1Perm", "ext1.target1", "ext1.action1"),
                new MIDPPermission("com.nokia.ext2.internal.Ext2Perm", "ext2.target1", "ext2.action1"),
                new MIDPPermission("com.nokia.ext2.internal.Ext2Perm", "ext2.target2", "ext2.action2"),
                new MIDPPermission("com.nokia.ext2.internal.Ext2Perm", "ext2.target3", "ext2.action3"),
                new MIDPPermission("com.nokia.ext2.internal.Ext2Perm", "ext2.target4", "ext2.action4"),
                new MIDPPermission("com.nokia.ext2.internal.Ext2Perm", "ext2.target5", "ext2.action5"),
            };
            for (int i=0; i<extPermissions.length; i++)
            {
                vPermissions.addElement(extPermissions[i]);
            }
        }
        MIDPPermission[] permissions = new MIDPPermission[vPermissions.size()];
        vPermissions.copyInto(permissions);
        return permissions;
    }

    private static MIDPPermission[] getDefaultPermissions()
    {
        MIDPPermission[] defaultPerms = new MIDPPermission[5];
        defaultPerms[0] = new MIDPPermission("java.util.PropertyPermission", "microedition.*", "read");
        defaultPerms[1] = new MIDPPermission("javax.microedition.PropertyPermission", "mobinfo.publicinfo", "read");
        defaultPerms[2] = new MIDPPermission("javax.microedition.PropertyPermission", "mobinfo.cellid", "read");
        defaultPerms[3] = new MIDPPermission("javax.microedition.PropertyPermission", "mobinfo.countrycode", "read");
        defaultPerms[4] = new MIDPPermission("javax.microedition.PropertyPermission", "mobinfo.networkid", "read");
        return defaultPerms;
    }

    public static boolean checkGrantedPermissions(Vector grantedPermissions, MIDPPermission[] expectedPermissions)
    {
        return checkGrantedPermissions(grantedPermissions, expectedPermissions, false);
    }

    public static boolean checkGrantedPermissions(Vector grantedPermissions, MIDPPermission[] expectedPermissions, boolean expectedPermsIsALargerSet)
    {
        if (!expectedPermsIsALargerSet && grantedPermissions.size() != expectedPermissions.length)
        {
            return false;
        }
        boolean[] grantedPermissionsChecked =
            new boolean[grantedPermissions.size()];
        initBoolArray(grantedPermissionsChecked);
        for (int i=0; i<grantedPermissions.size(); i++)
        {
            MIDPPermission grantedPermission = (MIDPPermission)grantedPermissions.elementAt(i);
            MIDPPermission permission = new MIDPPermission(
                grantedPermission.getName(), grantedPermission.getTarget(), grantedPermission.getActionList());
            int index = findPermission(permission,
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
        if (expectedPermsIsALargerSet)
        {
            return true;
        }
        else
        {
            return checkBoolArray(grantedPermissionsChecked);
        }
    }

    public static void initBoolArray(boolean[] boolArray)
    {
        for (int i=0; i<boolArray.length; i++)
        {
            boolArray[i] = false;
        }
    }

    public static boolean checkBoolArray(boolean[] boolArray)
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

    private static int findPermission(MIDPPermission permission, MIDPPermission[] permissions)
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

    private static int findString(String str, String[] strings)
    {
        for (int i=0; i<strings.length; i++)
        {
            if (str.equals(strings[i]))
            {
                return i;
            }
        }
        return -1;
    }

    private static MIDPPermission[] getPolicyPermissions(String policyName, String[] permissionNameFilter)
    {
        return getPolicyPermissions(policyName, permissionNameFilter, null);
    }

    public static MIDPPermission[] getPolicyPermissions(String policyName)
    {
        return getPolicyPermissions(policyName, null, null);
    }

    private static MIDPPermission[] getAssignedPermissions(String policyName)
    {
        return getPolicyPermissions(policyName, null, "assigned");
    }

    public static boolean checkGrantedPermissions(Vector grantedPermissions, MIDPPermission[] expectedPermissions1, MIDPPermission[] expectedPermissions2)
    {
        // combine the expected permissions
        MIDPPermission[] expectedPermissions = new MIDPPermission[expectedPermissions1.length + expectedPermissions2.length];
        for (int i=0; i<expectedPermissions1.length; i++)
        {
            expectedPermissions[i] = expectedPermissions1[i];
        }
        for (int i=expectedPermissions1.length; i<expectedPermissions1.length + expectedPermissions2.length; i++)
        {
            expectedPermissions[i] = expectedPermissions2[i-expectedPermissions1.length];
        }
        return checkGrantedPermissions(grantedPermissions, expectedPermissions, (expectedPermissions2 != null));
    }


}
