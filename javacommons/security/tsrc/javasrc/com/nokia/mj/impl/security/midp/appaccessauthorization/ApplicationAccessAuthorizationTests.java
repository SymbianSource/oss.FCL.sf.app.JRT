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


package com.nokia.mj.impl.security.midp.appaccessauthorization;

import java.util.Hashtable;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.security.midp.appaccessauthorization.ApplicationAccessAuthorization;
import com.nokia.mj.impl.security.midp.common.AuthorizationRestriction;
import com.nokia.mj.impl.security.midp.common.AuthorizationRestrictionAttribute;
import com.nokia.mj.impl.security.midp.common.AuthenticationAttribute;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.storage.StorageSession;
import com.nokia.mj.impl.storage.StorageFactory;
import com.nokia.mj.impl.storage.StorageNames;
import com.nokia.mj.impl.storage.StorageAttribute;
import com.nokia.mj.impl.storage.StorageEntry;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Attribute;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * ApplicationAccessAuthorization unit tests.
 */
public class ApplicationAccessAuthorizationTests extends TestCase implements InstallerMain
{

    // class to be tested
    ApplicationAccessAuthorization aaa;
    StorageSession session = StorageFactory.createSession();

    // references to error messages
    protected InstallerErrorMessage errorMessage = new InstallerErrorMessage();
    protected InstallerDetailedErrorMessage detailedErrorMessage = new InstallerDetailedErrorMessage();

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new ApplicationAccessAuthorizationTests("testAAA", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ApplicationAccessAuthorizationTests)tc).testAAA();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public ApplicationAccessAuthorizationTests()
    {
    }

    public ApplicationAccessAuthorizationTests(String aTestName, TestMethod aTestMethod)
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
        session.open();
    }

    protected void tearDown()
    {
        session.close();
    }

    private void testAAA()
    {
        boolean assertCondition = false;
        // check null values
        Uid A = Uid.createUid("A");
        Uid B = Uid.createUid("B");
        assertTrue(aaa.checkAccess(null, null) == aaa.ACCESS_DENIED
                   && aaa.checkAccess(A, null) == aaa.ACCESS_ALLOWED
                   && aaa.checkAccess(null, B) == aaa.ACCESS_DENIED);
        // check values without restrictions/credentials
        assertTrue(aaa.checkAccess(A, A) == aaa.ACCESS_ALLOWED
                   && aaa.checkAccess(A, B) == aaa.ACCESS_ALLOWED);
        // B has no restrictions
        writeCredentials(A, "vendor", "domain", "1,2", new String[] {"1-1", "2-1"}, new String[] {"cert1", "cert2"});
        assertCondition = (aaa.checkAccess(A, B) == aaa.ACCESS_ALLOWED);
        removeCredentials(A);
        assertTrue(assertCondition);
        // A has no credentials, B has tight signer restrictions
        writeRestrictions(B, new String[] {"1"}, new String[] {"domain=ANY;vendor=ANY;signer=alias"}, new String[] {"alias cert"});
        assertCondition = (aaa.checkAccess(A, B) == aaa.ACCESS_DENIED);
        removeRestrictions(B);
        assertTrue(assertCondition);
        // A has no credentials, B has tight vendor restrictions
        writeRestrictions(B, new String[] {"1"}, new String[] {"domain=ANY;vendor=vendor;signer=ANY"});
        assertCondition = (aaa.checkAccess(A, B) == aaa.ACCESS_DENIED);
        removeRestrictions(B);
        assertTrue(assertCondition);
        // A has no credentials, B has tight domain restrictions
        writeRestrictions(B, new String[] {"1"}, new String[] {"domain=SELF;vendor=ANY;signer=ANY"});
        assertCondition = (aaa.checkAccess(A, B) == aaa.ACCESS_DENIED);
        removeRestrictions(B);
        assertTrue(assertCondition);
        // A has no credentials, B has loose restrictions
        writeRestrictions(B, new String[] {"1"}, new String[] {"domain=ANY;vendor=ANY;signer=ANY"});
        assertCondition = (aaa.checkAccess(A, B) == aaa.ACCESS_ALLOWED);
        removeRestrictions(B);
        assertTrue(assertCondition);
        // A has credentials, B has restrictions, A's domain doesn't match
        writeCredentials(A, "vendorA", "domainA", "1", new String[] {"1-1"}, new String[] {"cert"});
        writeGeneralInfo(B, "vendorB", "domainB");
        writeRestrictions(B, new String[] {"1"}, new String[] {"domain=SELF;vendor=ANY;signer=ANY"});
        assertCondition = (aaa.checkAccess(A, B) == aaa.ACCESS_DENIED);
        removeCredentials(A);
        removeGeneralInfo(B);
        removeRestrictions(B);
        assertTrue(assertCondition);
        // A has credentials, B has restrictions, A's vendor doesn't match
        writeCredentials(A, "vendorA", "domainA", "1", new String[] {"1-1"}, new String[] {"cert"});
        writeRestrictions(B, new String[] {"1"}, new String[] {"domain=SELF;vendor=vendorB;signer=ANY"});
        assertCondition = (aaa.checkAccess(A, B) == aaa.ACCESS_DENIED);
        removeCredentials(A);
        removeRestrictions(B);
        assertTrue(assertCondition);
        // A has credentials, B has restrictions, A's signer doesn't match
        writeCredentials(A, "vendorA", "domainA", "1", new String[] {"1-1"}, new String[] {"certA"});
        writeRestrictions(B, new String[] {"1"}, new String[] {"domain=ANY;vendor=ANY;signer=alias"}, new String[] {"alias certB"});
        assertCondition = (aaa.checkAccess(A, B) == aaa.ACCESS_DENIED);
        removeCredentials(A);
        removeRestrictions(B);
        assertTrue(assertCondition);
        // A has credentials, B has restrictions, A's domain matches
        writeCredentials(A, "vendorA", "domainA", "1", new String[] {"1-1"}, new String[] {"cert"});
        writeGeneralInfo(B, "vendorB", "domainA");
        writeRestrictions(B, new String[] {"1"}, new String[] {"domain=SELF;vendor=ANY;signer=ANY"});
        assertCondition = (aaa.checkAccess(A, B) == aaa.ACCESS_ALLOWED);
        removeCredentials(A);
        removeGeneralInfo(B);
        removeRestrictions(B);
        assertTrue(assertCondition);
        // A has credentials, B has restrictions, A's vendor matches
        writeCredentials(A, "vendorA", "domainA", "1", new String[] {"1-1"}, new String[] {"cert"});
        writeGeneralInfo(B, "vendorA", "domainA");
        writeRestrictions(B, new String[] {"1"}, new String[] {"domain=SELF;vendor=vendorA;signer=ANY"});
        assertCondition = (aaa.checkAccess(A, B) == aaa.ACCESS_ALLOWED);
        removeCredentials(A);
        removeGeneralInfo(B);
        removeRestrictions(B);
        assertTrue(assertCondition);
        // A has credentials, B has restrictions, A's signer matches
        writeCredentials(A, "vendorA", "domainA", "1", new String[] {"1-1"}, new String[] {"certA"});
        writeRestrictions(B, new String[] {"1"}, new String[] {"domain=ANY;vendor=ANY;signer=alias"}, new String[] {"alias certA"});
        assertCondition = (aaa.checkAccess(A, B) == aaa.ACCESS_ALLOWED);
        removeCredentials(A);
        removeRestrictions(B);
        assertTrue(assertCondition);
        // A has credentials, B has loose restrictions (ANY)
        writeCredentials(A, "vendorA", "domainA", "1", new String[] {"1-1"}, new String[] {"certA"});
        writeRestrictions(B, new String[] {"1"}, new String[] {"domain=ANY;vendor=ANY;signer=ANY"});
        assertCondition = (aaa.checkAccess(A, B) == aaa.ACCESS_ALLOWED);
        removeCredentials(A);
        removeRestrictions(B);
        assertTrue(assertCondition);
    }

    private void writeCredentials(Uid uid, String vendor, String domain, String chainIndexes, String[] certSuffixes, String[] certValues)
    {
        StorageEntry entry = new StorageEntry();
        StorageAttribute uidAttr = new StorageAttribute(
            StorageNames.ID,
            uid.getStringValue());
        entry.addAttribute(uidAttr);
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VENDOR,
                               vendor));
        session.write(StorageNames.APPLICATION_PACKAGE_TABLE, entry);
        entry = new StorageEntry();
        entry.addAttribute(uidAttr);
        entry.addAttribute(new StorageAttribute(
                               StorageAttribute.SECURITY_DOMAIN,
                               domain));
        entry.addAttribute(new StorageAttribute(
                               StorageAttribute.VALID_CERTS,
                               chainIndexes));
        session.write(StorageNames.MIDP_PACKAGE_TABLE, entry);
        for (int i=0; i<certSuffixes.length; i++)
        {
            entry = new StorageEntry();
            entry.addAttribute(uidAttr);
            entry.addAttribute(new StorageAttribute(
                                   StorageNames.NAME,
                                   AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + certSuffixes[i]));
            entry.addAttribute(new StorageAttribute(
                                   StorageNames.VALUE,
                                   certValues[i]));
            session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        }
    }

    private void writeGeneralInfo(Uid uid, String vendor, String domain)
    {
        StorageEntry entry = new StorageEntry();
        StorageAttribute uidAttr = new StorageAttribute(
            StorageNames.ID,
            uid.getStringValue());
        entry.addAttribute(uidAttr);
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VENDOR,
                               vendor));
        session.write(StorageNames.APPLICATION_PACKAGE_TABLE, entry);
        if (domain != null)
        {
            entry = new StorageEntry();
            entry.addAttribute(uidAttr);
            entry.addAttribute(new StorageAttribute(
                                   StorageAttribute.SECURITY_DOMAIN,
                                   domain));
            session.write(StorageNames.MIDP_PACKAGE_TABLE, entry);
        }
    }

    private void writeRestrictions(Uid uid, String[] nameSuffixes, String[] values)
    {
        writeRestrictions(uid, nameSuffixes, values, null);
    }

    private void writeRestrictions(Uid uid, String[] nameSuffixes, String[] values, String[] values2)
    {
        StorageAttribute uidAttr = new StorageAttribute(
            StorageNames.ID,
            uid.getStringValue());
        StorageEntry entry;
        for (int i=0; i<nameSuffixes.length; i++)
        {
            entry = new StorageEntry();
            entry.addAttribute(uidAttr);
            entry.addAttribute(new StorageAttribute(
                                   StorageNames.NAME,
                                   AuthorizationRestrictionAttribute.MAIN_ATTRIBUTE_PREFIX  + nameSuffixes[i]));
            entry.addAttribute(new StorageAttribute(
                                   StorageNames.VALUE,
                                   values[i]));
            session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
            if (values2 != null && values2[i] != null)
            {
                entry = new StorageEntry();
                entry.addAttribute(uidAttr);
                entry.addAttribute(new StorageAttribute(
                                       StorageNames.NAME,
                                       AuthorizationRestrictionAttribute.SECOND_ATTRIBUTE_PREFIX  + nameSuffixes[i]));
                entry.addAttribute(new StorageAttribute(
                                       StorageNames.VALUE,
                                       values2[i]));
                session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
            }
        }
    }

    private void removeCredentials(Uid uid)
    {
        StorageEntry entry = new StorageEntry();
        StorageAttribute uidAttr = new StorageAttribute(
            StorageNames.ID,
            uid.getStringValue());
        entry.addAttribute(uidAttr);
        session.remove(StorageNames.APPLICATION_PACKAGE_TABLE, entry);
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, entry);
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
    }

    private void removeGeneralInfo(Uid uid)
    {
        StorageEntry entry = new StorageEntry();
        StorageAttribute uidAttr = new StorageAttribute(
            StorageNames.ID,
            uid.getStringValue());
        entry.addAttribute(uidAttr);
        session.remove(StorageNames.APPLICATION_PACKAGE_TABLE, entry);
        session.remove(StorageNames.MIDP_PACKAGE_TABLE, entry);
    }

    private void removeRestrictions(Uid uid)
    {
        StorageEntry entry = new StorageEntry();
        StorageAttribute uidAttr = new StorageAttribute(
            StorageNames.ID,
            uid.getStringValue());
        entry.addAttribute(uidAttr);
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
    }
}
