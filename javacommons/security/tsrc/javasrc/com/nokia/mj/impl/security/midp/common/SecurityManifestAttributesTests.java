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
 * SecurityManifestAttributes unit tests.
 */
public class SecurityManifestAttributesTests extends SecurityDescriptorAttributesTests
{
    // data being tested
    private AuthorizationRestrictionAttribute[] authRestrictionAttributes;

    // constants for the error messages to be displayed in case of failures
    private String INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG = "Authorization restriction attribute with invalid value threw InvalidAttributeException with following status code and message: ";
    private String INVALID_NETWORK_RESTRICTIONS_ATTRIBUTE_VALUE_MSG = "Network restrictions attribute with invalid value threw InvalidAttributeException with following status code and message: ";
    private String MISSING_AUTH_RESTRICTION_ATTRIBUTE_MSG = "Authorization restriction attribute without corresponding certificate threw InvalidAttributeException with following status code and message: ";
    private String JAD_ONLY_PERMISSION = "Placing a permission only in JAD threw InvalidAttributeException with following status code and message: ";

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new SecurityManifestAttributesTests("testAuthorizationRestrictionAttributes", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SecurityManifestAttributesTests)tc).testAuthorizationRestrictionAttributes();
            }
        }));
        suite.addTest(new SecurityManifestAttributesTests("testPermissionAttributes", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SecurityManifestAttributesTests)tc).testPermissionsAttributes();
            }
        }));
        suite.addTest(new SecurityManifestAttributesTests("testNetworkRestrictionsAttribute", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SecurityManifestAttributesTests)tc).testNetworkRestrictionsAttribute();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public SecurityManifestAttributesTests()
    {
    }

    public SecurityManifestAttributesTests(String aTestName, TestMethod aTestMethod)
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

    protected void testAuthorizationRestrictionAttributes()
    {
        // no attributes
        allAttributes.clear();
        securityAttributes = new SecurityAttributes();
        securityAttributes.addManifestAttributes(allAttributes);
        authRestrictionAttributes = securityAttributes.getAuthorizationRestrictionAttributes();
        assertTrue((authRestrictionAttributes == null) || (authRestrictionAttributes.length == 0));
        // valid MIDP3 restriction attribute in a MIDP2 descriptor
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=SELF;vendor=ANY;signer=alias"));
        allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","alias cert"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addManifestAttributes(allAttributes);
        assertTrue(true);
        // invalid MIDP3 restriction attribute in a MIDP2 descriptor
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("",""));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addManifestAttributes(allAttributes);
            assertTrue(MIDP3_ATTRIBUTE_NOT_ALLOWED_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(MIDP3_ATTRIBUTE_NOT_ALLOWED_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1"})));
        }
        // empty restriction attribute value
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("",""));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addManifestAttributes(allAttributes);
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1"})));
        }
        // empty restriction attribute value
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("",""));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addManifestAttributes(allAttributes);
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1"})));
        }
        // invalid restriction attribute value
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","something"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addManifestAttributes(allAttributes);
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1"})));
        }
        // invalid restriction attribute value
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","something_else"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addManifestAttributes(allAttributes);
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1"})));
        }
        // invalid restriction attribute value
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=some;vendor=vendor_name;signer=ANY"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addManifestAttributes(allAttributes);
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1"})));
        }
        // missing restriction attribute value
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=SELF;vendor=vendor_name;signer=alias"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addManifestAttributes(allAttributes);
            assertTrue(MISSING_AUTH_RESTRICTION_ATTRIBUTE_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(MISSING_AUTH_RESTRICTION_ATTRIBUTE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_MISSING,
                                                        new String[] {AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1"})));
        }
        // missing restriction attribute value
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=ANY;vendor=vendor_name1;signer=alias1"));
            allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","alias1 cert1"));
            allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "3", new Attribute("","domain=SELF;vendor=vendor_name3;signer=alias3"));
            allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "2", new Attribute("","alias2 cert2"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addManifestAttributes(allAttributes);
            assertTrue(MISSING_AUTH_RESTRICTION_ATTRIBUTE_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(MISSING_AUTH_RESTRICTION_ATTRIBUTE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_MISSING,
                                                        new String[] {AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "2"})));
        }
        // invalid restriction attribute value
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=SELF;vendor=vendor_name;signer=alias"));
            allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","aliascert"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addManifestAttributes(allAttributes);
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1"})));
        }
        // invalid restriction attribute value
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=SELF;vendor=vendor_name;signer=alias"));
            allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","alias1 cert"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addManifestAttributes(allAttributes);
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1"})));
        }
        // invalid restriction attribute value: signer is not base 64
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=SELF;vendor=vendor_name;signer=alias"));
            allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","alias *cert"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addManifestAttributes(allAttributes);
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1"})));
        }
        // invalid restriction attribute value: signeralias contains whitespaces
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=SELF;vendor=vendor_name;signer=alias "));
            allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","alias *cert"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addManifestAttributes(allAttributes);
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1"})));
        }
        // invalid restriction attribute value: second attribute contains more than one whitespace
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=SELF;vendor=vendor_name;signer=alias"));
            allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","alias  cert"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addManifestAttributes(allAttributes);
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1"})));
        }
        // duplicate attribute values -> only one returned (duplicates ignored)
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=SELF;vendor=vendor_name;signer=alias"));
        allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","alias cert"));
        allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "2", new Attribute("","domain=SELF;vendor=vendor_name;signer=alias"));
        allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "2", new Attribute("","alias cert"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addManifestAttributes(allAttributes);
        authRestrictionAttributes = securityAttributes.getAuthorizationRestrictionAttributes();
        assertTrue(authRestrictionAttributes != null
                   && authRestrictionAttributes.length == 1
                   && authRestrictionAttributes[0].getDomain().equals("SELF")
                   && authRestrictionAttributes[0].getVendor().equals("vendor_name")
                   && authRestrictionAttributes[0].getSigner().equals("cert"));
        // attributes ok
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=SELF;vendor=vendor_name;signer=alias"));
        allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","alias cert"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addManifestAttributes(allAttributes);
        authRestrictionAttributes = securityAttributes.getAuthorizationRestrictionAttributes();
        assertTrue(authRestrictionAttributes != null
                   && authRestrictionAttributes.length == 1
                   && authRestrictionAttributes[0].getDomain().equals("SELF")
                   && authRestrictionAttributes[0].getVendor().equals("vendor_name")
                   && authRestrictionAttributes[0].getSigner().equals("cert"));
        // whitespaces are ignored
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute(""," domain    =      SELF    ;       vendor =      vendor_name ;  signer=alias"));
        allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","alias cert"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addManifestAttributes(allAttributes);
        authRestrictionAttributes = securityAttributes.getAuthorizationRestrictionAttributes();
        assertTrue(authRestrictionAttributes != null
                   && authRestrictionAttributes.length == 1
                   && authRestrictionAttributes[0].getDomain().equals("SELF")
                   && authRestrictionAttributes[0].getVendor().equals("vendor_name")
                   && authRestrictionAttributes[0].getSigner().equals("cert"));
        // the lowest value of MUST be 1 and consecutive ordinals MUST be used. The first missing entry terminates the list. Any additional entries MUST be ignored.
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "0", new Attribute("","domain=SELF;vendor=vendor_name;signer=alias"));
        allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "0", new Attribute("","alias cert"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addManifestAttributes(allAttributes);
        authRestrictionAttributes = securityAttributes.getAuthorizationRestrictionAttributes();
        assertTrue(authRestrictionAttributes == null
                   || authRestrictionAttributes.length == 0);
        // the lowest value of MUST be 1 and consecutive ordinals MUST be used. The first missing entry terminates the list. Any additional entries MUST be ignored.
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=ANY;vendor=vendor_name1;signer=alias1"));
        allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","alias1 cert1"));
        allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "3", new Attribute("","domain=SELF;vendor=vendor_name3;signer=alias3"));
        allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "3", new Attribute("","alias3 cert3"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addManifestAttributes(allAttributes);
        authRestrictionAttributes = securityAttributes.getAuthorizationRestrictionAttributes();
        assertTrue(authRestrictionAttributes != null
                   && authRestrictionAttributes.length == 1
                   && authRestrictionAttributes[0].getDomain().equals("ANY")
                   && authRestrictionAttributes[0].getVendor().equals("vendor_name1")
                   && authRestrictionAttributes[0].getSigner().equals("cert1"));
        // signer=ANY and Auth-Cert-1 is present -> Auth-Cert-1 is present and syntactically ok, but it is ignored
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=ANY;vendor=vendor_name1;signer=ANY"));
        allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","alias1 cert1"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addManifestAttributes(allAttributes);
        authRestrictionAttributes = securityAttributes.getAuthorizationRestrictionAttributes();
        assertTrue(authRestrictionAttributes != null
                   && authRestrictionAttributes.length == 1
                   && authRestrictionAttributes[0].getDomain().equals("ANY")
                   && authRestrictionAttributes[0].getVendor().equals("vendor_name1")
                   && authRestrictionAttributes[0].getSigner().equals("ANY"));
        // signer=ANY and Auth-Cert-1 is present -> Auth-Cert-1 is present and syntactically nok -> ignore Auth-Cert-1 attribute
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=SELF;vendor=vendor_name;signer=ANY"));
        allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","alias *cert"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addManifestAttributes(allAttributes);
        authRestrictionAttributes = securityAttributes.getAuthorizationRestrictionAttributes();
        assertTrue(authRestrictionAttributes != null
                   && authRestrictionAttributes.length == 1
                   && authRestrictionAttributes[0].getDomain().equals("SELF")
                   && authRestrictionAttributes[0].getVendor().equals("vendor_name")
                   && authRestrictionAttributes[0].getSigner().equals("ANY"));
        // invalid restriction defined in MIDP3 JAD -> failure
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=SELF;vendor=vendor_name;signer=alias"));
            allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","aliascert"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1"})));
        }
        // invalid restriction attribute defined in legacy JAD -> failure
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=SELF;vendor=vendor_name;signer=alias"));
            allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","alias1 cert"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_AUTH_RESTRICTION_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1"})));
        }
        // ok restriction defined only in JAD -> failure
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=SELF;vendor=vendor_name;signer=alias"));
            allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","alias cert"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            securityAttributes.addManifestAttributes(allAttributes);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(JAD_ONLY_PERMISSION + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.ATTRIBUTE_MISMATCH
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_MISMATCH,
                                                        new String[] {AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1"})));
        }
        // R1 defined in JAD, R2 defined in JAR -> failure
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=SELF;vendor=vendor_name;signer=alias"));
            allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","alias cert"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "2", new Attribute("","domain=SELF;vendor=vendor_name;signer=alias"));
            allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "2", new Attribute("","alias cert"));
            securityAttributes.addManifestAttributes(allAttributes);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(JAD_ONLY_PERMISSION + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.ATTRIBUTE_MISMATCH
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_MISMATCH,
                                                        new String[] {AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1"})));
        }
        // R1 defined in JAD, R1,R2 defined in JAR -> ok
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=SELF;vendor=vendor_name;signer=alias1"));
        allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","alias1 cert1"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "1", new Attribute("","domain=SELF;vendor=vendor_name;signer=alias1"));
        allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","alias1 cert1"));
        allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + "2", new Attribute("","domain=ANY;vendor=ANY;signer=alias2"));
        allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + "2", new Attribute("","alias2 cert2"));
        securityAttributes.addManifestAttributes(allAttributes);
        // 100 restrictions with 100 certificates
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        for (int i=1; i<=100; i++)
        {
            switch (i%6)
            {
            case 0:
                allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + i , new Attribute("","domain=ANY;vendor=vendor"+ i + ";signer=alias" + i));
                allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + i , new Attribute("","alias" + i+" cert"+i));
                break;
            case 1:
                allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + i , new Attribute("","domain=SELF;vendor=vendor"+ i + ";signer=alias" + i));
                allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + i , new Attribute("","alias" + i+" cert"+i));
                break;
            case 2:
                allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + i , new Attribute("","domain=ANY;vendor=vendor"+ i + ";signer=ANY"));
                break;
            case 3:
                allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + i , new Attribute("","domain=ANY;vendor=ANY;signer=alias" + i));
                allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + i , new Attribute("","alias" + i+" cert"+i));
                break;
            case 4:
                allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + i , new Attribute("","domain=SELF;vendor=SELF;signer=alias" + i));
                allAttributes.put(AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX + i , new Attribute("","alias" + i+" cert"+i));
                break;
            case 5:
                allAttributes.put(AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX + i , new Attribute("","domain=SELF;vendor=vendor"+i +";signer=ANY"));
                break;
            }
        }
        securityAttributes = new SecurityAttributes();
        securityAttributes.addManifestAttributes(allAttributes);
        authRestrictionAttributes = securityAttributes.getAuthorizationRestrictionAttributes();
        assertTrue(authRestrictionAttributes != null && authRestrictionAttributes.length == 100);
        for (int i=1; i<=100; i++)
        {
            String domain = null;
            String vendor = null;
            String signer = null;
            switch (i%6)
            {
            case 0:
                domain = "ANY";
                vendor = "vendor"+i;
                signer = "cert"+i;
                break;
            case 1:
                domain = "SELF";
                vendor = "vendor"+i;
                signer = "cert"+i;
                break;
            case 2:
                domain = "ANY";
                vendor = "vendor"+i;
                signer = "ANY";
                break;
            case 3:
                domain = "ANY";
                vendor = "ANY";
                signer = "cert"+i;
                break;
            case 4:
                domain = "SELF";
                vendor = "SELF";
                signer = "cert"+i;
                break;
            case 5:
                domain = "SELF";
                vendor = "vendor"+i;
                signer = "ANY";
                break;
            }
            assertTrue(authRestrictionAttributes[i-1].getDomain().equals(domain)
                       && authRestrictionAttributes[i-1].getVendor().equals(vendor)
                       && authRestrictionAttributes[i-1].getSigner().equals(signer));
        }
        // authentication info is ignored from manifest attributes
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1", new Attribute("","signature"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "2", new Attribute("","signature"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addManifestAttributes(allAttributes);
        assertTrue(securityAttributes.getAuthenticationAttributes() == null);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert1"));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-2", new Attribute("","cert2"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addManifestAttributes(allAttributes);
        assertTrue(securityAttributes.getAuthenticationAttributes() == null);
    }

    private void testPermissionsAttributes()
    {
        // signed legacy, no permissions requested -> ok
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME , new Attribute("","signature"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes.addManifestAttributes(allAttributes);
        // signed legacy, mandatory permissions requested in JAD only -> ok
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME , new Attribute("","signature"));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME,
                          new Attribute("",MIDP2_SOCKET_PERMISSION));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes.addManifestAttributes(allAttributes);
        assertTrue(true);
        // signed legacy, opt permissions requested in JAD only -> ok
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME , new Attribute("","signature"));
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME,
                          new Attribute("",MIDP2_SOCKET_PERMISSION));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes.addManifestAttributes(allAttributes);
        assertTrue(true);
        // signed legacy, opt permission requested in JAD, mandatory perms requested in JAR -> ok
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME , new Attribute("","signature"));
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME,
                          new Attribute("",MIDP2_SOCKET_PERMISSION));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME,
                          new Attribute("",MIDP2_HTTP_PERMISSION));
        securityAttributes.addManifestAttributes(allAttributes);
        assertTrue(true);
        // signed legacy, mandatory permission requested in JAD, optional perms requested in JAR -> ok
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME , new Attribute("","signature"));
        allAttributes.put(PermissionAttribute.MANDATORY_LEGACY_ATTRIBUTE_NAME,
                          new Attribute("",MIDP2_DATAGRAM_PERMISSION));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(PermissionAttribute.OPTIONAL_LEGACY_ATTRIBUTE_NAME,
                          new Attribute("",MIDP2_COMM_PERMISSION));
        securityAttributes.addManifestAttributes(allAttributes);
        assertTrue(true);
        // signed MIDP3, no permissions requested -> ok
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" , new Attribute("","signature"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        securityAttributes.addManifestAttributes(allAttributes);
        // signed MIDP3, mandatory permissions requested in JAD only -> failure
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" , new Attribute("","signature"));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",
                              new Attribute("","MandatoryPermissionClassName TargetName ActionList"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            securityAttributes.addManifestAttributes(allAttributes);
            assertTrue(JAD_ONLY_PERMISSION, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(JAD_ONLY_PERMISSION + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.ATTRIBUTE_MISMATCH
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_MISMATCH,
                                                        new String[] {PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1"})));
        }
        // signed MIDP3, optional permissions requested in JAD only -> failure
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" , new Attribute("","signature"));
            allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1",
                              new Attribute("","OptPermissionClassName TargetName ActionList"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            securityAttributes.addManifestAttributes(allAttributes);
            assertTrue(JAD_ONLY_PERMISSION, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(JAD_ONLY_PERMISSION + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.ATTRIBUTE_MISMATCH
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_MISMATCH,
                                                        new String[] {PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1"})));
        }
        // signed MIDP3, opt P1, mandatory P2 requested in JAD, P1 requested in JAR -> failure
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" , new Attribute("","signature"));
            allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1",
                              new Attribute("","OptPermissionClassName TargetName ActionList"));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",
                              new Attribute("","MandatoryPermissionClassName TargetName ActionList"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1",
                              new Attribute("","MandatoryPermissionClassName TargetName ActionList"));
            securityAttributes.addManifestAttributes(allAttributes);
            assertTrue(JAD_ONLY_PERMISSION, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(JAD_ONLY_PERMISSION + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.ATTRIBUTE_MISMATCH
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_MISMATCH,
                                                        new String[] {PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1"})));
        }
        // signed MIDP3, mandatory P1, opt P2 requested in JAD, P1 requested in JAR -> failure
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" , new Attribute("","signature"));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",
                              new Attribute("","MandatoryPermissionClassName TargetName ActionList"));
            allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1",
                              new Attribute("","OptionalPermissionClassName TargetName ActionList"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",
                              new Attribute("","MandatoryPermissionClassName TargetName ActionList"));
            securityAttributes.addManifestAttributes(allAttributes);
            assertTrue(JAD_ONLY_PERMISSION, false);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(JAD_ONLY_PERMISSION + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.ATTRIBUTE_MISMATCH
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_MISMATCH,
                                                        new String[] {PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1"})));
        }
        // signed MIDP3, mandatory P1 requested in JAD, P1,P2 requested in JAR -> ok
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" , new Attribute("","signature"));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",
                          new Attribute("","MandatoryPermissionClassName TargetName ActionList"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(PermissionAttribute.MANDATORY_ATTRIBUTE_PREFIX + "1",
                          new Attribute("","MandatoryPermissionClassName TargetName ActionList"));
        allAttributes.put(PermissionAttribute.OPTIONAL_ATTRIBUTE_PREFIX + "1",
                          new Attribute("","OptionalPermissionClassName TargetName ActionList"));
        securityAttributes.addManifestAttributes(allAttributes);
    }

    // tests for network restrictions attribute
    private void testNetworkRestrictionsAttribute()
    {
        // unsigned legacy MIDlet -> value is ignored
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(NetworkRestrictionsAttribute.ATTRIBUTE_NAME, new Attribute("","invalid"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addManifestAttributes(allAttributes);
        assertTrue(securityAttributes.getNetworkRestrictionsAttribute() == null);
        // unsigned MIDP3 MIDlet -> value is ignored
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(NetworkRestrictionsAttribute.ATTRIBUTE_NAME, new Attribute("","invalid"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addManifestAttributes(allAttributes);
        assertTrue(securityAttributes.getNetworkRestrictionsAttribute() == null);
        // signed legacy MIDlet, empty value -> failure
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME , new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(NetworkRestrictionsAttribute.ATTRIBUTE_NAME, new Attribute("",""));
            securityAttributes.addManifestAttributes(allAttributes);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_NETWORK_RESTRICTIONS_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {NetworkRestrictionsAttribute.ATTRIBUTE_NAME})));
        }
        // signed MIDP3 MIDlet, empty value -> failure
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" , new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(NetworkRestrictionsAttribute.ATTRIBUTE_NAME, new Attribute("",""));
            securityAttributes.addManifestAttributes(allAttributes);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_NETWORK_RESTRICTIONS_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {NetworkRestrictionsAttribute.ATTRIBUTE_NAME})));
        }
        // signed legacy MIDlet, no "-" separator -> failure
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME , new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(NetworkRestrictionsAttribute.ATTRIBUTE_NAME, new Attribute("","123456"));
            securityAttributes.addManifestAttributes(allAttributes);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_NETWORK_RESTRICTIONS_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {NetworkRestrictionsAttribute.ATTRIBUTE_NAME})));
        }
        // signed MIDP3 MIDlet, MCC is not formed of digits -> failure
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" , new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(NetworkRestrictionsAttribute.ATTRIBUTE_NAME, new Attribute("","a23-45"));
            securityAttributes.addManifestAttributes(allAttributes);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_NETWORK_RESTRICTIONS_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {NetworkRestrictionsAttribute.ATTRIBUTE_NAME})));
        }
        // signed legacy MIDlet, MNC is not formed of digits -> failure
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(NetworkRestrictionsAttribute.ATTRIBUTE_NAME, new Attribute("","123-4b"));
            securityAttributes.addManifestAttributes(allAttributes);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_NETWORK_RESTRICTIONS_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {NetworkRestrictionsAttribute.ATTRIBUTE_NAME})));
        }
        // signed MIDP3 MIDlet, MNC is  made of 4 digits -> failure
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" , new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(NetworkRestrictionsAttribute.ATTRIBUTE_NAME, new Attribute("","123-4567"));
            securityAttributes.addManifestAttributes(allAttributes);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_NETWORK_RESTRICTIONS_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {NetworkRestrictionsAttribute.ATTRIBUTE_NAME})));
        }
        // signed legacy MIDlet, 2 pairs: one is valid the other one is invalid -> failure
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(NetworkRestrictionsAttribute.ATTRIBUTE_NAME, new Attribute("","123-456 12e-45"));
            securityAttributes.addManifestAttributes(allAttributes);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_NETWORK_RESTRICTIONS_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {NetworkRestrictionsAttribute.ATTRIBUTE_NAME})));
        }
        // invalid pair defined in JAD -> failure
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
            allAttributes.put(NetworkRestrictionsAttribute.ATTRIBUTE_NAME, new Attribute("","123-456 12e-45"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(INVALID_NETWORK_RESTRICTIONS_ATTRIBUTE_VALUE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.INVALID_JAR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {NetworkRestrictionsAttribute.ATTRIBUTE_NAME})));
        }
        // valid pair defined in JAD only -> failure
        try
        {
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
            allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
            allAttributes.put(NetworkRestrictionsAttribute.ATTRIBUTE_NAME, new Attribute("","123-456 111-00"));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
            securityAttributes.addManifestAttributes(allAttributes);
        }
        catch (InvalidAttributeException e)
        {
            assertTrue(JAD_ONLY_PERMISSION + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.ATTRIBUTE_MISMATCH
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_CORRUPT_PKG, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_MISMATCH,
                                                        new String[] {NetworkRestrictionsAttribute.ATTRIBUTE_NAME})));
        }
        // signed MIDP3 MIDlet, 2 valid pairs defined in JAR only
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" , new Attribute("","signature"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(NetworkRestrictionsAttribute.ATTRIBUTE_NAME, new Attribute("","123-46 789-014"));
        securityAttributes.addManifestAttributes(allAttributes);
        assertTrue(securityAttributes.getNetworkRestrictionsAttribute() != null
                   && securityAttributes.getNetworkRestrictionsAttribute().getRestrictions() != null
                   && securityAttributes.getNetworkRestrictionsAttribute().getRestrictions().length == 2
                   && securityAttributes.getNetworkRestrictionsAttribute().getRestrictions()[0].getMCC().equals("123")
                   && securityAttributes.getNetworkRestrictionsAttribute().getRestrictions()[0].getMNC().equals("46")
                   && securityAttributes.getNetworkRestrictionsAttribute().getRestrictions()[1].getMCC().equals("789")
                   && securityAttributes.getNetworkRestrictionsAttribute().getRestrictions()[1].getMNC().equals("014"));
        // signed legacy MIDlet, 2 valid pairs defined in JAD and JAR
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert"));
        allAttributes.put(AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME, new Attribute("","signature"));
        allAttributes.put(NetworkRestrictionsAttribute.ATTRIBUTE_NAME, new Attribute("","123-46 789-014"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(NetworkRestrictionsAttribute.ATTRIBUTE_NAME, new Attribute("","123-46 789-014"));
        securityAttributes.addManifestAttributes(allAttributes);
    }
}
