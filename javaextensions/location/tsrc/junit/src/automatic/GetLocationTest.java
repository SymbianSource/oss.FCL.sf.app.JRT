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

public class GetLocationTest extends TestCase
{
    private Location iLoc;

    final static int TIMETOFIX = 5;

    protected LocationProvider iLP = null;

    long[] iTimeArray;

    public GetLocationTest()
    {
    }

    public GetLocationTest(String sTestName, TestMethod rTestMethod)
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

        aSuite.addTest(new GetLocationTest("testBadArguments",
                                           new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetLocationTest) tc).testBadArguments();
            }
        }));

        aSuite.addTest(new GetLocationTest("testGetLocation", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetLocationTest) tc).testGetLocation();
            }
        }));

        aSuite.addTest(new GetLocationTest("testAltitudeData",
                                           new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetLocationTest) tc).testAltitudeData();
            }
        }));

        aSuite.addTest(new GetLocationTest("testSpeedData", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetLocationTest) tc).testSpeedData();
            }
        }));

        aSuite.addTest(new GetLocationTest("testAddressInfo", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetLocationTest) tc).testAddressInfo();
            }
        }));

        aSuite.addTest(new GetLocationTest("testGetLocationTimeout",
                                           new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetLocationTest) tc).testGetLocationTimeout();
            }
        }));

        aSuite.addTest(new GetLocationTest("testGetLatest", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((GetLocationTest) tc).testGetLatest();
            }
        }));

        return aSuite;

    }

    private void assertContinue(String aReason, boolean aCond)
    {
        if (!aCond)
            assertTrue(aReason, false);
    }

    public void testBadArguments()
    {
        try
        {
            providerSetUp(null);
            assertContinue("LocationProvider is null", iLP != null);

            assertBad(0);
            assertBad(-2);
            assertBad(-12345); // Sanity check
            assertBad(Integer.MIN_VALUE);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testGetLocation()
    {
        try
        {
            providerSetUp(null);
            assertContinue("LocationProvider is null", iLP != null);
            simpleGetLocation();

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testAltitudeData()
    {
        try
        {
            Criteria criteria = new Criteria();
            criteria.setAltitudeRequired(true);
            criteria.setPreferredResponseTime(100); // 100 ms

            providerSetUp(criteria);
            assertContinue("LocationProvider is null", iLP != null);
            simpleGetLocation();

            QualifiedCoordinates coords = iLoc.getQualifiedCoordinates();
            float altitude = coords.getAltitude();
            assertContinue("No altitude included", !Float.isNaN(altitude));
            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testSpeedData()
    {
        try
        {

            Criteria criteria = new Criteria();
            criteria.setPreferredResponseTime(100); // 100 ms
            criteria.setSpeedAndCourseRequired(true);

            providerSetUp(criteria);
            assertContinue("LocationProvider is null", iLP != null);
            simpleGetLocation();

            float speed = iLoc.getSpeed();
            assertContinue("No speed included", !Float.isNaN(speed));
            float course = iLoc.getCourse();
            assertContinue("No course included", !Float.isNaN(course));

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testAddressInfo()
    {
        try
        {
            Criteria criteria = new Criteria();
            criteria.setPreferredResponseTime(100); // 100 ms
            criteria.setAddressInfoRequired(true);

            providerSetUp(criteria);
            if (iLP != null)
            {
                assertContinue("LocationProvider is null", iLP != null);
                simpleGetLocation();

                AddressInfo addr = iLoc.getAddressInfo();
                if (addr != null)
                {
                    boolean hasNonNullField = false;
                    for (int i = 1; i <= 17; ++i)
                    {
                        String field = addr.getField(i);
                        if (field != null)
                        {
                            hasNonNullField = true;
                        }
                    }
                    assertContinue("All AddressInfo fields are null",
                                   hasNonNullField);
                }
            }
            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testGetLocationTimeout()
    {
        try
        {
            Criteria criteria = new Criteria();
            criteria.setCostAllowed(false); // This will select the right
            // provider
            providerSetUp(criteria);
            assertContinue("LocationProvider is null", iLP != null);
            iLoc = null;

            try
            {
                int TIMEOUT = 1; // seconds
                iLoc = iLP.getLocation(TIMEOUT);
                assertContinue("No timeout for getLocation", false);
            }
            catch (LocationException le)
            {
                // Timed out correctly
            }

            // Sanity check - test that normal request works ok
            simpleGetLocation();

            try
            {
                int TIMEOUT = 10; // seconds
                long startTime = System.currentTimeMillis();
                iLoc = iLP.getLocation(TIMEOUT);
                long duration = System.currentTimeMillis() - startTime;
                assertContinue("GetLocation took too long",
                               duration <= TIMEOUT * 1000);
                checkLocationData(iLoc);
            }
            catch (LocationException le)
            {
                assertContinue("Illegal timeout", false);
            }

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testGetLatest()
    {
        iLP = null;
        iLoc = null;

        // Get the last known location
        iLoc = LocationProvider.getLastKnownLocation();

        assertTrue("", true);
    }

    public void testGetLocationOutOfService()
    {
        try
        {
            Criteria criteria = new Criteria();
            criteria.setCostAllowed(false); // This will select the right
            // provider
            providerSetUp(criteria);

            assertContinue("LocationProvider is null", iLP != null);

            simpleGetLocation();

            // now we get the last known Location:
            Location lastKnownLocation = LocationProvider
                                         .getLastKnownLocation();
            checkLocationData(lastKnownLocation);

            assertContinue("Not the last known location returned",
                           lastKnownLocation.getTimestamp() == iLoc.getTimestamp());

            long start = System.currentTimeMillis();
            long end = 0;
            try
            {
                iLoc = iLP.getLocation(-1);
                assertContinue("No exception thrown for getLocation", false);
            }
            catch (LocationException le)
            {
                // Exception thrown correctly
                end = System.currentTimeMillis();
            }
            assertContinue("Expected LocationException immediately", end
                           - start < 500);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    // ------------------------ Helper methods -----------------------

    private void simpleGetLocation() throws Exception
    {
        iLoc = null;

        try
        {
            iLoc = iLP.getLocation(-1);
            checkLocationData(iLoc);
        }
        catch (LocationException le)
        {
            assertContinue("Could not get location: " + le, false);
        }
    }

    private void assertBad(int aBadParam) throws Exception
    {
        try
        {
            iLoc = iLP.getLocation(aBadParam);
            assertContinue("IllegalArgumentException was not thrown for: "
                           + aBadParam, false);
        }
        catch (IllegalArgumentException iae)
        {
            // Exception thrown correctly
        }
    }

    // ------------------------ Helper class -----------------------

    protected void providerSetUp(Criteria aCriteria)
    {
        iLP = null;
        try
        {
            iLP = LocationProvider.getInstance(aCriteria);
            if (iLP != null)
            {
                int state = iLP.getState();
                assertContinue("Initial state=" + state
                               + ", expected AVAILABLE",
                               state == LocationProvider.AVAILABLE);
            }
        }
        catch (LocationException le)
        {
        }
    }

    protected void checkLocationData(Location aLoc)
    {
        assertContinue("Location is null", aLoc != null);
        assertContinue("Location is invalid", aLoc.isValid());
        assertContinue("Location is valid, but Coordinates are null", aLoc
                       .getQualifiedCoordinates() != null);

        long timestamp = aLoc.getTimestamp();
        long now = System.currentTimeMillis();
        assertContinue("Timestamp incorrect: t=" + timestamp + ", now=" + now,
                       now >= timestamp && (now - timestamp < 30000) && timestamp > 0);

        QualifiedCoordinates coords = aLoc.getQualifiedCoordinates();
        double lat = coords.getLatitude();
        double lon = coords.getLongitude();

        assertContinue("Latitude out of range", lat >= -90.0 || lat <= 90.0);
        assertContinue("Longitude out of range", lon >= -180.0 || lon < 180.0);

        float hacc = coords.getHorizontalAccuracy();
        assertContinue("Horizontal accuracy is negative", Float.isNaN(hacc)
                       || hacc >= 0);

        float vacc = coords.getVerticalAccuracy();
        assertContinue("Vertical accuracy is negative", Float.isNaN(vacc)
                       || vacc >= 0);

        float speed = aLoc.getSpeed();
        assertContinue("Speed is negative", Float.isNaN(speed) || speed >= 0);

        float course = aLoc.getCourse();
        assertContinue("Course out of range", Float.isNaN(course)
                       || (course >= 0 && course < 360));

        String nmea = aLoc.getExtraInfo("application/X-jsr179-location-nmea");
        if (nmea != null)
        {
            assertTrue("Bad NMEA data", nmea.startsWith("$GP"));
        }
    }

}

// End of file
