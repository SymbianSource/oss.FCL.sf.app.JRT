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
import com.nokia.mj.impl.security.midp.authentication.OcspUserPreferences;
import com.nokia.mj.impl.security.common.InstallerSecurityException;
import com.nokia.mj.impl.security.midp.common.SecurityAttributes;
import com.nokia.mj.impl.security.midp.common.AuthenticationAttribute;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.storage.*;
import com.nokia.mj.impl.security.midp.storage.SecurityStorage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.security.utils.SecurityErrorMessage;
import com.nokia.mj.impl.security.utils.SecurityDetailedErrorMessage;
import com.nokia.mj.impl.rt.ui.RuntimeUiFactory;
import com.nokia.mj.impl.rt.ui.RuntimeUi;
import com.nokia.mj.impl.rt.ui.ConfirmData;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * InteractiveAuthenticationModule unit tests.
 */
public class InteractiveAuthenticationModuleTests extends AuthenticationModuleTests
{

    private static String TEST_DATA_DIR;

    private static String OCSP_URL = "http://" + System.getProperty("com.nokia.mj.impl.installer.test.server") + ":8888";

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
    protected SecurityErrorMessage securityErrorMessage = new SecurityErrorMessage();
    protected SecurityDetailedErrorMessage securityDetailedErrorMessage = new SecurityDetailedErrorMessage();


    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new InteractiveAuthenticationModuleTests("testOcsp", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((InteractiveAuthenticationModuleTests)tc).testOcsp();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public InteractiveAuthenticationModuleTests()
    {
    }

    public InteractiveAuthenticationModuleTests(String aTestName, TestMethod aTestMethod)
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
    }

    protected void tearDown()
    {
    }

    private void testOcsp()
    {
        MyOcspEventListener oscpEventListener = new MyOcspEventListener();
        OcspEvent ocspEvent = null;
        InstallerSecurityException ocspError = null;
        OcspChecker ocspChecker = null;
        boolean sucess = false;
        allAttributes = new Hashtable();
        SecurityAttributes securityAttributes;
        StorageSession session = StorageFactory.createSession();
        session.open();
        Uid appUID = Uid.createUid("myUID");
        OcspSettings ocspSettings = null;
        OcspUserPreferences ocspUserPreferences = new OcspUserPreferences();
        SecurityStorage storage = new SecurityStorage();
        // cancel ocsp when the ocsp is off
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_UNDEFINED, OcspSettings.OCSP_WARNING_UNDEFINED, false, "0", "0");
        ocspUserPreferences.setOcspMode(OcspUserPreferences.OCSP_MODE_OFF);
        ocspUserPreferences.setOcspUrl(null);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICyjCCAjOgAwIBAgIJALGqj4FMKNfXMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTEzMzAyMloXDTE1MDIwNDEzMzAyMlowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMGxLn8s08e8/JFPO1nHf26Qdm8cUdMtTaEhhXCPT5T5nB2KSB3RiP8s5GfSRoTwEeL7/nwC3djM1MA/08RlX+FnwPT/8F7xiIiIw+Lx8jQWSgZcboUyVjYtAebCIpMQx86+4jGmzn3ujRz/2bBhxXPwNx2SgAPRanKeZbCtJtx8TYyms4ZNyV4+e59gG7aEmO6UbYs1t2K/aqpNkwpKJ+KXN5tquUYMBvjFFIWBvzLHWHM5zMHmj3wjxxjQjLqJPNgX3cnxKCiCJ3hcDcx0/gKR+DgckiNBq2pkw0p1hp8z8mvxt6uqB7kqM8xkjPnu0axwkJ60UyM6k6nq6PeEq60CAwEAATANBgkqhkiG9w0BAQUFAAOBgQA7OTu36H/9RDPzJM1CE6bgwkjheydGz/5ftu/jJquDz6u2d+5ceg2SQ8jxkBsBBQfQmGZYKoVnM9nJnT6mQwF124vb2bn3PteUIC0ImcUUp4mGeSFyC2ljs05irsDVEAq2gt/q+fo4kDHh5SYB+UE21au96hYz/+xJy4HV5tBcFQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "p5HbQtH9bV/3imK6rfFRVILj1BzefOP7ebXItNcKeWzSOfeYmebFSJha++GyKY/SM9Zuwrkh4rvFyFc39QdOel3vpUM2FmgnGPwEDbBfVy0TViTYK6SoUCLrBWviKIadbg4N8vVZI/wdrjox+wPUWNVzk+Wflo1rubpJtmIQf29D2F8I+picirvjSj6xSqekr+889P7bNVjyI+OUowZYWftuPi1y81Mo+kRIuaqTzBMCQw7AxMoBAA7BurAiFGtasr8GocuXnNRHb5vKKzA1msY4FJ/k4d9kDjq+OTAfME2nY1Y1qwRrK3WwSaep0+BfIPMb9knwU0ph6OPCMjskUA=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        cancelOcsp(appUID);
        assertTrue(true);
        // UserInstallation: ocsp undefined, warning undefined, not silent mode, user prefs must, ocsp url not set
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_UNDEFINED, OcspSettings.OCSP_WARNING_UNDEFINED, false, "0", "0");
        ocspUserPreferences.setOcspMode(OcspUserPreferences.OCSP_MODE_MUST);
        ocspUserPreferences.setOcspUrl(null);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        oscpEventListener.reset();
        authenticationModule.registerOcspEventListener(appUID, oscpEventListener);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICyjCCAjOgAwIBAgIJALGqj4FMKNfXMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTEzMzAyMloXDTE1MDIwNDEzMzAyMlowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMGxLn8s08e8/JFPO1nHf26Qdm8cUdMtTaEhhXCPT5T5nB2KSB3RiP8s5GfSRoTwEeL7/nwC3djM1MA/08RlX+FnwPT/8F7xiIiIw+Lx8jQWSgZcboUyVjYtAebCIpMQx86+4jGmzn3ujRz/2bBhxXPwNx2SgAPRanKeZbCtJtx8TYyms4ZNyV4+e59gG7aEmO6UbYs1t2K/aqpNkwpKJ+KXN5tquUYMBvjFFIWBvzLHWHM5zMHmj3wjxxjQjLqJPNgX3cnxKCiCJ3hcDcx0/gKR+DgckiNBq2pkw0p1hp8z8mvxt6uqB7kqM8xkjPnu0axwkJ60UyM6k6nq6PeEq60CAwEAATANBgkqhkiG9w0BAQUFAAOBgQA7OTu36H/9RDPzJM1CE6bgwkjheydGz/5ftu/jJquDz6u2d+5ceg2SQ8jxkBsBBQfQmGZYKoVnM9nJnT6mQwF124vb2bn3PteUIC0ImcUUp4mGeSFyC2ljs05irsDVEAq2gt/q+fo4kDHh5SYB+UE21au96hYz/+xJy4HV5tBcFQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "p5HbQtH9bV/3imK6rfFRVILj1BzefOP7ebXItNcKeWzSOfeYmebFSJha++GyKY/SM9Zuwrkh4rvFyFc39QdOel3vpUM2FmgnGPwEDbBfVy0TViTYK6SoUCLrBWviKIadbg4N8vVZI/wdrjox+wPUWNVzk+Wflo1rubpJtmIQf29D2F8I+picirvjSj6xSqekr+889P7bNVjyI+OUowZYWftuPi1y81Mo+kRIuaqTzBMCQw7AxMoBAA7BurAiFGtasr8GocuXnNRHb5vKKzA1msY4FJ/k4d9kDjq+OTAfME2nY1Y1qwRrK3WwSaep0+BfIPMb9knwU0ph6OPCMjskUA=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        ocspEvent = oscpEventListener.getEvent();
        sucess = (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_START_EVENT);
        ocspEvent = oscpEventListener.getEvent();
        authenticationModule.unregisterOcspEventListener(appUID);
        assertTrue(sucess == true && (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_FINISH_EVENT && (ocspEvent.eventError instanceof InstallerSecurityException)));
        ocspError = (InstallerSecurityException)ocspEvent.eventError;
        assertTrue(ocspError.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                   && ocspError.getShortMessage().equals(securityErrorMessage.get(SecurityErrorMessage.OCSP_GENERAL_ERR, null))
                   && ocspError.getDetailedMessage().equals(securityDetailedErrorMessage.get(SecurityDetailedErrorMessage.OCSP_SETTINGS_ERR,
                           null)));
        // UserInstallation: ocsp undefined, warning undefined, not silent mode, user prefs on, ocsp url not set -> show warning (choose yes)
        showGuidelines("At the next prompt, choose YES");
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_UNDEFINED, OcspSettings.OCSP_WARNING_UNDEFINED, false, "0", "0");
        ocspUserPreferences.setOcspMode(OcspUserPreferences.OCSP_MODE_ON);
        ocspUserPreferences.setOcspUrl(null);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        oscpEventListener.reset();
        authenticationModule.registerOcspEventListener(appUID, oscpEventListener);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICyjCCAjOgAwIBAgIJALGqj4FMKNfXMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTEzMzAyMloXDTE1MDIwNDEzMzAyMlowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMGxLn8s08e8/JFPO1nHf26Qdm8cUdMtTaEhhXCPT5T5nB2KSB3RiP8s5GfSRoTwEeL7/nwC3djM1MA/08RlX+FnwPT/8F7xiIiIw+Lx8jQWSgZcboUyVjYtAebCIpMQx86+4jGmzn3ujRz/2bBhxXPwNx2SgAPRanKeZbCtJtx8TYyms4ZNyV4+e59gG7aEmO6UbYs1t2K/aqpNkwpKJ+KXN5tquUYMBvjFFIWBvzLHWHM5zMHmj3wjxxjQjLqJPNgX3cnxKCiCJ3hcDcx0/gKR+DgckiNBq2pkw0p1hp8z8mvxt6uqB7kqM8xkjPnu0axwkJ60UyM6k6nq6PeEq60CAwEAATANBgkqhkiG9w0BAQUFAAOBgQA7OTu36H/9RDPzJM1CE6bgwkjheydGz/5ftu/jJquDz6u2d+5ceg2SQ8jxkBsBBQfQmGZYKoVnM9nJnT6mQwF124vb2bn3PteUIC0ImcUUp4mGeSFyC2ljs05irsDVEAq2gt/q+fo4kDHh5SYB+UE21au96hYz/+xJy4HV5tBcFQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "p5HbQtH9bV/3imK6rfFRVILj1BzefOP7ebXItNcKeWzSOfeYmebFSJha++GyKY/SM9Zuwrkh4rvFyFc39QdOel3vpUM2FmgnGPwEDbBfVy0TViTYK6SoUCLrBWviKIadbg4N8vVZI/wdrjox+wPUWNVzk+Wflo1rubpJtmIQf29D2F8I+picirvjSj6xSqekr+889P7bNVjyI+OUowZYWftuPi1y81Mo+kRIuaqTzBMCQw7AxMoBAA7BurAiFGtasr8GocuXnNRHb5vKKzA1msY4FJ/k4d9kDjq+OTAfME2nY1Y1qwRrK3WwSaep0+BfIPMb9knwU0ph6OPCMjskUA=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        ocspEvent = oscpEventListener.getEvent();
        sucess = (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_START_EVENT);
        ocspEvent = oscpEventListener.getEvent();
        authenticationModule.unregisterOcspEventListener(appUID);
        assertTrue(sucess == true && (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_FINISH_EVENT));
        // UserInstallation: ocsp undefined, warning undefined, not silent mode, user prefs on, ocsp url not set -> show warning (choose no)
        showGuidelines("At the next prompt, choose NO");
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_UNDEFINED, OcspSettings.OCSP_WARNING_UNDEFINED, false, "0", "0");
        ocspUserPreferences.setOcspMode(OcspUserPreferences.OCSP_MODE_ON);
        ocspUserPreferences.setOcspUrl(null);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        oscpEventListener.reset();
        authenticationModule.registerOcspEventListener(appUID, oscpEventListener);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICyjCCAjOgAwIBAgIJALGqj4FMKNfXMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDIwNTEzMzAyMloXDTE1MDIwNDEzMzAyMlowVzELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk15IENvbW1vbiBOYW1lMR0wGwYDVQQKExRNeSBPcmdhbml6YXRpb24gTmFtZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMGxLn8s08e8/JFPO1nHf26Qdm8cUdMtTaEhhXCPT5T5nB2KSB3RiP8s5GfSRoTwEeL7/nwC3djM1MA/08RlX+FnwPT/8F7xiIiIw+Lx8jQWSgZcboUyVjYtAebCIpMQx86+4jGmzn3ujRz/2bBhxXPwNx2SgAPRanKeZbCtJtx8TYyms4ZNyV4+e59gG7aEmO6UbYs1t2K/aqpNkwpKJ+KXN5tquUYMBvjFFIWBvzLHWHM5zMHmj3wjxxjQjLqJPNgX3cnxKCiCJ3hcDcx0/gKR+DgckiNBq2pkw0p1hp8z8mvxt6uqB7kqM8xkjPnu0axwkJ60UyM6k6nq6PeEq60CAwEAATANBgkqhkiG9w0BAQUFAAOBgQA7OTu36H/9RDPzJM1CE6bgwkjheydGz/5ftu/jJquDz6u2d+5ceg2SQ8jxkBsBBQfQmGZYKoVnM9nJnT6mQwF124vb2bn3PteUIC0ImcUUp4mGeSFyC2ljs05irsDVEAq2gt/q+fo4kDHh5SYB+UE21au96hYz/+xJy4HV5tBcFQ=="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "p5HbQtH9bV/3imK6rfFRVILj1BzefOP7ebXItNcKeWzSOfeYmebFSJha++GyKY/SM9Zuwrkh4rvFyFc39QdOel3vpUM2FmgnGPwEDbBfVy0TViTYK6SoUCLrBWviKIadbg4N8vVZI/wdrjox+wPUWNVzk+Wflo1rubpJtmIQf29D2F8I+picirvjSj6xSqekr+889P7bNVjyI+OUowZYWftuPi1y81Mo+kRIuaqTzBMCQw7AxMoBAA7BurAiFGtasr8GocuXnNRHb5vKKzA1msY4FJ/k4d9kDjq+OTAfME2nY1Y1qwRrK3WwSaep0+BfIPMb9knwU0ph6OPCMjskUA=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        ocspEvent = oscpEventListener.getEvent();
        sucess = (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_START_EVENT);
        ocspEvent = oscpEventListener.getEvent();
        authenticationModule.unregisterOcspEventListener(appUID);
        assertTrue(ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_FINISH_EVENT && (ocspEvent.eventError instanceof InstallerSecurityException));
        ocspError = (InstallerSecurityException)ocspEvent.eventError;
        assertTrue(ocspError.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                   && ocspError.getShortMessage().equals(securityErrorMessage.get(SecurityErrorMessage.OCSP_GENERAL_ERR, null))
                   && ocspError.getDetailedMessage().equals(securityDetailedErrorMessage.get(SecurityDetailedErrorMessage.OCSP_SETTINGS_ERR,
                           null)));
        // cancel ocsp when the ocsp is on
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_UNDEFINED, OcspSettings.OCSP_WARNING_UNDEFINED, false, "0", "0");
        ocspUserPreferences.setOcspMode(OcspUserPreferences.OCSP_MODE_ON);
        ocspUserPreferences.setOcspUrl(OCSP_URL);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        oscpEventListener.reset();
        authenticationModule.registerOcspEventListener(appUID, oscpEventListener);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICuTCCAiKgAwIBAgIJAKdHK8lkGvSqMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDkyMDA3Mzk0M1oXDTE1MDkxOTA3Mzk0M1owRjELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk9DU1AgR09PRCBDZXJ0MQwwCgYDVQQKEwNPcmcwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDRHWZYToWVP8is/TWKzRQaRc0SmPNOKdFqkjJYF2NjPmz/EABJLKps4m2BZzN2kMIBRkTuirLKjnsdSelI8itekzZBJJMPgtBvsXn+wa3fPaHa3Lqhp9oa8rr/DaCCnp/B4Id/x6HV53uA8e4qs3PeDnagvomBCQsVApWpL1tcnIow1vMEHIA5uqTOVhCKgwXnnoWfl7AWmh6Tu+SbMYALj8fGzvDhPmxIhVte9NaF462Cxf0B6EuCEEdh0xEWw4qLakWpH9QlwZnshfYFaoSYLqLp83E84Md8SkcraLZed8nDS93Gh++rEjgQSldzkf0MIvZyrmKwf+vgNU+WpvzJAgMBAAEwDQYJKoZIhvcNAQEFBQADgYEACgN//8SRGqi50c5gmqZdH0YVC7ABUpBNMP1mP9z3+qadtHOOqIKn3q34rvKzyiZEndufZAt3vUQ1s+wlaJKQTUgAm7G8K8cYYCkexJoQVdfVTyJn2ZKTOkCdfoY9k/uxu3DmHhHteU7j0mmp1LVXrvrSGLtXmpzy+kHvVeH/UMw="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "NL+t9rmzVP7MFfYomU0pb/daz27COxIJ4qMxToYs4oHOn8hk4vwvdm//ORJZnLmk84gfnJRufassInIkwCszsxVoPx47jTi+tqSxFF398kT8MJdFNhr/F1CDQR6vPwMdW5CsRefPGBpr2IaL8oZRfzBDgEZL0bu8PPOOrFZCJbw69SACucYvsReQ1vHJf9XPu4eGEkIZHd9If2Bj/G8rxqUGzQ/ltVhUhhLj8rqrDghRF3+UeB1UPcCd6slKHQ+EyxKU/o2iuMe3B5rmcjDaoj52KsB+gk0puizvEgKn3F15a430u4/feKpqszDvFCWd8qxbWwBDDbLC92P2rc3T8g=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        cancelOcsp(appUID);
        ocspEvent = oscpEventListener.getEvent();
        if (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_START_EVENT)
        {
            ocspEvent = oscpEventListener.getEvent();
        }
        sucess = (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_FINISH_EVENT);
        authenticationModule.unregisterOcspEventListener(appUID);
        assertTrue(sucess);
        // UserInstallation: ocsp undefined, warning undefined, not silent mode, user prefs on, unknown -> fail
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_UNDEFINED, OcspSettings.OCSP_WARNING_UNDEFINED, false, "0", "0");
        ocspUserPreferences.setOcspMode(OcspUserPreferences.OCSP_MODE_ON);
        ocspUserPreferences.setOcspUrl(OCSP_URL);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        oscpEventListener.reset();
        authenticationModule.registerOcspEventListener(appUID, oscpEventListener);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICvDCCAiWgAwIBAgIJAKrSZo9qYhM/MA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDkyMDA3NTMzNVoXDTE1MDkxOTA3NTMzNVowSTELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxGjAYBgNVBAMTEU9DU1AgVU5LTk9XTiBDZXJ0MQwwCgYDVQQKEwNPcmcwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCaZ4VDJjntiOeNKbRd8BIkEjW/Z6k/fLzbiM5hQL/xHQC1PM1spLgnr4pf7Nf+F/bJbDq2eE1jDar6wmtJPcXFFkChl1eS5yJAyY/bhswyzbqNwTcwo1Lu64TSfC4gTdthEmUPNEhzxMcdCj6myrv8hNyhv6dhkcHvZgBwFgS4Pf1DXRmr/PPjCZxyMaoLoKzjezBXVgTC9rViN7moL715HrpO1jkIB2LRY93fzBmPa1leNdiqUqachFtkzYQuuO1MLlmHXKlNmNLOGqlreB/58q5PTPbIGOIzNiN+n0RYMkVSBVImLBQeepO2ICi/DSFW0yi70LpZvebx736P0yF1AgMBAAEwDQYJKoZIhvcNAQEFBQADgYEAGBzUyF4Cb/DN4lWY5yxd1/EI/6osAHySIgMEu5ClzyIqULc3QIvjhLrbLV/JKsVYT3h/gFZxwxoTGWdE1dApWj+Hja23TlLmo0uDDL/L3NOB/n041/gI2MQYw1Z9ME2sFzzWIli1P02JciykUQba/b0fRjPsY3ctzDS7QU2EFFs="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "UknTEEInZKDV9RCXnhv8W9KrXYppmRCUL/kSYXtdiiD3OmiwK0/4Nu0N8ix/Olo/IuWNLyPR+YZt6GLUQhw/AlWGd1mq8odlpiulFdB+e8ClkPvOW+3/KMvBO74ajyD2/PreEzCPYouM8YBqdpb49+TffnNQmEdaIpZ659ODppSNzMPmFpCcO6J2SoaU+KOp9uYdNab+JQfkGwrhF4F3FPYsQdPJAgS8Wt//khjQzAGFyOFIWmZ1ZwTbLxyf2QMHPrvtDWWH6h9qaR+2O9A90tF0PM2VUjznEOvBNigKwYTqBAW2j5oF6ZOQ4zyujqpQsm0pF6aPzaVjtZmfnpaCfA=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        ocspEvent = oscpEventListener.getEvent();
        sucess = (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_START_EVENT);
        ocspEvent = oscpEventListener.getEvent();
        authenticationModule.unregisterOcspEventListener(appUID);
        assertTrue(ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_FINISH_EVENT && (ocspEvent.eventError instanceof InstallerSecurityException));
        ocspError = (InstallerSecurityException)ocspEvent.eventError;
        assertTrue(ocspError.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                   && ocspError.getShortMessage().equals(securityErrorMessage.get(SecurityErrorMessage.OCSP_GENERAL_ERR, null))
                   && ocspError.getDetailedMessage().equals(securityDetailedErrorMessage.get(SecurityDetailedErrorMessage.OCSP_GENERAL_ERR,
                           null)));
        // UserInstallation: ocsp undefined, warning undefined, not silent mode, user prefs on, revoked
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_UNDEFINED, OcspSettings.OCSP_WARNING_UNDEFINED, false, "0", "0");
        ocspUserPreferences.setOcspMode(OcspUserPreferences.OCSP_MODE_ON);
        ocspUserPreferences.setOcspUrl(OCSP_URL);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        oscpEventListener.reset();
        authenticationModule.registerOcspEventListener(appUID, oscpEventListener);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICvDCCAiWgAwIBAgIJAM/3P7mVjkjRMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDkyMDA3NTcxMVoXDTE1MDkxOTA3NTcxMVowSTELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxGjAYBgNVBAMTEU9DU1AgUkVWT0tFRCBDZXJ0MQwwCgYDVQQKEwNPcmcwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDGIt32Ki+UkQYc/rswY4Bho/ygLyqV8UTVob4yH4k9AQTegIkdjw+AqgfU0LcqMdXguok2PS0sdkgEDPqCMawSMsQRQMsFYcab/LL/i7haWULQL33VI8Wt9grgzFZSpeuhFgr3YgWV0iyMHpKwb4ORiJvWhhvseCK+iI4sAOIhmQZ12HLhvBla5KXTyslLUoXA7V0CuRYqkB3SXn/RARk7GHcoVM+PIk6MtPUkHlyZzqXbHeNOAxGhSEbIp6WJTG2Bg/5OmdSg/kJ44ZSKMk0K2djNTgx70s83B+/vOU81Zxo+SUGiaOC/uw9Irh51WlZl8joq3A+gefO+fPSKCJZDAgMBAAEwDQYJKoZIhvcNAQEFBQADgYEAnk5fcldVed5INUs3djxRF9BZ1+ZFdgIjclysYB6B2mSvIdnxnT9H66oKnPeyDqbwcWYfdyDwyeBgsLIk4oOF85lWs5ItjKKcViLbZDPi9J3B+Av/qCWkrkWQokJ5Tkyw8K/sBQrNTM5kH1WrapcKsplcv8FgorE7FC57Ozpli48="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "KXg8rbURv+eZ75Cg0+8KPgaxhhrHqyf26nLFweLWKE7rgkX6aHHfmHirpt/LGzFqQt+516bbkVhG7DLJIshb3qoZBlgzKOqjvJoYwMnVw0CaIMoYjcevortyKk8jbeFnrYU34pp8v0pz3NEKzVgnYAdEnTtl88X+TBgcVDteVUOaz2IGB4JtPKWWRhG7kaOJdYXmCRuNeExv1gHqLYNq0Mr6rmECoMpV1ksrTJK7bJeWwOHRgtVo55rb6bI2MmzE5/Eo4LWg9MKjzmHnCbBWKvFyy5RuEZu5XHAzCJlLO52rfh6dJOq5yuPyB+L87bsgehWFyj1ptq+2jESw0KXuHQ=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        ocspEvent = oscpEventListener.getEvent();
        sucess = (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_START_EVENT);
        ocspEvent = oscpEventListener.getEvent();
        authenticationModule.unregisterOcspEventListener(appUID);
        assertTrue(ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_FINISH_EVENT && (ocspEvent.eventError instanceof InstallerSecurityException));
        ocspError = (InstallerSecurityException)ocspEvent.eventError;
        assertTrue(ocspError.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                   && ocspError.getShortMessage().equals(securityErrorMessage.get(SecurityErrorMessage.OCSP_GENERAL_ERR, null))
                   && ocspError.getDetailedMessage().equals(securityDetailedErrorMessage.get(SecurityDetailedErrorMessage.OCSP_REVOKED_ERR,
                           null)));
        // UserInstallation: ocsp undefined, warning undefined, not silent mode, user prefs on, good
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_UNDEFINED, OcspSettings.OCSP_WARNING_UNDEFINED, false, "0", "0");
        ocspUserPreferences.setOcspMode(OcspUserPreferences.OCSP_MODE_ON);
        ocspUserPreferences.setOcspUrl(OCSP_URL);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        oscpEventListener.reset();
        authenticationModule.registerOcspEventListener(appUID, oscpEventListener);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICuTCCAiKgAwIBAgIJAKdHK8lkGvSqMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDkyMDA3Mzk0M1oXDTE1MDkxOTA3Mzk0M1owRjELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk9DU1AgR09PRCBDZXJ0MQwwCgYDVQQKEwNPcmcwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDRHWZYToWVP8is/TWKzRQaRc0SmPNOKdFqkjJYF2NjPmz/EABJLKps4m2BZzN2kMIBRkTuirLKjnsdSelI8itekzZBJJMPgtBvsXn+wa3fPaHa3Lqhp9oa8rr/DaCCnp/B4Id/x6HV53uA8e4qs3PeDnagvomBCQsVApWpL1tcnIow1vMEHIA5uqTOVhCKgwXnnoWfl7AWmh6Tu+SbMYALj8fGzvDhPmxIhVte9NaF462Cxf0B6EuCEEdh0xEWw4qLakWpH9QlwZnshfYFaoSYLqLp83E84Md8SkcraLZed8nDS93Gh++rEjgQSldzkf0MIvZyrmKwf+vgNU+WpvzJAgMBAAEwDQYJKoZIhvcNAQEFBQADgYEACgN//8SRGqi50c5gmqZdH0YVC7ABUpBNMP1mP9z3+qadtHOOqIKn3q34rvKzyiZEndufZAt3vUQ1s+wlaJKQTUgAm7G8K8cYYCkexJoQVdfVTyJn2ZKTOkCdfoY9k/uxu3DmHhHteU7j0mmp1LVXrvrSGLtXmpzy+kHvVeH/UMw="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "NL+t9rmzVP7MFfYomU0pb/daz27COxIJ4qMxToYs4oHOn8hk4vwvdm//ORJZnLmk84gfnJRufassInIkwCszsxVoPx47jTi+tqSxFF398kT8MJdFNhr/F1CDQR6vPwMdW5CsRefPGBpr2IaL8oZRfzBDgEZL0bu8PPOOrFZCJbw69SACucYvsReQ1vHJf9XPu4eGEkIZHd9If2Bj/G8rxqUGzQ/ltVhUhhLj8rqrDghRF3+UeB1UPcCd6slKHQ+EyxKU/o2iuMe3B5rmcjDaoj52KsB+gk0puizvEgKn3F15a430u4/feKpqszDvFCWd8qxbWwBDDbLC92P2rc3T8g=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        ocspEvent = oscpEventListener.getEvent();
        sucess = (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_START_EVENT);
        ocspEvent = oscpEventListener.getEvent();
        authenticationModule.unregisterOcspEventListener(appUID);
        assertTrue(ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_FINISH_EVENT && ocspEvent.eventError == null);
        // UserInstallation: ocsp undefined, warning undefined, not silent mode, user prefs must, revoked
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_UNDEFINED, OcspSettings.OCSP_WARNING_UNDEFINED, false, "0", "0");
        ocspUserPreferences.setOcspMode(OcspUserPreferences.OCSP_MODE_MUST);
        ocspUserPreferences.setOcspUrl(OCSP_URL);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        oscpEventListener.reset();
        authenticationModule.registerOcspEventListener(appUID, oscpEventListener);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICvDCCAiWgAwIBAgIJAM/3P7mVjkjRMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDkyMDA3NTcxMVoXDTE1MDkxOTA3NTcxMVowSTELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxGjAYBgNVBAMTEU9DU1AgUkVWT0tFRCBDZXJ0MQwwCgYDVQQKEwNPcmcwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDGIt32Ki+UkQYc/rswY4Bho/ygLyqV8UTVob4yH4k9AQTegIkdjw+AqgfU0LcqMdXguok2PS0sdkgEDPqCMawSMsQRQMsFYcab/LL/i7haWULQL33VI8Wt9grgzFZSpeuhFgr3YgWV0iyMHpKwb4ORiJvWhhvseCK+iI4sAOIhmQZ12HLhvBla5KXTyslLUoXA7V0CuRYqkB3SXn/RARk7GHcoVM+PIk6MtPUkHlyZzqXbHeNOAxGhSEbIp6WJTG2Bg/5OmdSg/kJ44ZSKMk0K2djNTgx70s83B+/vOU81Zxo+SUGiaOC/uw9Irh51WlZl8joq3A+gefO+fPSKCJZDAgMBAAEwDQYJKoZIhvcNAQEFBQADgYEAnk5fcldVed5INUs3djxRF9BZ1+ZFdgIjclysYB6B2mSvIdnxnT9H66oKnPeyDqbwcWYfdyDwyeBgsLIk4oOF85lWs5ItjKKcViLbZDPi9J3B+Av/qCWkrkWQokJ5Tkyw8K/sBQrNTM5kH1WrapcKsplcv8FgorE7FC57Ozpli48="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "KXg8rbURv+eZ75Cg0+8KPgaxhhrHqyf26nLFweLWKE7rgkX6aHHfmHirpt/LGzFqQt+516bbkVhG7DLJIshb3qoZBlgzKOqjvJoYwMnVw0CaIMoYjcevortyKk8jbeFnrYU34pp8v0pz3NEKzVgnYAdEnTtl88X+TBgcVDteVUOaz2IGB4JtPKWWRhG7kaOJdYXmCRuNeExv1gHqLYNq0Mr6rmECoMpV1ksrTJK7bJeWwOHRgtVo55rb6bI2MmzE5/Eo4LWg9MKjzmHnCbBWKvFyy5RuEZu5XHAzCJlLO52rfh6dJOq5yuPyB+L87bsgehWFyj1ptq+2jESw0KXuHQ=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        ocspEvent = oscpEventListener.getEvent();
        sucess = (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_START_EVENT);
        ocspEvent = oscpEventListener.getEvent();
        authenticationModule.unregisterOcspEventListener(appUID);
        assertTrue(ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_FINISH_EVENT && (ocspEvent.eventError instanceof InstallerSecurityException));
        ocspError = (InstallerSecurityException)ocspEvent.eventError;
        assertTrue(ocspError.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                   && ocspError.getShortMessage().equals(securityErrorMessage.get(SecurityErrorMessage.OCSP_GENERAL_ERR, null))
                   && ocspError.getDetailedMessage().equals(securityDetailedErrorMessage.get(SecurityDetailedErrorMessage.OCSP_REVOKED_ERR,
                           null)));
        // UserInstallation: ocsp undefined, warning undefined, not silent mode, user prefs must, good
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_UNDEFINED, OcspSettings.OCSP_WARNING_UNDEFINED, false, "0", "0");
        ocspUserPreferences.setOcspMode(OcspUserPreferences.OCSP_MODE_MUST);
        ocspUserPreferences.setOcspUrl(OCSP_URL);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        oscpEventListener.reset();
        authenticationModule.registerOcspEventListener(appUID, oscpEventListener);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICuTCCAiKgAwIBAgIJAKdHK8lkGvSqMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDkyMDA3Mzk0M1oXDTE1MDkxOTA3Mzk0M1owRjELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk9DU1AgR09PRCBDZXJ0MQwwCgYDVQQKEwNPcmcwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDRHWZYToWVP8is/TWKzRQaRc0SmPNOKdFqkjJYF2NjPmz/EABJLKps4m2BZzN2kMIBRkTuirLKjnsdSelI8itekzZBJJMPgtBvsXn+wa3fPaHa3Lqhp9oa8rr/DaCCnp/B4Id/x6HV53uA8e4qs3PeDnagvomBCQsVApWpL1tcnIow1vMEHIA5uqTOVhCKgwXnnoWfl7AWmh6Tu+SbMYALj8fGzvDhPmxIhVte9NaF462Cxf0B6EuCEEdh0xEWw4qLakWpH9QlwZnshfYFaoSYLqLp83E84Md8SkcraLZed8nDS93Gh++rEjgQSldzkf0MIvZyrmKwf+vgNU+WpvzJAgMBAAEwDQYJKoZIhvcNAQEFBQADgYEACgN//8SRGqi50c5gmqZdH0YVC7ABUpBNMP1mP9z3+qadtHOOqIKn3q34rvKzyiZEndufZAt3vUQ1s+wlaJKQTUgAm7G8K8cYYCkexJoQVdfVTyJn2ZKTOkCdfoY9k/uxu3DmHhHteU7j0mmp1LVXrvrSGLtXmpzy+kHvVeH/UMw="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "NL+t9rmzVP7MFfYomU0pb/daz27COxIJ4qMxToYs4oHOn8hk4vwvdm//ORJZnLmk84gfnJRufassInIkwCszsxVoPx47jTi+tqSxFF398kT8MJdFNhr/F1CDQR6vPwMdW5CsRefPGBpr2IaL8oZRfzBDgEZL0bu8PPOOrFZCJbw69SACucYvsReQ1vHJf9XPu4eGEkIZHd9If2Bj/G8rxqUGzQ/ltVhUhhLj8rqrDghRF3+UeB1UPcCd6slKHQ+EyxKU/o2iuMe3B5rmcjDaoj52KsB+gk0puizvEgKn3F15a430u4/feKpqszDvFCWd8qxbWwBDDbLC92P2rc3T8g=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        ocspEvent = oscpEventListener.getEvent();
        sucess = (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_START_EVENT);
        ocspEvent = oscpEventListener.getEvent();
        authenticationModule.unregisterOcspEventListener(appUID);
        assertTrue(ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_FINISH_EVENT && ocspEvent.eventError == null);
        // UserInstallation: ocsp undefined, warning undefined, not silent mode, user prefs must, unknown
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_UNDEFINED, OcspSettings.OCSP_WARNING_UNDEFINED, false, "0", "0");
        ocspUserPreferences.setOcspMode(OcspUserPreferences.OCSP_MODE_MUST);
        ocspUserPreferences.setOcspUrl(OCSP_URL);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        oscpEventListener.reset();
        authenticationModule.registerOcspEventListener(appUID, oscpEventListener);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICvDCCAiWgAwIBAgIJAKrSZo9qYhM/MA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDkyMDA3NTMzNVoXDTE1MDkxOTA3NTMzNVowSTELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxGjAYBgNVBAMTEU9DU1AgVU5LTk9XTiBDZXJ0MQwwCgYDVQQKEwNPcmcwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCaZ4VDJjntiOeNKbRd8BIkEjW/Z6k/fLzbiM5hQL/xHQC1PM1spLgnr4pf7Nf+F/bJbDq2eE1jDar6wmtJPcXFFkChl1eS5yJAyY/bhswyzbqNwTcwo1Lu64TSfC4gTdthEmUPNEhzxMcdCj6myrv8hNyhv6dhkcHvZgBwFgS4Pf1DXRmr/PPjCZxyMaoLoKzjezBXVgTC9rViN7moL715HrpO1jkIB2LRY93fzBmPa1leNdiqUqachFtkzYQuuO1MLlmHXKlNmNLOGqlreB/58q5PTPbIGOIzNiN+n0RYMkVSBVImLBQeepO2ICi/DSFW0yi70LpZvebx736P0yF1AgMBAAEwDQYJKoZIhvcNAQEFBQADgYEAGBzUyF4Cb/DN4lWY5yxd1/EI/6osAHySIgMEu5ClzyIqULc3QIvjhLrbLV/JKsVYT3h/gFZxwxoTGWdE1dApWj+Hja23TlLmo0uDDL/L3NOB/n041/gI2MQYw1Z9ME2sFzzWIli1P02JciykUQba/b0fRjPsY3ctzDS7QU2EFFs="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "UknTEEInZKDV9RCXnhv8W9KrXYppmRCUL/kSYXtdiiD3OmiwK0/4Nu0N8ix/Olo/IuWNLyPR+YZt6GLUQhw/AlWGd1mq8odlpiulFdB+e8ClkPvOW+3/KMvBO74ajyD2/PreEzCPYouM8YBqdpb49+TffnNQmEdaIpZ659ODppSNzMPmFpCcO6J2SoaU+KOp9uYdNab+JQfkGwrhF4F3FPYsQdPJAgS8Wt//khjQzAGFyOFIWmZ1ZwTbLxyf2QMHPrvtDWWH6h9qaR+2O9A90tF0PM2VUjznEOvBNigKwYTqBAW2j5oF6ZOQ4zyujqpQsm0pF6aPzaVjtZmfnpaCfA=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        ocspEvent = oscpEventListener.getEvent();
        sucess = (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_START_EVENT);
        ocspEvent = oscpEventListener.getEvent();
        authenticationModule.unregisterOcspEventListener(appUID);
        assertTrue(ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_FINISH_EVENT && (ocspEvent.eventError instanceof InstallerSecurityException));
        ocspError = (InstallerSecurityException)ocspEvent.eventError;
        assertTrue(ocspError.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                   && ocspError.getShortMessage().equals(securityErrorMessage.get(SecurityErrorMessage.OCSP_GENERAL_ERR, null))
                   && ocspError.getDetailedMessage().equals(securityDetailedErrorMessage.get(SecurityDetailedErrorMessage.OCSP_GENERAL_ERR,
                           null)));
        // PreInstallation: ocsp enabled, warning ignore, silent mode, revoked
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_ENABLED, OcspSettings.OCSP_WARNING_IGNORE, true, "0", "0");
        ocspUserPreferences.setOcspMode(-1);
        ocspUserPreferences.setOcspUrl(OCSP_URL);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        oscpEventListener.reset();
        authenticationModule.registerOcspEventListener(appUID, oscpEventListener);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICvDCCAiWgAwIBAgIJAM/3P7mVjkjRMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDkyMDA3NTcxMVoXDTE1MDkxOTA3NTcxMVowSTELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxGjAYBgNVBAMTEU9DU1AgUkVWT0tFRCBDZXJ0MQwwCgYDVQQKEwNPcmcwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDGIt32Ki+UkQYc/rswY4Bho/ygLyqV8UTVob4yH4k9AQTegIkdjw+AqgfU0LcqMdXguok2PS0sdkgEDPqCMawSMsQRQMsFYcab/LL/i7haWULQL33VI8Wt9grgzFZSpeuhFgr3YgWV0iyMHpKwb4ORiJvWhhvseCK+iI4sAOIhmQZ12HLhvBla5KXTyslLUoXA7V0CuRYqkB3SXn/RARk7GHcoVM+PIk6MtPUkHlyZzqXbHeNOAxGhSEbIp6WJTG2Bg/5OmdSg/kJ44ZSKMk0K2djNTgx70s83B+/vOU81Zxo+SUGiaOC/uw9Irh51WlZl8joq3A+gefO+fPSKCJZDAgMBAAEwDQYJKoZIhvcNAQEFBQADgYEAnk5fcldVed5INUs3djxRF9BZ1+ZFdgIjclysYB6B2mSvIdnxnT9H66oKnPeyDqbwcWYfdyDwyeBgsLIk4oOF85lWs5ItjKKcViLbZDPi9J3B+Av/qCWkrkWQokJ5Tkyw8K/sBQrNTM5kH1WrapcKsplcv8FgorE7FC57Ozpli48="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "KXg8rbURv+eZ75Cg0+8KPgaxhhrHqyf26nLFweLWKE7rgkX6aHHfmHirpt/LGzFqQt+516bbkVhG7DLJIshb3qoZBlgzKOqjvJoYwMnVw0CaIMoYjcevortyKk8jbeFnrYU34pp8v0pz3NEKzVgnYAdEnTtl88X+TBgcVDteVUOaz2IGB4JtPKWWRhG7kaOJdYXmCRuNeExv1gHqLYNq0Mr6rmECoMpV1ksrTJK7bJeWwOHRgtVo55rb6bI2MmzE5/Eo4LWg9MKjzmHnCbBWKvFyy5RuEZu5XHAzCJlLO52rfh6dJOq5yuPyB+L87bsgehWFyj1ptq+2jESw0KXuHQ=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        ocspEvent = oscpEventListener.getEvent();
        sucess = (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_START_EVENT);
        ocspEvent = oscpEventListener.getEvent();
        authenticationModule.unregisterOcspEventListener(appUID);
        assertTrue(ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_FINISH_EVENT && (ocspEvent.eventError instanceof InstallerSecurityException));
        ocspError = (InstallerSecurityException)ocspEvent.eventError;
        assertTrue(ocspError.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                   && ocspError.getShortMessage().equals(securityErrorMessage.get(SecurityErrorMessage.OCSP_GENERAL_ERR, null))
                   && ocspError.getDetailedMessage().equals(securityDetailedErrorMessage.get(SecurityDetailedErrorMessage.OCSP_REVOKED_ERR,
                           null)));
        // PreInstallation: ocsp enabled, warning ignore, silent mode, good
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_ENABLED, OcspSettings.OCSP_WARNING_IGNORE, true, "0", "0");
        ocspUserPreferences.setOcspMode(-1);
        ocspUserPreferences.setOcspUrl(OCSP_URL);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        oscpEventListener.reset();
        authenticationModule.registerOcspEventListener(appUID, oscpEventListener);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICuTCCAiKgAwIBAgIJAKdHK8lkGvSqMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDkyMDA3Mzk0M1oXDTE1MDkxOTA3Mzk0M1owRjELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk9DU1AgR09PRCBDZXJ0MQwwCgYDVQQKEwNPcmcwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDRHWZYToWVP8is/TWKzRQaRc0SmPNOKdFqkjJYF2NjPmz/EABJLKps4m2BZzN2kMIBRkTuirLKjnsdSelI8itekzZBJJMPgtBvsXn+wa3fPaHa3Lqhp9oa8rr/DaCCnp/B4Id/x6HV53uA8e4qs3PeDnagvomBCQsVApWpL1tcnIow1vMEHIA5uqTOVhCKgwXnnoWfl7AWmh6Tu+SbMYALj8fGzvDhPmxIhVte9NaF462Cxf0B6EuCEEdh0xEWw4qLakWpH9QlwZnshfYFaoSYLqLp83E84Md8SkcraLZed8nDS93Gh++rEjgQSldzkf0MIvZyrmKwf+vgNU+WpvzJAgMBAAEwDQYJKoZIhvcNAQEFBQADgYEACgN//8SRGqi50c5gmqZdH0YVC7ABUpBNMP1mP9z3+qadtHOOqIKn3q34rvKzyiZEndufZAt3vUQ1s+wlaJKQTUgAm7G8K8cYYCkexJoQVdfVTyJn2ZKTOkCdfoY9k/uxu3DmHhHteU7j0mmp1LVXrvrSGLtXmpzy+kHvVeH/UMw="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "NL+t9rmzVP7MFfYomU0pb/daz27COxIJ4qMxToYs4oHOn8hk4vwvdm//ORJZnLmk84gfnJRufassInIkwCszsxVoPx47jTi+tqSxFF398kT8MJdFNhr/F1CDQR6vPwMdW5CsRefPGBpr2IaL8oZRfzBDgEZL0bu8PPOOrFZCJbw69SACucYvsReQ1vHJf9XPu4eGEkIZHd9If2Bj/G8rxqUGzQ/ltVhUhhLj8rqrDghRF3+UeB1UPcCd6slKHQ+EyxKU/o2iuMe3B5rmcjDaoj52KsB+gk0puizvEgKn3F15a430u4/feKpqszDvFCWd8qxbWwBDDbLC92P2rc3T8g=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        ocspEvent = oscpEventListener.getEvent();
        sucess = (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_START_EVENT);
        ocspEvent = oscpEventListener.getEvent();
        authenticationModule.unregisterOcspEventListener(appUID);
        assertTrue(ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_FINISH_EVENT && ocspEvent.eventError == null);
        // PreInstallation: ocsp enabled, warning ignore, silent mode, unknown
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_ENABLED, OcspSettings.OCSP_WARNING_IGNORE, true, "0", "0");
        ocspUserPreferences.setOcspMode(-1);
        ocspUserPreferences.setOcspUrl(OCSP_URL);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        oscpEventListener.reset();
        authenticationModule.registerOcspEventListener(appUID, oscpEventListener);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICvDCCAiWgAwIBAgIJAKrSZo9qYhM/MA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDkyMDA3NTMzNVoXDTE1MDkxOTA3NTMzNVowSTELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxGjAYBgNVBAMTEU9DU1AgVU5LTk9XTiBDZXJ0MQwwCgYDVQQKEwNPcmcwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCaZ4VDJjntiOeNKbRd8BIkEjW/Z6k/fLzbiM5hQL/xHQC1PM1spLgnr4pf7Nf+F/bJbDq2eE1jDar6wmtJPcXFFkChl1eS5yJAyY/bhswyzbqNwTcwo1Lu64TSfC4gTdthEmUPNEhzxMcdCj6myrv8hNyhv6dhkcHvZgBwFgS4Pf1DXRmr/PPjCZxyMaoLoKzjezBXVgTC9rViN7moL715HrpO1jkIB2LRY93fzBmPa1leNdiqUqachFtkzYQuuO1MLlmHXKlNmNLOGqlreB/58q5PTPbIGOIzNiN+n0RYMkVSBVImLBQeepO2ICi/DSFW0yi70LpZvebx736P0yF1AgMBAAEwDQYJKoZIhvcNAQEFBQADgYEAGBzUyF4Cb/DN4lWY5yxd1/EI/6osAHySIgMEu5ClzyIqULc3QIvjhLrbLV/JKsVYT3h/gFZxwxoTGWdE1dApWj+Hja23TlLmo0uDDL/L3NOB/n041/gI2MQYw1Z9ME2sFzzWIli1P02JciykUQba/b0fRjPsY3ctzDS7QU2EFFs="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "UknTEEInZKDV9RCXnhv8W9KrXYppmRCUL/kSYXtdiiD3OmiwK0/4Nu0N8ix/Olo/IuWNLyPR+YZt6GLUQhw/AlWGd1mq8odlpiulFdB+e8ClkPvOW+3/KMvBO74ajyD2/PreEzCPYouM8YBqdpb49+TffnNQmEdaIpZ659ODppSNzMPmFpCcO6J2SoaU+KOp9uYdNab+JQfkGwrhF4F3FPYsQdPJAgS8Wt//khjQzAGFyOFIWmZ1ZwTbLxyf2QMHPrvtDWWH6h9qaR+2O9A90tF0PM2VUjznEOvBNigKwYTqBAW2j5oF6ZOQ4zyujqpQsm0pF6aPzaVjtZmfnpaCfA=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        ocspEvent = oscpEventListener.getEvent();
        sucess = (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_START_EVENT);
        ocspEvent = oscpEventListener.getEvent();
        authenticationModule.unregisterOcspEventListener(appUID);
        assertTrue(ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_FINISH_EVENT && (ocspEvent.eventError instanceof InstallerSecurityException));
        ocspError = (InstallerSecurityException)ocspEvent.eventError;
        assertTrue(ocspError.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                   && ocspError.getShortMessage().equals(securityErrorMessage.get(SecurityErrorMessage.OCSP_GENERAL_ERR, null))
                   && ocspError.getDetailedMessage().equals(securityDetailedErrorMessage.get(SecurityDetailedErrorMessage.OCSP_GENERAL_ERR,
                           null)));
        // PreInstallation: ocsp enabled, warning confirm, not silent mode, revoked
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_ENABLED, OcspSettings.OCSP_WARNING_CONFIRM, false, "0", "0");
        ocspUserPreferences.setOcspMode(-1);
        ocspUserPreferences.setOcspUrl(OCSP_URL);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        oscpEventListener.reset();
        authenticationModule.registerOcspEventListener(appUID, oscpEventListener);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICvDCCAiWgAwIBAgIJAM/3P7mVjkjRMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDkyMDA3NTcxMVoXDTE1MDkxOTA3NTcxMVowSTELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxGjAYBgNVBAMTEU9DU1AgUkVWT0tFRCBDZXJ0MQwwCgYDVQQKEwNPcmcwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDGIt32Ki+UkQYc/rswY4Bho/ygLyqV8UTVob4yH4k9AQTegIkdjw+AqgfU0LcqMdXguok2PS0sdkgEDPqCMawSMsQRQMsFYcab/LL/i7haWULQL33VI8Wt9grgzFZSpeuhFgr3YgWV0iyMHpKwb4ORiJvWhhvseCK+iI4sAOIhmQZ12HLhvBla5KXTyslLUoXA7V0CuRYqkB3SXn/RARk7GHcoVM+PIk6MtPUkHlyZzqXbHeNOAxGhSEbIp6WJTG2Bg/5OmdSg/kJ44ZSKMk0K2djNTgx70s83B+/vOU81Zxo+SUGiaOC/uw9Irh51WlZl8joq3A+gefO+fPSKCJZDAgMBAAEwDQYJKoZIhvcNAQEFBQADgYEAnk5fcldVed5INUs3djxRF9BZ1+ZFdgIjclysYB6B2mSvIdnxnT9H66oKnPeyDqbwcWYfdyDwyeBgsLIk4oOF85lWs5ItjKKcViLbZDPi9J3B+Av/qCWkrkWQokJ5Tkyw8K/sBQrNTM5kH1WrapcKsplcv8FgorE7FC57Ozpli48="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "KXg8rbURv+eZ75Cg0+8KPgaxhhrHqyf26nLFweLWKE7rgkX6aHHfmHirpt/LGzFqQt+516bbkVhG7DLJIshb3qoZBlgzKOqjvJoYwMnVw0CaIMoYjcevortyKk8jbeFnrYU34pp8v0pz3NEKzVgnYAdEnTtl88X+TBgcVDteVUOaz2IGB4JtPKWWRhG7kaOJdYXmCRuNeExv1gHqLYNq0Mr6rmECoMpV1ksrTJK7bJeWwOHRgtVo55rb6bI2MmzE5/Eo4LWg9MKjzmHnCbBWKvFyy5RuEZu5XHAzCJlLO52rfh6dJOq5yuPyB+L87bsgehWFyj1ptq+2jESw0KXuHQ=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        ocspEvent = oscpEventListener.getEvent();
        sucess = (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_START_EVENT);
        ocspEvent = oscpEventListener.getEvent();
        authenticationModule.unregisterOcspEventListener(appUID);
        assertTrue(ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_FINISH_EVENT && (ocspEvent.eventError instanceof InstallerSecurityException));
        ocspError = (InstallerSecurityException)ocspEvent.eventError;
        assertTrue(ocspError.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                   && ocspError.getShortMessage().equals(securityErrorMessage.get(SecurityErrorMessage.OCSP_GENERAL_ERR, null))
                   && ocspError.getDetailedMessage().equals(securityDetailedErrorMessage.get(SecurityDetailedErrorMessage.OCSP_REVOKED_ERR,
                           null)));
        // PreInstallation: ocsp enabled, warning confirm, not silent mode, good
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_ENABLED, OcspSettings.OCSP_WARNING_CONFIRM, false, "0", "0");
        ocspUserPreferences.setOcspMode(-1);
        ocspUserPreferences.setOcspUrl(OCSP_URL);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        oscpEventListener.reset();
        authenticationModule.registerOcspEventListener(appUID, oscpEventListener);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICuTCCAiKgAwIBAgIJAKdHK8lkGvSqMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDkyMDA3Mzk0M1oXDTE1MDkxOTA3Mzk0M1owRjELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxFzAVBgNVBAMTDk9DU1AgR09PRCBDZXJ0MQwwCgYDVQQKEwNPcmcwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDRHWZYToWVP8is/TWKzRQaRc0SmPNOKdFqkjJYF2NjPmz/EABJLKps4m2BZzN2kMIBRkTuirLKjnsdSelI8itekzZBJJMPgtBvsXn+wa3fPaHa3Lqhp9oa8rr/DaCCnp/B4Id/x6HV53uA8e4qs3PeDnagvomBCQsVApWpL1tcnIow1vMEHIA5uqTOVhCKgwXnnoWfl7AWmh6Tu+SbMYALj8fGzvDhPmxIhVte9NaF462Cxf0B6EuCEEdh0xEWw4qLakWpH9QlwZnshfYFaoSYLqLp83E84Md8SkcraLZed8nDS93Gh++rEjgQSldzkf0MIvZyrmKwf+vgNU+WpvzJAgMBAAEwDQYJKoZIhvcNAQEFBQADgYEACgN//8SRGqi50c5gmqZdH0YVC7ABUpBNMP1mP9z3+qadtHOOqIKn3q34rvKzyiZEndufZAt3vUQ1s+wlaJKQTUgAm7G8K8cYYCkexJoQVdfVTyJn2ZKTOkCdfoY9k/uxu3DmHhHteU7j0mmp1LVXrvrSGLtXmpzy+kHvVeH/UMw="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "NL+t9rmzVP7MFfYomU0pb/daz27COxIJ4qMxToYs4oHOn8hk4vwvdm//ORJZnLmk84gfnJRufassInIkwCszsxVoPx47jTi+tqSxFF398kT8MJdFNhr/F1CDQR6vPwMdW5CsRefPGBpr2IaL8oZRfzBDgEZL0bu8PPOOrFZCJbw69SACucYvsReQ1vHJf9XPu4eGEkIZHd9If2Bj/G8rxqUGzQ/ltVhUhhLj8rqrDghRF3+UeB1UPcCd6slKHQ+EyxKU/o2iuMe3B5rmcjDaoj52KsB+gk0puizvEgKn3F15a430u4/feKpqszDvFCWd8qxbWwBDDbLC92P2rc3T8g=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        ocspEvent = oscpEventListener.getEvent();
        sucess = (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_START_EVENT);
        ocspEvent = oscpEventListener.getEvent();
        authenticationModule.unregisterOcspEventListener(appUID);
        assertTrue(ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_FINISH_EVENT && ocspEvent.eventError == null);
        // PreInstallation: ocsp enabled, warning confirm, not silent mode, unknown
        ocspSettings = new OcspSettings(OcspSettings.OCSP_MODE_ENABLED, OcspSettings.OCSP_WARNING_CONFIRM, false, "0", "0");
        ocspUserPreferences.setOcspMode(-1);
        ocspUserPreferences.setOcspUrl(OCSP_URL);
        authenticationModule.setOCSPFlags(ocspSettings);
        storage.removeAuthenticationStorageData(appUID);
        oscpEventListener.reset();
        authenticationModule.registerOcspEventListener(appUID, oscpEventListener);
        allAttributes.clear();
        allAttributes.put(MIDP_PROFILE_ATTRIBUTE_NAME,new Attribute("",MIDP3));
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICvDCCAiWgAwIBAgIJAKrSZo9qYhM/MA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTEwMDkyMDA3NTMzNVoXDTE1MDkxOTA3NTMzNVowSTELMAkGA1UEBhMCRkkxEDAOBgNVBAcTB1RhbXBlcmUxGjAYBgNVBAMTEU9DU1AgVU5LTk9XTiBDZXJ0MQwwCgYDVQQKEwNPcmcwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCaZ4VDJjntiOeNKbRd8BIkEjW/Z6k/fLzbiM5hQL/xHQC1PM1spLgnr4pf7Nf+F/bJbDq2eE1jDar6wmtJPcXFFkChl1eS5yJAyY/bhswyzbqNwTcwo1Lu64TSfC4gTdthEmUPNEhzxMcdCj6myrv8hNyhv6dhkcHvZgBwFgS4Pf1DXRmr/PPjCZxyMaoLoKzjezBXVgTC9rViN7moL715HrpO1jkIB2LRY93fzBmPa1leNdiqUqachFtkzYQuuO1MLlmHXKlNmNLOGqlreB/58q5PTPbIGOIzNiN+n0RYMkVSBVImLBQeepO2ICi/DSFW0yi70LpZvebx736P0yF1AgMBAAEwDQYJKoZIhvcNAQEFBQADgYEAGBzUyF4Cb/DN4lWY5yxd1/EI/6osAHySIgMEu5ClzyIqULc3QIvjhLrbLV/JKsVYT3h/gFZxwxoTGWdE1dApWj+Hja23TlLmo0uDDL/L3NOB/n041/gI2MQYw1Z9ME2sFzzWIli1P02JciykUQba/b0fRjPsY3ctzDS7QU2EFFs="));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "UknTEEInZKDV9RCXnhv8W9KrXYppmRCUL/kSYXtdiiD3OmiwK0/4Nu0N8ix/Olo/IuWNLyPR+YZt6GLUQhw/AlWGd1mq8odlpiulFdB+e8ClkPvOW+3/KMvBO74ajyD2/PreEzCPYouM8YBqdpb49+TffnNQmEdaIpZ659ODppSNzMPmFpCcO6J2SoaU+KOp9uYdNab+JQfkGwrhF4F3FPYsQdPJAgS8Wt//khjQzAGFyOFIWmZ1ZwTbLxyf2QMHPrvtDWWH6h9qaR+2O9A90tF0PM2VUjznEOvBNigKwYTqBAW2j5oF6ZOQ4zyujqpQsm0pF6aPzaVjtZmfnpaCfA=="));
        securityAttributes = new SecurityAttributes();
        securityAttributes.addDescriptorAttributes(allAttributes);
        authenticationModule.authenticateJad(appUID,null,securityAttributes.getAuthenticationAttributes());
        ocspEvent = oscpEventListener.getEvent();
        sucess = (ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_START_EVENT);
        ocspEvent = oscpEventListener.getEvent();
        authenticationModule.unregisterOcspEventListener(appUID);
        assertTrue(ocspEvent != null && ocspEvent.eventId == OcspEventListener.OCSP_FINISH_EVENT && (ocspEvent.eventError instanceof InstallerSecurityException));
        ocspError = (InstallerSecurityException)ocspEvent.eventError;
        assertTrue(ocspError.getOtaStatusCode() == OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE
                   && ocspError.getShortMessage().equals(securityErrorMessage.get(SecurityErrorMessage.OCSP_GENERAL_ERR, null))
                   && ocspError.getDetailedMessage().equals(securityDetailedErrorMessage.get(SecurityDetailedErrorMessage.OCSP_GENERAL_ERR,
                           null)));
    }

    public static void showGuidelines(String guidelines)
    {
        System.setProperty("com.nokia.mj.impl.rt.ui", "com.nokia.mj.impl.rt.ui.avkon.RuntimeUiAvkon");
        try
        {
            Class.forName("com.nokia.mj.impl.rt.ui.qt.RuntimeUiQt");
            System.setProperty("com.nokia.mj.impl.rt.ui", "com.nokia.mj.impl.rt.ui.qt.RuntimeUiQt");
        }catch(Throwable t)
        {
        }
        RuntimeUi ui = RuntimeUiFactory.getRuntimeUi(
                           true /* aIdentified */);
        ConfirmData cData = new ConfirmData(
            guidelines,
            null /*answerOptions*/,
            0 /* highlight always the first option */);
        ui.confirm("Test guidance",cData);
        ui.destroy();
    }

    class OcspCancelThread extends Thread
    {
        OcspCancelThread(Uid msUid, Monitor ocspCancelMonitor)
        {
            this.msUid = msUid;
            this.ocspCancelMonitor = ocspCancelMonitor;
        }
        public void run()
        {
            authenticationModule.cancelOcspCheck(msUid);
            ocspCancelMonitor.notifyMonitor();
        }
        private Uid msUid;
        private Monitor ocspCancelMonitor;
    }

    private void cancelOcsp(Uid msUid)
    {
        Monitor ocspCancelMonitor = new Monitor();
        OcspCancelThread t = new OcspCancelThread(msUid, ocspCancelMonitor);
        t.start();
        // wait for the thread to finish
        ocspCancelMonitor.waitOnMonitor();
    }
}
