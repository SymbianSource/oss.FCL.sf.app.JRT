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
import javax.microedition.location.*;

/*
 * This suite of testcases tests that:
 * - Criteria object can be created
 * - Default values are correct
 * - Criteria parameters can be changed
 */
public class CriteriaTest extends ViperUnitTest
{

    public CriteriaTest()
    {
        super("CriteriaTest");
    }

    protected void runTest() throws java.lang.Throwable
    {
        testConstructor();
        testSetters();
    }

    // Test that Criteria constructor works and that default values are correct
    void testConstructor()
    {
        setCurrentTest("testConstructor()");

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
        assertTrue(criteria.getHorizontalAccuracy() == hacc
                   && criteria.getVerticalAccuracy() == vacc
                   && criteria.getPreferredPowerConsumption() == power
                   && criteria.getPreferredResponseTime() == maxresptime
                   && criteria.isAllowedToCost() == costAllowed
                   && criteria.isSpeedAndCourseRequired() == speedRequired
                   && criteria.isAltitudeRequired() == altitudeRequired
                   && criteria.isAddressInfoRequired() == addressInfoRequired,
                   "Default Criteria values incorrect");

    }

    // Tests that all the setters work
    void testSetters()
    {
        setCurrentTest("testSetters()");

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
        assertTrue(criteria.getHorizontalAccuracy() == hacc
                   && criteria.getVerticalAccuracy() == vacc
                   && criteria.getPreferredPowerConsumption() == power
                   && criteria.getPreferredResponseTime() == maxresptime
                   && criteria.isAllowedToCost() == costAllowed
                   && criteria.isSpeedAndCourseRequired() == speedRequired
                   && criteria.isAltitudeRequired() == altitudeRequired
                   && criteria.isAddressInfoRequired() == addressInfoRequired,
                   "Retrieved Criteria values different from input");
    }

}
