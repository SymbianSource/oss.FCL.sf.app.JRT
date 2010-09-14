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


package com.nokia.mj.impl.security.midp.authentication;

import java.util.Hashtable;
import java.util.Vector;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.security.midp.authentication.AuthenticationModule;
import com.nokia.mj.impl.security.midp.authentication.OcspEventListener;
import com.nokia.mj.impl.security.midp.authentication.OcspUserPreferences;
import com.nokia.mj.impl.security.common.InstallerSecurityException;
import com.nokia.mj.impl.security.common.RuntimeSecurityException;
import com.nokia.mj.impl.security.common.Certificate;
import com.nokia.mj.impl.security.midp.common.SigningCertificate;
import com.nokia.mj.impl.security.midp.common.SigningInfo;
import com.nokia.mj.impl.security.midp.common.SecurityAttributes;
import com.nokia.mj.impl.security.midp.common.AuthenticationCredentials;
import com.nokia.mj.impl.security.midp.common.AuthenticationAttribute;
import com.nokia.mj.impl.security.midp.common.ProtectionDomain;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.storage.*;
import com.nokia.mj.impl.security.midp.storage.SecurityStorage;
import com.nokia.mj.impl.security.midp.storage.AuthenticationStorageData;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.security.utils.SecurityErrorMessage;
import com.nokia.mj.impl.security.utils.SecurityDetailedErrorMessage;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * AuthenticationModule unit tests.
 */
public class AuthenticationModuleTests extends TestCase implements InstallerMain
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
    StorageSession session;
    SecurityStorage storage;
    Uid appUID;

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
    protected SecurityErrorMessage securityErrorMessage = new SecurityErrorMessage();
    protected SecurityDetailedErrorMessage securityDetailedErrorMessage = new SecurityDetailedErrorMessage();

    protected String INVALID_CERTIFICATE_MSG = "Certificate with invalid value threw InstallerSecurityException with following status code and message: ";
    protected String INVALID_CERTIFICATE_SIGNATURE_MSG = "Certificate with invalid signature threw InstallerSecurityException with following status code and message: ";

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new AuthenticationModuleTests("testAuthentication", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AuthenticationModuleTests)tc).testAuthentication();
            }
        }));
        suite.addTest(new AuthenticationModuleTests("testVerifyAuthenticity", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AuthenticationModuleTests)tc).testVerifyAuthenticity();
            }
        }));
        suite.addTest(new AuthenticationModuleTests("testOcsp", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AuthenticationModuleTests)tc).testOcsp();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public AuthenticationModuleTests()
    {
    }

    public AuthenticationModuleTests(String aTestName, TestMethod aTestMethod)
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
        session = StorageFactory.createSession();
        storage = new SecurityStorage();
        session.open();
        appUID = Uid.createUid("myUID");
    }

    protected void tearDown()
    {
        session.remove(StorageNames.APPLICATION_PACKAGE_TABLE, appUID);
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        authenticationModule.removeSecurityData(session, appUID);
    }

    private void testAuthentication()
    {
        // prepare the storage data
        AuthenticationCredentials[] credentials;
        SigningCertificate[] details;
        String domainCategory;
        StorageEntry midpEntry;
        StorageEntry appPackageEntry;
        allAttributes = new Hashtable();
        SecurityAttributes securityAttributes;
        // 1. get protection domain category
        authenticationModule.removeSecurityData(session, appUID);
        assertTrue(authenticationModule.getProtectionDomainCategory(session, appUID) == null
                   && authenticationModule.getCertificatesDetails(session, appUID) == null);
        authenticationModule.removeSecurityData(session, appUID);
        // 2. unsigned, install from JAR which does not exist
        authenticationModule.removeSecurityData(session, appUID);
        try
        {
            authenticationModule.authenticateJar(appUID,null,"somethingWhichDoesNotExist", false);
            authenticationModule.addSecurityData(session, appUID, null);
            assertTrue(false);
        }
        catch (InstallerSecurityException e)
        {
            assertTrue("",
                       e.getOtaStatusCode() == OtaStatusCode.INTERNAL_ERROR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_UNEXPECTED_ERR, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.INTERNAL_ERROR,
                                                        new String[] {"Could not compute hash for somethingWhichDoesNotExist"})));
        }
        authenticationModule.removeSecurityData(session, appUID);
        // 3. unsigned, install from JAR
        authenticationModule.removeSecurityData(session, appUID);
        authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp"  + System.getProperty("file.separator") + "app.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        assertTrue(authenticationModule.getProtectionDomainCategory(session, appUID).equals(ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN)
                   && authenticationModule.getCertificatesDetails(session, appUID) == null);
        authenticationModule.removeSecurityData(session, appUID);
        // 3.1. unsigned, install from JAR, no session provided to addSecurityData -> no authentication data stored to storage
        authenticationModule.removeSecurityData(session, appUID);
        authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp"  + System.getProperty("file.separator") + "app.jar", false);
        authenticationModule.addSecurityData(null, appUID, null);
        StorageEntry query = new StorageEntry();
        query.addAttribute(new StorageAttribute(
                               StorageAttribute.ID,
                               appUID.getStringValue()));
        query.addAttribute(new StorageAttribute(
                               StorageNames.SECURITY_DOMAIN,
                               ""));
        query.addAttribute(new StorageAttribute(
                               StorageNames.SECURITY_DOMAIN_CATEGORY,
                               ""));
        query.addAttribute(new StorageAttribute(
                               StorageNames.HASH,
                               ""));
        query.addAttribute(new StorageAttribute(
                               StorageNames.CERT_HASH,
                               ""));
        query.addAttribute(new StorageAttribute(
                               StorageNames.VALID_CERTS,
                               ""));
        StorageEntry[] entries =         session.search(StorageNames.MIDP_PACKAGE_TABLE, query);
        assertTrue(entries == null || entries.length == 0);
        authenticationModule.removeSecurityData(session, appUID);
        // 4. unsigned, install from JAD
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP2));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        assertTrue(credentials != null && credentials.length == 1 && credentials[0].getProtectionDomainName().equals("UnidentifiedThirdParty")
                   && authenticationModule.getCertificatesDetails(session, appUID) == null);
        authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp"  +  System.getProperty("file.separator") + "app.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        assertTrue(authenticationModule.getProtectionDomainCategory(session, appUID).equals(ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN));
        authenticationModule.removeSecurityData(session, appUID);
        // 5. upgrade of authentication storage data
        authenticationModule.removeSecurityData(session, appUID);
        storage.writeAuthenticationStorageData(appUID, new AuthenticationStorageData("UnidentifiedThirdParty", ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN, "jarHash", "rootHash", new Vector()));
        authenticationModule.authenticateJar(appUID,appUID,TEST_DATA_DIR + "security_tmp" + System.getProperty("file.separator") + "app.jar", false);
        authenticationModule.addSecurityData(session, appUID, appUID);
        assertTrue(authenticationModule.getProtectionDomainCategory(session, appUID).equals(ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN)
                   && authenticationModule.getCertificatesDetails(session, appUID) == null);
        authenticationModule.removeSecurityData(session, appUID);
        // 6. cert validation ok (2 certs succesfully validated)
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICyjCCAjOgAwIBAgIJAKoRe0pf7UvDMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyMDI0NVoXDTE1MDIwNDEyMDI0NVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKoYd+BJxNOD8+bGcxCFfJvDr95SJzVGDwrIcANqnO5pq+nI0FWbYTNsTNzl0umP3DjQ+jAboZG+S0/hPh1dYs8aAxm7ORzwHOM70cIboYrrZzOB/Kw+BXY3HuFqrtO25oke/MfKbspVNjR4QErZafsa0IVsX2bKT9cgcUUMykQwht0JjsQjn7CMKb8dE/RjATIXl7a+ng0jBnDIm+rUK/+YCimmaIJT3xb+PAmFXFwU+tmGqFeNlIIi0EUIot7q8JJeSJWMCftrlcHgb3J+jFfyeHt/YcfIN0rxXtlDHFVQXvskPq4I7O+NSypfJBiR8dfxffYlPliNKJ2vG2iurZsCAwEAATANBgkqhkiG9w0BAQUFAAOBgQBpGzuZaf2N/f3c6auGkYzcYY34VB+4C9kFJA2dQBsNYnlfIeSF30o9uq0VAkFI5uzk4aYiDB0dN5lsZ/bv/jnmrElDaXEBRBn6KtkIsJRgIcKRyXLOfWFcskcChxTVknPV8hxLYyjmi4S+O2Km9cOHEy4bib+XlnEsoe5iiTaHKA=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" , new Attribute("",
                          "kwxpKeSSWSeSW5ieWCx9ARFD6Lrwabxk5t7Aas5FFVEtr89vslY1QIexKNiytQm51PTIRnCNhl6y+tcv7V7GRskV1VMlTHwbCVjK4rqoFFrcZrO3qHvkRh6K9wnHnw5PPtBsrMbg8lWKBrGaf4TYRYFW+CukmR+06R7u5zAZaL7CMhl69bCXd78Tz1Z4N5CxVFD8vGatp66TMBk+0YhZP7Lo1yIDu2XsojVtigAoV9sm8OF5/aXmgfFb2AUmANGxlaDAieUfOAm7RXvsOZgaPN72KEqya8XQW6r1/aGF6jMmJ62qgPqJV3TtJtbFSwH1suywAk1UDltVim4b/Ucw1A=="));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-1", new Attribute("", "MIICWDCCAcECBEhQwA0wDQYJKoZIhvcNAQEEBQAwczELMAkGA1UEBhMCZmkxEjAQBgNVBAgTCVBpcmthbm1hYTEQMA4GA1UEBxMHVGFtcGVyZTEOMAwGA1UEChMFTm9raWExDTALBgNVBAsTBEphdmExHzAdBgNVBAMMFkpQX0RldmljZV9NYW51ZmFjdHVyZXIwHhcNMDgwNjEyMDYxOTU3WhcNMTgwNjEwMDYxOTU3WjBzMQswCQYDVQQGEwJmaTESMBAGA1UECBMJUGlya2FubWFhMRAwDgYDVQQHEwdUYW1wZXJlMQ4wDAYDVQQKEwVOb2tpYTENMAsGA1UECxMESmF2YTEfMB0GA1UEAwwWSlBfRGV2aWNlX01hbnVmYWN0dXJlcjCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEApi3ipIPj8O33/rZre1euh7Itd9d6ZVf2mvQ/tLpHEsFJe5XLOsVViMvFskhhKUzmDsRbP4J03L6827/vEDIi/1i8kJGLaoFqJYaLGFWI2Zmxlj6aJV8cfZyOjmQPWJn1IDEe1ZAWbvMSp8xibWRsCLNEGKIkxQvAr/QDK/6iS+kCAwEAATANBgkqhkiG9w0BAQQFAAOBgQCDXt6klAs6yKvdTab3D2wkdoi3Lu4YFsMgLexJOu5HhAUJ/9VYO+Q2+mjd95MRsTa5rWQ2Jjvhn57Z3z/KBOwfHbymmNtMk6Gl14H0vQRoHa31jh3mTuAy5KotDVthaDp30aOizk473NU68wY1WdP4gFk5ZhrpNea9q3st13BxIQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "2" ,new Attribute("", "IcANmLKiOJQF8ABCNDj1PNNH/O8v9jfCVuiGBVm8enXDkM/gLwPjrC65sDKpOCHPqssUlHzjmVN5b9g8aRs4jxUOXNt2b732J7NSIPh97vw/WrP/KHdiooi/1KFUyklMyokK9ZrIv+GW1ttLCfKbuFupT9zmPAmWJQpnuD7J6sE="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        credentials = authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        authenticationModule.removeSecurityData(session, appUID);
        assertTrue(credentials != null && credentials.length == 2);
        // 7. cert validation ok (2 certs succesfully validated, but the signature of one failed) and JAR ok
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICyjCCAjOgAwIBAgIJAKoRe0pf7UvDMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyMDI0NVoXDTE1MDIwNDEyMDI0NVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKoYd+BJxNOD8+bGcxCFfJvDr95SJzVGDwrIcANqnO5pq+nI0FWbYTNsTNzl0umP3DjQ+jAboZG+S0/hPh1dYs8aAxm7ORzwHOM70cIboYrrZzOB/Kw+BXY3HuFqrtO25oke/MfKbspVNjR4QErZafsa0IVsX2bKT9cgcUUMykQwht0JjsQjn7CMKb8dE/RjATIXl7a+ng0jBnDIm+rUK/+YCimmaIJT3xb+PAmFXFwU+tmGqFeNlIIi0EUIot7q8JJeSJWMCftrlcHgb3J+jFfyeHt/YcfIN0rxXtlDHFVQXvskPq4I7O+NSypfJBiR8dfxffYlPliNKJ2vG2iurZsCAwEAATANBgkqhkiG9w0BAQUFAAOBgQBpGzuZaf2N/f3c6auGkYzcYY34VB+4C9kFJA2dQBsNYnlfIeSF30o9uq0VAkFI5uzk4aYiDB0dN5lsZ/bv/jnmrElDaXEBRBn6KtkIsJRgIcKRyXLOfWFcskcChxTVknPV8hxLYyjmi4S+O2Km9cOHEy4bib+XlnEsoe5iiTaHKA=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "Khn165ePip/dPyTN3p61MnjjzPsA1nnGLFhPbQ5Dv7YYVRPz5WrrHWlvIOyDA7MadOURwIO3xB8tfBm7cFtRIRKWB7cDj4a1MbrySDb/g1iCWnOyOYFGbpxj1d2QjAtlNlEt+0IAQlqLrQC5a0aFBoJHj4bvkfzcuRFxpbnQ8wY="));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-1", new Attribute("",
                          "MIICyjCCAjOgAwIBAgIJAKoRe0pf7UvDMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyMDI0NVoXDTE1MDIwNDEyMDI0NVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKoYd+BJxNOD8+bGcxCFfJvDr95SJzVGDwrIcANqnO5pq+nI0FWbYTNsTNzl0umP3DjQ+jAboZG+S0/hPh1dYs8aAxm7ORzwHOM70cIboYrrZzOB/Kw+BXY3HuFqrtO25oke/MfKbspVNjR4QErZafsa0IVsX2bKT9cgcUUMykQwht0JjsQjn7CMKb8dE/RjATIXl7a+ng0jBnDIm+rUK/+YCimmaIJT3xb+PAmFXFwU+tmGqFeNlIIi0EUIot7q8JJeSJWMCftrlcHgb3J+jFfyeHt/YcfIN0rxXtlDHFVQXvskPq4I7O+NSypfJBiR8dfxffYlPliNKJ2vG2iurZsCAwEAATANBgkqhkiG9w0BAQUFAAOBgQBpGzuZaf2N/f3c6auGkYzcYY34VB+4C9kFJA2dQBsNYnlfIeSF30o9uq0VAkFI5uzk4aYiDB0dN5lsZ/bv/jnmrElDaXEBRBn6KtkIsJRgIcKRyXLOfWFcskcChxTVknPV8hxLYyjmi4S+O2Km9cOHEy4bib+XlnEsoe5iiTaHKA=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "2" , new Attribute("",
                          "kwxpKeSSWSeSW5ieWCx9ARFD6Lrwabxk5t7Aas5FFVEtr89vslY1QIexKNiytQm51PTIRnCNhl6y+tcv7V7GRskV1VMlTHwbCVjK4rqoFFrcZrO3qHvkRh6K9wnHnw5PPtBsrMbg8lWKBrGaf4TYRYFW+CukmR+06R7u5zAZaL7CMhl69bCXd78Tz1Z4N5CxVFD8vGatp66TMBk+0YhZP7Lo1yIDu2XsojVtigAoV9sm8OF5/aXmgfFb2AUmANGxlaDAieUfOAm7RXvsOZgaPN72KEqya8XQW6r1/aGF6jMmJ62qgPqJV3TtJtbFSwH1suywAk1UDltVim4b/Ucw1A=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        details = (SigningCertificate[])authenticationModule.getCertificatesDetails(session, appUID);
        authenticationModule.authenticateJar(appUID, null,TEST_DATA_DIR + "security_tmp" + System.getProperty("file.separator") + "app.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        domainCategory = authenticationModule.getProtectionDomainCategory(session, appUID);
        // this empties the cache as well
        authenticationModule.removeSecurityData(session, appUID);
        appPackageEntry = new StorageEntry();
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageAttribute.ID,
                                         appUID.getStringValue()));
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
        assertTrue((domainCategory.equals(ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN)
                    || domainCategory.equals(ApplicationInfo.MANUFACTURER_DOMAIN)
                    || domainCategory.equals(ApplicationInfo.OPERATOR_DOMAIN)
                    || domainCategory.equals(ApplicationInfo.IDENTIFIED_THIRD_PARTY_DOMAIN)));
        // put manually the DB data back, so that we can test the certificates details
        appPackageEntry = new StorageEntry();
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageAttribute.ID,
                                         appUID.getStringValue()));
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageNames.NAME,
                                         "MIDlet-Certificate-1-1"));
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageNames.VALUE,
                                         "MIICyjCCAjOgAwIBAgIJAKoRe0pf7UvDMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyMDI0NVoXDTE1MDIwNDEyMDI0NVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKoYd+BJxNOD8+bGcxCFfJvDr95SJzVGDwrIcANqnO5pq+nI0FWbYTNsTNzl0umP3DjQ+jAboZG+S0/hPh1dYs8aAxm7ORzwHOM70cIboYrrZzOB/Kw+BXY3HuFqrtO25oke/MfKbspVNjR4QErZafsa0IVsX2bKT9cgcUUMykQwht0JjsQjn7CMKb8dE/RjATIXl7a+ng0jBnDIm+rUK/+YCimmaIJT3xb+PAmFXFwU+tmGqFeNlIIi0EUIot7q8JJeSJWMCftrlcHgb3J+jFfyeHt/YcfIN0rxXtlDHFVQXvskPq4I7O+NSypfJBiR8dfxffYlPliNKJ2vG2iurZsCAwEAATANBgkqhkiG9w0BAQUFAAOBgQBpGzuZaf2N/f3c6auGkYzcYY34VB+4C9kFJA2dQBsNYnlfIeSF30o9uq0VAkFI5uzk4aYiDB0dN5lsZ/bv/jnmrElDaXEBRBn6KtkIsJRgIcKRyXLOfWFcskcChxTVknPV8hxLYyjmi4S+O2Km9cOHEy4bib+XlnEsoe5iiTaHKA=="));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
        midpEntry = new StorageEntry();
        midpEntry.addAttribute(new StorageAttribute(
                                   StorageAttribute.ID,
                                   appUID.getStringValue()));
        midpEntry.addAttribute(new StorageAttribute(
                                   StorageAttribute.VALID_CERTS,
                                   "1"));
        session.write(StorageNames.MIDP_PACKAGE_TABLE, midpEntry);
        details = (SigningCertificate[])authenticationModule.getCertificatesDetails(session, appUID);
        appPackageEntry = new StorageEntry();
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageAttribute.ID,
                                         appUID.getStringValue()));
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
        authenticationModule.removeSecurityData(session, appUID);
        assertTrue(details != null);
        // play with invalid values for certificate -> see it does not return anything
        // delete from StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE
        midpEntry = new StorageEntry();
        midpEntry.addAttribute(new StorageAttribute(
                                   StorageAttribute.ID,
                                   appUID.getStringValue()));
        midpEntry.addAttribute(new StorageAttribute(
                                   StorageAttribute.VALID_CERTS,
                                   "1"));
        session.write(StorageNames.MIDP_PACKAGE_TABLE, midpEntry);
        appPackageEntry = new StorageEntry();
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageAttribute.ID,
                                         appUID.getStringValue()));
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageNames.NAME,
                                         "MIDlet-Certificate-1-1"));
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageNames.VALUE,
                                         "INVALID"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
        details = (SigningCertificate[])authenticationModule.getCertificatesDetails(session, appUID);
        authenticationModule.removeSecurityData(session, appUID);
        appPackageEntry = new StorageEntry();
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageAttribute.ID,
                                         appUID.getStringValue()));
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
        assertTrue(details == null);
        // 8. cert validation ok (2 certs succesfully validated, but the signature of one failed) and JAR nok
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICyjCCAjOgAwIBAgIJAKoRe0pf7UvDMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyMDI0NVoXDTE1MDIwNDEyMDI0NVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKoYd+BJxNOD8+bGcxCFfJvDr95SJzVGDwrIcANqnO5pq+nI0FWbYTNsTNzl0umP3DjQ+jAboZG+S0/hPh1dYs8aAxm7ORzwHOM70cIboYrrZzOB/Kw+BXY3HuFqrtO25oke/MfKbspVNjR4QErZafsa0IVsX2bKT9cgcUUMykQwht0JjsQjn7CMKb8dE/RjATIXl7a+ng0jBnDIm+rUK/+YCimmaIJT3xb+PAmFXFwU+tmGqFeNlIIi0EUIot7q8JJeSJWMCftrlcHgb3J+jFfyeHt/YcfIN0rxXtlDHFVQXvskPq4I7O+NSypfJBiR8dfxffYlPliNKJ2vG2iurZsCAwEAATANBgkqhkiG9w0BAQUFAAOBgQBpGzuZaf2N/f3c6auGkYzcYY34VB+4C9kFJA2dQBsNYnlfIeSF30o9uq0VAkFI5uzk4aYiDB0dN5lsZ/bv/jnmrElDaXEBRBn6KtkIsJRgIcKRyXLOfWFcskcChxTVknPV8hxLYyjmi4S+O2Km9cOHEy4bib+XlnEsoe5iiTaHKA=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "Khn165ePip/dPyTN3p61MnjjzPsA1nnGLFhPbQ5Dv7YYVRPz5WrrHWlvIOyDA7MadOURwIO3xB8tfBm7cFtRIRKWB7cDj4a1MbrySDb/g1iCWnOyOYFGbpxj1d2QjAtlNlEt+0IAQlqLrQC5a0aFBoJHj4bvkfzcuRFxpbnQ8wY="));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "2-1", new Attribute("",
                          "MIICyjCCAjOgAwIBAgIJAKoRe0pf7UvDMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyMDI0NVoXDTE1MDIwNDEyMDI0NVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKoYd+BJxNOD8+bGcxCFfJvDr95SJzVGDwrIcANqnO5pq+nI0FWbYTNsTNzl0umP3DjQ+jAboZG+S0/hPh1dYs8aAxm7ORzwHOM70cIboYrrZzOB/Kw+BXY3HuFqrtO25oke/MfKbspVNjR4QErZafsa0IVsX2bKT9cgcUUMykQwht0JjsQjn7CMKb8dE/RjATIXl7a+ng0jBnDIm+rUK/+YCimmaIJT3xb+PAmFXFwU+tmGqFeNlIIi0EUIot7q8JJeSJWMCftrlcHgb3J+jFfyeHt/YcfIN0rxXtlDHFVQXvskPq4I7O+NSypfJBiR8dfxffYlPliNKJ2vG2iurZsCAwEAATANBgkqhkiG9w0BAQUFAAOBgQBpGzuZaf2N/f3c6auGkYzcYY34VB+4C9kFJA2dQBsNYnlfIeSF30o9uq0VAkFI5uzk4aYiDB0dN5lsZ/bv/jnmrElDaXEBRBn6KtkIsJRgIcKRyXLOfWFcskcChxTVknPV8hxLYyjmi4S+O2Km9cOHEy4bib+XlnEsoe5iiTaHKA=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "2" , new Attribute("",
                          "kwxpKeSSWSeSW5ieWCx9ARFD6Lrwabxk5t7Aas5FFVEtr89vslY1QIexKNiytQm51PTIRnCNhl6y+tcv7V7GRskV1VMlTHwbCVjK4rqoFFrcZrO3qHvkRh6K9wnHnw5PPtBsrMbg8lWKBrGaf4TYRYFW+CukmR+06R7u5zAZaL7CMhl69bCXd78Tz1Z4N5CxVFD8vGatp66TMBk+0YhZP7Lo1yIDu2XsojVtigAoV9sm8OF5/aXmgfFb2AUmANGxlaDAieUfOAm7RXvsOZgaPN72KEqya8XQW6r1/aGF6jMmJ62qgPqJV3TtJtbFSwH1suywAk1UDltVim4b/Ucw1A=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        try
        {
            authenticationModule.authenticateJar(appUID, null,TEST_DATA_DIR + "security_tmp" + System.getProperty("file.separator") + "modified_app.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
            assertTrue(INVALID_CERTIFICATE_SIGNATURE_MSG, false);
        }
        catch (InstallerSecurityException e)
        {
            assertTrue(INVALID_CERTIFICATE_SIGNATURE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_AUTHORIZATION_ERR, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME})));
        }
        authenticationModule.removeSecurityData(session, appUID);
        // 9. cert validation nok (none of the certs were validated ok due to cert errors)
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("","cert11"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" , new Attribute("","signature1"));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        try
        {
            authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
            assertTrue(INVALID_CERTIFICATE_MSG, false);
        }
        catch (InstallerSecurityException e)
        {
            assertTrue(INVALID_CERTIFICATE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_AUTHENTICATION_ERR, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.CERT_UNSUPPORTED,
                                                        null)));
        }
        authenticationModule.removeSecurityData(session, appUID);
        // 10. cert validation nok (none of the certs were validated ok due to signing certs being expired)
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICczCCAdygAwIBAgIJAJT/0kM9aoDeMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTA4MTIwMTExMTI0M1oXDTA4MTIwMjExMTI0M1owVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAqqHuJXLGMi7XBbM+8t4z6BMA3KTjXIe9Q6GhnqdQ6+SrUKQlty+zfuD2w+NqQDAkQjEs/flMbDk90rNl1orV11kTgUt8mqwQ8k2HWbNP3Lh+vx172E0SmGQdqVM6ElLck+KdJBPAW7KVqtBRN9NethTU8kLydpoDiUZxVUgDYW0CAwEAAaMrMCkwDgYDVR0PAQH/BAQDAgeAMBcGA1UdJQQQMA4GDCsGAQQBXgExAQICAzANBgkqhkiG9w0BAQUFAAOBgQCPZR3E5BGuQtnpo8lmftM5czJrSyXUZqm8unjSJRcV/vst9c08utSX1s3KQsA18ghaHbiP9mlHdZWvMLJQUVZ1JScl6BayG3GZXqJnZjdb3jumXAA4lOSZCDfaApRB/SCuT4qAH93wOi8PWaajFfELLlldGyFWxE0W9CDyXWFqUA=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "MRjh1zZ3rRPOD/jUx/Nhf/v3cXyt7IlurT4VEWJ4tDUmaBdZitKAznrJPwwnhRi9fWhcN6JNbjl7TUuZ+lut3Ut43yDLVjXn7uT3xsHmBu8LOjXdbP4vQ+WwxP7UjTQtp2gyXF7STf+8T514OKvWU0tnCbJaxBP8Xcow1MVJgXw="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        try
        {
            authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
            assertTrue(false);
        }
        catch (InstallerSecurityException e)
        {
            assertTrue(INVALID_CERTIFICATE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_AUTHENTICATION_ERR, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.CERT_UNSUPPORTED,
                                                        null)));
        }
        authenticationModule.removeSecurityData(session, appUID);
        // 11. cert validation nok (none of the certs were validated ok due to having the root inside the chain)
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICBzCCAXACCQCyjGn678eCCjANBgkqhkiG9w0BAQUFADBIMQswCQYDVQQGEwJGSTEQMA4GA1UEBxMHVGFtcGVyZTEnMCUGA1UEAxQeUzYwX1JuRF9DZXJ0aWZpY2F0ZV9TZXJ2aWNlX0NBMB4XDTA3MTAwMzA4NDMxOVoXDTA4MTAwMjA4NDMxOVowSDELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxJzAlBgNVBAMUHlM2MF9SbkRfQ2VydGlmaWNhdGVfU2VydmljZV9DQTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAvQ+LJvaMlX9XMkrqJoczOi54w10MiolWX6cKH6GvpDwXBhoMtvTKXQPeNqoXBmNa2/1YFnfosan83GXZP7+GOL1g5qW/i3I9ZngU/HCiB1UJSbx1hHblV3g5jZ8wUPnATC5dOUgJeEkkr9pU1GK8Z33P29DMlF0JGnJhJ/80Vq0CAwEAATANBgkqhkiG9w0BAQUFAAOBgQCNwkvEW2shElVGL065k1o45HzFq4+EbinhjGLUmFrII65JiHysjkdCHbJamVwoOgvLJxhULlQDLJ+iHJevPMfuNcFTBvsIKlleV8X2X04SLAfXi09JkcdsMOKK6BNLjEZPrzOomLV/aniZ1sW3mEqGvMVgwhWQBhN/nCzxXWbqUw=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "sT72Uw6P2uSn3euuQxJ6ybkfckFLVFR9jtpUhltMdxwJFsnoIJJaQ1eiVyalygpqxUSs3GRV/1bIi3zDmT6JwE+MGpBFA3O15QJF9o0xZIjYedy7ZKzXw4XgG74jeufiP/CjasH2ZTwOc0NiDuhAuxc78eoqH7/pClouTyEyjCk="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        try
        {
            authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
            assertTrue(false);
        }
        catch (InstallerSecurityException e)
        {
            assertTrue(INVALID_CERTIFICATE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_AUTHENTICATION_ERR, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.CERT_UNSUPPORTED,
                                                        null)));
        }
        authenticationModule.removeSecurityData(session, appUID);
        // 12. cert validation nok (none of the certs were validated ok due to not having the root)
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICJjCCAY+gAwIBAgIJAIipukKWgXc+MA0GCSqGSIb3DQEBBQUAMEgxCzAJBgNVBAYTAkZJMRAwDgYDVQQHEwdUYW1wZXJlMScwJQYDVQQDFB5TNjBfUm5EX0NlcnRpZmljYXRlX1NlcnZpY2VfQ0EwHhcNMDgxMjAxMTAzNjA3WhcNMDkxMjAxMTAzNjA3WjA1MQswCQYDVQQGEwJGSTEQMA4GA1UEBxMHVGFtcGVyZTEUMBIGA1UEAxMLSmF2YURldkNlcnQwgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBANtT1AD8t4IAUt2pyzX8FyoEzrQj9RCR3R29u8EAvlRF0azkKJ/5IO/R/fQJBaAArt5QsxEcN6eQoZ0gEiR/EwHdYG2NmDLDqdTEJOXeKP5QWpondlCqawJUurQXnScOjcxyJ8Lmva5m08IXj9xsZir3WyyMNNr7ulKecTkfAjH1AgMBAAGjKzApMA4GA1UdDwEB/wQEAwIHgDAXBgNVHSUEEDAOBgwrBgEEAV4BMQECAgMwDQYJKoZIhvcNAQEFBQADgYEAc2QsDrLJY0ZsoLLiT971696XWRdAOtysUQjxicObbiD6TVu8UKwMRO4fFTktCq7biPAhPLEA3rHPTK+FMSiOS1YIwycmxKXRavFKmfCk/GzB2duq1bum9BlWojMbHpl5XRVCZrcyIfbHJ6RyTB+2xlvc9fXF7ScSMr3eVke85n8="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "gf60j7s2VDHCiluDxOikBUeHlxK1mAdQTcu1VZTr9E0SGWY+lOpsnueg7RKW/lxQoNC6VGlmEJei7Hu7TJKKmzZBQPEJyhs+bD42dRelewDv4UWhBoza0Lgwr4GI290mvMxRP5+qf65aTaGTu+6+vraR8MpMbjc79WsmGoHX8Bc="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        try
        {
            authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
            assertTrue(false);
        }
        catch (InstallerSecurityException e)
        {
            assertTrue(INVALID_CERTIFICATE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_AUTHENTICATION_ERR, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.CERT_UNSUPPORTED,
                                                        null)));
        }
        authenticationModule.removeSecurityData(session, appUID);
        // 14. cert validation nok (signature verification fails)
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1",new Attribute("",
                          "MIICyjCCAjOgAwIBAgIJAKoRe0pf7UvDMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyMDI0NVoXDTE1MDIwNDEyMDI0NVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKoYd+BJxNOD8+bGcxCFfJvDr95SJzVGDwrIcANqnO5pq+nI0FWbYTNsTNzl0umP3DjQ+jAboZG+S0/hPh1dYs8aAxm7ORzwHOM70cIboYrrZzOB/Kw+BXY3HuFqrtO25oke/MfKbspVNjR4QErZafsa0IVsX2bKT9cgcUUMykQwht0JjsQjn7CMKb8dE/RjATIXl7a+ng0jBnDIm+rUK/+YCimmaIJT3xb+PAmFXFwU+tmGqFeNlIIi0EUIot7q8JJeSJWMCftrlcHgb3J+jFfyeHt/YcfIN0rxXtlDHFVQXvskPq4I7O+NSypfJBiR8dfxffYlPliNKJ2vG2iurZsCAwEAATANBgkqhkiG9w0BAQUFAAOBgQBpGzuZaf2N/f3c6auGkYzcYY34VB+4C9kFJA2dQBsNYnlfIeSF30o9uq0VAkFI5uzk4aYiDB0dN5lsZ/bv/jnmrElDaXEBRBn6KtkIsJRgIcKRyXLOfWFcskcChxTVknPV8hxLYyjmi4S+O2Km9cOHEy4bib+XlnEsoe5iiTaHKA=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "Khn165ePip/dPyTN3p61MnjjzPsA1nnGLFhPbQ5Dv7YYVRPz5WrrHWlvIOyDA7MadOURwIO3xB8tfBm7cFtRIRKWB7cDj4a1MbrySDb/g1iCWnOyOYFGbpxj1d2QjAtlNlEt+0IAQlqLrQC5a0aFBoJHj4bvkfzcuRFxpbnQ8wY="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        try
        {
            authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
            assertTrue(INVALID_CERTIFICATE_SIGNATURE_MSG, false);
        }
        catch (InstallerSecurityException e)
        {
            assertTrue(INVALID_CERTIFICATE_SIGNATURE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_AUTHORIZATION_ERR, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                                                        new String[] {AuthenticationAttribute.SECOND_LEGACY_ATTRIBUTE_NAME})));
        }
        authenticationModule.removeSecurityData(session, appUID);
        // 15. check parsing of signing certificate info
        // no subject
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICsTCCAhqgAwIBAgIJAKJPf47MYBEgMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyMTkzOFoXDTE1MDIwNDEyMTkzOFowPjELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxHTAbBgNVBAoTFE15IE9yZ2FuaXphdGlvbiBOYW1lMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAopEiJMbNqMH8Y6KRrLIv4cTGTABO8wu2s9MZtD4tncP5OOlRgDDcN3suLNpmict4sDOa6b4G4NQX5GZltijHaEuizHxWBNPj0xahOoz1+kOR9Rt1ktLkBw5M/G4ndUGAbRGaP0WXIqxB3s1djcMd+tlZGCrA4UL46yps9rIQVOvPhRWlzPgoekW+HovBbkbR+QPT/d0hXm5N/KrZQSRJbosPquq9QOS6A8cSkJoxvjRHw+WbHwrKPCwOnXBiRZ/CgLnS8Owb9TH4KatsPsq/hFzd7CFLgskHShwEJwoGJNPCxEo71aMKVT1szOioYvfYR5gfhbW3ap3GE0OcdT2NowIDAQABMA0GCSqGSIb3DQEBBQUAA4GBAEknv3IppfU8kKw5bm4V0XMSb54OHrlcx7P/OP4R5e54UaVLzO4/zsWeHOgmOIt7EDENbnah4rbfwJhTDA6rNR3OGI5RB0PIe2EkvXkzo62onbFx4WWXhK/pZE1mbBvhuYinRiIvGcJkS85rFVS7wH6tn5GkfXAWp/1etY7HU5MD"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "GQE13S11H6pp7KfCmKofG/4L2szDx93jp9vpAjpDuEdJkG+xZZWi2Pt0+E7JyxRrGllxdD7D2iEMBp70z++E8ehTQ0uqb3cW7lj5a0rKSoObnHycplKwkpjo9RhJOcClBoPfbvg2ezXv0yT4W0O5RSTLWySKGl+ScSvVVedXHMcq9d2tiZLFW2X0vaDWq9sWNwdBxSkiGDLqX5yUV9NkA6pAs8BeEQ8Vmg5j3ibrIywHAizAjilcBn3yzkxQVA9NR+Z/67ssztygO1x/pz+ARYq8+qmNCMBbrNkawBFi55eRgdwHFteerRzER10Vsw7M7JLLGYjVtaVy6A9LiGRdeA=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        details = authenticationModule.getCertificatesDetails(session, appUID);
        assertTrue(details != null && details.length == 1 && details[0].getFormattedSubject() != null && details[0].getFormattedSubject().equals("My Organization Name, FI, Tampere"));
        // no organization
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICqzCCAhSgAwIBAgIJALwZDU9Un3bLMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyMjgyMVoXDTE1MDIwNDEyMjgyMVowODELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA1tsrsmfM//C2kJ3TJ/dDNktYyfFseNeFqiN/z1xb/hb1vRplVXM21Ouc+y05EtBz+5gXErC7dr9k910cM0TQFe9qB78hbNNaKUcGZv1alLZRKjFlEWIexBzyLleOxb7o/wAHzdabTlthYuWmU5WF7B7FCDErBQQxEoLwL/tJV9AKLjqcBtrQ/88/PPbQieexU46HYxnpwoV01lH8PdKknnam3jV8fwHBWoh62lKNe8aXT4HVINrdL4xFjk5RT5wWKs0ceEntO4x3mXa+P9tHTJS1/NQalvg6an+zu/epSjtnp3gID0SHubgmLQtRJE3MgKiXCKvV3GSRtgS/S8vTAwIDAQABMA0GCSqGSIb3DQEBBQUAA4GBAFEo1+F90QbQ+AFwH3Z3AJaUcuuqNb4/CwtpGhvb3H58aFbhfr4SO2qNjrQVdp5I35jeMbr/QMxTs/SQlY84YvxqzGa80+nDDrK0pdki/go11FoCWQx+o2V/ksZaIubqZ4O2FCiyIaT+jat2367ksbeFeCdlig6kEtPDV7wnsSns"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "nCPnbbo0yOnckgv6wT8N9pH1id3MZvHbQzU6mFleGZiIwvc8c+wITX6NbyFVsrG4wk8OhIkp51aIvLTFMze38gwRwGuYFfqiv5NooAtmBlzDVGfBjJczklROJgrrJjwuFt/Jd1mxYqUdojU0h3JZonDlOqjTc4HCR6KOn77QiCAKgaxtDlEfdKL0vkfJLdkqB5Y3Vj15QVsLGZUDHAEET1qHlPOUzTSBFnvLQ9/20isk35PZUbxHBiA5tC3atfNZOvmcD/IvDqTy0DFgcEapV2o9FRlv4yptr19hIfsKfMmSFYPRbiKWYt8IL0f1zH6ogKdMHGk0lzLyEHy/6CnDAA=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        details = authenticationModule.getCertificatesDetails(session, appUID);
        assertTrue(details != null && details.length == 1 && details[0].getOrganization() == null);
        // all elements present (My Company Name, My Organization Name)
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICyjCCAjOgAwIBAgIJAKmyKIcZTQ3PMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTEyMzA0NloXDTE1MDIwNDEyMzA0NlowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALiEQ8ApumyMFM9L5mIRWf8OyFPwO6lFLOfJvsD4o2TFBSLSo2va02ExbseehLlMFiyOZyf1VDva8oEe/pV2vjEInxpJEfWO734TGr5fX8E0kdZ8mURNQqcl5nBqwT0xnJfatm7rPIm7yvWnJzpEn9MrkNWrxCOnjHSA26AkU+1q9rJt2eQrXOe0J5AfOI9YLhSHyTsETI6PybGHme3lFtpjBVkbraZHGXWhxLWO+E3zkkjbVZjJwHs6MYZk1bLl/6hB9F9EBjVjsYPbXtmFPBMgf1TGIugTbttHqlCNqNyb6juzoqnptE7atoIjBh5ZhA0G4WtAleCVgf/aFYzX5EECAwEAATANBgkqhkiG9w0BAQUFAAOBgQCp4KaxMok1ijm6ywMfvJbwQnbau2vS/8sEjJd1fUV20p0yZem6W492OlyRCLlu3NLoBEyV2bGZBKRWRXURALdboLARJiSLd2NGSL3rTjxWiAylCPlXESo9jv8l3QUCWVfxnyFtmNJ8t1qsPNb48u5sU80YRTJwAQ6P3wfl+5Bb5Q=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "s0wTZDsS3XSM/mb6USV3oD0vE10/L2obQD6Hn/0Vi0Z8hWAdfH2RAcvXVMJ3IhaFd/mPJ6ZOPv3r1IM+j96PajN+cj4THSdjO3JOUl+oWJnr8y3AlgEOhCxVYGrtMqjACZy/tuaCux14OdF2fKUWqTjmMRb6IJh+ub9gTRuoZ18TfE9EffpjnqWAYGujMWGBHubcV3qpPL/MWSZ1hfNq659yDSrlkdMI2tstiRrr/StwuJNdrY17qtFVftbRiBFF0AOKeJgD5jU5DpWg3MFW9jkM8SoNO9dbOM1RC/H9E0xxlPC/rPayJ3b0sFa3ywbNfTaz5JFGmXbgIZynjFrL8Q=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        details = authenticationModule.getCertificatesDetails(session, appUID);
        SigningCertificate cert1 = null;
        SigningCertificate cert2 = new SigningCertificate(null,null,"My Organization Name",null,null, null, null, "98C116D3", "IdentifiedThirdParty","ITPD");
        SigningCertificate cert3 = new SigningCertificate(null,null,null,null,null, null, null, "F91CE143", "manufacturer","MFD");
        SigningCertificate cert4 = new SigningCertificate(null,null,"My Organization Name",null,null, null, null,null, null,"MFD");
        SigningCertificate cert5 = new SigningCertificate(null,null,"My Organization Name",null,null, null, null, "","manufacturer",null);
        SigningCertificate cert6 = new SigningCertificate(null,null,"My Organization Name",null,null, null, null, "6CC300CF","operator","OPD");
        assertTrue(details != null && details.length == 1 && checkCertificateDetails(details)
                   && !details[0].isSameSigner(cert1)
                   && details[0].isSameSigner(cert2)
                   && !details[0].isSameSigner(cert3)
                   && !details[0].isSameSigner(cert4)
                   && !details[0].isSameSigner(cert5)
                   && !details[0].isSameSigner(cert6));
        // 16. Developer Certificates/Manufacturer - sign to trusted third party domain
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIIC7DCCAlWgAwIBAgIJAIjarxZ6zTJ2MA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyMzYyNloXDTE1MDIwNDEyMzYyNlowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAs5/XLwdG8ykxn8bq3IhHcoeIKTN9fzrnkbW+Z80expjpG5Fbpsh7hR0vybYtu6kPP6DHV/SsYXeIfCvmYPRUXe9iR4qWb4DoqrCzd01wOPFaln3orRTXZhvlvKEMNmOlIcdjQGAdiKlFSm7aGXcWF7vRfL9x+CW8YjzV3FbBFIMCAwEAAaOBozCBoDApBgNVHSAEIjAgMA4GDCsGAQQBXgExAQEDBDAOBgwrBgEEASoCbgICAgMwDgYDVR0PAQH/BAQDAgeAMBcGA1UdJQQQMA4GDCsGAQQBXgExAQICAzBKBgwrBgEEAV4BMQECAgcBAf8ENzCCADMMDzA3NTMxNjYxNTc4NzUxMQwPODUzMTEwMzg2NjU2NzgyDA8wMDAwMDAwMDAwMDAwMDAwDQYJKoZIhvcNAQEFBQADgYEAK63bD9hyNjvIykCUaMYJENuJy/9kCEtGL4YQPZdHG+TeSQ0zgvUwiQXsWPwFaa/P6xY5TTA1Wf+uQY7hxFdWvnSISHgcDlw7uKk0prENl2el+bGMU1E4kVqcx+QRGPYdFRpFXq3VtL0GR9AC0sJRwjGo8ZIxIwSMrKI6NpkLOO8="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "otIHquw4orxOi5bhejtnZGdWPHyJW+EQPhTgqoGFqQt8zNhAl1MNbBxZCaXBu84IWChcMbTj3ziogyUDeW+CmnP5T85z18/A0GanqAXYj7otaYiba/Si2CiHYQdEBvGgjATuDuVr9wsv/1qWGqmmnPo6+1OqulM38etWnB2/zGQ="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        domainCategory = authenticationModule.getProtectionDomainCategory(session, appUID);
        assertTrue(domainCategory.equals(ApplicationInfo.IDENTIFIED_THIRD_PARTY_DOMAIN));
        // 17. Developer Certificates/Manufacturer - sign to operator party domain
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIIC7DCCAlWgAwIBAgIJAMx3Pmq32sbvMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyMzkyN1oXDTE1MDIwNDEyMzkyN1owVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAzcPrO75PU8yU2VyOa/boUeZ7Glvhnq4BgE3gLYLLQgKLAwlO5NfVn0v1JKKP02milDo+ptYxVTdGYnKDd8Bd3GE4IGaQY0PGEXEnZ6Oa/9afxjUkHQh3AgnyCceU9UJ1JVjdrVltiVMoa7fruqq8j6MXaWkctOBbXe0APnfHDpkCAwEAAaOBozCBoDApBgNVHSAEIjAgMA4GDCsGAQQBXgExAQEDBDAOBgwrBgEEASoCbgICAgEwDgYDVR0PAQH/BAQDAgeAMBcGA1UdJQQQMA4GDCsGAQQBXgExAQICAzBKBgwrBgEEAV4BMQECAgcBAf8ENzCCADMMDzA3NTMxNjYxNTc4NzUxMQwPODUzMTEwMzg2NjU2NzgyDA8wMDAwMDAwMDAwMDAwMDAwDQYJKoZIhvcNAQEFBQADgYEACZ4K4DQxzfuL5wVoQElbSi5w2vEQrtq7UvDb9TkBVjp/Ls+1g6yTzvp42sl6fJAl4Ooqxo+9SwiddYaiiB1DG3hP1V88HZOjQBDVuiv+iKU6+1L6yb2k5PkaBkYZG+bkg/iUGYOedzenTlocj3cHKOZtfcECVMoDpVR+YkDfvZ8="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "wXTAM75XDaBaBV89qlb/XnR/hRtQ2y8uVJmUJg/NY0JXD0g8o9mT3M7PGJbDIjUGE1zcdqN+NmWaDQtWo7yUX1G4nn8TEnXvupNo2hWqwhTMDM3wMgs7kgbK1W/lC/OZVuM9CXmbPw+/r8eRJmCspz1ocYonYS08ZQUQ943cRt0="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        domainCategory = authenticationModule.getProtectionDomainCategory(session, appUID);
        assertTrue(domainCategory.equals(ApplicationInfo.OPERATOR_DOMAIN));
        // 18. Developer Certificates/Manufacturer - sign to manufacturer domain
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIIC7DCCAlWgAwIBAgIJALV2Fc/8vZkDMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyNDAwN1oXDTE1MDIwNDEyNDAwN1owVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEA932ZtgmgBrqP6FN/KsVYOhhHouD0BZ8PIgIgbD14furlOWkUiex2CeWU6uNXosYui28E9d6eAoYh1ri3nH5AjHhDKNtF0jaQJOW9a9ZRf3MXeXGgNR+lb1X0KRvdNK3hukkeLfT98/UCnw3GZiu8kMAEUhvuyPELlmyIQUpSjmkCAwEAAaOBozCBoDApBgNVHSAEIjAgMA4GDCsGAQQBXgExAQEDBDAOBgwrBgEEASoCbgICAgIwDgYDVR0PAQH/BAQDAgeAMBcGA1UdJQQQMA4GDCsGAQQBXgExAQICAzBKBgwrBgEEAV4BMQECAgcBAf8ENzCCADMMDzA3NTMxNjYxNTc4NzUxMQwPODUzMTEwMzg2NjU2NzgyDA8wMDAwMDAwMDAwMDAwMDAwDQYJKoZIhvcNAQEFBQADgYEAZqNEIjA90iRuIPJE+OEnB+LpBlENkamEmNCSX5LtxFS/96BzUVc0gU90it1YP5A1HlqWF2+dK13b+QAU0bW1xfORw3bCCRhAZei1E93/watRQDf0yCjDhRqa3LFU19j7zNd8Ul7YynByAZiZUmXNQZHELmWpUmYruNEWBNORKyk="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "1ggIHsuOMDD+GeLcwZr3W3EnY9HD9g68WH7qe6fXLsCicGc+mEB3jqmvxIsMgv8zvv+fdAQsgQfXDqNWQ5EJ5pf5SKXIowpilwQu9Tvk2iRaqDXqAZIqp0en1ST4CZOEC1xEdBcXBBAaAMvAG2EJQtCeMtkdU27UfpFbdWy1eu4="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        domainCategory = authenticationModule.getProtectionDomainCategory(session, appUID);
        assertTrue(domainCategory.equals(ApplicationInfo.MANUFACTURER_DOMAIN));
        // 19. Developer Certificates/Operator - sign to trusted third party domain
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICxTCCAi6gAwIBAgIJAJBmh9ckN0IqMA0GCSqGSIb3DQEBBQUAMG8xCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMRswGQYDVQQDDBJKUF9PcGVyYXRvcl9Eb21haW4wHhcNMTAwMjA1MTI0MjM4WhcNMTUwMjA0MTI0MjM4WjBXMQswCQYDVQQGEwJGSTEQMA4GA1UEBxMHVGFtcGVyZTEXMBUGA1UEAxMOTXkgQ29tbW9uIE5hbWUxHTAbBgNVBAoTFE15IE9yZ2FuaXphdGlvbiBOYW1lMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCycnZBBs0xC3qAFELkRzdzldYLO6bWXq9+1uAnifZHtQBnHlTV28ejT+ZwlG5LXqOeQHShw+GRA7jUaeilMax2jCfCnNpIHpgxgp86HN3SLK8Bqwdt8dfSTfXdODlfsB3+Ivm5Ki46RbVJmFnS/qtgljAr/cbinAFWrryXh7oL4QIDAQABo4GAMH4wKQYDVR0gBCIwIDAOBgwrBgEEAV4BMQEBAwQwDgYMKwYBBAEqAm4CAgIDMA4GA1UdDwEB/wQEAwIHgDAXBgNVHSUEEDAOBgwrBgEEAV4BMQECAgMwKAYMKwYBBAFeATEBAgIHAQH/BBUwggARDA8qKioqKioqKioqKioqKiowDQYJKoZIhvcNAQEFBQADgYEAd0GNAT5doSHyUNHMvNWpy1rq5O0LNuMHGho3YQBEJ3N4NCSx490hhlCV/6VKGVj4aKkrTfqRGBQ26Oa00srH8EUme7wBTn0SuWj04a2aZ+PwzuG39kqmZag1jPmB+t5/+j5p+DW3YhDGn2297pKR7h0/uCT15NAq9xPMYopqHQA="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "mQ5yJoXJK1Uc4KXGm8Gue+Yi8LzzFEaRSfYyUiZSROXzapVTHpfnCYJKvG1cQOv/aZ9gDQ8LQoBLYIuZEXIK6RzzEaBCY/RcEaCviaSVq3JB/OiXglLcTZ1e+mFBvQQ0LUuPKaCW1jv4EJy3BfNL9ckeh/7dNxXc1qRSe5lZWFk="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        domainCategory = authenticationModule.getProtectionDomainCategory(session, appUID);
        assertTrue(domainCategory.equals(ApplicationInfo.IDENTIFIED_THIRD_PARTY_DOMAIN));
        // 20. Developer Certificates/Operator - sign to operator party domain
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICxTCCAi6gAwIBAgIJAIkqKsPh0EAcMA0GCSqGSIb3DQEBBQUAMG8xCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMRswGQYDVQQDDBJKUF9PcGVyYXRvcl9Eb21haW4wHhcNMTAwMjA1MTI0MzM2WhcNMTUwMjA0MTI0MzM2WjBXMQswCQYDVQQGEwJGSTEQMA4GA1UEBxMHVGFtcGVyZTEXMBUGA1UEAxMOTXkgQ29tbW9uIE5hbWUxHTAbBgNVBAoTFE15IE9yZ2FuaXphdGlvbiBOYW1lMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCvb2WSRLEM1REbIJ18v9UEFEoY9/oJYcstfUduI5uEcd0yXlc+Sy7la7v3oWGYa0N3QQ77vScJo1nfUUoJEIEwpcWYuJ0OHjkD5sFmlGoSXy9I19s+8Ia6hW4mTBPDFn0cUrVHpjiTNBJn2ukhQGqdb30mWUqwntqJDMwNt6WwpQIDAQABo4GAMH4wKQYDVR0gBCIwIDAOBgwrBgEEAV4BMQEBAwQwDgYMKwYBBAEqAm4CAgIBMA4GA1UdDwEB/wQEAwIHgDAXBgNVHSUEEDAOBgwrBgEEAV4BMQECAgMwKAYMKwYBBAFeATEBAgIHAQH/BBUwggARDA8qKioqKioqKioqKioqKiowDQYJKoZIhvcNAQEFBQADgYEAVlz/9UYZIorhMH23LQkwRN00FckIYjNbyCsAqnE2o21RLG08ay8uwc2BOCEoKaD5WVE5eyxlatW8IWFygYVN7tzUvtAE7xzb9gazwGYtjooaCi8c4EkHut46ixqjoREakJQzkMu3WGul+IGt4oOcJcC8UMwfUodmA3TuKXigFo0="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "oTIYybIjUD9bop+tDyOkVKP6kUKnAYi1JTI561Atrm6jhobZlQyBksNv8hI76xs1VOU8wnY0EYozhAnMR0WzC9QLGihJE0l5npLyvBi/0Q4CuFIpLg+2xpbBQ4vF3s7rDknimPVzd3VIsxo5mz7qGVeGCDfoxf85W+qMrY9lsy4="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        domainCategory = authenticationModule.getProtectionDomainCategory(session, appUID);
        assertTrue(domainCategory.equals(ApplicationInfo.OPERATOR_DOMAIN));
        // 21. Developer Certificates/Operator - sign to manufacturer domain -> still the operator domain is used
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICxTCCAi6gAwIBAgIJALxFDFpKvETiMA0GCSqGSIb3DQEBBQUAMG8xCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMRswGQYDVQQDDBJKUF9PcGVyYXRvcl9Eb21haW4wHhcNMTAwMjA1MTI0NDM4WhcNMTUwMjA0MTI0NDM4WjBXMQswCQYDVQQGEwJGSTEQMA4GA1UEBxMHVGFtcGVyZTEXMBUGA1UEAxMOTXkgQ29tbW9uIE5hbWUxHTAbBgNVBAoTFE15IE9yZ2FuaXphdGlvbiBOYW1lMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCwhV9kpgw9RYoM6zV/3z8L8y9Bz7Sqanq+T4xUb7eVvad2kRGU7BPD0f08Kt+o+td4StKKOcZDxGNx8qrQbMlLdDrZFI8lha2dCHUU221YMlvDxITG0YCjBWcMwowMC0Q62gBGr41Ee7pCb08V/E+xXJrNmhrPpluezuiMVJ9BIQIDAQABo4GAMH4wKQYDVR0gBCIwIDAOBgwrBgEEAV4BMQEBAwQwDgYMKwYBBAEqAm4CAgICMA4GA1UdDwEB/wQEAwIHgDAXBgNVHSUEEDAOBgwrBgEEAV4BMQECAgMwKAYMKwYBBAFeATEBAgIHAQH/BBUwggARDA8qKioqKioqKioqKioqKiowDQYJKoZIhvcNAQEFBQADgYEAPtSTGHXsY+owikYpiOIKqnN7ZwJ2fU2Eb3NkhctVPIcomvwMBBPkKQ4qbxkHs6nF0Q5EkRTt/7FsUjMpmHKjRGlQ3FwbAaMHgwtqD6bDX3CqltMAgntd1yDPex/JzBi+bT+fudlXBB2X5jXly1GXEKQAlw5e9E2FKXBG6zhMqlY="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "lLOY+UtDU4Edj2S8T3ERkDnwJvBDJgh6Y6ah/hTL6/njyWnnGUCXdu3UN2jayqP0xTNQvM5Xv0zc5lv0mpdwzpT+17xDG6t0xSu2QvI+xRgIqzS9ncxaSF9oa4aIFe8IE3kf2+LW+jRleKplC3d0rzOmOHJLCZspRr1Zrs2238M="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        domainCategory = authenticationModule.getProtectionDomainCategory(session, appUID);
        assertTrue(domainCategory.equals(ApplicationInfo.OPERATOR_DOMAIN));
        // 22. Developer Certificates/TrustedThirdParty - sign to manufacturer domain -> still the trusted third party domain is used
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICyTCCAjKgAwIBAgIJAPFYfswHG2oFMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTEyNDU0M1oXDTE1MDIwNDEyNDU0M1owVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAwWzPnCHOidkD066tY5KQzL4ezGeqJnHJbRFlaqsVbuMC07Lv52Bqomi0vUExK3L1Wh3oml0rh2eivbYsRpSJHOqUBcV28dmAcP+rsQ2BWs28VCaHJ2W/piH4aeXfENCh8n+MOvMTyEOrYMVH6HOKHn526BXCOC0ubUsqcHwhVHsCAwEAAaOBgDB+MCkGA1UdIAQiMCAwDgYMKwYBBAFeATEBAQMEMA4GDCsGAQQBKgJuAgICAjAOBgNVHQ8BAf8EBAMCB4AwFwYDVR0lBBAwDgYMKwYBBAFeATEBAgIDMCgGDCsGAQQBXgExAQICBwEB/wQVMIIAEQwPKioqKioqKioqKioqKioqMA0GCSqGSIb3DQEBBQUAA4GBAKilfiTDHUyKy+S50Q+XUL57HXGEJsDHTxQgzLtrBRkSt/2f8nUBtoC9BXNXRBB7wi8+Gbe8uH8XuHX2QhoWU/gQwsmrmRbRtAbbi3jcezappnal2AFO1+icwiWYdeaKicScTz7tgZZY986EBDVJr8fqpYX5Br394zsGbeUu0ueH"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "P80CAyaqrtCFH+c2vrXX1yJkwAo2NrFEboC/h5R/47W8D1dOfhUUWSkGmCSs3eQRgdgFXjIjZL/zmCQbNuZJ5mkE+o8Z3ptFNJEQCBlV9Nfh9D0YmdR81CJgSh0af7QNHJ4rlttKtTmvFEUD2Kc49g4FDXMjU+7ISp6ZbUhNk4Y="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        domainCategory = authenticationModule.getProtectionDomainCategory(session, appUID);
        assertTrue(domainCategory.equals(ApplicationInfo.IDENTIFIED_THIRD_PARTY_DOMAIN));
        // 23. Developer Certificates/TrustedThirdParty - sign to operator domain -> still the trusted third party domain is used
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICyTCCAjKgAwIBAgIJAJP72IjeLQ+5MA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTEyNDYyN1oXDTE1MDIwNDEyNDYyN1owVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAzyFOFxPXidq4NoJYKtMIBV3q3KEuZqavo4T1cQeVaCHFgTm7hE8VaBi/xujLJJ6SGyn7AFbM5widqhGVmsFa9Wq240zgTBfQWFY6vbZlLUge0PE5Xkre97DsUKz1YNf1JlI2FREWB+UUyff4Y2gs6jg9qDd/Rb7xsn4M/OPb6ucCAwEAAaOBgDB+MCkGA1UdIAQiMCAwDgYMKwYBBAFeATEBAQMEMA4GDCsGAQQBKgJuAgICATAOBgNVHQ8BAf8EBAMCB4AwFwYDVR0lBBAwDgYMKwYBBAFeATEBAgIDMCgGDCsGAQQBXgExAQICBwEB/wQVMIIAEQwPKioqKioqKioqKioqKioqMA0GCSqGSIb3DQEBBQUAA4GBAJbEVhlBD6ioHcCHuQfo5dQSlFGXk4iPhXwjxzXHL55RoYi2ENKhD1ts2jUg61Hi7P+Qdcv3LBQLnTufuNL93aQ0J0IKuHYUM9mXYOnUHPluvqrM2djK/WBfVwF/VxBuf2L3R7P7AwnBkGHxcNhW4ipTmkuzFnFCsqz87/8UP5HX"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "FVXATPVhIFWYUp7a/BcmQodZLHcuBi8L4gibaXVBFECjqIQG0TGqgzm00xw4nxCFikV+vqdCiF7qD/YJClc6DPxgcqd70QTaTouRJGMsitj0ax7CPISLcXkn22aYZREChMpm1qWegAa9OwGudln6hlFqJLqa8iyD8482NiVG2Z4="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        domainCategory = authenticationModule.getProtectionDomainCategory(session, appUID);
        assertTrue(domainCategory.equals(ApplicationInfo.IDENTIFIED_THIRD_PARTY_DOMAIN));
        // 24. Developer Certificates - no domain info -> fail
        try
        {
            authenticationModule.removeSecurityData(session, appUID);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                              "MIIC3DCCAkWgAwIBAgIJALhAaTo8j0WHMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyNTExMVoXDTE1MDIwNDEyNTExMVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAqZaB/y491gIIPzwvdfhy91OvJAz8qrtY0gEq6QZjBA8lZclJJBP4pNTDrY7Nk/8SBlB92HlplzRprPj6i2g/mQsGolY5d9YR7415EwTtNZ9J3PoJGtd7Xd25wiQWOHdLeEv413+EiqHB1G4Xn2PZpE3j51C7Ld1618vvU0dPfMMCAwEAAaOBkzCBkDAZBgNVHSAEEjAQMA4GDCsGAQQBXgExAQEDBDAOBgNVHQ8BAf8EBAMCB4AwFwYDVR0lBBAwDgYMKwYBBAFeATEBAgIDMEoGDCsGAQQBXgExAQICBwEB/wQ3MIIAMwwPMDc1MzE2NjE1Nzg3NTExDA84NTMxMTAzODY2NTY3ODIMDzAwMDAwMDAwMDAwMDAwMDANBgkqhkiG9w0BAQUFAAOBgQCQ0VMGqVKujIoaxtho7bF16B8xpT98QKvz8C5WpERVhT8uj4FPuz0l0hRcsi/g7SatO1S0uwDqR7bqcUWlYY3EjmwXhuZ6a6HeIEPhLDowZ64lLK0arT6RsPqD0ITRwOm7OposUcLZaQqpAPS1aaNzQDyRMCxeNv1czI6+rYHi1w=="));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                              "Z+UFeHCTi5FKIftISDn8mNCgCCTnRZ4iL6nwadOE3lFlcQxGid8720QUkBMt1xGwFHfvMec23snNx6YLDHOpL2EVLYoFAFg91cY58P/BWs4ykFm4N9DJOAysr9uE+etuIt7ZU2O+TxY0wN/KpQ60daogW4tBTmw99PfmKmyZ1Z0="));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
            assertTrue(false);
        }
        catch (InstallerSecurityException e)
        {
            assertTrue("",
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_AUTHORIZATION_ERR, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.CERT_UNSUPPORTED,
                                                        null)));
        }
        // 25. Developer Certificates - no IMEI list present, but domain info is present (trusted third party domain) -> domain is trusted third party
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICnjCCAgegAwIBAgIJAKb+FzQaAzk6MA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyNTQzM1oXDTE1MDIwNDEyNTQzM1owVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEA3w6P5ngznrWh4cLg4+4JiY9Fnqhu8H+gxISVZKhSFojtXHaEscZcWpUWm6WyUKh1yjaherCyERD9lUHtbmUsNrLCvVc54mptHmSf/6c4QzpCofGUtcPncWzEpKMuwbh2LqSiuVGYhoz9n1RpHM+79ZqCC3H5yAhqH6YxF6icaHsCAwEAAaNWMFQwKQYDVR0gBCIwIDAOBgwrBgEEAV4BMQEBAwQwDgYMKwYBBAEqAm4CAgIDMA4GA1UdDwEB/wQEAwIHgDAXBgNVHSUEEDAOBgwrBgEEAV4BMQECAgMwDQYJKoZIhvcNAQEFBQADgYEAUwo4DMNoHaMY0shLnPDeaY10zvSPewrWHmfNsFoIspCxRQDCsZc0YkjjzdfBtfvB5gCFpVpkRGjp1aM7IFpAMdYoJ8zDUI4fkkmyLylc3z58th0Pe3FJp0fzKFthwGkKtCUKssF04IbqH7TqCeZiN0vdfL6K49htQ7oS2f45NLg="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "sOpL9KbJMxvT8Cwtvlj9rLfJKSDVkWYm13rGWgU27djNP77+48d/VBN5WkUp1MW2XLmn3umA3sstFBk0YfLGOSe4N0TusvcdmQXwBovK82xJfX1yNXs23Z6eRvDFanxC85IEfKLXcuIw5tK6v0lAG94AbMaQPhB5olAamdtgzLc="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        domainCategory = authenticationModule.getProtectionDomainCategory(session, appUID);
        assertTrue(domainCategory.equals(ApplicationInfo.IDENTIFIED_THIRD_PARTY_DOMAIN));
        // 26. Developer Certificates - empty IMEI list -> failure
        try
        {
            authenticationModule.removeSecurityData(session, appUID);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                              "MIICtzCCAiCgAwIBAgIJAK6/9Mp3gawiMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyNTYxOVoXDTE1MDIwNDEyNTYxOVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAtnKR28ledYg2hiOUcWyfYfm2KaAo3laoc6lxYS0/laIAbSKW5lysbtzY2yxDFP6f1o2eg/oXjJyTSm2tJGALwFlobP5afKtsldMt9yyddk6OWPpGelb87KuYUZEDiAAlFB/THRSfUL/seoPuC2oSRdSHNTnpkUmE9BHy4deMs6sCAwEAAaNvMG0wKQYDVR0gBCIwIDAOBgwrBgEEAV4BMQEBAwQwDgYMKwYBBAEqAm4CAgIDMA4GA1UdDwEB/wQEAwIHgDAXBgNVHSUEEDAOBgwrBgEEAV4BMQECAgMwFwYMKwYBBAFeATEBAgIHAQH/BAQwggARMA0GCSqGSIb3DQEBBQUAA4GBAHKrkiSm6g9+Do1qW+a19rukHghBM3Rb1lqLjIJRvxr6I1GJ+R2pIjKRmxob7zdAQ8yKdFp5TG9v0l3HQKatoMARNQXh8NGCryILCjMLVh4gI/GOHWcmpsndm5xlXod+qeIF866pFeCJfbD6F6gWzz9Qm5CL4mCv3OEFn/tlsvdB"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                              "rvwdao/zvNxmzM2kQq42prVmspMV+gnyeEBlzz7d5kc4lsok+pn3hg65Nwr1cM/DfYd8NYyTuU6bLlOV7n9iMjgqKMNPeiU7M9g3EC7xoL7kY+dn0Og5IjHfmovMj11i0yV9u8+oFOai2EbkPMxS9sXXMFkXZpnm1yOWmtzVGxo="));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
            assertTrue(false);
        }
        catch (InstallerSecurityException e)
        {
            assertTrue(INVALID_CERTIFICATE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_AUTHENTICATION_ERR, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.CERT_UNSUPPORTED,
                                                        null)));
        }
        // 27. Developer Certificates - invalid IMEI list -> failure
        try
        {
            authenticationModule.removeSecurityData(session, appUID);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                              "MIICvDCCAiWgAwIBAgIJAOPDxqOqcY29MA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyNTgwNVoXDTE1MDIwNDEyNTgwNVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAo/4uRMIbsXGeK02CLubUZXqne6VanxwnS784JJ1XjBajMENnCh8DmeQA2SlUbUNljXZ3gfm5tEN3r7AkgMJOSvlLQ086biMn49emNuenXQLp+YdDAylJoODirUed2NAhCCWvC8BT6k8NAJqpsJZTO6Q3rJIDI8Ybw0kAOJpk1jECAwEAAaN0MHIwKQYDVR0gBCIwIDAOBgwrBgEEAV4BMQEBAwQwDgYMKwYBBAEqAm4CAgIDMA4GA1UdDwEB/wQEAwIHgDAXBgNVHSUEEDAOBgwrBgEEAV4BMQECAgMwHAYMKwYBBAFeATEBAgIHAQH/BAkwggARDAN4eXowDQYJKoZIhvcNAQEFBQADgYEAjwgEv/CTRm03vEiQ4IDc8P/ebw7aXhmbeoXy/OYfe+PjktHFD9Z45tQEjm+ydtK1FJQcUtQaIwMusi8CmMdfRQPA3H+tbTNuKyvvVFznaaQIhTKZuZqT1nbocj13BNUGljg/y4jcJhlPkewlm7e8dk2AaM8MzF/bMb++xUWb5kE="));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                              "oeWf4S/xsTMnNf8ojeLHSNeya04fAXzKP2/9gOrqY7GJEVMJ92r7eQgWmgezhFfZRwOZyhJJa7h70OAYWMzoVX2RwXVvVQUTPb7Mii7H/L5/w8euRYQJcHqkaJ6b2XOt40ML7qRXhetPpQ9nQDCNQFeyz8zVIrEDW76ba1PUlNw="));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
            assertTrue(false);
        }
        catch (InstallerSecurityException e)
        {
            assertTrue(INVALID_CERTIFICATE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_AUTHENTICATION_ERR, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.CERT_UNSUPPORTED,
                                                        null)));
        }
        // 28. Developer Certificates - 1 IMEI, no match found -> failure
        try
        {
            authenticationModule.removeSecurityData(session, appUID);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                              "MIICyTCCAjKgAwIBAgIJAJDvl9w146MAMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyNTk0MFoXDTE1MDIwNDEyNTk0MFowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEA1VmImlDul7gwWQAPSJ/cmWm9C5+KAFOsDmgehzmAAwR1iHdeCkXMq7C9/4wjTikCc0PHs+3mDEZBQQcdKpYdUc7mwz1zyDqvjddNdQDZY1o5Jm/LDYBI8vo6oPIdLB4CpQhsTZEvWmGBrdo5xI9e0xV/0/NrIEQzYbMpWMskGjECAwEAAaOBgDB+MCkGA1UdIAQiMCAwDgYMKwYBBAFeATEBAQMEMA4GDCsGAQQBKgJuAgICAzAOBgNVHQ8BAf8EBAMCB4AwFwYDVR0lBBAwDgYMKwYBBAFeATEBAgIDMCgGDCsGAQQBXgExAQICBwEB/wQVMIIAEQwPKioqKioqKioqKioqKioxMA0GCSqGSIb3DQEBBQUAA4GBAAYblCj3vO3937pO7z6zgaqkvbFQywBEHC297Yy1cwJkzJSxayyDIMX2FRnlnlJKArLbbSgjRYsyUHjKtlq2u7YUiD7CBZxTFFfBpCjvCan2tW2Z0BmR7CY4Skmb9qC5/VwyYJ0++TZvwcHothzkbbVJ4AGm9Y98GWIbLiwOxKaj"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                              "hvTba5gR2NAiijc8gbfXBJkp4K8YroJYkXH/CxeDPTW1gRLdsgnqkkhRCdlzcv8w2kpMyi04826Let7EmUED1fDsRO5v/Ot57J9r/i/5PXctUqlKb3XgUttPQrNQepX8wTH0dpGNgTauS6MX8VNb4/pu9GxAOdRz3URYm4Qqyy0="));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
            assertTrue(false);
        }
        catch (InstallerSecurityException e)
        {
            assertTrue(INVALID_CERTIFICATE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_AUTHENTICATION_ERR, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.CERT_UNSUPPORTED,
                                                        null)));
        }
        // 29. Developer Certificates - 10 IMEIs, no match found -> failure
        try
        {
            authenticationModule.removeSecurityData(session, appUID);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                              "MIIDZzCCAtCgAwIBAgIJAKVNeS6JpAvfMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEzMDM0M1oXDTE1MDIwNDEzMDM0M1owVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAwkL7axzvM73fdtf8LFVFXxstdoQ4jkkyMsD2fcmkHjKgA0QIZ63+r0ALXrZrKwUnu48MRE0ipOrNZoRjnpm8846enlvl2PamNnBpCKOCQk6mToeGPslVPO207MrTblUpaI9yWZiHg2p05f067PU3dwGubzhTpSjBEwbKE9+1P0sCAwEAAaOCAR0wggEZMCkGA1UdIAQiMCAwDgYMKwYBBAFeATEBAQMEMA4GDCsGAQQBKgJuAgICAzAOBgNVHQ8BAf8EBAMCB4AwFwYDVR0lBBAwDgYMKwYBBAFeATEBAgIDMIHCBgwrBgEEAV4BMQECAgcBAf8Ega4wggCqDA82MzEwMzY0MDcxODI4NDYMDzI1MjQwMDYwNzgwMTQ4OAwPNzQ3MDQyODM2MDg4MjQxDA8wODgxODUyNDI2MTA0NTIMDzUwMzcxMzI1NzAzNDcwMQwPNjU4MjY0NzQwNzA4MzUwDA80MTcwMTA4ODI4MjUwODQMDzE0MDE4NTUzNjg0MTA0NwwPNzY2NDcwMzc2MzYyMzM1DA8zNTI0NDcxNzI0NjIzMjYwDQYJKoZIhvcNAQEFBQADgYEANFnXZBKG3avDMuqCVq0kL9qKDQIgdSCz4d9VF6i+LpbCMvMFDmmfYYvWDLC7Rr8DjKyGsU3HyScP2imHK2d1C4UXrXzN0Mfw6a6mOmNmhyFY3tW+09ESB5iKOeeLnlUHsH+sQ4ooXXZetUQL9c0sAvKSgTz0GNDzff6Q1H2aWrQ="));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                              "duw2pV1BgJeuuiWM3wvUfFEVIdIyoJaSv5k1odz/J5WPAO2UI90NiNpkXbe/kf99TuPL9dVytegt9+8b8Kllb++CSy29x4fuRrlHl1jJIafjILgSDEoogKiIyDetl/9ywccHCVPlev0evBevLgOjXDLh3OWfqAUsVR5f/aC/jxQ="));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
            assertTrue(false);
        }
        catch (InstallerSecurityException e)
        {
            assertTrue(INVALID_CERTIFICATE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_AUTHENTICATION_ERR, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.CERT_UNSUPPORTED,
                                                        null)));
        }
        // 30. Developer Certificates - 2000 IMEIs, the last one is a match
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIKHjzCChvigAwIBAgIJAPsXiHu9EXbWMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEzMDU1MFoXDTE1MDIwNDEzMDU1MFowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAvMUUuI10Lf/lmiwjfLsI6ryM3M1TAxtlNKcXY016/buVLOGeiLv5a7oLtsImSZQNW2vzHpXMZSrfO+jhQaCK/pBp061+iyRD5Qu69jkGNT1EPLF5e9B/NH/QExd6omkeHPp2BIqINn65vIyiA5zOn3v4vCbToJdzBgeLHcmFLccCAwEAAaOChUUwgoVBMCkGA1UdIAQiMCAwDgYMKwYBBAFeATEBAQMEMA4GDCsGAQQBKgJuAgICAzAOBgNVHQ8BAf8EBAMCB4AwFwYDVR0lBBAwDgYMKwYBBAFeATEBAgIDMIKE6QYMKwYBBAFeATEBAgIHAQH/BIKE1DCChNAMDzM2ODAwMjc3MDI0NTU2MgwPNzcxMDg1NDEwMjM4ODgzDA81ODExMDQzNzQxMzA3NjcMDzEzMzQxNTU3NjA0MTg1OAwPNDU0MDI2MzQ1ODA3MTAwDA82MDg3MTEyODM3MDA0NTEMDzU1MzE0NjYyMzcyMzcwMAwPNTQxMTQ0Nzc2NDAyNDE2DA83MzczNTU1MDc3NDM1MTgMDzQxODMyMzIxNzMyNDgwMgwPNzEzNTU2MTc3MTM0MTMxDA81NDUyNDUzODQ3MTI1MDEMDzM0NzQyNzc2NTM1NjEyMAwPNzU1NzAwNTA1MzEwMjE4DA8wNDA4NTY3NzQwMzM2MDEMDzQwMTM0NTgzNDE2NTczNgwPMjE0ODIxNTEzNzA3NTYzDA81Mjc4MjA2MTA3ODA3NTIMDzYzODU0MDUwMTY3Nzc3MwwPNDM1NTAyNDMzNTg2MzU4DA82ODE0ODgwNzg3NDM3NDMMDzU0MzgxODc2NDc2NTM0MwwPMDI1NDMxMzQ1MDQ1MTU1DA84MjA1MTEwODQyNDgzNjMMDzQ4MjUzNzAyNzAyODQ1NgwPNjMwNzUzMjczNjUwMjE3DA80NzUzNjM4MjE0ODUxNDYMDzQ3MzYxNzgwMzg3NzQwNQwPNzEyNDA3MDczMTI0NDQ4DA8wNTMyMjUwNTMwMTYxMzgMDzQzNjA2NTQ0NTE0ODYxMAwPNTU1NTc4MTMwNTI0MTAwDA81MjM3NDI0MDU3NzQ0MjMMDzE4NDI2MjQ1ODUxNDMwMAwPNjg1ODQxNjY1MzYzNTcyDA83NzQ4NDE0MzI2NjIxNTgMDzUyNDM0MjIzNjA1MjQyMgwPNjM2Njc3MzM1MTAyNDE2DA82NjUwODY2NDQwNzgzMDYMDzYwODUzNDI3MTUzNjMzMwwPMDM3NTYwNzU0Mjc1NDE0DA8xNDYyMDc3NjQzNDAxNTQMDzg3NTcwMTA0ODUxMjI4MAwPMDI0MjE2NzM4NDM3NDE1DA82NDU0NzQ3NTI1NTM2NTEMDzEyNjY3MzgwNzc2NjA0MwwPMDgyMjM0ODM4MzUwODI0DA8yNjgzNzYyMjMyNTMwMzgMDzg2NjA4MjM2NDc0MTcyMgwPMDg4MTY2MjQ0MTg0MTI2DA83NzYxNDYyMjA4NzgzMTgMDzI1Njg3NTM2MjM4NjY1MAwPNDgyMjgyMTIwNjAzMTY2DA80ODE4ODg1MzM1MDc1NzQMDzA2MTcwMDI3ODI1MDAzMwwPODg3Mjc3NjA3MTE4MDg4DA80MzM4MTUzODc3NjQ4MjUMDzcwMzIzMzY4MDI4NDU0NwwPMTU2NzM2NDUxNTUzODQ3DA84MTAyNDc1ODM0NTAwMTMMDzgyNzM0NTUzNzM0ODU3NgwPMjY2MTYyNzU0MzYzMDg3DA81NTQwNDA0Mjg1NjczMTIMDzU1Mjc2MzEyMjI2ODY0MAwPNDI3MjI1NDY4MDY2MDMzDA82ODM1MDgwODA3NjQ2MTUMDzUxNDI0MjY1NDQwNjgzMwwPODgwMjI0MzA3MDU1MTE0DA80MDI0MjYxMDMwMjA3MTEMDzE1MjU4NjM1NzU3ODYzOAwPMTg4MjU1MTU3NDYwODQ0DA8wNDI0MjEwMTcwNzIyMDgMDzg0NDcxODE4NjMzNTQwNQwPMjQ2MjIyNTg3MjE0NzU3DA80MjgwNTg1NDEyNTc3NTEMDzg2MzcyNTMzODQ4NTg2MAwPMDIyNzg4NjgyMzYzODc3DA8yODYyNTAyMTE0MTU2MjUMDzQzMjUxMTgyNDI2NzI0MAwPMzQwODM2ODc0MDg0NDMyDA83MzA4ODcxMjIzNTc1NDIMDzg2Mzc1ODQ0MTU3MTM1MAwPNjQzMjMxNzYxNjM2NzEwDA84MDg1NzY0NjA0MjUzMTcMDzM2MTc4NDc2MTAwNTA3MAwPNjUwMDMwMjcxNTg1NDIyDA8yNzczMjAxODQ3MDAzNjAMDzExNjY3NDczMzU4MDAxNAwPNzU0MTA2NTM4ODEyNzIyDA80MTIzNzEwNDU2MTI2MTYMDzc1MTA2ODIyODEzODA1NAwPODUyNTU0NzU3MDc2ODg3DA8xMzQ0MDgwODA4MzQ2MzMMDzEzMzU2NjUxMDcyMDg4MAwPNjU4NzEyODI4ODUzNjU2DA81MjAyODg4NzQ0NTg4MTMMDzI0NjczNDY3MTE3Mzc3OAwPNjIyNDcxNDAwMTgyMjg2DA80MzQzNDYwMTU4NTY4ODEMDzc1MTcwNDE2NzI1NzE1OAwPODg3ODAwMjM0NzYzNDc1DA80MzA2Mjg2NTIwMjAzMzQMDzY4MDA2NDMxNzA1MzYxNgwPNDQxNTYxNzM1Mjg0NDE1DA80MTA4NzQ1MDY0NTc4NDgMDzQxNjg0MzQzMjEyMjAxMwwPNjQ3NzI2NDIxNjUwODE4DA84MjI3NTQwMzQ3NzQxMzQMDzUxMDAwODM4NTYxODEwMQwPODQ2MTAxMzMyNjMxNzcyDA84NDU1MDUxNzYyMzE2MzIMDzAwMTgxODQ3NDgyNTM3OAwPNzgwNzY4MDAwNzA0ODYxDA82NzEwMTYxODczNTAzNDAMDzUzNjMyNzA2NTE3MjcxOAwPMTY3NjUxODY4ODM1ODIzDA8yODA4MDUzMzA4NDgyMTMMDzU2NDQyNTU3MzczMTMxOAwPMDU0NjA3NTQ2ODg0NTE4DA81ODE2MzE0NjI3Mzg4NjEMDzU4MTAxMDg4MDcyMTIwMQwPNTIzNjU0MDEzNTIwMjQ1DA82MTIyODgwMTI4MzIyMTcMDzUxNzQ2MzcxMjUzNjcxMQwPNTg1ODU4MzIwNzM0NDIwDA82ODc1ODU1ODYzMTAwMzQMDzYzNDY4NDAzNDIzNDM0MAwPMjgwMTc2ODM3MzM3NzYyDA81ODYxNjcyMzE0MTU3NDMMDzc0MzQ4NjA4MjY2MjY1OAwPODQ2MzYxMDY1NzA3NzAwDA8xMjcxODQwMDUwMzE4NjUMDzA1MzMwMTc4ODc4NzQzNAwPMjQwODM3NTM1ODM0ODIzDA8xMjg3MDI3NTEwMzQzNjEMDzU2Mjc1NTc1NjAyNzExNgwPNzc4MjgzNDM4MjA3MTgxDA83MjY2MDYxNzAwMzcwODYMDzY3MTEzMDg4MTY2NTA0MAwPMDA2NjU2NDEwODI0NDgyDA8wMTgwODcwODQ0NTM0NDAMDzUzMzU3MTE1MDQ2NDg4MgwPMzI2MTIwNTgzMTQ4NDM4DA80NTMyMzU2MDgwNTc0NTgMDzgwNjc0MTYxNzU2NTUyNAwPMDgwNTU1ODE1ODE4NTIzDA84NzExODgyNzY1NTY3NTgMDzc4NTU2MDM4Njc3NDY1NQwPODUwMjcxMzc3MjQ3MDEyDA8yMjAzNjg4MDEyMDE2ODQMDzE1Nzc2MzU0NjgxMTgzMQwPODcyNTAwMTc1NDU0MDQ1DA8wNDEwMTY3MjI3MDA3MjIMDzQ2ODUyMzgzNTE0MDAyNgwPNDUyMDU4NTAwMTgxODA3DA82NjA3NjMyNDc4Njg1MTMMDzU3NDMyMTE3ODA4MTY3MwwPNTI4MTA0NTE0NTU2MjIxDA8wNzU1ODc1NTMzNzMzNzAMDzgzMzE0MjI1NjgwNTU3MAwPMDQwMzY0ODczNzQwMjU2DA8xMTI1NTcwMzM3NzM4NDEMDzgwNDIzNDI0NzU3NTc2MAwPNTExNDEzMzMyMzUxMDI4DA83NTE0ODMxNTA4ODAyNzMMDzUxNjE1NTM2Njc0MjQ2MwwPNjEyMjQxNTEyMjI4MTcwDA84NzI0ODMyNzcxODg0MzcMDzYxNzUxNjMxNzMyNjY3NQwPMTQzMjAxMDcwNzIxMTM3DA82NTA4ODEzNTcwMDAyMzAMDzQyNzEwMDM1NDM1MTM2NwwPMzU3NTI1MjU0MjU4NjUyDA84MjQwMDQzNDEzMDcwNzYMDzI0NDc3NjY1NDIwNjc0NQwPNjM1NzIwMTYwNTA1NjcxDA84MTc3MzcyNTMwNDM0NjgMDzU3NTg4MDcxNjAyODM0MwwPODQwMzQ2ODE1ODYwNDg2DA83MjUzMDQyMDUyMjIyNTQMDzQwODE0ODc4MjM4NDUyMgwPMTg2MzYwNjIxMzgxMTg1DA80NTY2MzA1MjgyMjI2MzIMDzc1ODM1NzAxMjY3MzAxMgwPMTI2MzM2MzMxNTgyNjc2DA80NTgyNTQ1MTAxNTc1NDYMDzI3Mzg0MDUzNDE1NjM2NgwPMzQ3MjYxNTU3NjY2Njc3DA84ODE2ODY1MjcwMDM1NDAMDzQxNDA2NjM2ODM0NzQwMAwPMzcwMjczNTcyNDA3MTQ4DA8zMTU4NTY3NDIxMzEyNjcMDzg3NDY1MzE0NzY2NTQzOAwPMzYyMjU2NDg1MDgxMzcxDA8xNjE2ODQxMTg2MzIxMDEMDzU3MjYxMjU4ODQwNDc2MQwPODIyMzU0MjQ1ODg2MTA1DA82NDMyMDE4NjMzODYyNzAMDzQxMzIxNTA2MzEyNDM2MAwPMTAzNzcwMDAzNjAwMDYwDA8xODQ2MTMyNjc1ODU1NDEMDzQxNTYyMTg3MjYwMDQwMwwPNjAwNTM0MTUwNzc2NzE2DA8xNDg0Mzg2Mzg0NjI4NjYMDzM3NTQ0NTczODY1MjQ3MwwPNzc1MTMyNDAwNzYzODQ4DA82MTAyMjM3NjMxNzI2MjIMDzc4NDQ4NDYxMzgwMDQ4MwwPMzYxMDM3NTE4MzM3ODA1DA84MjM0MzAyMzgwNzUzNjAMDzY4ODY2NDMxMTA1ODAwNAwPMDAzNTA2NTU4NzE2MDQ3DA81MjEyNDUxMTAwMzEwMzIMDzcyMjE0NzI4NTg2MDI1MQwPMTU2NjAwNTA3MDUwMDA3DA8wNDY1NjY1NDQ0NDA3NzAMDzEzNDU1MzQ3ODU4MDA0MAwPNTc2MjcyNDQxNDgxMDYyDA82NjY2MTM2ODAzNjM0MjcMDzM1MjM2NzcwNDEyMjI1OAwPODQ4MTU3MTgyNjE4ODU4DA83NjYwNjIxNDcyODE1NzEMDzU1NDY2NzM1NzMwNTg0NwwPNjMyMDM1MTcyMzc0MTIwDA81MDM4NDI4NjEzMjAwODUMDzcxNTQ2NTg4NzI4ODA3MwwPNzc2NjI0MDEwMzAyMjIwDA8xODM2NjEwNzU0ODM3NjYMDzA3MzA3MTQ2NTUwMzQ1MwwPMzQzMjMwMzA1NTQ3ODQyDA8zMjU1NDI2NDQ3MjM3MDUMDzMwMzc4NzcwMDYxNTIwMAwPNDczODUxNjY2NTI2NTY4DA8zNDA3Mjc4MTMyNDE0NTEMDzgwODQyMzM4ODgwODMxNAwPNzUwNjM2NzMzMTAzMTQ2DA8xODE2NTEzMTI4NTU1MzYMDzIzMTQ0NTAwNjU1NTMwMAwPNTE3NDUxNzYzODQzMDYyDA84NjQwNjg3Nzg3MjE4MTAMDzAyMjMyMDI2Nzc3MzU1MgwPNTA2NjQzMTA2NTAzNTczDA80NzE3MzUwMzMxNTA1NjYMDzYzNzMwNzU2NzcwODE2NgwPODM2MzM0NTY0MzAwODE3DA82NzI3MTcxNTAzNTg4MzUMDzE4NzM4NDg2NjQ4NDI1NgwPNzgxNjE0NTYyNTg2NzY4DA84ODE2MDUyNzYyMDU0NzMMDzcwNTE4MzA4NjYzNzQ3MAwPMjgxMDE0NDcxMjc1NjIxDA84Mjc1NTQyNzgwMjg4NDAMDzc4NDE1NTMzMzY1ODc0NAwPMDA2MDg3NjMxMjg4ODA0DA8xMzYwNjU1NTYxODA2NjEMDzcwODY0MDU2NDI4NzA1NwwPNTgxMTQ4NzcxMTQyNTU4DA82ODQzNzMwMzA1NzU0MTAMDzY0Nzg2NDY1NzE2NDE3MQwPMDEyMzMwNDMyNDAzNzM3DA81MzgzNTAyNTY1MTYwMDYMDzEwNTgyNTg1MTU1MjAwNQwPMTI4MzEyNjQ0MDc4NDM0DA8wNjIwNjg4MDI1NTAyNTIMDzY0NjU4MzczNDUzMjI1MgwPMDUxNzI1NTQ4ODgyMDczDA82MjUzODYxMjMwNjEzNjIMDzMwNzM0NzUwNDUyMjEzNgwPMTM1MTA3NDAxNjczNzY1DA8yMjY0ODg1NzIyMjgwNjQMDzQzMzM4NDQ4MzQxODc0NAwPNTczMTA3NDUwNTEzMTcwDA80NDgzMDg3MzY4MDcxMjgMDzYyMzI2NTgwMzY0ODcwMwwPNjE1NTQwMTI2NzQ2NDY2DA8xMDIxNjA1ODI0NDQ1MzcMDzM3MTExNjM0NjAwNTg1NAwPNzI2NzAxMjQxMjA0NzE0DA84Nzc3NjY1NDM1MTI2NjIMDzcxMDc1MTUzODY1MjIzNgwPNTU0NDgyNDA0ODc3NTMzDA83NDYxMjQ2ODUwMzg3NDEMDzc3MjQzODc2NjcxNjE0MQwPNjIzODU1MjQwNjE3Nzg1DA8xMjU3ODU3NTAzMjYxMjMMDzY1MzY3NzA2NDgwMDY0OAwPMjY4MDI1NTQxNzIxODE1DA8yNjE3MzY3NjgyNjU2ODcMDzA3MjE4MTUwNzM2NDY0NwwPODE0NzA0NjQwNzM2NDc3DA84NjI4MzUyMjgxODcxNjUMDzM2NDE2ODAyNTIzNjYxMAwPMDg4NzU4MTIyMzQwMDIxDA8xODMwNDI4NDM4MTgxMTAMDzM1Mzc4NDEzNzA1MDQwOAwPMTMyNTA0NDc4MTI0MTI4DA8xNjI0ODY4NDM3NTQ0NjMMDzYzNjUzMTcxODU3MzcwNwwPNDMzNjQ1MDYxNDQwNTUwDA82Njg1Nzg1NjMwMzE0NjIMDzcxMjEzODExNjA1NzA4MAwPNjIyMjMwMzMzMjYzMjA3DA8zMDQwMDU4NTMyNjg0NDQMDzgzNzQzNDM2ODYwNzI1MAwPODU1NzcwMDY0NDcxMzI0DA8yNzIwMTE3NTYxNTU1MDgMDzIxMDcxMjYyNDE4MTYxNgwPMTQ2MjA1MDcxMTIzMTIwDA82MzA1MjI4MzU4NzM0MjcMDzY0MDUxMzE2MTg1ODU3MgwPNTAyNjQyMTQ0NjEwMzYxDA8wNzEwNjg0ODExMDg3NDgMDzYwNTI4NDgzODU3MzcyMAwPNTQyNDQxNjQzNzc1MTM0DA82MTM2NTI4NTAxMjczMjEMDzE2NzA1NTEzODY1MDYxNAwPNzIyODg3MTA3MDc3MzA1DA8yMjQ2NDM1MzQ0NzQzNDgMDzg1MDMyMTQ0ODI0MjY2MwwPMzc0NDc2NTUxMTExMjI3DA83MjE3ODUwODM4NjM3MzAMDzY4MjczMDY0MDU1MzMxOAwPNjE3NjcyMTIxNTEyMjM1DA8yMDQyNDM4NDIxMzM4NzEMDzUyNzcwNTg1ODAyNDE4MgwPODYyNjcwODgyNDc4MTgxDA82MjMyMjU0MjcyNTY3MDEMDzcyNDgwMzI0MjExMDE1MQwPMTQxNjI4NTM2MDM4MDExDA82NjQyNjM0MTM2MDY2MzcMDzA4MDIyNjA1NTM0MzY4NQwPMzQ0NjMxMTQ1NDY1MjMzDA80MTg1MDg4NTc3MjA2MjYMDzA1NTM4NDY3NDA1NzA4NgwPNjQ2MTE3MjI0MTU4NTYzDA8xMTcyNjAxNDg3MDU2NzQMDzc0MDAzODcxNzI2MjY0MQwPNTg1MTc1NTI2MDUwMTU1DA83MjE0MzY4MTYzMjM2NDEMDzQyMDMxNTA0ODQzNjAxNgwPNDYzMjAxNjg2MjE3MjA1DA8wNjUxMDAyMDQ1NjEzNjEMDzc3MTUxMjA3MjM1ODc2OAwPNjQ1NjMxODU3NDQ0ODUwDA8yNTgyMjQ0ODUxNDI3MzYMDzc4MjgxNzUwMTUzNTM4NQwPMTI2MzA0NjE2NzU4ODY4DA8xMzgxNjc2MDY4NDAzMTYMDzMzMTg2NjM0NzY1ODg0NwwPMjQ4NDg1NjE0MjI1MTcyDA83ODA4MTAzNjgwNDA3MDQMDzE3ODA1NjQ3NjAxMjMyNwwPMDAyMDY0NDE3NjgxMTI2DA83NjE4NjM0NzM1NTM3MDQMDzU4ODI0NjIzMzc2NzUxMAwPMjQ1NDE4MTU0MjYxODg3DA81NzQwMzAyNzMyNzM3ODIMDzU2NzUzODI1MjY4MjMwNgwPMTYyNDc3NzE2MTE0MzU1DA83ODE0MDUwNTUyNzY1NTYMDzM0NDQzMDQ4ODI0NTIyMgwPNzM2ODgxMTg2MjQ2MTc1DA84NTA4MTY3NjU1ODgxMjcMDzA0ODc0MzQzNDMxMDE3NAwPNzE1NTI0MjY3NTc3MDMwDA81NjcxNjg2MTcwMTQwMzgMDzcwNzA0NjYzMTQ4NTgzMwwPNTA1NTc0NDIwMjI3NzY0DA82MzgyNjg2MjcyMTYzMzYMDzQxNDc0NDAzNDMxMDM2OAwPNDM3ODQ0NTU4MDg2MjIyDA84NDE2MjgyODc4MjUzODQMDzgzNDc2NzYzMTQ3MTExNwwPMTgzMTc4MDYxMTU0NDM2DA82ODMwMjIxMTMxNDc2NDAMDzMwNTI1MjY2NTA2MjY3NgwPNTQ0MzIwNzc3ODM4NDY3DA8xNDQyMTc3MzAwMTgwODIMDzA0MzI0NDczNDE2MDA1NQwPMjc1MzYyMzg1MjE4MDc3DA81ODc0MTU1NTc2NjgzODQMDzI4NzQ1NDg2NzAzNzgzMQwPMjA2NDAyMzA1NjEyNjI2DA8yNTg3NzgyODA3MjA0NjEMDzg0Njc1MDYwODcyNzY1NwwPNzgzMTYwMDI4NzY0ODA4DA81NjgxNjE0MjQwODExMTgMDzA3NTcwNDA4NzA4NTY4MAwPNjU0Mzc3MDA3NTY1NDc1DA8yNDcwNDEwNzYyODEzNTQMDzIxODU3MTc4NDY2MTMwMQwPNDE0NDYxNjI3MjY3NTExDA80ODc4MDIzMjAxNjE3NTUMDzM0Mjc0ODQ1MjMzNDQ1OAwPMzUwMjcxMjYxMTM2MTEwDA8xNzA2NTI2MjMzNTc2NzgMDzg2MjYyNzAyMDUzNzg2NQwPMjA3NjYyMjEyNjI3NDgzDA80NTY0MzE1MTI0NjMyMzIMDzg4MjYyMzMxMTY4NTEzNAwPNDE3ODA4MjcxNzYxODE3DA80NDEyNjUzMTQ0NDMyMTMMDzY3NTUzNDY4NzE3MjIwMAwPMzA3MjQyMDY2NzM4MjY1DA8yODE3NjIxNTU4Njc1NjMMDzI1MzA3ODIyMzY0MjYzNwwPMTQyNzIxMDM1NTQwMDg0DA8zNTE1NDEyMDcwMzI0NzMMDzE2MTg3MTcwNDczNjc4OAwPNDU1NTcwMDUzNzE2MzM1DA8wMTQ4Nzc4NTYwNDI2ODMMDzQwNDM0ODI0MjExMTQzNwwPMjcwMDAxODI3MDQyODg0DA81Mzc2MTMzNzMyNzYwNjIMDzgxODE1ODI4NDEzNzg1NQwPMTE3MDEzMzcxMzQwMDU0DA81NTU1ODg1MjE3NDE3MzIMDzIyODI0NjQ0Mzg4MjU2NgwPMDA1ODEyMDE2NTMwODEzDA8zNDY4NjUyMjAwNjEwODQMDzc0MzY0ODIxMjA4NTc1NAwPNTQ3MzI2Njg3MTUzNTM0DA8xMjUzODczNTIwNjQwMDMMDzMxNDM3MDMzNjcxMjM2OAwPNzYxMzM0ODEzMzI0NDcxDA8yNjIyNzg1MjAxMjI1MTQMDzM1MDMyNjQ4NjI2Mzg0NwwPMjQyMzY0NzIwMDYzMzc2DA8wNTY0MjMzNTUwMDQ3MDcMDzYxMDg2NzU2NzM4Mzc3NwwPMjY4MjM3MDUwODU1NDYyDA8zNDc0NTg2NzM2MzE1MTcMDzc2NTI2MzAxODUwODMzNwwPODU1Mjc3MzMzNTM1NzQyDA83MTUxMTM1MzQwODMzNzUMDzEwNjMyMjg0ODA4MDUzNgwPMzU2NjcyMjEwODY4ODU0DA81MjMyODUxODE1MDM0ODIMDzM0MzQzNzg4ODcwNDA3NgwPMDg3MDIzNTIyODA0NTEyDA8yNzg1MDI4NjA2NTUyODMMDzMzMTA2Njc4NTA3NDY2OAwPMzc4NzA2MzgwNzYzNTIwDA8xMTE4MDEwNzI4MDY1MDIMDzYzMDc2NzUwMzEzNTQzNAwPODAzMjgxNDAwMDc3Mzc4DA8yNDE4MjE4MDg3MzYxNDYMDzQwMTE4NTA1NTA3MTg0NgwPODgyNzI3NjYzMTE0Mzc4DA82NDU3Njc1NjYwNTIxODcMDzc2MDMzNzE3NzE4ODE2NwwPMDYzMzYwMTUyMDE4MjQwDA8wMDE2NjYyNzc0NTMzODgMDzI2NDMyMzI0NDc0MjI1MgwPNjMxNDE4Nzg0NjA0NDU2DA80MDEzNzg3NzY3MjU4NzEMDzIxODQ0NzExMTMxMDI0MgwPODYwNzY1MDQ4Mzg1NTIxDA82NjYxMTQ1NjYwMDI2NjYMDzQxNDc2NTA1NjUwMjM2NAwPNzY3MjM3NjU2ODE4NjAxDA84NTUzNzgwMTU1ODMwODMMDzM2MDcxODcwNzU0MTgwMQwPODc1NzQ1NDA1NDcwNzU0DA8yMDQ0NTc2Mjc1NjIzNTgMDzQ2ODE1MzY1NjgwMTEyMQwPMTA0Mjg1NTcwNTczMjMzDA80MDEwMDQ2MjUzNjc2MjAMDzYzNTI4NzQxMTcwMjE3NwwPNzE2MzM0NTIxMjQzNjA4DA80NjgwMDE0MDAyMzcyNDIMDzQ0NjU0NDgyODU0MDE1NwwPMTIyNDUzNTUyNzEyMjc3DA81NTAwODg0MTE2ODAxMzAMDzcwODEwMTM4MDgzNDg0MwwPMjU1NjcyNDczNTU3MDc2DA84NDEzNzM3MjYwMTA0MTcMDzI1MDA2MjI1ODYxNTE4NgwPMTQzMTA0NTc1MTgxMTA0DA80MjM0MzIyNTgwNDA1MjAMDzAxNDAyNTE4MDY3NTQwNgwPNjI1MTcwNjg3MzgyNDI3DA80MDA1MTM3MDEwMzcyMDcMDzAzODEzNjYwMDUxNzUwMgwPNDU2ODM0MTE4ODMzMjA1DA8zNTUyMjgzNDI2NDM3MjcMDzc4ODI4ODU4NDc3NjM2MAwPODY2NjA0NDI0NjYzMzIwDA8yMDI4NjE4MDE4ODYxMzEMDzU0NzIzMzc4NTg2Njg2NwwPNDgwMjgwNjgzNzgxODQxDA8zNDA2NDc3NjYxNjIwNzUMDzc1ODEyNDIzNjE0Mjc2NgwPODIwMTgyODY3MTI0MjY1DA80ODM2MTM1MTUwNTM4NTAMDzI2NTgxODAyMjc1MDUzOAwPMzc2NTExODIzMjAwMTAxDA8yODg1NDgwMzEwMjQ2MDAMDzMyODg1ODE3NjU3MTcxMQwPNTEwMjQ4MTI2NTY1ODAxDA80MjQ0MjI2ODEyNDQ1NzcMDzMyMTEzNzYwNjc3NTI3NAwPNDAxMDY1ODY2NjQ0MDE0DA81MDc0NDc3NTM0MDAzNTMMDzAzNTg1NDc4MDY1NDU1NQwPNDc0ODQxMTI1NDE3NTgxDA82Mzc3Mjg4NTYzNTY0NzMMDzQ1Njg2MDI2MDQwNjU0MAwPNDM1ODM3NTQzMzQyNDc3DA8yMjg0MjAxODUwMDI1MzYMDzQwMjg4MDY0NDUyODQ2MAwPMjIwMjU2NDI4MzgyODgwDA8yNjEwODI1NjgzNDY4NTIMDzQzMjcxMjQ3MzYyMTU4NQwPNTYyNTI4MjA3NDg2MTgxDA81NjU3MDUzNjAzNTA2MjYMDzU3MTYxNDU4NTA1ODAyMQwPNjE1NjI3MTQ2NDM2NTcxDA8zMjI4NzgxNjg1NDA2ODcMDzYxNjcwODM3ODMxMzQ4OAwPMDAwODc0NDE4NTQyNzIyDA80NDM1MTIxNzU2MTg0NjUMDzgxODQxNTgwNjA3MzgzNAwPMjc4MTczNzg4MjE1ODQyDA8yODM1MDA0MzE4MzY1NzIMDzc0NDEzMjUzNjA0ODQ4MQwPMTM0NTU0NTMxMzIxMzQzDA8zMjAzMjQxMTMzMDIwMzQMDzAzODUzNTM1NTc1ODM4NwwPNjg1ODY4Mjg3NzIxODUzDA82NDI1NDIwMjA1NTIxMzMMDzA2NTg0NDM1NzU0NTU0NQwPMDcwMDUwMzMxMjI2NTY4DA84MDQwODQ4ODcxMTgwNzYMDzU0ODIzMDUxNzEwNTI1NQwPNzc4MjQxODQxNjgwNTQ2DA84NzgzNDc4ODYxMDgxMDAMDzMzMDg0MTU2MTgzNjMyNQwPMzc4NjMxMTA2MzQwMTIwDA8zMzUwNjE3ODYzODIzODcMDzAyMzcyMzY2NDM0MTYxNQwPMjMwNjU0MjE4NzE2ODE0DA84MDM4NjE4ODgyNjQwNzQMDzQzNzI0MjIzNDIxMzI4MQwPNTA3ODEyMTg2NjAzODEzDA8wNzg0NDMzMjYxMzQxNDIMDzY4NzgwMTc0NzU3NTM0NQwPNTQ3Mzc0MTUxNTUwODgyDA8xMzE3NDA2Mzg2MzIzNDAMDzQyNzY4NTU1MjMwMDAwNwwPODExMTA1NjIwNjU4MjE2DA83MzU4NjM0ODM0MTE0NTMMDzQ0MjgzMTczNzgwMjIxOAwPMzMzODQ1NjQ0NDUyNzE1DA8wNjQzMjI2NjUyMTE1NTgMDzQzODYyNjcyODcxNjEwNwwPMjg1NTEyNzQxNjUwMjE4DA81MDcyODExMDg4NjA3MzgMDzYxMDEwODExNTQyMDY0OAwPNDYzMjcwMzI2NjMyNTUzDA8wNjE2MDE4Njg4MzgyNzUMDzAxODMyMTYyNjQwNjg2OAwPMzA4NDI0NjIxMDUxNDAxDA8xNjIwMDMyNDUwMzA2NDAMDzQ0NzA0Njc2MzY4ODQ0MQwPMzgzMTA3NzI3NTMxNDEzDA80Nzg3NjE1NDY2MzE0NDIMDzI0MzUxNTc4NDU4NzAxMAwPMjE0NTY0MTExNDA4NjIxDA83ODE1NDA0MDUyNTQzNzEMDzM0MTg0MjIzMDI0Njg4MgwPNjEzNDQ3NDAxNjcyNjQ1DA8zNDIxNTg3MTAzNTU2MzYMDzEyNTU0MjI0NDUyMjE4MgwPMjExMTcxNTgxNDYxODMyDA8yNjUwMjU1NTAyMTMzNTUMDzcwNzMxODQ4ODMxNTM1NQwPMjM2NzYwMDU3ODQzNzUwDA84NzY3NTYyMjIwMjY3ODYMDzAxNzMwNzUxNzg3MzUyNAwPMTMzODMwMjgzMTQxNTc4DA80MTEyMDUzMzc3MTU4MzYMDzA3MDA2MjEyNDM1MjgzNAwPNTUyNjY4ODc1MDY1MjU4DA8yODE3MDM0NjU3NjYwMDQMDzUxMTY2MjE1NjYwNjgyNAwPNTIyMzA0MDgzNTY0NDUzDA8wMDczMzA3ODMwMTcwNzYMDzEzMjE3NTA1MTUyNzMxNAwPODIzNTA1MjMwNDE4MzM4DA83MTA4NTQwNDIyMTYyODUMDzAwODU0Mjc1NzQyNDgyOAwPNDYxNjg0MzM2ODExNTQ3DA8yNDU0NTE4MDMzNDU0NTMMDzY1MjUwNjc0MzY2MDU1NwwPNTEyNzgwNTg3NjQ2MjE4DA83NjUyODQ0NjA1NjQxODIMDzM0NDM0MzYwMzYwNjAyNAwPMTEzODcwMjg0MjE2MDM0DA8xNjAyODUwMzQzMjQ3MzcMDzIxMTcwODQzMzU2NjQ0NAwPMjE1NjE3NzUwNzA0NTE4DA8wODQwNjA1NDA0MTE4NTUMDzUyMjQ2MTUxMDU0MDI2MgwPODA4NTU0ODQ0NTM0NTcxDA8wMTEzNTYwMTEyODAxNTQMDzg1NjI4MzU4NjE4NTU4MgwPMzcyMTU1MzczMjc0MDMwDA84NDQwNDc3NzM2NDQzMTcMDzYwNjc3NTgwNTg1MTE0NgwPNDQzNzE3NTAyMDQ2NzgyDA81MTQzNTU2MzA4Mjg4ODAMDzI3MzI1MTY0NjAzMDY2NwwPMTUyMTQ3ODEzMDg4NDE2DA83Mzc1MjI3ODc3MDgxNTMMDzIzNjgxNjAzMTcyMDM4NwwPNDI2NzYzMTIxMTIxODY3DA83MDAxODQwMDA4MTQzNTcMDzYyMTU4NzY3NzE0NzUzOAwPNzYwMjY3MzMxMjUzODIzDA83MjYxMjgxNjIxMDExMTIMDzcyNzI3NzEwMjcwNTI2NQwPNzQ3NTM4NzMwMjI3MDg0DA80NTYxNDgzNzAwMDg2MjUMDzg0NDYxNDg1MjA2ODMwMAwPMjY0MzQyMTE0ODAyNTMzDA83MDg4NjQyMTYzNDI4MTQMDzEzMDY4Njg1MjMwMTIxMgwPNTAyMDc4MTc1MjcyMzYwDA84MTE4MzMzNTY2MzEyMTUMDzg2NDE4MjE0NjU3ODU4MgwPNzAzNzg1ODE0NzMwMjE4DA81MTQ3MzM0NzQxODczNDIMDzI0MDUzMjQ0MDI0NzYyMQwPMDMxMTc0MTU1NTYxMTU1DA83NTA1Mzc3NjU4NzA0ODUMDzYwNzM1NDUxMDg4MzA2OAwPMjA0MDM4Mjc1NDQ0Mzc2DA80MjY2MTQzNzcyNzQ0NTIMDzY0MDUxNDc1NTU2ODQzMwwPNDEyODE1ODUyMDc0NTM0DA8zNDgzNzY1MTMzNzU3MDEMDzg4ODc4ODI3NzEyMzIwNAwPNjM3NDE0NTYwMTc3MDE2DA83MDAzMjM3MTIzNzg0MTUMDzQ4MzE4MzQwMDgxMTc0NgwPNDgwMDI3NjE4Mjc0ODU2DA81NTYwNjIxNTE3MjQ4MTIMDzIxMDg4ODQzMjU0MzYzMQwPNDgwMjMyMjE1MjI3NzU3DA8yMjUxNzQ1NjI0NjMwMTAMDzA1NDExMTUzMjIzODQzMAwPMzI1NjI4MDIxMzIwNzA1DA8xMTU4Njg2MzQ2NDAyNTYMDzI0NDE0NzQ1MTYzNzYwNQwPNzgwODM3MjgzNTY4NTMyDA8zODEwMTg1MDg1Nzg3MjMMDzY0NDU2NTIwMjU3MTMyMwwPNDE1NTMwNTYzMjI1MjEwDA8yMTQ3MTA0NzU3NjA3MjQMDzc2NzUxNDM3Mjc1NzMwNQwPMTUxMzQ1Mjg3Nzg4MjU3DA82NDUxODIzNjU3NTQxMDgMDzIxMzIwNTczMjU3NjUwNQwPMjQ1Mjg3MTEwNDY2NzMwDA83NjM1MjExNzc1NzcxODcMDzgzNjQ1ODQ1NTUzNjgzNgwPNzIyNTMwNDA0MzU3Mjc4DA83NTM3NTE0NTgzMTgwMTEMDzU4MTM2MjIxNzMyMTI4MwwPMzg1MTgxMDExMTYzMTMwDA84Mzg2ODA2MDc4NDgxMzgMDzg2ODI1MTMzNzIyMTY2NwwPMjEzNTEzMTY0NDc4MjI4DA8zODQ4MDgzNzg4NDEyMjMMDzIxODEzODU1NDQ0MDM0MgwPMTMxNzMwMTg3NjAzMjAzDA81NDg2NzMzNDA0NzgxMjQMDzU3ODYwMDAzNDcxNTM3NgwPODIzMzc0MjI1Mzg0MjMyDA80ODMwODE0MTMxMDU4MTYMDzExNjQ0ODg1NTE3NTgwNwwPNDE1MjMyMTg2MDI1MTg0DA80ODIwMTI4MTU4MDQ0ODQMDzMzNjYwNzAyMTU3NjczNwwPMTE1MjE3ODcyMDczMTU3DA8wNTYxNDgwNzc0MTI1MjcMDzA0MDYxNzAxNDIwMzcwMQwPNzQ1MDY0MTI2NjE1MjI4DA80NTEzNjY0MjI0NzMxNTMMDzU3NjI0NDQ4MTc4MjgwNAwPODg1NjgyNDgzMzY1NDEzDA8zNjgzMzAyMTY0MTAyMDEMDzE2NDU1MDMyNjQ0MjEzMQwPNzM2Mjg4MTM4MTgxMDExDA8wNjQxMjE2NDEzNzgwNTQMDzQwMzc3MTA2NjY0MDU0NgwPMTU0MDc2ODA1MTI0ODY1DA8zMDEyMTY2NTUzMDU0NTMMDzQ0ODA1MzU3MTMwMTE4MwwPMDAxMDI2NDM2MjMyNzAwDA80NTA3MDMzMTAzODQxMDgMDzcxMzI2Mjc2NTU3NTYwNQwPNDI1MjgwNDA3MjU2MzUxDA8yNTYwODc3ODM3MDg2NjMMDzE4MjMxMjE0ODI3NDM0MQwPNTQ3NDQ0MDYzNTU2MzE4DA84NDY4MTYwMTM3MTg0MzgMDzg0NTA1MzU1NjU3MTIxMQwPMjg1MTEzNjY0NDE3NjQzDA83Mzc3ODMyMjcxNzExNTUMDzM2MjQ1NTczNjI0NjA4NQwPNjE4NjU1ODEzNjE3MTI0DA8zMzczMjUwMjcyMzQ0ODAMDzg1MTI0NTA4MDg4ODMyNgwPNjQ4NjM2NTA1ODQzNTM0DA81MjgxNDAxNTAyMzg4NjMMDzI2NDg4MzcyODc3NTI4NAwPNzc4MjUxNzUwNDYzMTM4DA84Mzc0Nzc3ODM4NzU4ODcMDzU2NzQyMDgxMjYwMzc1MgwPODExODE2MTY1NjIyMjgzDA8zNzY0NDY4NjExMjU3NTgMDzE3MzEzODc2NTcyODIzMgwPMzgzNTExNzc3ODQ4MTAzDA8xODAxODc0NzMzNTI0MjgMDzAwMDc3NTQyNDgwMzY4NQwPMDIyNDQ1MTMzNjI2ODcyDA83ODg4ODM2Nzg1NDgzMjgMDzExNDQ1NjI1NDc3MzA3MAwPMTUzMTM2NjUyMjY4NDQ3DA82Nzg3MzMzNTI2MTA4MzEMDzA2NzQ0NDMzNTUyODA2MwwPODMzNTc1MzIyODA1NTMzDA8zMTc3MzU2MjAyNDU1NTYMDzEzNTcxMDUyMDA4NjI3MwwPMjc3NTE3MTUxNTA2MzgzDA8yODgwMjYwMzc2MjExMzMMDzYxMzc4MzY2NTU3MDM1MAwPNjM3NzIwMTc1Mjg1NDI4DA82MDY0NzM3MzM0MTg3MzYMDzMwNzE2MjEzMzY0NjM2MAwPMzEwODY4MTcwNjQwNjYzDA8wMzQ0MzA1MTI1MTQyNTMMDzUyMjQ4MDExNTcwMzYwOAwPNTIxMjMzODI4MTY1MjI4DA81MTM4MDg0ODUwMDg3MDYMDzQ3ODI1NjE0MzM2MDU2NQwPODY1NzUyMzA4MDA2Njc3DA8xNjMzMzI0NjIxMTEzNDQMDzQyNTYxMDI4NTI0MjM4NgwPNTU1MDEwMDM1MTIxMDc4DA83MTM4MDc0ODQxMzY4MjcMDzAzMzczMjc4NjAyMzAzMQwPMzM1NDU0NzgxMTYyMDY1DA8zNDIwMDQ0NjE3MDQ2MTQMDzg1MDcwNjM2MDIxNDQ3MQwPNDU2MTI1MjA4NzEyODU3DA8zMDAzMTUzODAwMzQ0NDEMDzY0NjQ1NDQ1MDEyNTM1NwwPNjU2MTA3MzQwNjgxNzI0DA80NTYwMDI4NTQ4NzI4MzMMDzg0NzIyMDAxODE0NDc3NQwPNjMyNzE3NDM3MjEzMTYxDA8xODI2MDczNjMxMTUyMDYMDzU1ODA1NzU3NzgwODIxMAwPMjQxNjEwNzEzMTMzNTgyDA8yMzMxNzcyNTc1MjY4ODEMDzY0NTA1MTE1MTgyMDE2MgwPMjAzMDE2NDY3NTEzNDI2DA81NDY3Mzg1NTM0MDUwNTEMDzQ3NzQ0MDEwMTcwMjMxMgwPMjE3ODE3NDQ2NzgzNjIyDA82Mjc1MjQwNTI4MTYxMjYMDzY4NDI2MDE2MzIwNDYzNgwPMTgyNDg4MDUwODQ3NTY4DA84NzI3MTc0MzY4NDU1NDgMDzUwNTQyMDU1MzI2MTY3NQwPNjQwNjMxNTAwODAyNTUzDA80NTQ2NzUyMzI2NzY2MjUMDzcxMTI1MDg4MDMxMzIyMgwPNTE3MDc4NTUwMzc3NTc1DA8yNzYwNDcyNzEwNzE1MDAMDzI3NTY4NzAwNzIyNjI0NgwPMzIxMDM1MDYwMjE3MzUxDA8yNTg3NTY2NDEwNTYxNDMMDzI0Mzc1MjczMTI3MjQxNAwPNTc2MDIxMzgyMDM0MzQzDA83NTQ3NzI0MjEzNzM2MDgMDzEwMTMyODg2MDAyMTA3MwwPODM3NDEwMjUwNzA2NzE3DA8xMjEzNzY3NDgzMjQ2MjEMDzE4NDQyMDgyNzQ0MTIwNwwPNTMwNTE4MTEzNTM3NTgyDA8xNzE2MTY3MjcwMzc4NDAMDzQzNjUwNTg1NzUzNzU1NgwPMTMxNzA1NDY0ODY2MDMzDA8xMDcyNDEzNDg1MDAyMzcMDzg4NzE0ODU2MzczNzE3NQwPODgwNjM0MDIzNDA2NjY2DA8wMDM2NjYzNzAzMTUxNTUMDzEzNDQ0MDYyMzY3MzUzNwwPMDIzNTEwMDcwNTgyMjYwDA81NTA3MjY2NjQzODYxODAMDzc3NjQzMTA0MzA3NTU2MQwPNzU2MzAyNTMwNDg1NzYyDA84ODM3NzEzMzgyMjgyNDEMDzI4NzYxNzcwNjQzNDc4MQwPNTU1NjA2MjgyNjIyNDU0DA81NjExNjg2NTIzNzg0NzAMDzg4Mjc3MTAzNDQwNTM4MgwPMjc1MDE4NDIyNTE1MjA2DA81MDcxNjUxODcyODgyNDEMDzI2NzE0MjQwMTg2ODExMAwPODYxODI2NzI1NzAzNzczDA81MDQxMzAzNzgyMDA2NDcMDzIyMjEzNTI0MDM0NzUzOAwPNjc2NDY1MjMxNTg2NjM0DA84MjQ0MjM3ODgxMDI1NDgMDzM2MTI2ODc2NzczNTQ2NwwPNTI1NTIxNjE0MTU3MTI3DA80MzI1Mzg3NTgwNTA0NTIMDzcwNjAzMzEwNDgyMzMzNwwPNDcwODExODA3Njc4NDU4DA8wMDg2NjUyNzUzMjgxNzIMDzE4MTg1NjU1MzQ4MDA2MwwPMzc4MTIzMzI4MDE2ODI1DA83MDIzNDMyMjczMTA4ODgMDzQzMTg4NjM1MDY3NzcxNwwPMzQ1NjA1NTIzMjE0ODY4DA8zNjA1MzQ2NjU1NjUzMzMMDzI0Mjg4NzE3MjIxMDczMgwPNTY0NDEyNzI3NjY0ODIwDA80MTUwNjAyODA0MDIyNDYMDzEyMjQ1MTg2MjQ2MzYzMAwPNzA2MDg0NTQxMzQwMDI2DA8wMDA1Nzg3MTI0MjcwMjIMDzU1NDc3Mjg3NjQ2NjUwMQwPNDE0MDQ1MjcxNzA1MDU0DA84NzQ4MjU0MDU2MTI2MDQMDzQyNTU2Njg3MzUzODE2NwwPMDMwMzI3NzE0MDI2NjIxDA82Njg3NTcwNzcxNTIxODUMDzA4NDI0Njg2MzE2MDYwNwwPNTgyNDAwODYxNzgzNjU2DA83ODE0NTg2NDEyMDU4MjMMDzEzMTcyMDExNTY3NzMzNgwPMzI1ODQ2MjcwNjI0NjEzDA80MDUyMDY1NTE0MTY3MjcMDzUwNDA1MDA0MTMyMjc2NgwPNjU0NDIzODI2NjA0ODQ2DA81NTM2MzUyNDc4MDQ0MjEMDzg3NjAwMDMwMzE4MDYzNgwPMDY2ODUzODc2MTIxMjA3DA83Nzg1NjU3ODI2NzAyMTEMDzUzMDgzMjM3NDIyNjU3MQwPNDQyNjUwODIzMjIzNjE2DA80NzAzNTM4Nzc1NjExNjQMDzg0NDI1ODY1ODc1NzMyMQwPMTUxNzYxODA1MjMwMjY1DA8xNTM4MzAyMzI3MzI1NTIMDzIyNzAxMTQ2NDcxMzQ3MQwPNTExNDY3MTM0MDY1MTM4DA8wNDExNDMzMDgxNDMzNTQMDzM3NzEzNjgwODQyMTUxNQwPODY3NDgwNTE0NzEwMjMxDA81NDI2NzIwMDQ2MDIzMzcMDzM4NDgyNDg4NTgyNzMxNwwPNjY4Mjc2NTAzODY2NDg0DA80MTEyMjIxODUxMDE4NzIMDzI3NTU4MDA1MDA4NTIwNAwPNzM4NzI3NDIzMjUzMzYzDA81MzgxODYyMTEzMzEzMjQMDzY0NTA0ODgzMjc2ODY4MwwPNjUwMjUwODIyMzA1MzA3DA83ODU1NDI2NTM3ODUxMDMMDzY0NzI4ODYyMzYyNTI2MAwPMjgyNTY3Njc1NTY3NjI4DA84MjA1NDE3NjI0MDg1MzcMDzY4MDIyMzc1NjQ3NzM3MgwPMzEwNDE3NTMzNzg1NDUzDA8zMzYzNDc1Mjc2MTEyNTUMDzQ4NDYwMTQxMjEwODQwMAwPNzQ1ODQxMDI3MDE4NTE4DA83ODI2NTA2NTU1MjUwNTAMDzExNjA4MDMxMjc2MjQ0NwwPNDc4NDg4Njc4MjIwNTQ1DA81NzE1MzM4NDQzODQxMjcMDzUzMjc4MjM1MTEyMzA4MQwPNzAwMjg3MTY4ODc2ODUxDA8yNTM2NDEzNTIzODMzNTIMDzQ4Njg4NzQzMzA0NzA0OAwPNzgzODAzNDY1Mjg4MTAxDA8zNTgzNjU1NDU4ODE0NDcMDzAzMjQwNzU4MTczODc1NgwPNDA0NDQ3MTQ1NTQ3NzQ0DA81MDc2NzE2NDM3NjExNjgMDzc4MTUxNDE4ODgxNTIzNAwPMjYzNTQ3MjI3NjY1Njc2DA81NjM1NTQ1NTM1MzU2NjEMDzE0MzU2MTMwNDU2NzU3NAwPMTM0NTY1MjY3MTA2MzcwDA8wMDAyMjcxMTMxMTYxNjgMDzEwNDExNDg4NTYyMDc1MwwPNTIyMTIxMzAyMjc1NzQ4DA82MzU3MTc2MDQ1NTM0MTYMDzcwMzMwMjE3MTUxODMyNAwPNjExNDQ1MjY0NDE3ODczDA84MDc2MTgxMjM4MjI4MzQMDzQ1MzUxMTM0NDc2MzYyNgwPNTI0NTc3MjYxMjgwODI2DA83Njg1MTMwMzExODUxMzEMDzM2MDY4NTE1NDE1NTMzMwwPNjEyNTYxMDIwMzI4ODY1DA83MTYwNzEzMjg2NjYwNjQMDzQ2NDA4MTIzNDM3MzMyMQwPNzAyNjMyMzE2ODU0MjA3DA80ODc1MzY2NzYwMjQ3MTAMDzE1NjQxNDI4NjI1NjUwNwwPMDc4NjM3MjEwNDMzMzg2DA8wMjQxNzA4MzgzNzgwNjYMDzUxMzgwMjIwNjI0MDcxNAwPNDMyNTM1ODMyMjYxMTQ4DA8zMzUyMzExMjEzNzc3MzgMDzEwODM3MDgzMDYxMTE3NwwPMzE4MTE1MzMwODU3Mzg4DA8zMTM3NjAwMDUxNzg2NTAMDzg3MTE4MTgwMTUwNjE2MwwPNzQyNDU2NDAxNTMzODg0DA84NjE0Njc0ODEyMzE1ODIMDzg3MDMwMTIzODU4ODA3NwwPNjM0NzE1ODg1MTI0Mjg2DA83Njc2MDczNTczNzU2ODEMDzIwNzAyMjM1NDAyMDgwNwwPNjE1MDYyNjU2NTIzODE4DA8xNDQ4NjcyMzY0NzQwMTMMDzgwNDc1MjY0NzY4NDgyMgwPMDYyMDMwMDEzMDExMDg3DA80NDIyMTY3Nzc0MDg2MTcMDzE2MDQwMTgxODAwODUwNwwPNDY4NDU4MzE0NDQ3NjUxDA8wNDgxNjAxMTc0NzExNTMMDzc2MDA2ODc4MTgzMzY4OAwPNTY0MzM3ODcyMzExNzQ2DA8yNDg1MjcwMTQ2NDcxNzAMDzg1NDM4MDM1NTcyMTUzOAwPNDA3MzU3NzIyMDc2MTc1DA8xMDUyODg2MDMwMTExMjYMDzc0NDMxMDM3MDQ0NjUwMQwPMjE3NTc3NjM0MjQ0NDM1DA84NjM1MTI2NjEzMzc4NDcMDzEyMTM2NjUyNzU3MDQzNQwPMTg0NDg4ODA2MjA4MjczDA8xNTc0MTA1MzAxNzMyNDIMDzAwNjE2NzIxODE4ODc3NAwPMjI3MzMxMjQ3MzU1MzAyDA81MDMwNzc4MjY4NTQ0NzYMDzMyNjMwMDg0NTI0NTY0OAwPMzM3ODM4NTI2MTAzMDUyDA8xMjYyNDE4NDQ2NDE2NDUMDzE1MTM0MjU3MjQxMTA4MwwPMjEzNTI3MTgxMDgwMDU2DA8wMDg0MzMyMjIzMzI2ODcMDzg1NDM0Mjg4MDUwMzQxNgwPMjM2MzE3MzU3NjI1NjUxDA8xNTUwNTEyNDY4NzA1NjYMDzQ0MjY0NTI0NTEyMDcyMQwPMzIwMDM1NzI3NzYxNzIxDA82MzIyNjg0Njc4NzY2NjMMDzI2NDY4NDMwNDIwNzUyMwwPMzMxMjEzMTM4NzY4NzM3DA81MTQxMTczMTY2MTI1MjMMDzQ0NzE0MzAxNTIwODM1MgwPNzg4NjExMTIyNzI2MzE0DA8yMDM4MTg0MDM1MjcwMzMMDzgxMzEwNjU1MDMxNjYzNwwPODc0NTA4MTM1MTgzODg0DA81MzMxMDIzMTMxMjQzNDcMDzM1MzY2ODYwODUwNTgxNwwPMTE3NDc2MDc0Nzc4NTIzDA8wMjczNjI4ODYxMDc4ODQMDzY0MTMyMDM1MjUwNjQ1OAwPNTMwMDA4MjA2NzA2MjAzDA8wNjA2Nzc0MjU1NTU1ODQMDzg2MzU4MDcwNDgwMjI2NgwPNjcyMjYxMDgzMDA1MTI4DA84MTM0MjI2Njg0ODMzNjAMDzcxNjc2NjU3NTI0NDU0NQwPODgyNDMxNzAzMzIyODUzDA82Mjc0MzE3NTEyODQ0NTUMDzY2NzAxMDE0MjcwMjczMQwPNTgyMzAyNjAzNTExODY0DA81MjExNjMwMzAxNzgwODcMDzYwMTY2NDY0MTc0MTQxOAwPNjU2MjAyNDEwMzM1MTEzDA82MzMwMTEwMDc4Mzg3MTYMDzUzNjYxMDE1Mzg4MzMwOAwPNjM1MzY0NjU3MDI4MDI0DA8wMjU4NjgzNDQyMDE3MjAMDzYxNjUyMzg3MDI4MDU1MAwPMDU2MDczMzMxNjQwNzE1DA8xNjIxNTUyNzY4MzAzNTAMDzIyNDQyMzcxMTUwMDA1MAwPNzMxNTgwNTA2MDg3NDUwDA80MTI1ODA0MjMyMjI4MDcMDzg4NjQzMTAzNzc0NDU3NgwPMTU3MDA3MjY2NDI4MjQ0DA81NzIzNDQxODU3ODczNjUMDzM4NDU3NjM0MzU3NjY3NwwPMzE3ODA2ODQ0MDUyMjYyDA80Njg0MTI4Nzg4ODgzNzYMDzUzNzMwNjc1NzY1NjEyNAwPMTQ2NDMyNTY0NTQwNzI1DA8xNDMwNzUzMTg4ODE4MzAMDzI1MzQ4NDIyMDU1MjQ2NwwPMjg3MzM1MDQzODg1NDM3DA8zMDMyODY3NDczMjE1MjIMDzg3NTUwMDU1MzQ1MTU4NgwPMjU0MTg1NzIyMTYwNTIwDA8zMjcyNjI1MDY1MzgzMDcMDzExODIwODMyMjI3ODE0MAwPMDc2NTExNDYyNTAyNzA2DA81Mzg3MzY4NzE1MTM1NjQMDzU3MzQ0NzM2NjY0NzIzNwwPODIwNDUwMjI3NDMwMDM1DA83NDQ0NjU4NzUyMTE1MzcMDzczMzc0Mjg4NDE0MjYyMwwPNzc4MjQxMDYwMTY3MjE1DA82NDA3MzAxMjQ2ODg3MTYMDzMzMDY1NDA1NTIyMTY1NQwPMDQ2MTQzMjIxMjQzNDM0DA84MTIzNzI4ODAyNzI1NDUMDzI1MTA2ODYzNTUzNjUyMgwPNzQ3MTg1ODU3NTcyNjQ2DA8yMTAyNjc2NTQwNzM0MjEMDzIyNjI0MjgwMDQ2NTQyMgwPMzg3MzQwNDc3NzQ2NDQyDA8zODY3MDczMzY0MDA4ODIMDzMwNjYzODU0NzAwMjE1NAwPNjcxNjYyMjc4MzUwNzIyDA8wNzE3NTY1NDIyNzE3NzQMDzA4NzIxMjIzMzE3MzM0NAwPNTAyMzAyMDYzMTgwODgwDA82NjEzODg4NzcyNDAyNjQMDzc4NTgzMDg2ODI3NjA0MQwPMTUzMzAwNDQwMTUxMDAxDA8xMTEzMDMxODA1NjcxMzAMDzQ4MTU3ODY3NzQwNDUyNQwPNjE4ODE3NDcxNTc1NTE2DA8zMTczODEzNDE0NjgzNDAMDzM4ODU3MjI1MDAwMjgzMQwPMzE4NjI0NDA4MzU3NTU1DA8xMzIyMjI4MzM4NTU2NDIMDzcxMTMyNDM3NjEyMDAxOAwPNTIwNTYxNDA1MDYyMTQ1DA84NjQzNDY3NDA4MTEzMDIMDzIxNDExODg1MzUzMzM3NQwPMjc0ODg4NTc1MDU2MDAxDA83MDIxMTY4NDU1ODExNzMMDzc3NTY0MTAyODg4MTQ4NgwPMTgwNjY1MTc3MzU4MzczDA83Nzc4MzQyNDAxMTIwMjEMDzUwMDUzMjUyODI3NjgwMQwPODE4MjAyNDYxNTg0NTQ2DA83Njg0NTMzNjM2NDcyMzIMDzQ4MDE3Mjg3NzQ0NzI1OAwPODEzNjE0MDM0NDU4MDI4DA8yMTUzMzYzMDY1MjgwMDQMDzU3NDg3NTYxNjI0Mjc4MgwPMTYxNjU4NjU2Mzg4NzgwDA8zNjI2MTE3MDE3MTU0MjgMDzQxNjU0Njc1MzAwMDI0NQwPNzQ3ODc2Mjg3NjI4NjQ3DA8zMTA1MzM4Mzg3NjI2MjMMDzI2MTM0NzE4MTU4MDgyMgwPMzI0NDEyNDA3MTExMjIyDA80MDg4NTA0MzgzNDE2NDQMDzMzNDEwMTA1NDIwODIwMQwPODMwNDQwNjI4MTgwNjY1DA83MzQ4NjQzMjE2NTc1MzYMDzQ4NzU2NTE4NzQ3NDIwNwwPODcyNDUzNTU1NTUwNDMwDA82NTA4MjUwMDg3NTQzNTIMDzUxNzg4NjUwNjg4Mzg0MAwPNzY1NDExNDY0MDg1NDE1DA82NTc2NTc4Njc3MTg4NDYMDzU0MTMzNzgxODcwNjUyNAwPMjMwODg0MTM0MTMzNTE3DA8xMjQwODA3NDAwODM3NzcMDzU1MTUzMzg3NTc3MDQxNQwPNTcyMzIzODY4MDU0MDExDA84ODgwMTI3NzYyNzYyODEMDzY3NjM4ODA1NzIzMTgwNwwPNTcyMzUxODMyNjg3Nzc0DA83MzQxMTUyNDE2MTgzNjQMDzQ0MjM1MTE4MDY0NTYxMAwPMDYxMDcwMzMzMzY3MDU1DA8wNzc1MTYwMDIwNTU1MjQMDzUwMDY1MzgwMDc0Nzc1MAwPNzMwNzEwNjQwNDY1ODcyDA82MzgwNzE4ODgzNDY1MjIMDzA4ODM1NzQ0MzA1NjMzNQwPMjI4NDQ2Njc0ODY1MDEyDA83MDA3MzEyMTc0Njc2NjAMDzEwNjU0NTYwNDA1NDU1NwwPNTEzNjU1MjExODE1NzU2DA81MDYyNzgxMjYzNjIxNTgMDzEzMTEwODI4NjQ4MjM0MAwPMTQxMzI0NDU0NjcxNzY4DA8zMjg1NjEzNzU1MDYzMzYMDzczMjQ3MTUyODM2NTQzNgwPODE2MDU2NDczNDg0MDYxDA8yNDYzNzY1MTg2MzgyODMMDzA0NzQ0ODE2NzM2NTIzMwwPNDI2NDcxODY1MjQ3NjgyDA81MTczODMyNTI4NTM2MzcMDzI1NTg0Mzc0MTQ2NDUyMgwPMDAwODAxNjIzNTEzNjYyDA8xMDA4Nzg2NjUyNjM3ODUMDzU4MjU4NTc0MzE4MDc2NgwPNTg3NzQ2MTQ0NjA0MDc2DA8xNzEwNDEyMDgyNTU3MjYMDzc4MDU2NDEzMjE2Nzc1MwwPMjA4Nzc1MTExMTcyNDI4DA8wODM4ODY3MDY0MTA1MjgMDzUxNjM1NTQ1MzczNDY4NQwPMTUxMjUwMTA2ODgxMjgxDA83MTcxNzQxNjI3NTg1MjQMDzU4MzAwMjEzMTIzODM2MgwPODI0MTc4MzE0ODIwNDYzDA82ODQ0MjczMzY4MjA0NDQMDzU3MjMwMjMwMzc1Mzc3NAwPMzEzODYyNzEyNTc1NDQwDA8zMzMzNDMyMTI1MzQzNDIMDzYwMjUyNjg1NDg2MDIxNAwPNjM4MDQ4NzczNjIwMzYzDA83NDc1NDM2ODMzMDQwNTIMDzUyODYyMjUzMTc4MjQwMQwPNzE1NjI4NTcxMjY2MTQwDA80NTEyNzcxMzczODI4MzIMDzEwMDEzMjEzMTA2MzYzMgwPNjgzNjc0MTg0NDUzNTcwDA8zNjIwMTc1NjQzMjM2NDIMDzU2MzQ3MjY0NDU1MTA4OAwPMjUzMDU2NTgyMjQyMDA3DA8wMjA1Njc0ODQ1NTMzNjEMDzgyNzYxMzc0ODU3MjE4MAwPNjE2MzU2MzUzMTIwNDE1DA83MzA3MTQ2NjgyNTYyNTIMDzY1NDI4NzUxNTU2MjUxMQwPMzYyNzE3MTQ4NDczNjc1DA8wMTczMjE1MDc0NzEzODcMDzUzMzY1ODQ0NzQ0NTA4MwwPNzEzNjUwMjA0NzcxMDY2DA8xNTI0Mjg4NTYzMjMzMjIMDzE3MzQyNDczMjgxNjUwOAwPMzA4NTA1Mzc2MzU0MDY2DA8xNTY4MDY2MzM4MDQ2ODgMDzU4MTY1ODM4MzE2MDc3MAwPMTgwMTA2MjQ0NTMxODM1DA8yODAzMDg4NTc0MjM0NzAMDzQwNDU3MjU4NjUzNDY2NgwPNDA1MzgyNjQ2MTE4NzUxDA82NzMyMTMwMzY1NTI3NzcMDzQwMTY4NTgxMDE0ODg3MQwPMDUxNTMyNTQ0MTA3MzA1DA83NjM2MTc1NjMzMzY1MDIMDzczODc0MjY0MDY0MTMwNQwPMzMwNTIxNTY0NDIzMzg4DA8wMDc1NjU1MTIzMjAyNzgMDzE2MzI4ODQwNTE1NzIzNgwPNjA0MDIxNzIxODg0ODUwDA8zNjY0MjMwMjc1MTIwODgMDzY1MDA3MDY1ODYxMTI2NAwPMjIzNDAxMjYyNTUzODgxDA83NjY4NjA2NjQ0MTUxNjAMDzcxNzQxNzA1NTUxMDA4MQwPNzU3Mjg0MDMzMjc3NDYxDA8wNTY3ODEzODA3NTAzMzMMDzc0MjM2NDM4NjA2NTIzMgwPNzE0MjYwNTU3MTc1NjAzDA82MjE0NjYxNzEyMTU2MzYMDzgwMjc3NzEwMDgyNTI4MwwPNjQzMjg0NDUyMTc3MDQ2DA8xMzQ3Mjc2MzM0NTY3NDQMDzczMTYyMDI1NDY2MjE1MgwPMTMwNDUyMTM2MzIzNTc0DA8xNzU1NjEzNzgzMjM4NzQMDzE0ODE3NjM2MDUwMjI2MAwPNzY3ODc3NDEwMzYyNjA1DA83NzcwMTUzNTQ3NzA2NzcMDzE4MjU2MTA3MzMzMDc2NQwPMTYyMzI1ODQwNDA4NjQ0DA83MTY0ODE4NTc4NzEzMzEMDzg3NzE4Mzc3NDIwMjc1NgwPNTU1NjUyNDUxODM1NDYyDA81MjgxNzgyNDI1MDM4ODQMDzc4ODQwNjI4ODQxMDc2MAwPNzg4NzI3NjY4NTAwNDYxDA80MDE1Njg0NTAyODQ4MjYMDzY4MTUwNTI1MTMwMzg2OAwPNzYwNjIzMDAzMzQ0MTMzDA8wNjQ3NTMyODY1NzI3ODYMDzIxMzIxNjg1Mjg0ODYyMQwPODAyMTIwMDI4MzY4NTQzDA8xMjgwMDQwMzMxNTcwMjIMDzU3NzQ1MjI1MzI2NTIwOAwPMjY3NDQ2MTc3NDAxODE0DA81NjY2MzY0MTExNDU3NTEMDzcyMjI4MDQ3NDE3NjE3MAwPNjE1MTM2NDgzMjA2MzgzDA8yODY4NTQ1MDcwMzIyNTcMDzI2NDQ2NjM0NjQ4MTg4MgwPODI1MTI1ODA1NDEwMTAxDA8wNDg0MTA3MzA4MTU1NDMMDzA2MDgyODQ1ODU3NTE3MwwPNDEzMDUzNTQyODEwNzUwDA8zNTYwNzg3MzY1NzY3MDYMDzg3NTEzMTQxMTE2NTYxNwwPNzYwMzMzODE2NjcyMjE0DA82NjUwNjgyNzg0NTU1ODQMDzAyNTc1MjYzODg2MTU4MgwPNDAyMTQ2MTg0ODY0MDEwDA82Nzc4MTU1NzIzODI0NjMMDzU3MjEyMzY2ODU0NTYyMQwPNjE0Mjc2NDMwNzYzMzY2DA8wNTIxNTE3NDQ1MzAzNzMMDzI4MDM3NTA3MTgxMTE1NQwPMzc0MTQ0MDcwNTIzMDI0DA8wMDIxNTMxMjc4NTg1ODgMDzY3MTIwMjYxMTY0NzIxOAwPMDQxNDU2NzE2ODc0NTMyDA83NTYyMDU2MjUwMDI3MTMMDzAxNjc0MDg1ODAyMTA0NAwPODg0MTU2NzQ0NDc0MTcxDA8xNzIzNzEyNDA4MDM0MDUMDzYxNTc1NDAwMjQ1ODYzNgwPNjQ0Nzg1MDA0MjM3MTcwDA82MzQ2MTAwODYxNzQ1NTgMDzI1Mjc4NzE4Mzc4NDU1MgwPNDU2ODEyNTg3NTA4MjQ0DA8xODg4MTIwODI2MTE0NjMMDzUwNjU1Mjg2NzExMzcxMwwPMDYzNzIyMjM0MjI4MTU0DA8yNDMwMjA1ODY3NTEzNjMMDzg4NDc3NzM1MDUyMjUyNAwPNjAwODE0NDE2NTM3MjE2DA83ODE4NjQ4MjQ1MjMwMzQMDzUxNTAzODgwODE4NzQ4MAwPNTUwNzExNDM1Mzg1ODAxDA82NTAxNDQwMjMxMTMyNjUMDzA2MTcyMjQzODYzMDQ1OAwPNDc4MTgxMDAyNTgwODM3DA83MjIwMDU4MDA0NzExNTcMDzQzMDYzODMwNzYzMTQwMAwPODUwMDExMTAxODQ3MTE3DA8zNzE1NDg0MTU1MjgxODcMDzgzMzQwMzU4NzY3Nzg4NwwPNDQxMTc1MjQ4NjYwMzczDA80NDY0MjA3NjA2NjA1MzUMDzg1NDE1MDYwMDYyODQzMgwPNjQyODc2NzIzNzYxMzgyDA81ODAxMDg2NDAzMjQxODMMDzU2NDU4MjcxNDg4NzI1NwwPMzA0NjQ2NjMyMTc3NDUxDA81Nzc4NTgzMzUyNjUyMzIMDzAzMDI1MzM0MjMwMjEyNgwPMzQ3ODc2MTMzMjQ1ODMxDA8wMDI2NTYzNzI0NDMxMzQMDzUxODY3NTE2NTcyMTYwNAwPNTg0MDU1ODc2MTIxMDU1DA8zMzI1NDU2NDg4MzU1NDgMDzIzMzExMDI1MDE2MDE1MgwPMDQ1NTIyMjEzMjUwMDQzDA84NzcwNzAwMTQyMDAyMDcMDzcyNzgyNDYwNTA0Njc1MgwPNDA0NDQ3NjYwNTc2ODA2DA8zNzEyNDA3MzA1MjIwNDUMDzcxMjI0MzE2NTA4NjYxMwwPNzMyODI3MzI0MDM0MDYwDA8xNDE3NzEyMzAyMjAzMzcMDzMzMTMzODMzMDM0MzI3NAwPNTc1MTc4ODMxMTgwMTg0DA82MDI2NzY2NjYwNTA0NjMMDzM4NTc3NTAyNTA2MzI4NgwPMDE3MTMxNTg2NDQ0MTI0DA8wNzc3NTI3NzAxODY0NDMMDzc2ODE1MTAyMzI3MDc1NQwPNzcxMTg0MTI4NDUyNTcxDA84NDM0NjMxODAwMDI0MjAMDzMzODIzMTU1ODg4NTgzMwwPODQ3NTIzNDE3MjQyNDYzDA8wMjQyODExMzE1MTY4NzYMDzA1NjgyNDExNzczNDQ4NAwPMTgzMjgwODQ2ODQ0NTAwDA82MzEwMDYzNTgxODgyNDYMDzYzNTAyNjAwMzA1NDYzMQwPMzI0ODY1MjM0NTY4NzE1DA81MDg0NDE2MjAwMTEwNzIMDzAwNjQ2NjM2MjgyODY3MwwPNjQ0MjEwNjAxNzQ4MjE2DA83NzYxMDQzODAxMDY1MDMMDzAyNjA2MTUyNTMwNDQzNQwPNjI1ODAwNzgwMDAxMDg0DA80NjYzMDIyODYzMDc1NTIMDzgxNzI3NzM4NzI3Njg3NwwPNjcxMDE1NDMxMDg3MTEwDA8wNjIzMjA3MzMwMzc4NzUMDzU1NzYwMjg1NzEyNDg2MwwPMDcyNTEyMjcwNDg3Mjg4DA8zMDU4NTAyNDc4ODQxNjYMDzA2MDAzNjAwNjM3NDQ2OAwPMTQ2NTU1ODc1Nzc3MzA1DA80MjI4NDQ2MzU4MzU0NTEMDzI4NTA0NjUxMzMwNzIwMwwPNzgwNDE2MDI3MDgxNjM0DA8zMTc3MDMwNzQ3MDE3MjQMDzY0MTE3NDEzNTcxMTI4MwwPMTQ2MjYzMjMzMzg2NDM3DA8zMDcwODM1NjI2MzMyNjUMDzAyNzI2NjgyNzM1MjAxNgwPODM2MzA1MzY0MzcxODQ2DA83MjA0NDA3Mjc2MDYxNjIMDzE3NDQ1Nzg1MDM2NzQ0MwwPMTgwNTIzODYxMzE0MzAyDA8zMjMwMjg2Njg1NTI0MTUMDzA1NTU4MDE3NjIwMjI0NAwPMTM1NjQ3NTI1MDQ4NjcwDA8wODU4NTc1ODg3NTI2MTIMDzE2Njg3NjA0NTA4MzI4MwwPNDUyMTUyMzg0NTMwNDAwDA8yNTY1MTg4MzM2MzQ0MjQMDzE3NDU2MzAyNDg0MDYxNQwPMjA4Njc2NzY4NTc4ODczDA8xODc4NzgzNDcyMzE1NjcMDzY4MjMxNzY3NzQ2NTIyMgwPMjUwMTA2ODY1NzYzNjY1DA80MTA1NTQ2NDIwNTQ2NjMMDzUzNjM2MDUxMDQ4ODQwNQwPODY3MzMzODE1MjY3MjcwDA84NjI2Nzc0MTIyODU3MzMMDzUyMDMyMDYyNjgzNDMwMgwPMDU1MjYyMzQxMjA3NzE1DA8xMzUxNzI4ODEyMTUxNjgMDzI4NDIzMzMwODM0NTE2NwwPNzAzODUxMjcwNDEyMzEyDA81NTE3MDIwMDI0NzEyMzUMDzYzMTA1MDY0ODU1NTg3NgwPMzUyNTQ2NjEyODc0MDI3DA81NDU3ODI3NDE1MzAxMjAMDzQ4MTM4MTA1MTgzMjc2NAwPMjMzMjE4MTIyMDY4Mzc4DA81MDg0ODYwNTQyMDUzNzMMDzE3NTQ0NDI4Njg3NDgyNAwPMDE0MDYzODMwMDY1MzY3DA8yNDI1ODc2Mjg0MTQ4MzIMDzg2NjU3MjQ2NjMyNDQ4MQwPMDQ3MjMzMDM3MzEwMjM1DA83MjExNzM1NjU3NTQ4MjQMDzE3ODcyMDMwODM2MjU4MQwPNDAwNTg1NzYwMjM3MTQyDA81MzA2NjQyMzUwNzMyMzYMDzU4NTQ1MDA0MDEzNTgxNgwPMjUzMzY0MzgwODM0NzM1DA8yNTA0NTEwMzM1MjQwMjEMDzczMTExMDg0Nzg2ODUyNwwPMzA1MTYzMzIzNTUzMTMwDA8zODMxMzI3NDg4NDQ1MjQMDzA1NzA0MTIwNDAyNDMzMQwPMzMzMzQ1MTEzMDA3MzY3DA8yNjY3NDM0MTExNDE4NTUMDzE2MDczNDU4ODcyNTMyMQwPNTI1MTU2MjY1NzU2Mzg3DA8wNjAwMTgwODYxNjI4NjAMDzQ2MDQxMjM1NDczNzU0MwwPMjQ0ODE2MjM0MTA1NjUxDA81NDE3NDU2MDcwMDg2MDYMDzc1Njg1MTE2MjQ4ODMxOAwPMjE0NzA0MTA3MjcwMjQ1DA8wODEzNjE0MDMxMDIyMjEMDzYyNDU3NDM3ODg3MDYxNwwPNDQxODQ2MDMxNjI0MjQ0DA83NjM3MTczMTIzNzU3NDQMDzYzNzA3MjMyMjMwODU1OAwPODM3MTA4MjQxMTM3MzczDA8zNDQ4NzUzMTU1NzYyMTcMDzY0NTA3Njc0ODczMjgxMAwPODYwODQ3NDgyMDU4Njg4DA8yNTgxMDQ4MTAxODQwNDUMDzM4NTUxMzg3NzA1NTAyMQwPODU3NTg2MzM3MzgzODAxDA8yMTc0ODEzNjM3ODYxNTYMDzU1MjY4MTg1NTgxNjM0NAwPNDY1MTA0MDQ4MTY2NTU1DA8wMzAwMzMzMjUyMjQ0MzIMDzE4NzYzODMwNDA4NTY0MAwPNjM1MDAyMjI1Mzc4MzYyDA8yMTg3MTU2MjczNDY1ODcMDzU4NTcyMTgxNTE2MTIxMwwPODg0MjM4NDczNDI0NzQ1DA81MTA0Nzc1Mjg1ODQzNzgMDzMwNDgyNjM4Nzc0MzUyNQwPMzI3MTc4MzQ3MDc3MzU4DA80MDY1MzQ4NTAzNDc1MzAMDzMxODQ0ODAwMTc0MDgzNwwPNjQxMjUxMDQ4NjI1MTEyDA80NzY1MTUxNTM1MTM1ODMMDzYyNDMxMzE0NTE1MDU2MgwPODMxNDY2NDI0NjE2MDA0DA82NjA2NTU3MzM3MzMwMjIMDzQzMzc0ODY0NDU1MjQ1NAwPNDEwNDczMzI3ODIxNTM2DA83NjUwODU3MDY1MjgzODUMDzg2MzIxMDY4MzExMDAwMAwPMzYxNTA0MDMzNTUyMDE0DA8wMjExMzIwMDMyMTE1MTIMDzcxODUzNDc0MDAwNDE3NgwPNzIwMjYwMDM0NDMyMzYwDA82NTYyODYwMTMwNjU1NzgMDzY0MDQ3NzA1NjUxNzY4NQwPNTg1NjUzMzA1NTgzMTQ2DA8yNjQxNDUyNDg2NzUzNjAMDzIwNjMyNDg4ODE4NTAyNgwPMTA3Nzc2NzgxODI1ODQ4DA8zMjQyMzQzMjQ2MDI3NTIMDzg0MjY0NjE0MjU3ODExNQwPNzEzNjU0NjM2NDIxNDAwDA82MTM1NjE3NjMzNzMzMTMMDzQ2ODIxNzIzMjA1MTU0MQwPNjQ3MDIwNDY3ODQ2MzA4DA8zMjg3NjIyMTUxNDAxMjAMDzAzMTg2MTgwODM1NzEyMgwPNTA2NTQ1NTYwMDQwMzAxDA8yNDc0NDgxNjQ1MTY4NzQMDzcwODQ1MDQ4NDQ2MjQ3MQwPODcyMzU1MjY0NjI1ODc3DA80ODg0NjY4MTE3NDM4NzAMDzI0MDQzNDQ1NjYxMzY0NgwPODA1NTQ3NTA2MTE1ODA3DA8xMzQwMjE4MzQ0MDA2MjgMDzc4MjEyNjE3MjIwODMwNQwPNDIyODA1NTY4ODE3MzQzDA83NTYzNzA2MjcxNzcyMTAMDzIwMDMwNTIyNTY1MjM0NgwPNjM0MjQ1NTExNTcxMjcxDA81MjUwNTc2NjQ2Njc1MTcMDzA0MjExNjE4MDg3MzUyOAwPMTc3NDUzMTAxNDU3NjgxDA82MDY4NjUxNjc0NzM0NjYMDzA0ODcwMTMwMzA4MTQ1OAwPMjU2NzgxMjU2MTcxODU3DA8zNjY4MDY1MzUwMTYwMTEMDzAwMzgzNTgyODM3NDM3NwwPNjgyODQ1NDYxODA3MzQ3DA83MTU1Nzc3NjU4ODIxODEMDzI1NTQyMjMwMjcxNjM4MwwPMzcyODQ4MTIzNjE3MDUwDA83MjEwNTMzNzE3MzQ2MTYMDzUzMzI1NDgwNTA4NTAyMwwPNDgxMDU4MDg1NTAzNzE4DA8wMTAwNTY1MzU3NzAwNTUMDzgxNjMyNDc0MjUxNjc2NAwPNjM3NzYwMzg4NjM2NTI1DA83MDM0MzAwNjA3NTMzNDEMDzg0MzY4MDU2NDEwNTU2NwwPMTQxNzYzNzg2Mzg4NTAwDA8xMjQ4ODE0MjIyNjYxNDQMDzQ0MjUyNjA1ODIyMDY4MgwPNTE2MDE2NzQ3NTY4MTUyDA8zODYwNjE3NTgwMzgyNzcMDzczMTI1MDU2NDExMDM1MQwPNzU0NDM1MDIyNjM0NzYwDA8zMDQyNzI3ODMzNjAyMDQMDzYwODI4NDU4MzcxMTg4MAwPNDc4MDI2ODc1NTQ4NzU0DA81MTA1MTUwNDMwMjc0NjgMDzUzNTY3MzY2ODA1MjQzNwwPNzg2ODE4NTU1NjM1NjMyDA8xNzQwNTI3MzUxMjA3NzIMDzc4NDUxMjM2MzcxMjYwMwwPNjgzMjQwNTMyMzU4MDQ2DA8zMDI4NTA0MjYwNzY2MTYMDzAxNDY4MDAyNDE2MjY1NwwPMjMyNjU3MTQ1NDc4NTg0DA8xNDU4MjI3NTUwMjIwNjAMDzQ3MDI2NTg2MzIwMTQ2MAwPNDYxMDc1NzE0MzE2NDAwDA82NTQ1ODgzMjg4MDU1ODMMDzg4MzcxNDc2ODE3NjIyOAwPNTc1NTgzMjU4MzU4Nzc2DA84MzQzNDExMjgyNzgyNzgMDzE3NTMyNjU1NDAxMjczMwwPNDU0ODU1MDY2MzQ4NjUzDA8xNTE0MjQ0ODc3NTIxMjYMDzE1NzczMzY2MTI2MTc3MwwPNTg1ODAwMzM1ODM3NjU4DA83Nzg1NzMwNDczNDQzNjYMDzQzMjE4ODI3MzAwMjMxOAwPMDU1MDcyMDEzMDcwMTcxDA8yMjA0NjI3MTQ3ODcyNzUMDzA4MjI0NTYwMDUwMDg4NgwPMzE0NTg3MDU3NTA4Nzg2DA8zMzE3MjgwNzE0Mzg3NjUMDzExMzM0NzUwMDc4ODIyNwwPMTEyMjc3NDM3NzIzMzE1DA84NTY4NTY4MDc0NDY2NTgMDzAzNjEzMDY4MTQwMjUxNAwPNjU2MDgwMjUxNDA0NjAwDA8wMjMyODgxMTczNDA3MDEMDzY2NjU2NjgyMDU4MTM0MgwPNDg4NTAyODc0ODIzMzU4DA81MTQzODI2NjE1Mzg4NTQMDzYyMDQwMTAzMDExNDU2OAwPODg3NDgwNTE2NTUyNzA3DA84NDg3Nzc3NTc1NzE1ODYMDzU1ODg3ODQ0NDg4NjM1NgwPMDY1NTQwMjg0MTUwNjU0DA8yMTAxNTc0ODg1NzUwMDcMDzgzNDc0MjM4MTI1NDg4NAwPMzQ1ODQyODE3MDA2ODgyDA8xMTY2MTE4NjQ4Njg3NjcMDzE0ODY3MjM2NTEzNDc1MQwPNTI1NTQyMTIxMjE1MTQxDA82MTM1ODc4MjgzMDQ2NzgMDzUwODQ0MjQzMzI4ODQ3NgwPODM2Mjc1MzgzNTMwMDUzDA83MTI2MjM3Mzg1NjUwNDIMDzM1NzcyNzA0ODE0MTg2NAwPNjA1MjQ0NzE1NTUzODEwDA8yNDE2Nzc2MDgwMTUwNTIMDzQxNjEzNjIxNTEwMzgyNgwPNDU0NzE4NjExMzgyMTMzDA8wMDIxMzMwMzE1MTc0NTYMDzM2NzE4MjQ4MTc4ODE4MQwPNDIwNjA4Nzg0MTA1NzQ1DA8xMTQzMDQ2NzYzMDAzMTcMDzM2NjEwNDQ2NzA0NDY3NgwPMTA3NDAzNTUxMjU0MzMxDA8yMzU3MzM1Mzg0NDU3MzAMDzcwMjAwNDA1MDE4MTcxNAwPNDMzMjAzMTExNDU1NDQ2DA83NjE2Nzg0MjM4MjcxODcMDzc3Mjg4MTY1ODY2MDE3MAwPNTM1ODcwNjc2NDAxMjU0DA8wNTQ4ODQ0NTcxNjI3NTcMDzU3NzM1NzgzODg2NTQ0NAwPMjE1NDA2MjI0NzE3MTYxDA81MjA2NTg4Njc1NDAwMjgMDzc4NTYwODQ0MTUwNTYyMwwPNjY2Njc3MDA0MDc2MjA4DA8xMzQxNDIyMTc0NTU3MjYMDzA2MjQ3MjU0MDM2MzQ3MAwPNjI1NzQ1MTMxNzgzNTc4DA81MDg0NjY4NjQ2MDY4MjQMDzEwNjg2MTI3NjAwNjM4NAwPMTA0NTcwMzY0MTg4MjAyDA8xMDAzNTgyNTEwNDY4MjQMDzY1MTE2NTA4NDM0MTU2NwwPMzc1NzcwNzMzMjM0ODAzDA8zMzY3NzU0NDUzNjQyMzUMDzc0Mzg1NTQzNTU1MDAwMgwPODUxMDY2MzgzNTYxMjA0DA81MjE0MTE1ODgxMDAzMDMMDzY2MTEyMzIxMTQ3NjAxNgwPNDg2NzIyMDQwODYxMzc2DA81MDQyMDQxMDgwMjA2NzIMDzUzNTYzNTMyMjQ4NTQzNwwPNjczNjY4MDUyNzA2MTE0DA8wNDM2ODA1NDUzMDExMDgMDzA0NDMxMjM1MzA0NzMxOAwPMzczNjQ1NzA4MzM4NDA3DA8wNTE1NjEwMDYyMTE4NTMMDzQwMzY2ODQzNjM0NzY4MgwPMDA0Mjc1MDYwMzY1NTI3DA8yNTgzMDc0MDA3Nzg0MTcMDzgzMTI2ODAwMDQ4NDY2MQwPMDM4MTE3ODYzMjY4NTg0DA8wNjQ3ODQxMTM0ODAyNTUMDzczNzA1MTM0MTg0ODMyNwwPNDEyMDYzNTU4MzA2MDI3DA8yMjU0MDE4NTg2NTMzNzUMDzQ4NjAxMTI2NTc1MTUwMwwPNjczNzU4NjI0Mjg4NjQ3DA8yMTYxNjY1Mjc3MDY4MDIMDzU2NjAwMDE4ODQyNzM4NwwPMTUyMzYwMDQzODA3MzU1DA83NzIyMDg0Mzg0NDI0MTcMDzQwODE4ODMxMzI4ODgzMQwPNTUwMTEwODM0NjY1MjE4DA84MTY3MTE0ODQxNzQ3MTYMDzE3NTM4MTUyMTA2ODAwNQwPMTQ0NzYzMTAwNjMwNzYyDA84ODMzODM1ODMzMDQyNjEMDzU2NTQ3NjQ3NTI2Mjg0MgwPNzU0NTc0Mjg1NjE1MjAwDA83ODA0NjEwMTg4MjA0MDAMDzY0MTg3MDI3MDY2ODU3MQwPNzYyMTY1NzQ0MjU2NjIwDA82MzM3MzgzNDAwMDgyMzgMDzcxMTEyMDIwNDc0NDE4OAwPODU2Mjg3NjQxNzgxNTA3DA8wNTU1NTIwMTQ0MjMzMzgMDzUxNTcyODU1NTM1NTQ0NQwPNTc4MjI0NTY4MjQ4NDI3DA8xMTAxODIxMTc2NzYzNzgMDzMxNDczNjU3NDA4MTU4MQwPNTQ4ODE0MTIxMDE1NTM2DA83MjIyMjIyMzczNDc2NTQMDzY3NDY3MTQ3MjM4NjgyMwwPNjA1NDc0NzUyMTE0MjA0DA84MTIxMjc0NTE1NzYxODIMDzc4MTcyMzUxMjA0NDAzMQwPNDEzMzA0ODMyMTg1NjgwDA81MDUyMzE0ODMyODMwMDYMDzY0NTIzNDAzMjUxMDY1MgwPNjgzODg2NjE0MjI4MTU0DA84MjM3MzA0MTc2NDQ4ODQMDzI4NjM4ODA2NzI4MTAxNAwPMTA2NjI4MDIzNTI4MTg2DA84NzM2NDMzMzUyMjc3NjYMDzc3NTY1MTAzNDU2NjM1MQwPNTYyNDE0MDc3Njg2NTg4DA81MzEzMDY3ODUyMDE3MTcMDzY4NzA3NTg3NTA0ODYzNQwPNDc2MjAxNTUyNTc4NDAxDA8xNDgzMTE3NjQzMDM2NjUMDzg0MjI4MzU4ODg3MDY1NwwPMDM3NTgyMTU0ODY4MzYzDA80NzY3NTQ3MzA3NDA3NzcMDzY1MjIzNDgxNDI2NDU2MwwPNDgwMDE0ODQ3NDA0NDE2DA84NjQ1NDAwNTc0MDMwNTMMDzQ0MDY1MjQ0NjM4ODc2MAwPNjAwNjMxMjY2ODQ2NTY3DA81MTgyMjc1ODg4Njg2MDIMDzE1NjYxNDAwNTQ3NDQ3OAwPMzYyODIzNzUyMjA1NzI4DA84MzE2MzY2ODA0MDE3MTUMDzE0MTQ1MTEzMzU1MzU3MAwPMjcyMjE3NjMyNDQyNTE3DA8wMzc4NDQ0NjgwMjQzODUMDzcxODM0NjI3MTgyMjY3MwwPNTExMzU2MjgwODY3MjI2DA8wMTUxMDIxMzc2MDEzODMMDzQzMTUyNzcwMzQ0NTMwNgwPNDM1NTA3MTc4MTUxMTc1DA8wNzc1MzY3MjY3MjA1MzAMDzc3NTE2MzU4ODY3Mjc0MQwPMjc2MTM3MjQ2NjA0NTU3DA83NzYyODI3NjM0NDg1MDEMDzczMjQ2MjQxNDE0NTU2NAwPODYwNDIwODM3NTQzMTMxDA82ODQzMzQyNTQ4MDU1MDUMDzQ0MTU0NTI0NTEzMzMzMgwPMDg0MjAyNjAwODE3NDc3DA8xMDc3MTQ2MjcxMjY3ODcMDzM2MDUyNjAyODAwNTQ1MQwPNzY2MjYwNTgyMjg2MjUyDA8zODI0NzU4NzgwMzEwMTMMDzQyMzg4MjE4MTcyNDQ2NgwPNjQyNzM4NDQ3NzIyNjgwDA82MTEwODEwMTAwNTY0NDYMDzM0NzIyNDQxMDY1NDAyNQwPNzA2MTEyMDE2MDQzNzgzDA82NTE2MDQ1MDI0ODE3MDYMDzcyMjAyMDA1MjMwMjE2NAwPNjcwMjczNTcxNTM0NjQ3DA82MDAwODU4MDQyMTgyNTUMDzc3ODU0MDcwMjczMzYwMQwPNzA3NDQzODEzNDQ2NDc4DA80NzEyMzgyNTgyMTc2ODAMDzQ1MDg3NTU3NDUxMDMyNgwPNjMxMzIyMDg3NDMyMTY2DA82MDI4MDAxMzQ0MjUyMjAMDzc1MTQ1MDM3MDY2NTYxNwwPMjM2NDU1MjMxNzUzNTMwDA8xMjE1MjM2MTIyMjI4MzEMDzc4MDU1MDU1NTA4NzIwNQwPNDAxMzUxMzAxMDAxMDE0DA84MzMwMTMwNDM1MzM2NTgMDzcxMDUwMTcwNDM1NTc4NAwPMTY4NTU4MTY3MzcyODc4DA80NjQzMTMzODY4Nzg4NzYMDzAyMDE0MzU1NDE4MDQ3OAwPMzI1MDYxNTc3MTQxNjQ0DA82MjEwODMzNjU3MjM3MzYMDzMxMTE2NTgxNzU0MzYyMQwPNTYxNjY3NjU3NTc4NjExDA8wNDI4NDgxNTg1MzMyMDMMDzE1NTI3MjExMTE1MDA0MwwPMzExNTQzODY0MjM2NTE0DA83NDUxNTMxNjYwMTIxNzgMDzMxMTIxNDgzMDAyMDcyNwwPODc1ODQwNjY1ODExNjUxDA8yNTIzMDU1NDYzNDcxNzUMDzU1NjExNzM0NzIwMjIzNQwPNDA2NTA3NDE4NDYzNTczDA81NzcxMDA3MDcyMDMyMzcMDzc2NjEyODA3ODAyMzg2NAwPNDgzODczMzQ3Mjc2MDg3DA83MTQ3MTU1MzM3ODY3MDcMDzMwNzAwMDU1MzU1MzcwNAwPMzIxMjUzMzMyMDY0Mzc1DA8yMzUzNzg4MzM2MDM0MzYMDzA4NzQzNTE0NDAxMzAwOAwPMzI4ODMzNDU4NjE1MjYyDA82MjEwNzg0NzE1ODQwMTIMDzA3MzEyNTg4NDU0MzQ0NQwPODM4NjE4MDQ0MzM1MDQ2DA8yMjM4NjMxMTc4MTE2NTMMDzYwMDA4NzY0NjIxNjQ3OAwPNjQwNzM2MTYxMDMxODMzDA84MjM4ODAxNzU0NzEyMDYMDzMzMjg2NTM3MzI1MjQ2NQwPMjQ0MzY3NjY3ODgyMzExDA81NjYwMzc0Mjg0NjcxNTcMDzQ2MzQyNzU0ODUxNzEzNgwPNDA0MDU2MTMzMzA4MzEzDA8yMTMzNTE3MzA2Njc3NDUMDzA3NDYwMTcxMDA2NTg4MAwPNzIxNTQwMTUyMzExODgxDA8wMTY3NDQxNjc0NTg0NDcMDzM0NDcyNTM3MTAyNTYxMwwPNjcwNDc3MDQxNDYyNzI1DA8xMjgyMDgwODE2ODczNDUMDzM0NjcwMzQyODUyMDcyNgwPNjQ0NTMyNjE1MDg4NTU4DA80NDUzODI0MTYxNjE3ODEMDzMyNjQ1MjY2NDEwODYwNAwPMzgxMTE2MzUzNTczMzQ1DA84NjQ1ODI1MzE3NDEwNTUMDzIyNDY4NjAwMDg1NDM4NgwPNDc0NDgzNzA0MDE2NDIwDA8yMzMwNzYxNzYzNzM2NDYMDzUwMzI2ODMxNjMyMTQyNwwPNDU0NzAzNzE3NjI2NjcyDA8yMjAxODA4MjM0NjUzMDEMDzg1NjU0NDczMDAyMDU1MQwPMjYwMTgxMTcwMjYyNjMyDA83MTQyNzcwMjY4NTU4NTQMDzU1NTc4MDcyNTEzNDEzMgwPNjg1MzcwMzI4ODM0NTc1DA84ODczMjg1NzEyMDA1ODQMDzY2NjI3NjgwMjU4NDI3OAwPMTUyNTY2MTM1NTEzMjM2DA83MjcyNTU4NjYyMjEzMjQMDzQ4MDc3MjU3NDgyNjQzNwwPMTU4NDU4NDE3MDY2ODc3DA84MjI1MzEwODQxNDIyODEMDzIzNzgyNzA4MDI3ODg3NQwPMDMzMjQ3NDgwODU1ODcyDA83Mzg4MzgzMjQwNTgyNTYMDzA1NDU1ODEyNDA2NTgxMwwPODYyNzExMjgwMjIyNzIzDA8zNDQ4MTUzMjI2MzMxMTEMDzU2NzcwNDQyNjE4ODYxNAwPMDU3MTQzODA0NjE0MjcxDA82ODcxODQxMTU0NjA4MTUMDzYxMjUzNjA2MTczNDEzOAwPMDYyNDI2ODI1NzUzNDYzDA8wMzMwMTY4NjEzMzgyNjUMDzg1MjMyMzc0NjQ2MTQzNgwPNTcwNjE1ODgzNjI2MDg0DA80MjIzMzIyNjE1ODQwMTUMDzQzODc0NDc0MjQzNTY1MgwPMDcyODE1MDAzNDUwMjI4DA8zMDA1NjQ0MzU0NDQzNjMMDzE2NzAzNDY2MTU3MDc1MwwPNTQyODAzMDg0NTQxMjc3DA8xMDA2NTQ2ODEzMjAzNjgMDzEzNTc3MjMyNDM4MzU0OAwPODA1NDcyNzA3NjgxNTYzDA82NDU0NDQ0NTYyNzM2MTgMDzU1NDE2MDQ3MTUwODYwNAwPNDU1MTAyNDExMTg2NzY2DA84NzU2MjYxNjIyMDUyNDgMDzM3Nzg3MjA0MDU2NzQyMgwPMzYzNTE4NTA2MTg0Njc2DA81NjIzMDQyMTgxMjY2NjAMDzUxMDUyODc0MzIyMDgxNAwPMTA3NDA3MzMyNzg4NTAxDA8yMTA3NDE4NjQ4MzM0NjQMDzc4NDU4NzA0MTgxNjgzMwwPMTIyNDA4MTAzMDExNTIwDA84MjE0ODA2MTM3MzYzMTgMDzIwODUxMDc2NTgxMzYyOAwPMjQwMTc4NzMxMDI0NjUxDA8xNzc1ODQ2MTI3Nzc2ODcMDzEwNTg3MTQ1ODA4ODI4OAwPMzc0MTQ0NTg1ODMzNjc2DA8wNjY3NjE1MjY2MjE4NjIMDzU0ODMwMDMwODM0NzYxMwwPNjI4NTMyMzgzNTA2MzQxDA81NDg1ODU3NzIzMDIxMjQMDzIyMDE0NDg4NjIwMjU0MgwPMzA3MzIzODQ3NDQxMjE2DA83NzUxNDUyMTQ3MDMzMjIMDzA3ODAyMzgyNzc1NDczMwwPMjgxNjA4MDAxNDg3NTE3DA8xNTQwNDYzNzU3NTUyODgMDzAwMTgzMjEwMjA2NDg1MQwPMDU2ODM2NzE4MDQwNTQzDA8yODg3MDQzMTM2MzEwMzUMDzU3MDAzMjE3NTQ2MjAxMQwPMDgzMTgzNjYzODA3NTU4DA82Nzg2MjQ0MDEzMTc4NzIMDzI4NDM1MDI4MzY3NjUyMgwPMTM4MDgwMTY4NTcwMDMyDA8xODA1MjczMDM3MzI1NTYMDzMxMjM0NDUxMDU1ODE1NwwPNDMxNjI4MzY0ODI4MjUxDA82ODYyMzc0MTIzMzgwNjUMDzQ0MjIzODYyMTc4MTQyNAwPMjgxNTYxODY3MzY3MDE3DA81MzY2MTcwNzc1ODc3MTQMDzcwMjY2Njg3ODI0ODQwMAwPMzg3MDMxMjQwMzY2Mjg2DA8yNjQ4MzU3NjcwMTA2MjQMDzA4ODM4NDE2MDQzNzE4MgwPMjQzMjY1ODE4MDU4MDI4DA80MDIwMTQ0NzcyNTE3MzAMDzIxMjI0MjgzNTE4NzA2NAwPMDYyNjMwNjI4NTYyNjc3DA8wNDgwNDc4NDA0NDAzMjYMDzg0ODA0MTEwMDMwNjM4MAwPMjAyMTgwMzQ1Mzc3ODM1DA84MzAwNjIwMTMzMzE1MDIMDzAzNzcyMjAzMTMzMzIzMAwPMjc0MDE2Nzc2NTQ0NTgzDA8zODI3MjgyNTMyODQxNDgMDzA0ODAyMzI4MTI2MDAxMQwPMzgwNTMxNTM4MjYyNDE4DA80NjAwMTMyNTMwMjgyNTMMDzMwMDQ4NTUzMzM1MzgwMAwPNjI0NTI1ODUxMjg1NDc2DA8zNjg0MDQ3NjA2MzIxNjQMDzIxODAyNDI3MTc2NzU3MAwPMjEyMzE2MTI2NDA2NzM0DA80MjY0NDYwNzA0NDY2NDgMDzI4NTg4MDUwODc0NjcwMQwPNzE4NDg2ODU2MDUxMTgwDA81MDQ4NzIwMzI4NDA1MjgMDzM2ODU2MTYzMzI2NjI1MQwPNDcxMTg0ODcyNjEzMzExDA8zMjE2NjA1MDM0MjI1MjUMDzE4NDgxNTEwNjM2ODE3NgwPMjEzMjgwMTg1NTM1NjA3DA84NDU3ODYxNzI4NDQ1NzcMDzU0MjUzMzUwMDIwNjg0NgwPMDEyMzQwODAyNzExNTYxDA83MzMyNDIxNDcxMTg0ODIMDzQ3MjcxMDc1NDMxMTMxNgwPMzQxMDQxODM1MTQ1Njg1DA83NzI1Mzc0NjQyNjIzMzYMDzc1MjgwMDI3ODcyMTYwMgwPODUzMDQ1NTM1MDgxODE0DA83MDYwMTE1MDA1NzI2NTIMDzYwMzM4NTg3NzY1MTQ3NgwPNzUxNDU4MzczNTMzNTcxDA82NzYxNTA2MzQ0MjA3ODQMDzcwMTQ2MDg2NTU0NDA4OAwPMTI3MjEwNjE2MDcwNjQzDA83ODA1NzczMTM1NjcwNDMMDzEwMDMwNzUzMDUwMDY2NQwPMTcwNzY0MDg0NTg1MjgwDA8zNzAwODYzNTc3MTg1NDAMDzM1NTgyODQyNzc3MTA2MgwPMjU0MzE0ODU2NTU3NTgxDA84ODcyNzUyMDY1NzUzNjYMDzMzNDI2MTc2MzQzMDc3MwwPNjIzODE0NTU2MDcwNjE2DA82NTE3NzM4NTEzMjM1ODYMDzc3ODUyNDEyODcxMjQ3MQwPNjMwMjc0Njc1NDE1MDU4DA80NzU2NDgxNDAzNzEyNTAMDzcxNTIxMTM2NTg2Nzg3MQwPMDM1MTgxMzgxMzczMDc4DA8yNTEwODY2NjY3MzgwMDAMDzA2ODMzNjYyNzQ3NTA3MwwPNDY1NDM0MDQxMjc2NTczDA8wMDU4NTE4MDI4MzI0MzMMDzQ3MzMyNzM1MTc4ODYwNQwPNTI2MjM0ODgxNDcyMTY4DA82NTI4NzU2ODg0NzExODYMDzU2NTUyODA1Mzg2NDQwNgwPMzI0NzM4MzU2MTM3MTM4DA83NDcwMTg1NzE2MzI0NjIMDzUwNTQ0NzAyNTM4MjQ0NwwPMDU2MzEzODM3MjE1NDgzDA82NjM0MTQyNTYzNzU4MTMMDzcyNzMyNDA0MjAyNzc1NQwPNjUxNzc2MTM0NTExNTg1DA83ODE0MjQxMzA0NDUxNDUMDzgxODM0MzI0NzMyMDA1OAwPNzg2NDIzMzg0MjQ4ODc0DA82ODEwNDE0ODYzNzMyNjgMDzY4MjA2MzcyMjYxNjc0NgwPNTU1MjM0Nzg1NzYyNzA4DA84NDI3MTg3NjYxMTQyNjMMDzU1MDQ3NjExNTI2NjgzMAwPMDg2NTg1ODQ2MDM4NDgzDA8xNzMwMTMxNTc1ODM2MjEMDzYxODMwNzcwMzU0NDc1NAwPMjEwMjE0ODQ0MTAwMTI2DA84MjE1MDcxMDIyMTAwNjIMDzE3MTAxODUxMzg3NzE1NAwPNjY4MTI0NzY1NTgwMjE3DA8zNjIyMTMxNzE3MzQ2MTUMDzU4MzYxNjY0ODYyNzE2NgwPMTAxMjYzMDc2NTEyNTExDA83MjgwODI0MjQwNzMxMTgMDzcyNzcyODQyNDMwMzQ4MAwPODM1ODE3NzIzNDAwNzc4DA83NTcyNzM1NjEwNDc2NzYMDzIyMDEwMjIxNzY3NjgyMQwPNzYwMDc4MDU1NjUwMzcyDA8zMDI3NTM4NDg1NTAyNTUMDzIwODA2MTI1NDUwNDIwNwwPNjc0ODcyMjI0NjU3NzA4DA81NDMyMDE3NzYzNDUwNzIMDzI2NzIyNzM3MjYxNDQ4OAwPODE1NTU3NTE0NjYyMzU1DA8zNjgzNzgwNTIwNDc3MDAMDzE4MTAyMTAzMDAwMDA3NQwPNzA1NDU3ODQ3Mjc4NzEwDA84MDUzNDEzODUxODI4NDgMDzU3NTUyMzg0MDc3NDMyMAwPMTc2NTMxNTU1NDYyODU4DA83NTM0MzQ1MzYxMDgyMDQMDzI4MTA4NDYzNjU1NjQ2NwwPNDYwNjI2NDYzNDY1NTY3DA84MTI1NTc1MDQzMTM4ODUMDzE4MjExMzM2NTQ1MjExNQwPODMyNTA1MDI4NDUzNTEyDA80MzA4NTU3MDU1NDAzMjYMDzY4NTc0MDUxMDgyMjM4OAwPNTY0MzAxMzE1MDg3NjAwDA8yMjY2MzQwMDc1MjI1NTEMDzQwMDgyNTQzMTUxMjAwOAwPNjcxODU0MjQwMzQ2NTU3DA84ODE3MzczNDgyMDg0NTcMDzE4MTQ0NTAyMzcyNDcyOAwPMjcxMzAxMDEyODEwNDgxDA8xNjg4ODc1MTYwMDA0MDMMDzY1MTYzNTI2ODYwNzAzMwwPMjQwMTE4NzAzMDEzNDA2DA83MTY2MTUxNTczNzA4MzYMDzEwNTM2MjUzODA1NzY2MwwPNjY3Mjc1MjUxMjczODE3DA83ODA0ODE3Njg4NDgyMjYMDzYxNjA3MjgwNTM3NDM3MwwPMDAwMTc2MDMyMjQ0NDA0DA81NzEyNzEzMjYxNDU2NDYMDzI3MjUzNDY4MDIyMTgwNAwPMDI4NjYyMDAxMTEzNTgxDA80MDM2NjM3MTUwMDMwNDcMDzIxMjgwNTg4NTUwODY2MAwPNTgxMDUzODI4NDM2MjI2DA8xNTUzMjcwMTUzMDcwNzEMDzU4ODc4Mzg2NDg2MDczNAwPNDUwMjAwMTM1NTgxMTczDA84NzQ3Nzg1NTI3MzUwNzcMDzg2MjEwNjg3MDY2ODIzMQwPODIzMzEyNDE3NjQzMTI2DA8yMjY2MTI2NzQ0NTE3MTYMDzQwMjYyMzQxMDI2MzQ2MQwPNTg4NDM1NzQwMzM0NjU2DA80MTIwNzI0MzM3NTUyMDcMDzc1MTUxMDAzNDIxMDQ1OAwPODQ4NjY3MzE1NDU0MTA3DA8wNjg4MDM3ODY3NjU0MTIMDzQzNjE1ODQzMzcyNjI4OAwPNzQ3MTM3NjM0MzE2MTAxDA8xNDAwMDg0ODIwMTQwODMMDzQzNTYyNTI3ODczODc2OAwPNDE0MzEyODEzODYzMjE2DA81MzAzMjY1MTU0MjM4NDIMDzM0NjM4MzM4NzE4NDEwOAwPMDYwMTAyMjEwODAwNzI1DA84ODY1NDgzMjYwNDA4MzcMDzM2NTM4ODc3ODMwNDU1MQwPMTY3NjAxNzg4ODcxMTI0DA8yMzExNDcyNDU1MTA0MjIMDzgyMzMzNjM1MzE0MTM2NwwPMzQ2MzczNTQyMzYzNDAxDA8yNTgyODU0Nzc2ODcwODQMDzU2MjE4NDg0ODYyNDUzMwwPODY2NTE2MzU4ODc1NTE1DA8zNzEzMzUwNjA3NjExNDEMDzIyNzQwMzUwODc3ODU0MAwPODQ3ODIzMDcxMTcyNTgxDA8xMzczNDM1MzAzNjA4NTAMDzY4MTU2MTc1NTMyMDc1MAwPNDg3NDU0MzgyMDc1MTQwDA8yMjA1NjQxMDUyMDYyODIMDzI3MTEzMjQwODAyMTc4NgwPODU1NzU4MDczMjc0NTU0DA8yMjYyMTg2NzMwNzg1NDQMDzYwNDU4NDMwNzQyMjMzMAwPMDAwMDAwMDAwMDAwMDAwMA0GCSqGSIb3DQEBBQUAA4GBAJ8Z3SEBLw0sv686HSCpWnqRIYhzEl0nEWIf9XkCU8q6J4y3Jhmdjf8ob9nRxWVFXEe4o+nHrFKGMU7ZkzHuAd/e079l2uEmVH9+NBnfcPhcNNGi5hp3GUMAvnw0ZqIJOFJyXwWpDgELSr5fCumU3MDs3jNslALZNO7qtGEefsk8"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "jEDzd/ZrEth/wjcxTkwk5Rh+Jr8+mCZgFTcHGVhsPFtHxX1/qCugoGAHUfhBNJhouId1TlrpAEXwK0uvLHJK3jW8bdz6r54QZ2htwrA1MQZKc3hxfeAdGX0TmKuWFzsbwMZlzRHN6r+kCfmV4qzp5d9utM6GwaV4mJMhGoHIMKM="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        assertTrue(true);
        // 31. test the signing info for signed application
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        session.remove(StorageNames.APPLICATION_PACKAGE_TABLE, appUID);
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICyjCCAjOgAwIBAgIJAKoRe0pf7UvDMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyMDI0NVoXDTE1MDIwNDEyMDI0NVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKoYd+BJxNOD8+bGcxCFfJvDr95SJzVGDwrIcANqnO5pq+nI0FWbYTNsTNzl0umP3DjQ+jAboZG+S0/hPh1dYs8aAxm7ORzwHOM70cIboYrrZzOB/Kw+BXY3HuFqrtO25oke/MfKbspVNjR4QErZafsa0IVsX2bKT9cgcUUMykQwht0JjsQjn7CMKb8dE/RjATIXl7a+ng0jBnDIm+rUK/+YCimmaIJT3xb+PAmFXFwU+tmGqFeNlIIi0EUIot7q8JJeSJWMCftrlcHgb3J+jFfyeHt/YcfIN0rxXtlDHFVQXvskPq4I7O+NSypfJBiR8dfxffYlPliNKJ2vG2iurZsCAwEAATANBgkqhkiG9w0BAQUFAAOBgQBpGzuZaf2N/f3c6auGkYzcYY34VB+4C9kFJA2dQBsNYnlfIeSF30o9uq0VAkFI5uzk4aYiDB0dN5lsZ/bv/jnmrElDaXEBRBn6KtkIsJRgIcKRyXLOfWFcskcChxTVknPV8hxLYyjmi4S+O2Km9cOHEy4bib+XlnEsoe5iiTaHKA=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" , new Attribute("",
                          "kwxpKeSSWSeSW5ieWCx9ARFD6Lrwabxk5t7Aas5FFVEtr89vslY1QIexKNiytQm51PTIRnCNhl6y+tcv7V7GRskV1VMlTHwbCVjK4rqoFFrcZrO3qHvkRh6K9wnHnw5PPtBsrMbg8lWKBrGaf4TYRYFW+CukmR+06R7u5zAZaL7CMhl69bCXd78Tz1Z4N5CxVFD8vGatp66TMBk+0YhZP7Lo1yIDu2XsojVtigAoV9sm8OF5/aXmgfFb2AUmANGxlaDAieUfOAm7RXvsOZgaPN72KEqya8XQW6r1/aGF6jMmJ62qgPqJV3TtJtbFSwH1suywAk1UDltVim4b/Ucw1A=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        authenticationModule.authenticateJar(appUID, null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        SigningInfo info = authenticationModule.getSigningInfo("appName","appVersion","appVendor");
        // app is unknown
        assertTrue(info == null);
        // make the application known
        appPackageEntry = new StorageEntry();
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageAttribute.ID,
                                         appUID.getStringValue()));
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageNames.NAME,
                                         "MIDlet-Certificate-1-1"));
        appPackageEntry.addAttribute(new StorageAttribute(
                                         StorageNames.VALUE,
                                         "MIICyjCCAjOgAwIBAgIJAKoRe0pf7UvDMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIwNTEyMDI0NVoXDTE1MDIwNDEyMDI0NVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKoYd+BJxNOD8+bGcxCFfJvDr95SJzVGDwrIcANqnO5pq+nI0FWbYTNsTNzl0umP3DjQ+jAboZG+S0/hPh1dYs8aAxm7ORzwHOM70cIboYrrZzOB/Kw+BXY3HuFqrtO25oke/MfKbspVNjR4QErZafsa0IVsX2bKT9cgcUUMykQwht0JjsQjn7CMKb8dE/RjATIXl7a+ng0jBnDIm+rUK/+YCimmaIJT3xb+PAmFXFwU+tmGqFeNlIIi0EUIot7q8JJeSJWMCftrlcHgb3J+jFfyeHt/YcfIN0rxXtlDHFVQXvskPq4I7O+NSypfJBiR8dfxffYlPliNKJ2vG2iurZsCAwEAATANBgkqhkiG9w0BAQUFAAOBgQBpGzuZaf2N/f3c6auGkYzcYY34VB+4C9kFJA2dQBsNYnlfIeSF30o9uq0VAkFI5uzk4aYiDB0dN5lsZ/bv/jnmrElDaXEBRBn6KtkIsJRgIcKRyXLOfWFcskcChxTVknPV8hxLYyjmi4S+O2Km9cOHEy4bib+XlnEsoe5iiTaHKA=="));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appPackageEntry);
        StorageEntry entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.PACKAGE_NAME,
                               "appName"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VENDOR,
                               "appVendor"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VERSION,
                               "appVersion"));
        session.write(StorageNames.APPLICATION_PACKAGE_TABLE, entry);
        info = authenticationModule.getSigningInfo("appName","appVersion","appVendor");
        assertTrue(info.getProtectionDomain() != null
                   && info.getProtectionDomain().getName() != null
                   && info.getProtectionDomain().getCategory() != null
                   && checkCertificateDetails(info.getSigningCertificate())
                   && checkCertificateDetails(info.getRootCertificate()));
        // 32. unsigned application
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        authenticationModule.authenticateJar(appUID, null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        info = authenticationModule.getSigningInfo("appName","appVersion","appVendor");
        assertTrue(info.getProtectionDomain() != null
                   && info.getProtectionDomain().getName() != null
                   && info.getProtectionDomain().getCategory() != null
                   && info.getSigningCertificate() == null
                   && info.getRootCertificate() == null);
        session.remove(StorageNames.APPLICATION_PACKAGE_TABLE, appUID);
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        authenticationModule.removeSecurityData(session, appUID);
        // authenticate to certain domain - null value
        authenticateToDomain(null, true);
        // authenticate to certain domain - unknown value
        authenticateToDomain(new ProtectionDomain("myOwnDomain", "myOwnCategory"), true);
        // authenticate to operator domain
        authenticateToDomain(ProtectionDomain.getOperatorDomain());
        // authenticate to manufacturer domain
        authenticateToDomain(ProtectionDomain.getManufacturerDomain());
        // authenticate to trusted third party domain
        authenticateToDomain(ProtectionDomain.getIdentifiedThirdPartyDomain());
        // authenticate to untrusted third party domain
        authenticateToDomain(ProtectionDomain.getUnidentifiedThirdPartyDomain());
        // extended key usage - no extension
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICWjCCAcOgAwIBAgIJAKCax8NgVHTGMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIyNDEwMjA1MloXDTE1MDIyMzEwMjA1MlowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEA1/O0o24bSr6C5kCw8lg/UzubEpOvqmwwgWI5TO2S7KfxFkF/O1P+5qc8vU7+sJlgGwynki9HCI00t4pAE+sF0hTAjY8hQaMg0q/kgh3QoCiEvWH9xC/eX6p7X9H2WcWVogS1hmAfrGopsjT5juYn5W9N7pc2gy+ypfZ8obbP4n8CAwEAAaMSMBAwDgYDVR0PAQH/BAQDAgeAMA0GCSqGSIb3DQEBBQUAA4GBAC7rgPLkOWVy6RvmEUZ/nSRAAfbxZvRVLb49pmMI3ZIYWuDbNgxBPZZY/H02yhBw4plXvySQiBm8jtEHSt/wABdoz10go9Gv2chOnetI+1/c88GY1CDmNZ8pFbvHoBQj2F+yDP2lsDedzhlnEHJaw63u9sgxl1BnRtihgfhIkILm"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "trKz6/JqmFFrAnP/hgZofqpT1EeQHqY0qYHi6vPkmBAtw36bZAaV7UANIXirdTLGJk2OZo3DsFRBIgbz+38vWjif0MP5lxN0821bgLwYNzbjjejzNZc3RAyV8cHR15L45FH73gp0F8Cfs6+C0QT7z1yr6V+Jbm9+DdlsTkwfYeo="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        assertTrue(true);
        // x509 code signing extension
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICcjCCAdugAwIBAgIJANsDitVsBUyMMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIyNDEwMjI0MloXDTE1MDIyMzEwMjI0MlowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAx6iv41CzNgQV3khVn6jl2DCeIb2WRnf3oiulukhjQn3y9o6zZDzKBLF47MT+roVLbnswRHnAhkuVSBA4JyYBumOfFqxXQHNgrnKrxpupVFhXTUxtjqeX1zxeEnS0AOBmGLbndykn+8peSCBZom1r6gTXrMDYPCPKJYvphXMmyXECAwEAAaMqMCgwDgYDVR0PAQH/BAQDAgeAMBYGA1UdJQEB/wQMMAoGCCsGAQUFBwMDMA0GCSqGSIb3DQEBBQUAA4GBAIbDGnCc9oFzcPtAssoGWcSenLfKdiTRoySwwyuIzhfvUGWMO2h3ta5Sp46t+nl+2tygeYy1fkl65UAgUqmeFsSgDhEIC+TBmw+S4Nzm6Ktq28r4Gk6Yqnx/oWoiyjdamQIjdNWQRoKRnVT/3TIW3zYOg/InLbbV1rY1Jr8SQ7qu"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "deuHPO2Bqid4gJTVOvf4zcnJWBgi/rbEj3VDVPkHmF+QuombD4Iz4whZ+fxlCdk0ygRxsoBOEFvXL/D7hJPVbNggD3kksFBpYDMFANJXn3FFReU0naTZS7qN1DUck0GzX4DmgKeRFvn0DgLOnn++cgNAVkaTEIJlggGKrc/wYjk="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        assertTrue(true);
        // nokia code signing extension
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICdjCCAd+gAwIBAgIJAMlxOCiz1uFuMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIyNDEwMjM0MVoXDTE1MDIyMzEwMjM0MVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEA3HxYBECqzXsnZfh+Ctn1JBuSjiW7gbsmOb2HorLlt6tVAEISly+OtzYKXUgkW+dWO/7RR2QfWpV1TSULifFx7198btlTLG/M6pcZEPIHzP4/7BQZWDXMgPpOH1vqob1Nps9K/1mIqR2HiFNL7QcbjMwQnWdGYOK88kawO6VEvHsCAwEAAaMuMCwwDgYDVR0PAQH/BAQDAgeAMBoGA1UdJQEB/wQQMA4GDCsGAQQBXgExAQICAzANBgkqhkiG9w0BAQUFAAOBgQBGQIcQWQeWVMfBeBKcwYhEUc3ITlor6jV1VV9pwiblNrtMWoFJpyPRpYIZsVhdqsicp38EwvqobFVhVY0QK6PHqwn+eurkNB4TZWLW0JMUGjoIPtt45iXrCqvJfn6xZpkO/ZFzvyfza9AQLa3WuFTyLzzEf1YRjVEvyBFaRxql7w=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "ryh/rqKP6XcVy1PSot7ghvD/xg+lhVtjLYk3jPVeCRNl2ez4hSGPBnZB+jqQyTecH+se9qA3B9PtYuJzEn0jWhDqTCTYqxnIrJxZFimk9giI68rqlajDUK/XEmN7DyxM3knuGUPtKYFLTlp3Wazfl/eXsTMwR1+G+Q2XMDtFNiM="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        assertTrue(true);
        // nokia and x509 code signing extension
        authenticationModule.removeSecurityData(session, appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                          "MIICgDCCAemgAwIBAgIJAON0Llj25rWmMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIyNDEwMjQ0NVoXDTE1MDIyMzEwMjQ0NVowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAq7maHvQS6JWxWSSAZOhK6fa0Sj11PmwGzPqRbPSXxBamWN8USNyR6H1IVg5KY0Vd+zHLwv63Hj+cnQHMH0cSmkvS9hVu5FCvKjHTWYcmZgGyG4lZ7HKNIcrZb8WEDEVGvel58upfbHiouz+14JlQXUuEPatpFUF2npj3aXHvFSECAwEAAaM4MDYwDgYDVR0PAQH/BAQDAgeAMCQGA1UdJQEB/wQaMBgGDCsGAQQBXgExAQICAwYIKwYBBQUHAwMwDQYJKoZIhvcNAQEFBQADgYEAlIBZziJ1jBCyNZmVeNKF2G7g++TdvwLuCuXVb/Tk+cELLEH0ueOd1Njn7wqciXEMf7uQGGuA9A5XoxW01Hb/KFGenOTZk2e5rSfB/JV1ANtT2qgIOPmnihiq0sEzwLkWHw8G2d+vC2PkO8GWcDB+KRVwHTXz0WD3sCsCqSSu3Zo="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                          "TucdBtmePWgkWjXhteRSz45TUSI4H8dyZA1wKHvMuwBnuA8UhmdlpZ3oFJJQixxXxAulePNouG6cg5GtWJCcyYm4+OV0DeW0C5lo76lmIfjmBCAiQm0Ro26eSnkwDOtqoSPQNz9rPoIiMAQLEzM+jaDKQvCS81Btr9ljB1L6WwQ="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        assertTrue(true);
        // unknown extension
        try
        {
            authenticationModule.removeSecurityData(session, appUID);
            allAttributes.clear();
            allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
            allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",
                              "MIICbDCCAdWgAwIBAgIJAOI97LkyZNDOMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9EZXZpY2VfTWFudWZhY3R1cmVyMB4XDTEwMDIyNDEwMjUzNFoXDTE1MDIyMzEwMjUzNFowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAooouUypXTdFIwfRasYdNWfkknLJYD/iMfnbRneScXnOSipEwY+5pCrISbcvb/P6nBqhlCeCblwO3dEiPp+iFXHFnyuvC4rh90JpRXApMe0Nf2XlIeZVLQYWGJ16VePatp1rArFCy8jSjOQIWmFik88tP88ajse8sb0bg+QeG6ZMCAwEAAaMkMCIwDgYDVR0PAQH/BAQDAgeAMBAGA1UdJQEB/wQGMAQGAioDMA0GCSqGSIb3DQEBBQUAA4GBACsCSxGu9Rol5YW/121ntS5iSoofJoLoInYuTzw7wuQ3mejyvtwkyKmba3hIyXQz1V/gXFH7PuiWLUMLQkCAet3/v3KneIfM1LSqM5MEDWT02bo9E1iO57pawbTkYy+27sMnmJ/d1YeH4c3b4I6gfofBo9ZDY6jtupoCfflNiCi5"));
            allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("",
                              "njU2OjnbLQigE/LhhszSc/oUXTcsZ7ZmjRiZMzGmHeQBtce+9dX8JkgsjAS7cZ6C1lbszGANXqSnMOr5PhFO0bo6Llnu+iJjRKBK/P8l8YYFXIlsjba7iBwm0Kb9lIY860F00bdgZpW2o5gBMZzO/kRgs3rP5mv37Cha62oE8h4="));
            securityAttributes = new SecurityAttributes();
            securityAttributes.addDescriptorAttributes(allAttributes);
            authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
            assertTrue(false);
        }
        catch (InstallerSecurityException e)
        {
            assertTrue(INVALID_CERTIFICATE_MSG + e.getOtaStatusCode() + " " + e.getShortMessage() + "," + e.getDetailedMessage(),
                       e.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_AUTHENTICATION_ERR, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.CERT_UNSUPPORTED,
                                                        null)));
        }
    }

    private void authenticateToDomain(ProtectionDomain domain)
    {
        authenticateToDomain(domain, false);
    }

    private void authenticateToDomain(ProtectionDomain domain, boolean expectToFail)
    {
        session.remove(StorageNames.APPLICATION_PACKAGE_TABLE, appUID);
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        authenticationModule.removeSecurityData(session, appUID);
        try
        {
            authenticationModule.authenticateJar(appUID, null, domain, TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar");
        authenticationModule.addSecurityData(session, appUID, null);
            assertTrue(!expectToFail);
        }
        catch (InstallerSecurityException e)
        {
            assertTrue(expectToFail);
            assertTrue("",
                       e.getOtaStatusCode() == OtaStatusCode.INTERNAL_ERROR
                       && e.getShortMessage().equals(errorMessage.get(InstallerErrorMessage.INST_UNEXPECTED_ERR, null))
                       && e.getDetailedMessage().equals(detailedErrorMessage.get(InstallerDetailedErrorMessage.INTERNAL_ERROR,
                                                        new String[] {"Unknown protection domain " + domain})));
            return;
        }
        String domainCategory = authenticationModule.getProtectionDomainCategory(session, appUID);
        session.remove(StorageNames.APPLICATION_PACKAGE_TABLE, appUID);
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        authenticationModule.removeSecurityData(session, appUID);
        assertTrue(domainCategory.equals(domain.getCategory()));
    }

    private void testVerifyAuthenticity()
    {
        StorageSession session = StorageFactory.createSession();
        SecurityStorage storage = new SecurityStorage();
        session.open();
        Uid appUID = Uid.createUid("myUID");
        StorageEntry entry = null;
        boolean success = false;
        // invalid jar path does
        try
        {
            authenticationModule.verifyMIDletSuiteAuthenticity(appUID, new AuthenticationStorageData(
                        null,
                        null,
                        null,
                        null,
                        null,
                        null));
            assertTrue(false);
        }
        catch (RuntimeSecurityException e)
        {
            assertTrue(e.getShortMessage().equals(securityErrorMessage.get(SecurityErrorMessage.UNEXPECTED_ERR, null))
                       && e.getDetailedMessage().equals(securityDetailedErrorMessage.get(SecurityDetailedErrorMessage.UNIDENTIFIED_APPLICATION, null)));
        }
        try
        {
            authenticationModule.verifyMIDletSuiteAuthenticity(appUID, new AuthenticationStorageData(
                        null,
                        null,
                        null,
                        null,
                        null,
                        ""));
            assertTrue(false);
        }
        catch (RuntimeSecurityException e)
        {
            assertTrue(e.getShortMessage().equals(securityErrorMessage.get(SecurityErrorMessage.UNEXPECTED_ERR, null))
                       && e.getDetailedMessage().equals(securityDetailedErrorMessage.get(SecurityDetailedErrorMessage.UNIDENTIFIED_APPLICATION, null)));
        }
        // cert_hash does not exist
        authenticationModule.verifyMIDletSuiteAuthenticity(appUID, new AuthenticationStorageData(
                    "protectionDomain",
                    "protectionDomainCategory",
                    "AE8772B3815F19020E55E70E0F83919AFCE4D164",
                    null,
                    TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar"));
        authenticationModule.verifyMIDletSuiteAuthenticity(appUID, new AuthenticationStorageData(
                    "protectionDomain",
                    "protectionDomainCategory",
                    "AE8772B3815F19020E55E70E0F83919AFCE4D164",
                    "",
                    TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar"));
        assertTrue(true);
        // jar path does not point to an existing jar
        try
        {
            authenticationModule.verifyMIDletSuiteAuthenticity(appUID, new AuthenticationStorageData(
                        null,
                        null,
                        "AE8772B3815F19020E55E70E0F83919AFCE4D164",
                        "6CC300CF",
                        "jar_path"));
            assertTrue(false);
        }
        catch (RuntimeSecurityException e)
        {
            assertTrue(e.getShortMessage().equals(securityErrorMessage.get(SecurityErrorMessage.JAR_NOT_FOUND, null))
                       && e.getDetailedMessage().equals(securityDetailedErrorMessage.get(SecurityDetailedErrorMessage.JAR_NOT_FOUND, null)));
        }
        // root not enabled
        try
        {
            authenticationModule.verifyMIDletSuiteAuthenticity(appUID, new AuthenticationStorageData(
                        null,
                        null,
                        "AE8772B3815F19020E55E70E0F83919AFCE4D164",
                        "unknownRoot",
                        TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar"));
            assertTrue(false);
        }
        catch (RuntimeSecurityException e)
        {
            assertTrue(e.getShortMessage().equals(securityErrorMessage.get(SecurityErrorMessage.CERT_NOT_AVAILABLE, null))
                       && e.getDetailedMessage().equals(securityDetailedErrorMessage.get(SecurityDetailedErrorMessage.SIM_CHANGED, null)));
        }
        // ok (signed MIDlet suite )case
        authenticationModule.verifyMIDletSuiteAuthenticity(appUID, new AuthenticationStorageData(
                    null,
                    null,
                    "AE8772B3815F19020E55E70E0F83919AFCE4D164",
                    "6CC300CF",
                    TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar"));
        assertTrue(true);
        // ok (unsigned MIDlet suite case)
        authenticationModule.verifyMIDletSuiteAuthenticity(appUID, new AuthenticationStorageData(
                    null,
                    null,
                    "AE8772B3815F19020E55E70E0F83919AFCE4D164",
                    null,
                    TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar"));
        assertTrue(true);
        // tampered jar
        try
        {
            authenticationModule.verifyMIDletSuiteAuthenticity(appUID, new AuthenticationStorageData(
                        null,
                        null,
                        "AE8772B3815F19020E55E70E0F83919AFCE4D164",
                        "6CC300CF",
                        TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "modified_app.jar"));
            assertTrue(false);
        }
        catch (RuntimeSecurityException e)
        {
            assertTrue(e.getShortMessage().equals(securityErrorMessage.get(SecurityErrorMessage.JAR_TAMPERED, null))
                       && e.getDetailedMessage().equals(securityDetailedErrorMessage.get(SecurityDetailedErrorMessage.JAR_TAMPERED, null)));
        }
        // ok network restrictions
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               "Nokia-MIDlet-Operator-Allowed"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "123-45 456-789 000-000"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        authenticationModule.verifyMIDletSuiteAuthenticity(appUID, new AuthenticationStorageData(
                    null,
                    "OPD",
                    "AE8772B3815F19020E55E70E0F83919AFCE4D164",
                    "6CC300CF",
                    TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar"));
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        assertTrue(true);
        // network restrictions for something else than operator MIDlet
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               "Nokia-MIDlet-Operator-Allowed"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "123-45"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        authenticationModule.verifyMIDletSuiteAuthenticity(appUID, new AuthenticationStorageData(
                    null,
                    "TTPD",
                    "AE8772B3815F19020E55E70E0F83919AFCE4D164",
                    "6CC300CF",
                    TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar"));
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        assertTrue(true);
        // network restrictions violated
        session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
        entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID,
                               appUID.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME,
                               "Nokia-MIDlet-Operator-Allowed"));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.VALUE,
                               "123-45"));
        session.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        try
        {
            authenticationModule.verifyMIDletSuiteAuthenticity(appUID, new AuthenticationStorageData(
                        null,
                        "OPD",
                        "AE8772B3815F19020E55E70E0F83919AFCE4D164",
                        "6CC300CF",
                        TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "app.jar"));
            session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
            assertTrue(false);
        }
        catch (RuntimeSecurityException e)
        {
            session.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, appUID);
            assertTrue(e.getShortMessage().equals(securityErrorMessage.get(SecurityErrorMessage.NETWORK_RESTRICTION_VIOLATION, null))
                       && e.getDetailedMessage().equals(securityDetailedErrorMessage.get(SecurityDetailedErrorMessage.NETWORK_RESTRICTION_VIOLATION, null)));
        }
    }

    private void testOcsp()
    {
        MyOcspEventListener oscpEventListener = new MyOcspEventListener();
        OcspEvent ocspEvent = null;
        InstallerSecurityException ocspError = null;
        allAttributes = new Hashtable();
        SecurityAttributes securityAttributes;
        StorageSession session = StorageFactory.createSession();
        session.open();
        Uid appUID = Uid.createUid("myUID");
        OcspSettings ocspSettings = null;
        OcspUserPreferences ocspUserPreferences = new OcspUserPreferences();
        SecurityStorage storage = new SecurityStorage();
        // UserInstallation: ocsp undefined, warning undefined, not silent mode, user prefs off
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_UNDEFINED, OcspSettings.OCSP_WARNING_UNDEFINED, false, "0", "0");
        ocspUserPreferences.setOcspMode(OcspUserPreferences.OCSP_MODE_OFF);
        ocspUserPreferences.setOcspUrl(null);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICyjCCAjOgAwIBAgIJAM2cvDMJ0ZWCMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTExMzY0N1oXDTE1MDIwNDExMzY0N1owVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAPKlvYkOR5iZw52pBc9C9zlmqMNqLhRnebv0hl0RRdNE7PHzqqVI64OTJ5Fqv9iP386ERMH3LH+e/IeZcqrn5LBcQmO37aWoE+hniPEZyEUmaVdeojOI6TfSfft0IPHjk7EQ2RVaCZpoKXHTd51WZL9s4EsvAHbbPC6yYxxVTYRAAAOrHSyW3Roa7CYYZH1MvK8hj+Z7PFDmkhf0AE2dlnwfR4YUbySIflWyLu+1BOBInr3+FyijWJPQXIns8AhexxaJrUAzYC5N9vBWpuxaHvf2XuQKt5mqOvFQZwMWNbaCtJJgoejsqCqn6F9W7DWPE9ioME+UEpESSNmx+9Wo2JMCAwEAATANBgkqhkiG9w0BAQUFAAOBgQBYEBGOrAJQSVkYi2FfXK11VyaVGxgujdAwa/iFxFDEIHthyKWpYJ16T1H3XwE1RjRkRn/IT2A7GSeu3naZgM/9TweZWbYPTvYz00FPFog3cTVaBezj3xnKXcpzgt2Fe3dV/ZcghbmWphe98Afm4R0GKTveIaoOMX/FsaC8l3syMg=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "iVq1INapoB9ghKhO0UuE7b3ckggYyUDjARitXruH3p9pr7pjBgSL7GXgBhdKnQWWFUIKeLCokMuJu+s+GWC8VkVvTaJU6Z85uGq2PtrQ9av7Dr/Omz9YMWaIUUGks8+gVB3Rv8pXICh3iJ+wcbWnECLmoPIGyH0cpBHUSVdux7+c+JIrxRPY755Pr+VESB3aa/F8EC2HjgxdQbPEI8Wlsv9Z2YcTMDUwUNW/3YyfjpskC9sIoJYoD2wF3bhntmO8dedr0oDaSU2ABPPOC6w5R7Gks0Kvx4T90Mecr9wMOSfKeQo3EnkELJ3SzAH0pePoY3rbl5leCGkGE1R56Rm8Ng=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        securityAttributes.addManifestAttributes(allAttributes);
        authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        assertTrue(true);
        // PreInstallation: ocsp disabled, warning undefined, silent mode
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_DISABLED, OcspSettings.OCSP_WARNING_UNDEFINED, true, "0", "0");
        ocspUserPreferences.setOcspMode(0);
        ocspUserPreferences.setOcspUrl(null);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICyjCCAjOgAwIBAgIJAM2cvDMJ0ZWCMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTExMzY0N1oXDTE1MDIwNDExMzY0N1owVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAPKlvYkOR5iZw52pBc9C9zlmqMNqLhRnebv0hl0RRdNE7PHzqqVI64OTJ5Fqv9iP386ERMH3LH+e/IeZcqrn5LBcQmO37aWoE+hniPEZyEUmaVdeojOI6TfSfft0IPHjk7EQ2RVaCZpoKXHTd51WZL9s4EsvAHbbPC6yYxxVTYRAAAOrHSyW3Roa7CYYZH1MvK8hj+Z7PFDmkhf0AE2dlnwfR4YUbySIflWyLu+1BOBInr3+FyijWJPQXIns8AhexxaJrUAzYC5N9vBWpuxaHvf2XuQKt5mqOvFQZwMWNbaCtJJgoejsqCqn6F9W7DWPE9ioME+UEpESSNmx+9Wo2JMCAwEAATANBgkqhkiG9w0BAQUFAAOBgQBYEBGOrAJQSVkYi2FfXK11VyaVGxgujdAwa/iFxFDEIHthyKWpYJ16T1H3XwE1RjRkRn/IT2A7GSeu3naZgM/9TweZWbYPTvYz00FPFog3cTVaBezj3xnKXcpzgt2Fe3dV/ZcghbmWphe98Afm4R0GKTveIaoOMX/FsaC8l3syMg=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "iVq1INapoB9ghKhO0UuE7b3ckggYyUDjARitXruH3p9pr7pjBgSL7GXgBhdKnQWWFUIKeLCokMuJu+s+GWC8VkVvTaJU6Z85uGq2PtrQ9av7Dr/Omz9YMWaIUUGks8+gVB3Rv8pXICh3iJ+wcbWnECLmoPIGyH0cpBHUSVdux7+c+JIrxRPY755Pr+VESB3aa/F8EC2HjgxdQbPEI8Wlsv9Z2YcTMDUwUNW/3YyfjpskC9sIoJYoD2wF3bhntmO8dedr0oDaSU2ABPPOC6w5R7Gks0Kvx4T90Mecr9wMOSfKeQo3EnkELJ3SzAH0pePoY3rbl5leCGkGE1R56Rm8Ng=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        securityAttributes.addManifestAttributes(allAttributes);
        authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        assertTrue(true);
        // PreInstallation: ocsp enabled, warning ignore, silent mode, ocsp url not set
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_ENABLED, OcspSettings.OCSP_WARNING_IGNORE, true, "0", "0");
        ocspUserPreferences.setOcspMode(-1);
        ocspUserPreferences.setOcspUrl(null);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICyjCCAjOgAwIBAgIJAM2cvDMJ0ZWCMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTExMzY0N1oXDTE1MDIwNDExMzY0N1owVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAPKlvYkOR5iZw52pBc9C9zlmqMNqLhRnebv0hl0RRdNE7PHzqqVI64OTJ5Fqv9iP386ERMH3LH+e/IeZcqrn5LBcQmO37aWoE+hniPEZyEUmaVdeojOI6TfSfft0IPHjk7EQ2RVaCZpoKXHTd51WZL9s4EsvAHbbPC6yYxxVTYRAAAOrHSyW3Roa7CYYZH1MvK8hj+Z7PFDmkhf0AE2dlnwfR4YUbySIflWyLu+1BOBInr3+FyijWJPQXIns8AhexxaJrUAzYC5N9vBWpuxaHvf2XuQKt5mqOvFQZwMWNbaCtJJgoejsqCqn6F9W7DWPE9ioME+UEpESSNmx+9Wo2JMCAwEAATANBgkqhkiG9w0BAQUFAAOBgQBYEBGOrAJQSVkYi2FfXK11VyaVGxgujdAwa/iFxFDEIHthyKWpYJ16T1H3XwE1RjRkRn/IT2A7GSeu3naZgM/9TweZWbYPTvYz00FPFog3cTVaBezj3xnKXcpzgt2Fe3dV/ZcghbmWphe98Afm4R0GKTveIaoOMX/FsaC8l3syMg=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "iVq1INapoB9ghKhO0UuE7b3ckggYyUDjARitXruH3p9pr7pjBgSL7GXgBhdKnQWWFUIKeLCokMuJu+s+GWC8VkVvTaJU6Z85uGq2PtrQ9av7Dr/Omz9YMWaIUUGks8+gVB3Rv8pXICh3iJ+wcbWnECLmoPIGyH0cpBHUSVdux7+c+JIrxRPY755Pr+VESB3aa/F8EC2HjgxdQbPEI8Wlsv9Z2YcTMDUwUNW/3YyfjpskC9sIoJYoD2wF3bhntmO8dedr0oDaSU2ABPPOC6w5R7Gks0Kvx4T90Mecr9wMOSfKeQo3EnkELJ3SzAH0pePoY3rbl5leCGkGE1R56Rm8Ng=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        securityAttributes.addManifestAttributes(allAttributes);
        authenticationModule.authenticateJar(appUID,null,TEST_DATA_DIR + "security_tmp" +  System.getProperty("file.separator") + "HelloWorld.jar", false);
        authenticationModule.addSecurityData(session, appUID, null);
        assertTrue(true);
    }

    private boolean checkCertificateDetails(SigningCertificate[] details)
    {
        if (details == null || details.length == 0)
        {
            return false;
        }
        for (int i=0; i<details.length; i++)
        {
            if (details[i].getIssuer() == null
                    || details[i].getIssuer().length() == 0
                    || details[i].getSerialNumber() == null
                    || details[i].getSerialNumber().length() == 0
                    || details[i].getFingerprint() == null
                    || details[i].getFingerprint().length() == 0
                    || details[i].getSubject() == null
                    || details[i].getSubject().length() == 0
                    || details[i].getOrganization() == null
                    || details[i].getOrganization().length() == 0)
            {
                return false;
            }
        }
        return true;
    }

    private boolean checkCertificateDetails(Certificate cert)
    {
        if (cert == null)
        {
            return false;
        }
        if (cert.getIssuer() == null
                || cert.getIssuer().length() == 0
                || cert.getSerialNumber() == null
                || cert.getSerialNumber().length() == 0
                || cert.getFingerprint() == null
                || cert.getFingerprint().length() == 0
                || cert.getSubject() == null
                || cert.getSubject().length() == 0)
        {
            return false;
        }
        return true;
    }

    class Monitor
    {
        Monitor()
        {
            iNotifyCnt = 0;
            iMonitor = new Object();
        }
        public void waitOnMonitor()
        {
            synchronized (iMonitor)
            {
                try
                {
                    if (iNotifyCnt > 0)
                    {
                        iNotifyCnt--;
                    }
                    else
                    {
                        iMonitor.wait();
                    }
                }
                catch (InterruptedException e) {}
            }
        }
        public void notifyMonitor()
        {
            synchronized (iMonitor)
            {
                iMonitor.notify();
                iNotifyCnt++;
            }
        }
        public void resetMonitor()
        {
            synchronized (iMonitor)
            {
                iNotifyCnt = 0;
            }
        }
        private Object iMonitor;
        private int iNotifyCnt;
    }

    public class OcspEvent
    {
        public int eventId;
        public Throwable eventError;
    }

    public class MyOcspEventListener
            implements OcspEventListener
    {
        private Monitor monitor = new Monitor();
        private OcspEvent event = null;
        public void ocspEvent(int eventId, Throwable eventError)
        {
            event = new OcspEvent();
            event.eventId = eventId;
            event.eventError = eventError;
            monitor.notifyMonitor();
        }

        public OcspEvent getEvent()
        {
            monitor.waitOnMonitor();
            return event;
        }

        public void reset()
        {
            monitor.resetMonitor();
        }
    }
}
