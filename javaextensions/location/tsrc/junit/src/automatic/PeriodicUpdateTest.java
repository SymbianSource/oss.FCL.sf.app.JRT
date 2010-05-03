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

public class PeriodicUpdateTest extends TestCase implements LocationListener
{
    final static int ONLY_STATUS_INTERVAL = 0;

    final static int DEFAULT_INTERVAL = -1;

    final static int INTERVAL = 10;

    final static int TIMEOUT = 3;

    final static int MAXAGE = 2;

    final static int LARGE_TIMEOUT = 12;

    final static int LARGE_MAXAGE = 11;

    final static int[] badInterval = new int[] { -2, -12345, Integer.MIN_VALUE };

    final static int[] badTimeout = new int[] { 0, -2, -12345,
            Integer.MIN_VALUE
                                              };

    final static int[] badMaxAge = badTimeout;

    private Coordinates currentCoords;

    private Location[] iLocations;

    private int iNumUpdates;

    private long[] iLocUpdateCallbackTime;

    final static int TIMETOFIX = 5;

    protected LocationProvider iLP = null;

    long[] iTimeArray;

    public PeriodicUpdateTest()
    {
    }

    public PeriodicUpdateTest(String sTestName, TestMethod rTestMethod)
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

        aSuite.addTest(new PeriodicUpdateTest("testBadArguments",
                                              new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((PeriodicUpdateTest) tc).testBadArguments();
            }
        }));

        aSuite.addTest(new PeriodicUpdateTest("testGoodArguments",
                                              new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((PeriodicUpdateTest) tc).testGoodArguments();
            }
        }));

        aSuite.addTest(new PeriodicUpdateTest("testLocationListenerDefault",
                                              new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((PeriodicUpdateTest) tc).testLocationListenerDefault();
            }
        }));

        aSuite.addTest(new PeriodicUpdateTest(
                           "testLocationListenerInterval_1s", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((PeriodicUpdateTest) tc)
                .testLocationListenerInterval_1s();
            }
        }));

        aSuite.addTest(new PeriodicUpdateTest(
                           "testLocationListenerInterval_3s", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((PeriodicUpdateTest) tc)
                .testLocationListenerInterval_3s();
            }
        }));

        aSuite.addTest(new PeriodicUpdateTest(
                           "testLocationListenerInterval_7s", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((PeriodicUpdateTest) tc)
                .testLocationListenerInterval_7s();
            }
        }));

        aSuite.addTest(new PeriodicUpdateTest("testChangeListener",
                                              new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((PeriodicUpdateTest) tc).testChangeListener();
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
        providerSetUp(null);
        assertContinue("LocationProvider is null", iLP != null);

        assertBad(INTERVAL, TIMEOUT, LARGE_MAXAGE); // MaxAge > Interval
        assertBad(INTERVAL, LARGE_TIMEOUT, MAXAGE); // Timeout > Interval
        assertBad(INTERVAL, LARGE_TIMEOUT, LARGE_MAXAGE);

        for (int i = 0; i < badInterval.length; ++i)
        {
            assertBad(badInterval[i], TIMEOUT, MAXAGE);
        }

        for (int i = 0; i < badTimeout.length; ++i)
        {
            assertBad(INTERVAL, badTimeout[i], MAXAGE);
            assertBad(INTERVAL, badTimeout[i], LARGE_MAXAGE);
        }

        for (int i = 0; i < badMaxAge.length; ++i)
        {
            assertBad(INTERVAL, TIMEOUT, badMaxAge[i]);
            assertBad(INTERVAL, LARGE_TIMEOUT, badMaxAge[i]);
        }

        for (int i = 0; i < badInterval.length; ++i)
        {
            for (int j = 0; j < badTimeout.length; ++j)
            {
                for (int k = 0; k < badMaxAge.length; ++k)
                {
                    assertBad(badInterval[i], badTimeout[j], badMaxAge[k]);
                }
            }
        }
        assertTrue("", true);
    }

    public void testGoodArguments()
    {
        providerSetUp(null);
        assertContinue("LocationProvider is null", iLP != null);

        // These arguments should not throw exceptions
        testGoodWithBad(this, DEFAULT_INTERVAL);
        testGoodWithBad(this, ONLY_STATUS_INTERVAL);

        testGoodWithBad(null, INTERVAL);
        testGoodWithBad(null, DEFAULT_INTERVAL);
        testGoodWithBad(null, ONLY_STATUS_INTERVAL);

        for (int i = 0; i < badInterval.length; ++i)
        {
            testGoodWithBad(null, badInterval[i]);
        }

        int bigValue = Integer.MAX_VALUE / 1000000;
        assertGood(this, bigValue, bigValue, bigValue);

        assertGood(this, Integer.MAX_VALUE, Integer.MAX_VALUE,
                   Integer.MAX_VALUE);

        assertTrue("", true);
    }

    public void testLocationListenerDefault()
    {
        final int MAXTIME = 1000 * (TIMETOFIX + 13 + 1); // Interval ~ 6.25

        providerSetUp(null); // Simulation PSY should be default
        assertContinue("LocationProvider is null", iLP != null);

        // Start listening for location changes
        iLP.setLocationListener(this, -1, -1, -1);

        getUpdates(2, MAXTIME);

        checkLocationData(iLocations[0]);
        checkLocationData(iLocations[1]);

        iLocations = null;
        assertTrue("", true);
    }

    public void testLocationListenerInterval_1s()
    {
        testInterval(1, 4);
        assertTrue("", true);
    }

    public void testLocationListenerInterval_3s()
    {
        testInterval(3, 5);
        assertTrue("", true);
    }

    public void testLocationListenerInterval_7s()
    {
        testInterval(7, 3);
        assertTrue("", true);
    }

    public void testChangeListener()
    {
        providerSetUp(null);
        assertContinue("LocationProvider is null", iLP != null);

        final int PERIOD1 = 10; // seconds
        final int PERIOD2 = 7; // seconds

        LocationListener listener = new LocationListener()
        {
            public void providerStateChanged(LocationProvider lp, int s)
            {
            }

            public void locationUpdated(LocationProvider lp, Location l)
            {
            }
        };

        // Change interval
        iLP.setLocationListener(this, PERIOD1, -1, -1);
        int maxtime = 1000 * (2 * PERIOD1 + TIMETOFIX);
        getUpdates(2, maxtime);

        iLP.setLocationListener(this, PERIOD2, -1, -1);
        maxtime = 1000 * (2 * PERIOD2 + TIMETOFIX);
        getUpdates(2, maxtime);

        // Change listener
        iLP.setLocationListener(listener, -1, -1, -1);

        try
        {
            if (iLP != null)
                iLP.setLocationListener(null, -1, -1, -1);
        }
        catch (Exception e)
        {
        }
        assertTrue("", true);
    }

    public void providerStateChanged(LocationProvider l, int event)
    {
    }

    public void locationUpdated(LocationProvider l, Location fix)
    {
        iLocUpdateCallbackTime[iNumUpdates] = System.currentTimeMillis();

        iLocations[iNumUpdates] = fix;
        ++iNumUpdates;
        assertContinue("LocationProvider is null", l != null);
    }

    // ------------------------ Helper methods -----------------------

    private void assertBad(int aInterval, int aTimeout, int aMaxAge)
    {
        try
        {
            iLP.setLocationListener(this, aInterval, aTimeout, aMaxAge);
            // Oops, no exception thrown, remove listener
            iLP.setLocationListener(null, -1, -1, -1);
            assertContinue("Bad arguments should throw exception: " + aInterval
                           + "," + aTimeout + "," + aMaxAge, false);
        }
        catch (IllegalArgumentException iae)
        {
            // OK
            assertContinue("no message allowed for the exception: " + iae, iae
                           .getMessage() == null);
        }
        catch (Exception e)
        {
            assertContinue("Unknown error: " + e, false);
        }
    }

    private void testGoodWithBad(LocationListener aListener, int aInterval)
    {
        assertGood(aListener, aInterval, LARGE_TIMEOUT, LARGE_MAXAGE);

        for (int i = 0; i < badTimeout.length; ++i)
        {
            assertGood(aListener, aInterval, badTimeout[i], LARGE_MAXAGE);
        }

        for (int i = 0; i < badMaxAge.length; ++i)
        {
            assertGood(aListener, aInterval, LARGE_TIMEOUT, badMaxAge[i]);
        }

        for (int j = 0; j < badTimeout.length; ++j)
        {
            for (int k = 0; k < badMaxAge.length; ++k)
            {
                assertGood(aListener, aInterval, badTimeout[j], badMaxAge[k]);
            }
        }
    }

    private void assertGood(LocationListener aListener, int aInterval,
                            int aTimeout, int aMaxAge)
    {
        try
        {
            iLP.setLocationListener(aListener, aInterval, aTimeout, aMaxAge);
        }
        catch (IllegalArgumentException iae)
        {
            assertContinue("setLocationListener(" + aListener + "," + aInterval
                           + "," + aTimeout + "," + aMaxAge + ") threw exception:\n"
                           + iae, false);
        }

        // remove listener
        try
        {
            if (iLP != null)
                iLP.setLocationListener(null, -1, -1, -1);
        }
        catch (Exception e)
        {
        }
    }

    private void testInterval(int aInterval, int aNumberOfFixes)
    {
        assertContinue("Must test with at least one fix!", aNumberOfFixes > 0);

        final int TIMETOFIX = 100; // 100 ms
        final int TIMEOUT = 1000 + 2 * TIMETOFIX;
        final int MAXTIME = 1000 * (aNumberOfFixes * aInterval) + TIMEOUT;

        Criteria criteria = new Criteria();
        criteria.setPreferredResponseTime(TIMETOFIX);

        providerSetUp(criteria);
        assertContinue("LocationProvider is null", iLP != null);

        // Start listening for location changes
        iLP.setLocationListener(this, aInterval, -1, -1);

        getUpdates(aNumberOfFixes, MAXTIME);

        checkLocationData(iLocations[0]);
        // Fixes at interval starts after first fix
        long startTime = iLocUpdateCallbackTime[0];
        long lastTimeOfFix = startTime;

        for (int i = 1; i < aNumberOfFixes; ++i)
        {
            checkLocationData(iLocations[i]);
            long timeOfFix = iLocUpdateCallbackTime[i];
            long timeDiff = timeOfFix - lastTimeOfFix;
            assertContinue("Period difference is less than allowed for fix" + i
                           + ": " + timeDiff, timeDiff > (aInterval - TIMEOUT) * 1000);
            assertContinue("Period difference is more than allowed for fix" + i
                           + ": " + timeDiff, timeDiff < (aInterval + TIMEOUT) * 1000);
            assertContinue("Timestamp too late for fix" + i,
                           (timeOfFix - startTime) < ((i + 1) * aInterval * 1000));
            lastTimeOfFix = timeOfFix;
        }
        iLocations = null;
    }

    private void getUpdates(int aNumUpdates, int aTimeout)
    {
        iLocations = new Location[aNumUpdates];
        iLocUpdateCallbackTime = new long[aNumUpdates];
        iNumUpdates = 0;

        // Wait for the first result or timeout
        boolean timeout = false;
        long startTime = System.currentTimeMillis();
        while (iNumUpdates < aNumUpdates)
        {
            // Wait a moment before checking things
            try
            {
                Thread.sleep(250);
            }
            catch (InterruptedException ie)
            {
            }

            // Use a safeguard timeout in case the updating thread hangs
            // or takes too long
            if (System.currentTimeMillis() - startTime > aTimeout)
            {
                timeout = true;
                break;
            }
        }

        try
        {
            if (iLP != null)
                iLP.setLocationListener(null, -1, -1, -1);
        }
        catch (Exception e)
        {
        }

        assertContinue("Got a timeout", !timeout);
    }

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
