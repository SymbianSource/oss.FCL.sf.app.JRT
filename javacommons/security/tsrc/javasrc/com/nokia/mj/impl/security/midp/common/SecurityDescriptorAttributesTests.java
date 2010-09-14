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
import com.nokia.mj.impl.security.midp.common.SecurityAttributes;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.ErrorMessageBase;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Attribute;

import java.util.Hashtable;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * SecurityDescriptorAttributes unit tests.
 */
public class SecurityDescriptorAttributesTests extends TestCase implements InstallerMain
{
    int assertTrace = 0;

    /**
     * Hashtable holding all the attributes. Tests populate this hashtable
     * prior to being executed
     */
    protected Hashtable allAttributes;

    // constants for permissions
    protected static final String MIDP2_SOCKET_PERMISSION = "javax.microedition.io.Connector.socket";
    protected static final String MIDP2_SERVER_SOCKET_PERMISSION = "javax.microedition.io.Connector.serversocket";
    protected static final String MIDP2_HTTP_PERMISSION = "javax.microedition.io.Connector.http";
    protected static final String MIDP2_HTTPS_PERMISSION = "javax.microedition.io.Connector.https";
    protected static final String MIDP2_SSL_PERMISSION = "javax.microedition.io.Connector.ssl";
    protected static final String MIDP2_DATAGRAM_PERMISSION = "javax.microedition.io.Connector.datagram";
    protected static final String MIDP2_DATAGRAM_RECEIVER_PERMISSION = "javax.microedition.io.Connector.datagramreceiver";
    protected static final String MIDP2_COMM_PERMISSION = "javax.microedition.io.Connector.comm";
    protected static final String MIDP2_PUSH_REGISTRY_PERMISSION = "javax.microedition.io.PushRegistry";

    private static final String INTERNAL_SOCKET_PERMISSION = "com.nokia.mj.impl.gcf.protocol.socket.SocketPermissionImpl";
    private static final String INTERNAL_HTTP_PERMISSION = "com.nokia.mj.impl.gcf.protocol.http.HttpPermissionImpl";
    private static final String INTERNAL_HTTPS_PERMISSION = "com.nokia.mj.impl.gcf.protocol.https.HttpsPermissionImpl";
    private static final String INTERNAL_SSL_PERMISSION = "com.nokia.mj.impl.gcf.protocol.ssl.SSLPermissionImpl";
    private static final String INTERNAL_DATAGRAM_PERMISSION = "com.nokia.mj.impl.gcf.protocol.datagram.DatagramPermissionImpl";
    private static final String INTERNAL_COMM_PERMISSION = "com.nokia.mj.impl.gcf.protocol.comm.CommPermissionImpl";
    private static final String INTERNAL_PUSH_REGISTRY_PERMISSION = "com.nokia.mj.impl.gcf.PushRegistryPermissionImpl";

    /**
     * Hashtable containing the mapping between MIDP3 permissions and their
     * default targets
     */
    private static Hashtable midp2PermissionTargets = new Hashtable();
    static
    {
        midp2PermissionTargets = new Hashtable();
        midp2PermissionTargets.put(INTERNAL_SOCKET_PERMISSION, "socket://*");
        midp2PermissionTargets.put(INTERNAL_HTTP_PERMISSION, "http://*");
        midp2PermissionTargets.put(INTERNAL_HTTPS_PERMISSION, "https://*");
        midp2PermissionTargets.put(INTERNAL_SSL_PERMISSION, "ssl://*");
        midp2PermissionTargets.put(INTERNAL_DATAGRAM_PERMISSION,"datagram://*");
        midp2PermissionTargets.put(INTERNAL_COMM_PERMISSION, "comm:*");
        midp2PermissionTargets.put(INTERNAL_PUSH_REGISTRY_PERMISSION, "*");
    }

    // data being tested
    protected SecurityAttributes securityAttributes;
    private PermissionAttribute[] permissionAttributes;
    private AuthenticationAttribute[] authAttributes;

    // references to error messages
    protected InstallerErrorMessage errorMessage = new InstallerErrorMessage();
    protected InstallerDetailedErrorMessage detailedErrorMessage = new InstallerDetailedErrorMessage();

    // general-purpose constants
    protected static final String MIDP_PROFILE_ATTRIBUTE_NAME = "MicroEdition-Profile";
    protected static final String MIDP3 = "MIDP-3.0";
    protected static final String MIDP2 = "MIDP-2.0";
    private static final String COMMA = ",";

    // constants for the error messages to be displayed in case of failures
    protected String LEGACY_NOT_ALLOWED_MSG = "Placing a MIDP3 attribute in a MIDP2 descriptor threw InvalidAttributeException with following status code and message: ";
    protected String MIDP3_ATTRIBUTE_NOT_ALLOWED_MSG = "Placing a MIDP3 attribute in a MIDP2 descriptor threw InvalidAttributeException with following status code and message: ";
    protected String MIDP2_SIGNATURE_NOT_ALLOWED_MSG = "Placing a MIDP3 signature in a MIDP3 descriptor threw InvalidAttributeException with following status code and message: ";
    protected String UNKNOWN_PERMISSION_MSG = "Placing an unknown permission into descriptor threw InvalidAttributeException with following status code and message: ";
    protected String EMPTY_CERTIFICATE_MSG = "Certificate with empty value threw InvalidAttributeException with following status code and message: ";
    protected String INVALID_CERTIFICATE_MSG = "Certificate with invalid value threw InvalidAttributeException with following status code and message: ";
    protected String CERTIFICATE_WITHOUT_SIGNATURE_MSG = "Certificate without corresponding signature threw InvalidAttributeException with following status code and message: ";
    protected String INVALID_CERTIFICATE_SIGNATURE_MSG = "Certificate with invalid signature threw InvalidAttributeException with following status code and message: ";
    protected String INVALID_PERMISSION_VALUE_MSG = "Placing a permission attribute with invalid value threw InvalidAttributeException with following status code and message: ";
    protected String MISSING_CERTIFICATE_ATTRIBUTE_MSG = "Missing certificate attribute threw InvalidAttributeException with following status code and message: ";

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new SecurityDescriptorAttributesTests("testAuthenticationAttributes", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SecurityDescriptorAttributesTests)tc).testAuthenticationAttributes();
            }
        }));
        suite.addTest(new SecurityDescriptorAttributesTests("testPermissionAttributes", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SecurityDescriptorAttributesTests)tc).testPermissionAttributes();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public SecurityDescriptorAttributesTests()
    {
    }

    public SecurityDescriptorAttributesTests(String aTestName, TestMethod aTestMethod)
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
        allAttributes = new Hashtable();
        securityAttributes = new SecurityAttributes();
    }

    protected void tearDown()
    {
    }

    // tests for chain and signature attributes
    protected void testAuthenticationAttributes()
    {
        // no attributes
        allAttributes.clear();
        authAttributes = securityAttributes.getAuthenticationAttributes();
        assertWithTrace((authAttributes == null) || (authAttributes.length == 0));
        // empty certificate attribute
        try
        {
            allAttributes.clear();
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("", ""));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1"})));
        }
        // invalid certificate attribute
        try
        {
            allAttributes.clear();
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","?%cert"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1"})));
        }
        // missing signature attribute
        allAttributes.clear();
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        assertWithTrace(securityAttributes.getAuthenticationAttributes() == null);
        // missing certificate attribute
        try
        {
            allAttributes.clear();
            allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_MISSING,
                                                        new String[] {AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1"})));
        }
        // invalid signature attribute value
        try
        {
            allAttributes.clear();
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","?signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME})));
        }
        // invalid signature attribute
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","?signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1"})));
        }
        // missing signature
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "2", new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_MISSING,
                                                        new String[] {AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-1"})));
        }
        // MIDP3 signature attribute in a MIDP2 descriptor
        try
        {
            allAttributes.clear();
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_DESCRIPTOR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1"})));
        }
        // MIDP2 signature in MIDP3 descriptor is illegal
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_DESCRIPTOR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME})));
        }
        // MIDP2 signature in MIDP3 descriptor is illegal, even though there are sufficient MIDP3 signatures
        try
        {
            // MIDP2 signature is ignored, but this case succeeds since the MIDP3 signature is present
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","signature"));
            allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_DESCRIPTOR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME})));
        }
        // one certificate chain with 2 certificates and signature
        allAttributes.clear();
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authAttributes = securityAttributes.getAuthenticationAttributes();
        assertWithTrace(authAttributes != null
                   && authAttributes.length == 1
                   && checkChainsAndSignatures(authAttributes,
                                               new String[] {"cert1", "cert2"},
                                               new String[] {"signature"}));
        // 2 identical certificate chains and 1 signature -> only one chain and signature returned (duplicates ignored)
        allAttributes.clear();
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authAttributes = securityAttributes.getAuthenticationAttributes();
        assertWithTrace(authAttributes != null
                   && authAttributes.length == 1
                   && checkChainsAndSignatures(authAttributes,
                                               new String[] {"cert1", "cert2"},
                                               new String[] {"signature"}));
        // 2 identical certificate chains and 2 signatures -> only one chain and signature returned (duplicates ignored)
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","signature"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "2", new Attribute("","signature"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authAttributes = securityAttributes.getAuthenticationAttributes();
        assertWithTrace(authAttributes != null
                   && authAttributes.length == 1
                   && checkChainsAndSignatures(authAttributes,
                                               new String[] {"cert1", "cert2"},
                                               new String[] {"signature"}));
        // one certificate chain with 2 certificates and signature
        allAttributes.clear();
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-4", new Attribute("","cert4"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authAttributes = securityAttributes.getAuthenticationAttributes();
        assertWithTrace(authAttributes != null
                   && authAttributes.length == 1
                   && checkChainsAndSignatures(authAttributes,
                                               new String[] {"cert1", "cert2"},
                                               new String[] {"signature"}));
        // two certificate chains with missing one signature -> pick up the valid certificate and ignore the invalid one
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-1", new Attribute("","cert21"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-2", new Attribute("","cert22"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert11"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert12"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-4", new Attribute("","cert14"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "4-1", new Attribute("","cert41"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "4-2", new Attribute("","cert42"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" , new Attribute("","signature1"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authAttributes = securityAttributes.getAuthenticationAttributes();
        assertWithTrace(authAttributes != null
                   && authAttributes.length == 1
                   && checkChainsAndSignatures(authAttributes,
                                               new String[] {"cert11", "cert12"},
                                               new String[] {"signature1"}));
        // two invalid certificate chains: first one with missing signature, the second one invalid certificate -> signal the invalid certificate
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-1", new Attribute("","cert21"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-2", new Attribute("","*cert22"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert11"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert12"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-4", new Attribute("","cert14"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "4-1", new Attribute("","cert41"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "4-2", new Attribute("","cert42"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "2" , new Attribute("","signature2"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-2"})));
        }
        // two invalid certificate chains: first one with invalid certificate, the second one with missing signature -> signal the invalid certificate
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-1", new Attribute("","cert21"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-2", new Attribute("","cert22"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert11"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","*cert12"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" , new Attribute("","signature1"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2"})));
        }
        // two certificate chains with two signatures
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-1", new Attribute("","cert21"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-2", new Attribute("","cert22"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-3", new Attribute("","cert23"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert11"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert12"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-4", new Attribute("","cert14"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "4-1", new Attribute("","cert41"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "4-2", new Attribute("","cert42"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" , new Attribute("","signature1"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "2" , new Attribute("","signature2"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "5" , new Attribute("","signature5"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authAttributes = securityAttributes.getAuthenticationAttributes();
        assertWithTrace(authAttributes != null
                   && authAttributes.length == 2
                   && checkChainsAndSignatures(authAttributes,
                                               new String[] {"cert11", "cert12", "cert21", "cert22", "cert23"},
                                               new String[] {"signature1", "signature2"}));
        // 100 certificate chains (10 certificates each) with 100 signatures
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        for (int i=1; i<=100; i++)
        {
            for (int j=1; j<=10; j++)
            {
                allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + i + "-" + j, new Attribute("","cert"+ i + "" + j));
            }
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + i , new Attribute("","signature" + i));
        }
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authAttributes = securityAttributes.getAuthenticationAttributes();
        assertWithTrace(authAttributes != null && authAttributes.length == 100);
        for (int i=0; i<100; i++)
        {
            // make sure we have 10 certs in each chain and a corresponding signature
            assertWithTrace(authAttributes[i].getCertChain() != null
                       && authAttributes[i].getSignature() != null
                       && authAttributes[i].getCertChain().length == 10
                       && authAttributes[i].getSignature().equals("signature" + (i+1)));
            // check each chain
            for (int j=0; j<10; j++)
            {
                assertWithTrace((authAttributes[i].getCertChain())[j]
                           .equals("cert" + (i+1) + "" + (j+1)));
            }
        }
    }

    // tests for permission attributes
    protected void testPermissionAttributes()
    {
        // MIDP2 mandatory permissions attribute in a MIDP3 descriptor not allowed
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME, new Attribute("","Perm1, Perm2"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_DESCRIPTOR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME})));
        }
        // MIDP2 optional permissions attribute in a MIDP3 descriptor not allowed
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME, new Attribute("","Perm1, Perm2"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_DESCRIPTOR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME})));
        }
        // MIDP3 mandatory permissions attribute in a MIDP2 descriptor not allowed
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",
                              new Attribute("","PermissionClassName TargetName ActionList"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_DESCRIPTOR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1"})));
        }
        // MIDP3 optional permissions attribute in a MIDP2 descriptor not allowed
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1",
                              new Attribute("","PermissionClassName TargetName ActionList"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_DESCRIPTOR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1"})));
        }
        // MIDP2 mandatory permissions handling - invalid permission value, but because the auth info is not present
        // (MIDlet suite unsigned) the permission attributes are ignored, instead one PermissionAttribute is returned
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME,
                          new Attribute("","PermissionClassName TargetName ActionList"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        permissionAttributes = securityAttributes.getPermissionAttributes();
        assertWithTrace(permissionAttributes != null
                   && permissionAttributes.length == 1
                   && permissionAttributes[0].isLegacyAttribute());
        // MIDP2 mandatory permissions handling - invalid permission value
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME,
                              new Attribute("","PermissionClassName TargetName ActionList"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
            allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                                                        new String[] {PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME})));
        }
        // MIDP2 mandatory permissions handling - unknown permission value
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME,
                              new Attribute("",MIDP2_SOCKET_PERMISSION + COMMA
                                            + MIDP2_SERVER_SOCKET_PERMISSION + COMMA
                                            + "MyPermission"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
            allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                                                        new String[] {PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME})));
        }
        // MIDP2 mandatory permissions handling - known permission value
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME,
                          new Attribute("",MIDP2_SOCKET_PERMISSION
                                        + COMMA + "   "
                                        + MIDP2_HTTP_PERMISSION));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        permissionAttributes = securityAttributes.getPermissionAttributes();
        assertWithTrace(permissionAttributes != null
                   && permissionAttributes.length == 2
                   && checkImportance(permissionAttributes, 0, 2)
                   && checkNamesAndTargets(permissionAttributes,
                                           new String[] {INTERNAL_SOCKET_PERMISSION, INTERNAL_HTTP_PERMISSION})
                   && checkActionLists(permissionAttributes));
        // MIDP2 mandatory permissions handling - two permissions with same name -> one permission only (discard duplicates)
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME,
                          new Attribute("",MIDP2_SSL_PERMISSION + COMMA
                                        + MIDP2_SSL_PERMISSION));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        permissionAttributes = securityAttributes.getPermissionAttributes();
        assertWithTrace(permissionAttributes != null
                   && permissionAttributes.length == 1
                   && checkImportance(permissionAttributes, 0, 1)
                   && checkNamesAndTargets(permissionAttributes,
                                           new String[] {INTERNAL_SSL_PERMISSION})
                   && checkActionLists(permissionAttributes));
        // MIDP2 optional permissions handling - invalid permission value
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME, new Attribute("","*?"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
            allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(true);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(false);
        }
        // MIDP2 optional permissions handling - unknown permission value
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME,
                              new Attribute("","MyPermission," + MIDP2_SOCKET_PERMISSION));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
            allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(true);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(false);
        }
        // MIDP2 optional permissions handling - known permission value
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME,
                          new Attribute("",MIDP2_DATAGRAM_PERMISSION + COMMA
                                        + MIDP2_COMM_PERMISSION));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        permissionAttributes = securityAttributes.getPermissionAttributes();
        assertWithTrace(permissionAttributes != null
                   && permissionAttributes.length == 2
                   && checkImportance(permissionAttributes, 2, 0)
                   && checkNamesAndTargets(permissionAttributes,
                                           new String[]
                                           {
                                               INTERNAL_DATAGRAM_PERMISSION,
                                               INTERNAL_COMM_PERMISSION
                                           })
                   && checkActionLists(permissionAttributes));
        // MIDP2 optional permissions handling - two permissions with same name -> one permission only (discard duplicates)
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME,
                          new Attribute("",MIDP2_PUSH_REGISTRY_PERMISSION + COMMA
                                        + MIDP2_PUSH_REGISTRY_PERMISSION));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        permissionAttributes = securityAttributes.getPermissionAttributes();
        assertWithTrace(permissionAttributes != null
                   && permissionAttributes.length == 1
                   && checkImportance(permissionAttributes, 1, 0)
                   && checkNamesAndTargets(permissionAttributes,
                                           new String[] {INTERNAL_PUSH_REGISTRY_PERMISSION})
                   && checkActionLists(permissionAttributes));
        // both optional and mandatory present and ok
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME,
                          new Attribute("",MIDP2_DATAGRAM_PERMISSION + COMMA
                                        + "                  "
                                        + MIDP2_COMM_PERMISSION));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME,
                          new Attribute("",MIDP2_SOCKET_PERMISSION + COMMA + MIDP2_HTTP_PERMISSION));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        permissionAttributes = securityAttributes.getPermissionAttributes();
        assertWithTrace(permissionAttributes != null
                   && permissionAttributes.length == 4
                   && checkImportance(permissionAttributes, 2, 2)
                   && checkNamesAndTargets(permissionAttributes,
                                           new String[]
                                           {
                                               INTERNAL_DATAGRAM_PERMISSION,
                                               INTERNAL_COMM_PERMISSION,
                                               INTERNAL_SOCKET_PERMISSION,
                                               INTERNAL_HTTP_PERMISSION
                                           })
                   && checkActionLists(permissionAttributes));
        // MIDP3 permissions without name and without target
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("","MyMandatoryClass"));
        allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1",new Attribute("","MyOptionalClass"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        permissionAttributes = securityAttributes.getPermissionAttributes();
        assertWithTrace(permissionAttributes != null
                   && permissionAttributes.length == 2
                   && checkImportance(permissionAttributes, 1, 1)
                   && checkNamesAndTargets(permissionAttributes,
                                           new String[] {"MyMandatoryClass", "MyOptionalClass"}, new String[] {null, null})
                   && checkActionLists(permissionAttributes));
        // MIDP3 permissions without target
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("","MyMandatoryClass MyMandatoryTarget"));
        allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1",new Attribute("","MyOptionalClass MyOptionalTarget"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        permissionAttributes = securityAttributes.getPermissionAttributes();
        assertWithTrace(permissionAttributes != null
                   && permissionAttributes.length == 2
                   && checkImportance(permissionAttributes, 1, 1)
                   && checkNamesAndTargets(permissionAttributes,
                                           new String[] {"MyMandatoryClass", "MyOptionalClass"}, new String[] {"MyMandatoryTarget","MyOptionalTarget"})
                   && checkActionLists(permissionAttributes));
        // MIDP3 valid permissions
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("","MyMandatoryClass MyMandatoryTarget MyMandatoryAction1,MyMandatoryAction2,MyMandatoryAction3"));
        allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1",new Attribute("","MyOptionalClass MyOptionalTarget MyOptionalAction"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        permissionAttributes = securityAttributes.getPermissionAttributes();
        assertWithTrace(permissionAttributes != null
                   && permissionAttributes.length == 2
                   && checkImportance(permissionAttributes, 1, 1)
                   && checkNamesAndTargets(permissionAttributes,
                                           new String[] {"MyMandatoryClass", "MyOptionalClass"}, new String[] {"MyMandatoryTarget","MyOptionalTarget"})
                   && checkActionLists(permissionAttributes, new String[] {"MyMandatoryAction1,MyMandatoryAction2,MyMandatoryAction3", "MyOptionalAction"}));
        // MIDP3 valid permissions - 2 identical permissions -> only one permission returned (duplicates ignored)
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("","MyMandatoryClass MyMandatoryTarget MyMandatoryAction1,MyMandatoryAction2,MyMandatoryAction3"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "2",new Attribute("","MyMandatoryClass MyMandatoryTarget MyMandatoryAction1,MyMandatoryAction2,MyMandatoryAction3 "));
        allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1",new Attribute("","MyOptionalClass MyOptionalTarget MyOptionalAction"));
        allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "2",new Attribute("","MyOptionalClass MyOptionalTarget MyOptionalAction"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        permissionAttributes = securityAttributes.getPermissionAttributes();
        assertWithTrace(permissionAttributes != null
                   && permissionAttributes.length == 2
                   && checkImportance(permissionAttributes, 1, 1)
                   && checkNamesAndTargets(permissionAttributes,
                                           new String[] {"MyMandatoryClass", "MyOptionalClass"}, new String[] {"MyMandatoryTarget","MyOptionalTarget"})
                   && checkActionLists(permissionAttributes, new String[] {"MyMandatoryAction1,MyMandatoryAction2,MyMandatoryAction3", "MyOptionalAction"}));
        // invalid mandatory permission
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("","MyMandatoryClass MyMandatoryTarget MyMandatoryAction1 MyMandatoryAction2"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1"})));
        }
        // invalid mandatory permission
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",new Attribute("","MyMandatoryClass   MyMandatoryTarget   MyMandatoryAction1 MyMandatoryAction2"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertWithTrace(false);
        }
        catch (InvalidAttributeException e)
        {
            assertWithTrace(
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1"})));
        }
        // skip trailing and leading spaces for the permission triplet
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1",new Attribute("","    MyMandatoryClass            MyMandatoryTarget  MyMandatoryAction      "));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        permissionAttributes = securityAttributes.getPermissionAttributes();
        assertWithTrace(permissionAttributes != null
                   && permissionAttributes.length == 1
                   && checkImportance(permissionAttributes, 1, 0)
                   && checkNamesAndTargets(permissionAttributes,
                                           new String[] {"MyMandatoryClass"}, new String[] {"MyMandatoryTarget"})
                   && checkActionLists(permissionAttributes, new String[] {"MyMandatoryAction"}));
        // invalid classnames
        String[] JAVA_KEYWORDS =
        {
            "abstract","continue","for","new","switch",
            "assert","default","if","package","synchronized",
            "boolean","do","goto","private","this","break",
            "double","implements","protected","throw","byte",
            "else","import","public","throws","case","enum",
            "instanceof","return","transient","catch","extends",
            "int","short","try","char","final","interface","static",
            "void","class","finally","long","strictfp","volatile",
            "const","float","native","super","while",
            "true", "false", "null"
        };
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        String className = null;
        for (int i=0; i<JAVA_KEYWORDS.length; i++)
        {
            switch (i%3)
            {
            case 2:
                className = JAVA_KEYWORDS[i] + ".mypackage.myclassame";
                break;
            case 0:
                className = "mypackage." + JAVA_KEYWORDS[i] + ".myclassame";
                break;
            case 1:
                className = "mypackage.myclassame." + JAVA_KEYWORDS[i];
                break;
            }
            try
            {
                allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1",new Attribute("",className + "            MyMandatoryTarget  MyMandatoryAction      "));
                securityAttributes = new SecurityAttributes();
                securityAttributes.addDescriptorAttributes(allAttributes);
                assertWithTrace(false);
            }
            catch (InvalidAttributeException e)
            {
            }
        }
    }

    private boolean checkImportance(PermissionAttribute[] permissionAttributes, int optionalPerms, int mandatoryPerms)
    {
        int oPerms = 0;
        int mPerms = 0;
        for (int i=0; i<permissionAttributes.length; i++)
        {
            switch (permissionAttributes[i].getImportance())
            {
            case PermissionAttribute.OPTIONAL_PERMISSION:
                oPerms++;
                break;
            case PermissionAttribute.MANDATORY_PERMISSION:
                mPerms++;
                break;
            }
        }
        return (oPerms == optionalPerms && mPerms == mandatoryPerms);
    }

    private boolean checkNamesAndTargets(PermissionAttribute[] permissionAttributes, String[] permissionNames)
    {
        return checkNamesAndTargets(permissionAttributes, permissionNames, midp2PermissionTargets, true);
    }

    private boolean checkNamesAndTargets(PermissionAttribute[] permissionAttributes, String[] permissionNames, String[] permissionTargets)
    {
        Hashtable permissionTargetsMapping = new Hashtable();
        for (int i=0; i < permissionNames.length; i++)
        {
            if (permissionTargets[i] == null)
            {
                permissionTargetsMapping.put(permissionNames[i], "");
            }
            else
            {
                permissionTargetsMapping.put(permissionNames[i], permissionTargets[i]);
            }
        }
        return checkNamesAndTargets(permissionAttributes, permissionNames, permissionTargetsMapping, false);
    }


    private boolean checkNamesAndTargets(PermissionAttribute[] permissionAttributes, String[] permissionNames, Hashtable permissionTargets, boolean legacyAttributes)
    {
        boolean[] namesAndTargetsChecked =
            new boolean[permissionAttributes.length];
        initBoolArray(namesAndTargetsChecked);
        // traverse the permissions and check their names and targets
        for (int i=0; i<permissionAttributes.length; i++)
        {
            if (legacyAttributes && !permissionAttributes[i].isLegacyAttribute())
            {
                return false;
            }
            int index = findString(permissionAttributes[i].getName(),
                                   permissionNames);
            if (index == -1)
            {
                return false;
            }
            String target = (String)permissionTargets.get(permissionAttributes[i].getName());
            if ((target.length() == 0 && permissionAttributes[i].getTarget() != null)
                    || (target.length() != 0 && !permissionAttributes[i].getTarget().equals((String)target)))
            {
                return false;
            }
            if (!namesAndTargetsChecked[i])
            {
                namesAndTargetsChecked[i] = true;
            }
            else
            {
                return false;
            }
        }
        return checkBoolArray(namesAndTargetsChecked);
    }

    private boolean checkActionLists(PermissionAttribute[] permissionAttributes)
    {
        return checkActionLists(permissionAttributes, null);
    }

    private boolean checkActionLists(PermissionAttribute[] permissionAttributes, String[] actionLists)
    {
        boolean[] actionListsChecked =
            new boolean[permissionAttributes.length];
        if (actionLists != null)
        {
            initBoolArray(actionListsChecked);
        }
        for (int i=0; i<permissionAttributes.length; i++)
        {
            if (actionLists == null && permissionAttributes[i].getActionList() != null)
            {
                return false;
            }
            if (actionLists != null)
            {
                int actionListIndex = findString(permissionAttributes[i].getActionList(), actionLists);
                if (actionListIndex == -1)
                {
                    return false;
                }
                if (!actionListsChecked[i])
                {
                    actionListsChecked[i] = true;
                }
                else
                {
                    return false;
                }
            }
        }
        if (actionLists != null)
        {
            return checkBoolArray(actionListsChecked);
        }
        return true;
    }

    private boolean checkChainsAndSignatures(AuthenticationAttribute[] authAttributes, String[] certs, String[] signatures)
    {
        boolean[] certsChecked = new boolean[certs.length];
        initBoolArray(certsChecked);
        boolean[] signaturesChecked = new boolean[signatures.length];
        initBoolArray(signaturesChecked);
        // traverse the permissions and check their names and targets
        for (int i=0; i<authAttributes.length; i++)
        {
            // check chain
            String[] chain = authAttributes[i].getCertChain();
            if (chain != null)
            {
                for (int j=0; j<chain.length; j++)
                {
                    int certIndex = findString(chain[j],certs);
                    if (certIndex == -1)
                    {
                        return false;
                    }
                    if (!certsChecked[certIndex])
                    {
                        certsChecked[certIndex] = true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            // check signature
            int sigIndex = findString(authAttributes[i].getSignature(),
                                      signatures);
            if (sigIndex == -1)
            {
                return false;
            }
            if (!signaturesChecked[sigIndex])
            {
                signaturesChecked[sigIndex] = true;
            }
            else
            {
                return false;
            }
        }
        return (checkBoolArray(certsChecked) && checkBoolArray(signaturesChecked));
    }

    private void assertWithTrace(boolean aCondition)
    {
        assertTrue("" + assertTrace, aCondition);
        assertTrace++;
    }


    private int findString(String str, String[] strings)
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
}
