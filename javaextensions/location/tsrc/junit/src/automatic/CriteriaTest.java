/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
package com.nokia.mj.test.lapi;

import j2meunit.framework.*;
import javax.microedition.location.*;

/**
 * <b>TEST CASE SPECIFICATION</b>
 *
 * This suite of testcases tests that: - Criteria object can be created -
 * Default values are correct - Criteria parameters can be changed
 *
 */

public class CriteriaTest extends TestCase
{

    public CriteriaTest()
    {
    }

    public CriteriaTest(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /***************************************************************************
     * Creates the test suite. You need to add a new aSuite.addTest antry for
     * any new test methods, otherwise they won't be run.
     */
    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new CriteriaTest("testConstructor", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((CriteriaTest) tc).testConstructor();
            }
        }));

        aSuite.addTest(new CriteriaTest("testSetters", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((CriteriaTest) tc).testSetters();
            }
        }));

        return aSuite;

    }

    // Test that Criteria constructor works and that default values are correct
    public void testConstructor()
    {

        Criteria criteria = new Criteria();

        // Default values
        int hacc = Criteria.NO_REQUIREMENT;
        int power = Criteria.NO_REQUIREMENT;
        boolean costAllowed = true;
        int vacc = Criteria.NO_REQUIREMENT;
        int maxresptime = Criteria.NO_REQUIREMENT;
        boolean speedRequired = false;
        boolean altitudeRequired = false;
        boolean addressInfoRequired = false;

        // Check the defaults
        assertTrue("Default Criteria values incorrect", criteria
                   .getHorizontalAccuracy() == hacc
                   && criteria.getVerticalAccuracy() == vacc
                   && criteria.getPreferredPowerConsumption() == power
                   && criteria.getPreferredResponseTime() == maxresptime
                   && criteria.isAllowedToCost() == costAllowed
                   && criteria.isSpeedAndCourseRequired() == speedRequired
                   && criteria.isAltitudeRequired() == altitudeRequired
                   && criteria.isAddressInfoRequired() == addressInfoRequired);

    }

    // Tests that all the setters work
    public void testSetters()
    {

        // Define a Criteria object with non-default parameters and check that
        // the values are unchanged when read.
        int hacc = 10;
        int power = Criteria.POWER_USAGE_LOW;
        boolean costAllowed = false;
        int vacc = 45;
        int maxresptime = 100;
        boolean speedRequired = true;
        boolean altitudeRequired = true;
        boolean addressInfoRequired = true;

        Criteria criteria = new Criteria();
        criteria.setHorizontalAccuracy(hacc);
        criteria.setVerticalAccuracy(vacc);
        criteria.setPreferredPowerConsumption(power);
        criteria.setPreferredResponseTime(maxresptime);
        criteria.setCostAllowed(costAllowed);
        criteria.setSpeedAndCourseRequired(speedRequired);
        criteria.setAltitudeRequired(altitudeRequired);
        criteria.setAddressInfoRequired(addressInfoRequired);

        // Check that the values are correct
        assertTrue("Retrieved Criteria values different from input", criteria
                   .getHorizontalAccuracy() == hacc
                   && criteria.getVerticalAccuracy() == vacc
                   && criteria.getPreferredPowerConsumption() == power
                   && criteria.getPreferredResponseTime() == maxresptime
                   && criteria.isAllowedToCost() == costAllowed
                   && criteria.isSpeedAndCourseRequired() == speedRequired
                   && criteria.isAltitudeRequired() == altitudeRequired
                   && criteria.isAddressInfoRequired() == addressInfoRequired);
    }

}
