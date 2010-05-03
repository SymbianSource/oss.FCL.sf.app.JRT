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
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICgTCCAeqgAwIBAgIJAIROWRW4DjXyMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTA5MDUyODA4NDc1M1oXDTEwMDUyODA4NDc1M1owgZExCzAJBgNVBAYTAkZJMRAwDgYDVQQHEwdUYW1wZXJlMRcwFQYDVQQDEw5PQ1NQIEdPT0QgQ2VydDEMMAoGA1UEChMDT3JnMSMwIQYDVR0PExpjcml0aWNhbCwgZGlnaXRhbFNpZ25hdHVyZTEkMCIGA1UdJRMbMS4zLjYuMS40LjEuOTQuMS40OS4xLjIuMi4zMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDHoWvDNxIjGzm2J9OmYJtVrUPZJoIXzFVAXpSztxUcsxq18K1y/wsQcxyrjMD4C6eMkxygaDun0sZSHV8MDvt36A9gjoshhRWsV+yXrM3I3XjAFKP/fLrS4nnNJM9OEOpld7kjA+xgV51/Yn3kJyFKQjqbv/WZon7rozt82MspKQIDAQABMA0GCSqGSIb3DQEBBQUAA4GBADMn5WNp0pA7Z/4GRGIk2JMShEvtuHDnYvFg0xWk90rEQoAB6XsMe8IL8e7le9sPvppjiuyImjFTK6bOQYhGAVmjHDlkxlxf5SfRmhRilOl5ZHJKeGIeI696orXiqPiuL6ltb+W9hc0ymu2yZGew1Ptt9+tlYZRT0DuM7zqW3hip"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "s3Z/MqNA0EXP8le8s5806RbzyNfLgD3DqX+0Tr2LjTJIa5ent1QcP1sqWrJm4XieprCGv3ja7upiVp/rfDYiS7kcXQwVxZy5nxCV0MfpT+wlf42vwE3cd6rbfoAyTH3ErSeQK6VoIyYwoaFIzOZKCIwkplc6+etacxLpCZn26to="));
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
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIIChDCCAe2gAwIBAgIJAN1kvjmXKegVMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTA5MDUyODA4NDkwOVoXDTEwMDUyODA4NDkwOVowgZQxCzAJBgNVBAYTAkZJMRAwDgYDVQQHEwdUYW1wZXJlMRowGAYDVQQDExFPQ1NQIFVOS05PV04gQ2VydDEMMAoGA1UEChMDT3JnMSMwIQYDVR0PExpjcml0aWNhbCwgZGlnaXRhbFNpZ25hdHVyZTEkMCIGA1UdJRMbMS4zLjYuMS40LjEuOTQuMS40OS4xLjIuMi4zMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDIbnFJT2y3ID0lwBix+gGbByuocroHp3preSrSmGG5OC/rZLnp21wdyjMuvo1Ga5WSfzDY6YNaSIlKKxrjYLBW+HJUJaQ0A3w3T38xCAwULEiU8e/hcK3mawlL/llNGqVnRIbpu2cuSD+ICTt7oqVvlo/TQyflRGeWT2WQ3ZBTgwIDAQABMA0GCSqGSIb3DQEBBQUAA4GBABXIPy6FpATzxN2A8Sb8rEowoTDXNK1O4DMQ26Ri+otWDt1iThirqTl3RcuLi6PzkyXElc1VXTYKiUu3MFy5Wq5qCt6Fwntbcwx21MgnuceNqMdksMRDF1YVmZHhIb1ee/MzkuR8y8iSn+xALH+YfKO3faBwVQ9ly1ZQAgil/eBd"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "dgTivrr4heaBCvnX7aCcAgO0T5reljSYhuX95Rh7+SeDKNQCYqn6q860daWxCP/tk+QceBaGK1ISghzSf1PIPuKPiKhA0ia+dFLYJUjkxYJRZc8XJSRczBaF7eks3rVh5TTQGwzIG0C3w1p4OpSL+pBSO+pMrogWBgq9IL2Pe5c="));
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
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIIChDCCAe2gAwIBAgIJAN0+hRIgND0QMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTA5MDUyODA4NDg0MFoXDTEwMDUyODA4NDg0MFowgZQxCzAJBgNVBAYTAkZJMRAwDgYDVQQHEwdUYW1wZXJlMRowGAYDVQQDExFPQ1NQIFJFVk9LRUQgQ2VydDEMMAoGA1UEChMDT3JnMSMwIQYDVR0PExpjcml0aWNhbCwgZGlnaXRhbFNpZ25hdHVyZTEkMCIGA1UdJRMbMS4zLjYuMS40LjEuOTQuMS40OS4xLjIuMi4zMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDNss9Gx9Wo7LXBEQSCkIpjwNZlOUEBzByeZ1GjwaEblL+RIlXQfq2DtercXxa9gJlryWIze/9vPX1V0wpEO9+M1fDY7Ro6E59kxfZtBnDoEFjITzChZfwqAxLo+EQUpQ2lN8sjyEW0apy73Odqkikwufzlrr9h/utDcSoED0RIMwIDAQABMA0GCSqGSIb3DQEBBQUAA4GBAJMYPjz8Q6DHsnUEva3RS2Yyjk0XcY+ldq4GGpOauY1cbZD5A/fmk/TKJahB1uV5XnuYs65vrJUuxAEzGuTzMOnMwJgAncSXFp2AQL4o02ETrp493BhOi825iwbwaHqqx2ijxJ6Jy84t1Rj+mwkwiLsFsJFMxViV0dxjbwLNQzho"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "r1aZCR/spVeK4eDOp1awTbas0muOpDI7y1Bz7l5OipxyZqZI4T8vy+e3q/w263bSchhOWZryPbuE6IuaeThE9qa/daK29oxR5JmmfttrlyGTsEap11NCpTCsK03HS8jwzACOD02m9hOS+EIqIdhNmoBB8ZhWdlNirTHw2UgEk/4="));
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
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICgTCCAeqgAwIBAgIJAIROWRW4DjXyMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTA5MDUyODA4NDc1M1oXDTEwMDUyODA4NDc1M1owgZExCzAJBgNVBAYTAkZJMRAwDgYDVQQHEwdUYW1wZXJlMRcwFQYDVQQDEw5PQ1NQIEdPT0QgQ2VydDEMMAoGA1UEChMDT3JnMSMwIQYDVR0PExpjcml0aWNhbCwgZGlnaXRhbFNpZ25hdHVyZTEkMCIGA1UdJRMbMS4zLjYuMS40LjEuOTQuMS40OS4xLjIuMi4zMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDHoWvDNxIjGzm2J9OmYJtVrUPZJoIXzFVAXpSztxUcsxq18K1y/wsQcxyrjMD4C6eMkxygaDun0sZSHV8MDvt36A9gjoshhRWsV+yXrM3I3XjAFKP/fLrS4nnNJM9OEOpld7kjA+xgV51/Yn3kJyFKQjqbv/WZon7rozt82MspKQIDAQABMA0GCSqGSIb3DQEBBQUAA4GBADMn5WNp0pA7Z/4GRGIk2JMShEvtuHDnYvFg0xWk90rEQoAB6XsMe8IL8e7le9sPvppjiuyImjFTK6bOQYhGAVmjHDlkxlxf5SfRmhRilOl5ZHJKeGIeI696orXiqPiuL6ltb+W9hc0ymu2yZGew1Ptt9+tlYZRT0DuM7zqW3hip"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "s3Z/MqNA0EXP8le8s5806RbzyNfLgD3DqX+0Tr2LjTJIa5ent1QcP1sqWrJm4XieprCGv3ja7upiVp/rfDYiS7kcXQwVxZy5nxCV0MfpT+wlf42vwE3cd6rbfoAyTH3ErSeQK6VoIyYwoaFIzOZKCIwkplc6+etacxLpCZn26to="));
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
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIIChDCCAe2gAwIBAgIJAN0+hRIgND0QMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTA5MDUyODA4NDg0MFoXDTEwMDUyODA4NDg0MFowgZQxCzAJBgNVBAYTAkZJMRAwDgYDVQQHEwdUYW1wZXJlMRowGAYDVQQDExFPQ1NQIFJFVk9LRUQgQ2VydDEMMAoGA1UEChMDT3JnMSMwIQYDVR0PExpjcml0aWNhbCwgZGlnaXRhbFNpZ25hdHVyZTEkMCIGA1UdJRMbMS4zLjYuMS40LjEuOTQuMS40OS4xLjIuMi4zMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDNss9Gx9Wo7LXBEQSCkIpjwNZlOUEBzByeZ1GjwaEblL+RIlXQfq2DtercXxa9gJlryWIze/9vPX1V0wpEO9+M1fDY7Ro6E59kxfZtBnDoEFjITzChZfwqAxLo+EQUpQ2lN8sjyEW0apy73Odqkikwufzlrr9h/utDcSoED0RIMwIDAQABMA0GCSqGSIb3DQEBBQUAA4GBAJMYPjz8Q6DHsnUEva3RS2Yyjk0XcY+ldq4GGpOauY1cbZD5A/fmk/TKJahB1uV5XnuYs65vrJUuxAEzGuTzMOnMwJgAncSXFp2AQL4o02ETrp493BhOi825iwbwaHqqx2ijxJ6Jy84t1Rj+mwkwiLsFsJFMxViV0dxjbwLNQzho"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "r1aZCR/spVeK4eDOp1awTbas0muOpDI7y1Bz7l5OipxyZqZI4T8vy+e3q/w263bSchhOWZryPbuE6IuaeThE9qa/daK29oxR5JmmfttrlyGTsEap11NCpTCsK03HS8jwzACOD02m9hOS+EIqIdhNmoBB8ZhWdlNirTHw2UgEk/4="));
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
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICgTCCAeqgAwIBAgIJAIROWRW4DjXyMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTA5MDUyODA4NDc1M1oXDTEwMDUyODA4NDc1M1owgZExCzAJBgNVBAYTAkZJMRAwDgYDVQQHEwdUYW1wZXJlMRcwFQYDVQQDEw5PQ1NQIEdPT0QgQ2VydDEMMAoGA1UEChMDT3JnMSMwIQYDVR0PExpjcml0aWNhbCwgZGlnaXRhbFNpZ25hdHVyZTEkMCIGA1UdJRMbMS4zLjYuMS40LjEuOTQuMS40OS4xLjIuMi4zMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDHoWvDNxIjGzm2J9OmYJtVrUPZJoIXzFVAXpSztxUcsxq18K1y/wsQcxyrjMD4C6eMkxygaDun0sZSHV8MDvt36A9gjoshhRWsV+yXrM3I3XjAFKP/fLrS4nnNJM9OEOpld7kjA+xgV51/Yn3kJyFKQjqbv/WZon7rozt82MspKQIDAQABMA0GCSqGSIb3DQEBBQUAA4GBADMn5WNp0pA7Z/4GRGIk2JMShEvtuHDnYvFg0xWk90rEQoAB6XsMe8IL8e7le9sPvppjiuyImjFTK6bOQYhGAVmjHDlkxlxf5SfRmhRilOl5ZHJKeGIeI696orXiqPiuL6ltb+W9hc0ymu2yZGew1Ptt9+tlYZRT0DuM7zqW3hip"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "s3Z/MqNA0EXP8le8s5806RbzyNfLgD3DqX+0Tr2LjTJIa5ent1QcP1sqWrJm4XieprCGv3ja7upiVp/rfDYiS7kcXQwVxZy5nxCV0MfpT+wlf42vwE3cd6rbfoAyTH3ErSeQK6VoIyYwoaFIzOZKCIwkplc6+etacxLpCZn26to="));
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
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIIChDCCAe2gAwIBAgIJAN1kvjmXKegVMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTA5MDUyODA4NDkwOVoXDTEwMDUyODA4NDkwOVowgZQxCzAJBgNVBAYTAkZJMRAwDgYDVQQHEwdUYW1wZXJlMRowGAYDVQQDExFPQ1NQIFVOS05PV04gQ2VydDEMMAoGA1UEChMDT3JnMSMwIQYDVR0PExpjcml0aWNhbCwgZGlnaXRhbFNpZ25hdHVyZTEkMCIGA1UdJRMbMS4zLjYuMS40LjEuOTQuMS40OS4xLjIuMi4zMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDIbnFJT2y3ID0lwBix+gGbByuocroHp3preSrSmGG5OC/rZLnp21wdyjMuvo1Ga5WSfzDY6YNaSIlKKxrjYLBW+HJUJaQ0A3w3T38xCAwULEiU8e/hcK3mawlL/llNGqVnRIbpu2cuSD+ICTt7oqVvlo/TQyflRGeWT2WQ3ZBTgwIDAQABMA0GCSqGSIb3DQEBBQUAA4GBABXIPy6FpATzxN2A8Sb8rEowoTDXNK1O4DMQ26Ri+otWDt1iThirqTl3RcuLi6PzkyXElc1VXTYKiUu3MFy5Wq5qCt6Fwntbcwx21MgnuceNqMdksMRDF1YVmZHhIb1ee/MzkuR8y8iSn+xALH+YfKO3faBwVQ9ly1ZQAgil/eBd"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "dgTivrr4heaBCvnX7aCcAgO0T5reljSYhuX95Rh7+SeDKNQCYqn6q860daWxCP/tk+QceBaGK1ISghzSf1PIPuKPiKhA0ia+dFLYJUjkxYJRZc8XJSRczBaF7eks3rVh5TTQGwzIG0C3w1p4OpSL+pBSO+pMrogWBgq9IL2Pe5c="));
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
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIIChDCCAe2gAwIBAgIJAN0+hRIgND0QMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTA5MDUyODA4NDg0MFoXDTEwMDUyODA4NDg0MFowgZQxCzAJBgNVBAYTAkZJMRAwDgYDVQQHEwdUYW1wZXJlMRowGAYDVQQDExFPQ1NQIFJFVk9LRUQgQ2VydDEMMAoGA1UEChMDT3JnMSMwIQYDVR0PExpjcml0aWNhbCwgZGlnaXRhbFNpZ25hdHVyZTEkMCIGA1UdJRMbMS4zLjYuMS40LjEuOTQuMS40OS4xLjIuMi4zMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDNss9Gx9Wo7LXBEQSCkIpjwNZlOUEBzByeZ1GjwaEblL+RIlXQfq2DtercXxa9gJlryWIze/9vPX1V0wpEO9+M1fDY7Ro6E59kxfZtBnDoEFjITzChZfwqAxLo+EQUpQ2lN8sjyEW0apy73Odqkikwufzlrr9h/utDcSoED0RIMwIDAQABMA0GCSqGSIb3DQEBBQUAA4GBAJMYPjz8Q6DHsnUEva3RS2Yyjk0XcY+ldq4GGpOauY1cbZD5A/fmk/TKJahB1uV5XnuYs65vrJUuxAEzGuTzMOnMwJgAncSXFp2AQL4o02ETrp493BhOi825iwbwaHqqx2ijxJ6Jy84t1Rj+mwkwiLsFsJFMxViV0dxjbwLNQzho"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "r1aZCR/spVeK4eDOp1awTbas0muOpDI7y1Bz7l5OipxyZqZI4T8vy+e3q/w263bSchhOWZryPbuE6IuaeThE9qa/daK29oxR5JmmfttrlyGTsEap11NCpTCsK03HS8jwzACOD02m9hOS+EIqIdhNmoBB8ZhWdlNirTHw2UgEk/4="));
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
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICgTCCAeqgAwIBAgIJAIROWRW4DjXyMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTA5MDUyODA4NDc1M1oXDTEwMDUyODA4NDc1M1owgZExCzAJBgNVBAYTAkZJMRAwDgYDVQQHEwdUYW1wZXJlMRcwFQYDVQQDEw5PQ1NQIEdPT0QgQ2VydDEMMAoGA1UEChMDT3JnMSMwIQYDVR0PExpjcml0aWNhbCwgZGlnaXRhbFNpZ25hdHVyZTEkMCIGA1UdJRMbMS4zLjYuMS40LjEuOTQuMS40OS4xLjIuMi4zMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDHoWvDNxIjGzm2J9OmYJtVrUPZJoIXzFVAXpSztxUcsxq18K1y/wsQcxyrjMD4C6eMkxygaDun0sZSHV8MDvt36A9gjoshhRWsV+yXrM3I3XjAFKP/fLrS4nnNJM9OEOpld7kjA+xgV51/Yn3kJyFKQjqbv/WZon7rozt82MspKQIDAQABMA0GCSqGSIb3DQEBBQUAA4GBADMn5WNp0pA7Z/4GRGIk2JMShEvtuHDnYvFg0xWk90rEQoAB6XsMe8IL8e7le9sPvppjiuyImjFTK6bOQYhGAVmjHDlkxlxf5SfRmhRilOl5ZHJKeGIeI696orXiqPiuL6ltb+W9hc0ymu2yZGew1Ptt9+tlYZRT0DuM7zqW3hip"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "s3Z/MqNA0EXP8le8s5806RbzyNfLgD3DqX+0Tr2LjTJIa5ent1QcP1sqWrJm4XieprCGv3ja7upiVp/rfDYiS7kcXQwVxZy5nxCV0MfpT+wlf42vwE3cd6rbfoAyTH3ErSeQK6VoIyYwoaFIzOZKCIwkplc6+etacxLpCZn26to="));
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
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIIChDCCAe2gAwIBAgIJAN1kvjmXKegVMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTA5MDUyODA4NDkwOVoXDTEwMDUyODA4NDkwOVowgZQxCzAJBgNVBAYTAkZJMRAwDgYDVQQHEwdUYW1wZXJlMRowGAYDVQQDExFPQ1NQIFVOS05PV04gQ2VydDEMMAoGA1UEChMDT3JnMSMwIQYDVR0PExpjcml0aWNhbCwgZGlnaXRhbFNpZ25hdHVyZTEkMCIGA1UdJRMbMS4zLjYuMS40LjEuOTQuMS40OS4xLjIuMi4zMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDIbnFJT2y3ID0lwBix+gGbByuocroHp3preSrSmGG5OC/rZLnp21wdyjMuvo1Ga5WSfzDY6YNaSIlKKxrjYLBW+HJUJaQ0A3w3T38xCAwULEiU8e/hcK3mawlL/llNGqVnRIbpu2cuSD+ICTt7oqVvlo/TQyflRGeWT2WQ3ZBTgwIDAQABMA0GCSqGSIb3DQEBBQUAA4GBABXIPy6FpATzxN2A8Sb8rEowoTDXNK1O4DMQ26Ri+otWDt1iThirqTl3RcuLi6PzkyXElc1VXTYKiUu3MFy5Wq5qCt6Fwntbcwx21MgnuceNqMdksMRDF1YVmZHhIb1ee/MzkuR8y8iSn+xALH+YfKO3faBwVQ9ly1ZQAgil/eBd"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "dgTivrr4heaBCvnX7aCcAgO0T5reljSYhuX95Rh7+SeDKNQCYqn6q860daWxCP/tk+QceBaGK1ISghzSf1PIPuKPiKhA0ia+dFLYJUjkxYJRZc8XJSRczBaF7eks3rVh5TTQGwzIG0C3w1p4OpSL+pBSO+pMrogWBgq9IL2Pe5c="));
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
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIIChDCCAe2gAwIBAgIJAN0+hRIgND0QMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTA5MDUyODA4NDg0MFoXDTEwMDUyODA4NDg0MFowgZQxCzAJBgNVBAYTAkZJMRAwDgYDVQQHEwdUYW1wZXJlMRowGAYDVQQDExFPQ1NQIFJFVk9LRUQgQ2VydDEMMAoGA1UEChMDT3JnMSMwIQYDVR0PExpjcml0aWNhbCwgZGlnaXRhbFNpZ25hdHVyZTEkMCIGA1UdJRMbMS4zLjYuMS40LjEuOTQuMS40OS4xLjIuMi4zMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDNss9Gx9Wo7LXBEQSCkIpjwNZlOUEBzByeZ1GjwaEblL+RIlXQfq2DtercXxa9gJlryWIze/9vPX1V0wpEO9+M1fDY7Ro6E59kxfZtBnDoEFjITzChZfwqAxLo+EQUpQ2lN8sjyEW0apy73Odqkikwufzlrr9h/utDcSoED0RIMwIDAQABMA0GCSqGSIb3DQEBBQUAA4GBAJMYPjz8Q6DHsnUEva3RS2Yyjk0XcY+ldq4GGpOauY1cbZD5A/fmk/TKJahB1uV5XnuYs65vrJUuxAEzGuTzMOnMwJgAncSXFp2AQL4o02ETrp493BhOi825iwbwaHqqx2ijxJ6Jy84t1Rj+mwkwiLsFsJFMxViV0dxjbwLNQzho"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "r1aZCR/spVeK4eDOp1awTbas0muOpDI7y1Bz7l5OipxyZqZI4T8vy+e3q/w263bSchhOWZryPbuE6IuaeThE9qa/daK29oxR5JmmfttrlyGTsEap11NCpTCsK03HS8jwzACOD02m9hOS+EIqIdhNmoBB8ZhWdlNirTHw2UgEk/4="));
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
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIICgTCCAeqgAwIBAgIJAIROWRW4DjXyMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTA5MDUyODA4NDc1M1oXDTEwMDUyODA4NDc1M1owgZExCzAJBgNVBAYTAkZJMRAwDgYDVQQHEwdUYW1wZXJlMRcwFQYDVQQDEw5PQ1NQIEdPT0QgQ2VydDEMMAoGA1UEChMDT3JnMSMwIQYDVR0PExpjcml0aWNhbCwgZGlnaXRhbFNpZ25hdHVyZTEkMCIGA1UdJRMbMS4zLjYuMS40LjEuOTQuMS40OS4xLjIuMi4zMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDHoWvDNxIjGzm2J9OmYJtVrUPZJoIXzFVAXpSztxUcsxq18K1y/wsQcxyrjMD4C6eMkxygaDun0sZSHV8MDvt36A9gjoshhRWsV+yXrM3I3XjAFKP/fLrS4nnNJM9OEOpld7kjA+xgV51/Yn3kJyFKQjqbv/WZon7rozt82MspKQIDAQABMA0GCSqGSIb3DQEBBQUAA4GBADMn5WNp0pA7Z/4GRGIk2JMShEvtuHDnYvFg0xWk90rEQoAB6XsMe8IL8e7le9sPvppjiuyImjFTK6bOQYhGAVmjHDlkxlxf5SfRmhRilOl5ZHJKeGIeI696orXiqPiuL6ltb+W9hc0ymu2yZGew1Ptt9+tlYZRT0DuM7zqW3hip"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "s3Z/MqNA0EXP8le8s5806RbzyNfLgD3DqX+0Tr2LjTJIa5ent1QcP1sqWrJm4XieprCGv3ja7upiVp/rfDYiS7kcXQwVxZy5nxCV0MfpT+wlf42vwE3cd6rbfoAyTH3ErSeQK6VoIyYwoaFIzOZKCIwkplc6+etacxLpCZn26to="));
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
        allAttributes.put(AuthenticationAttribute.MAIN_ATTRIBUTE_PREFIX + "1-1", new Attribute("",        "MIIChDCCAe2gAwIBAgIJAN1kvjmXKegVMA0GCSqGSIb3DQEBBQUAMHMxCzAJBgNVBAYTAmZpMRIwEAYDVQQIEwlQaXJrYW5tYWExEDAOBgNVBAcTB1RhbXBlcmUxDjAMBgNVBAoTBU5va2lhMQ0wCwYDVQQLEwRKYXZhMR8wHQYDVQQDDBZKUF9UaGlyZF9QYXJ0eV9UcnVzdGVkMB4XDTA5MDUyODA4NDkwOVoXDTEwMDUyODA4NDkwOVowgZQxCzAJBgNVBAYTAkZJMRAwDgYDVQQHEwdUYW1wZXJlMRowGAYDVQQDExFPQ1NQIFVOS05PV04gQ2VydDEMMAoGA1UEChMDT3JnMSMwIQYDVR0PExpjcml0aWNhbCwgZGlnaXRhbFNpZ25hdHVyZTEkMCIGA1UdJRMbMS4zLjYuMS40LjEuOTQuMS40OS4xLjIuMi4zMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDIbnFJT2y3ID0lwBix+gGbByuocroHp3preSrSmGG5OC/rZLnp21wdyjMuvo1Ga5WSfzDY6YNaSIlKKxrjYLBW+HJUJaQ0A3w3T38xCAwULEiU8e/hcK3mawlL/llNGqVnRIbpu2cuSD+ICTt7oqVvlo/TQyflRGeWT2WQ3ZBTgwIDAQABMA0GCSqGSIb3DQEBBQUAA4GBABXIPy6FpATzxN2A8Sb8rEowoTDXNK1O4DMQ26Ri+otWDt1iThirqTl3RcuLi6PzkyXElc1VXTYKiUu3MFy5Wq5qCt6Fwntbcwx21MgnuceNqMdksMRDF1YVmZHhIb1ee/MzkuR8y8iSn+xALH+YfKO3faBwVQ9ly1ZQAgil/eBd"));
        allAttributes.put(AuthenticationAttribute.SECOND_ATTRIBUTE_PREFIX + "1" ,new Attribute("", "dgTivrr4heaBCvnX7aCcAgO0T5reljSYhuX95Rh7+SeDKNQCYqn6q860daWxCP/tk+QceBaGK1ISghzSf1PIPuKPiKhA0ia+dFLYJUjkxYJRZc8XJSRczBaF7eks3rVh5TTQGwzIG0C3w1p4OpSL+pBSO+pMrogWBgq9IL2Pe5c="));
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
