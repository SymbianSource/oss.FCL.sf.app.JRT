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


package com.nokia.mj.impl.properties.mobinfo;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.rt.support.ApplicationInfo;

/**
 * Checks cases related to canRead()
 */
public class TestProperties extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestProperties("testProperties", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestProperties) tc).testProperties();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestProperties()
    {
    }

    public TestProperties(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    protected void setUp()
    {
        //path = TestEnvironment.Main_Test_Folder + "testcanread/";
    }

    private static final String NETWORK_ACCESS = "com.nokia.network.access";
    private static final String BATTERY_LEVEL = "com.nokia.batterylevel";
    private static final String COUNTRY_CODE = "com.nokia.mid.countrycode";
    private static final String IMEI = "com.nokia.mid.imsi";
    private static final String IMSI = "com.nokia.mid.imei";
    private static final String NETWORK_AVAILABILITY = "com.nokia.mid.networkavailability";
    private static final String NETWORK_ID = "com.nokia.mid.networkid";
    private static final String NETWORK_SIGNAL = "com.nokia.mid.networksignal";
    private static final String EMAIL_RECIEVE_SETTINGS = "com.nokia.mid.settings.email-receive-protocol";
    private static final String EMAIL_SEND_SETTINGS = "com.nokia.mid.settings.email-send-protocol";
    private static final String CELLID = "com.nokia.mid.cellid";
    private static final String MSISDN = "com.nokia.mid.msisdn";
    private static final String DATEFORMAT = "com.nokia.mid.dateformat";
    private static final String TIMEFORMAT = "com.nokia.mid.timeformat";
    private static final String NETWORKSTATUS = "com.nokia.mid.networkstatus";


    public void testProperties()
    {
        try
        {
            System.out.println("BATTERY_LEVEL: "+System.getProperty(BATTERY_LEVEL));
            System.out.println("COUNTRY_CODE: "+System.getProperty(COUNTRY_CODE));
            System.out.println("IMEI: "+System.getProperty(IMEI));
            System.out.println("IMSI: "+System.getProperty(IMSI));
            System.out.println("NETWORK_AVAILABILITY: "+System.getProperty(NETWORK_AVAILABILITY));
            System.out.println("NETWORK_ID: "+System.getProperty(NETWORK_ID));
            System.out.println("NETWORK_SIGNAL: "+System.getProperty(NETWORK_SIGNAL));
            System.out.println("CELLID: "+System.getProperty(CELLID));
            System.out.println("MSISDN: "+System.getProperty(MSISDN));
            System.out.println("DATEFORMAT: "+System.getProperty(DATEFORMAT));
            System.out.println("TIMEFORMAT: "+System.getProperty(TIMEFORMAT));
            System.out.println("NETWORKSTATUS: "+System.getProperty(NETWORKSTATUS));
        }
        catch (Exception e)
        {
            assertTrue("Test Properties Failed: "+e, false);
        }
    }
}
