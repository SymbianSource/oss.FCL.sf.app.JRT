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

class PeriodicUpdateTest extends ViperUnitTest implements LocationListener
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

    PeriodicUpdateTest()
    {
        super("PeriodicUpdateTest");
    }

    protected void runTest() throws Throwable
    {
        testBadArguments();
        testGoodArguments();
        testLocationListenerDefault();
        testLocationListenerInterval_1s();
        testLocationListenerInterval_3s();
        testLocationListenerInterval_7s();
        testLocationListenerTimeout();
        testLocationListenerMaxAge();
        testChangeListener();
    }

    private void testBadArguments()
    {
        setCurrentTest("testBadArguments()");
        providerSetUp(null);
        assertTrue(iLP != null, "LocationProvider is null");

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

    }

    private void testGoodArguments()
    {
        setCurrentTest("testGoodArguments()");
        providerSetUp(null);
        assertTrue(iLP != null, "LocationProvider is null");

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
    }

    private void testLocationListenerDefault()
    {
        setCurrentTest("testLocationListenerDefault()");
        final int MAXTIME = 1000 * (TIMETOFIX + 13 + 1); // Interval ~ 6.25

        providerSetUp(null); // Simulation PSY should be default
        assertTrue(iLP != null, "LocationProvider is null");

        // Start listening for location changes
        iLP.setLocationListener(this, -1, -1, -1);

        getUpdates(2, MAXTIME);

        checkLocationData(iLocations[0]);
        checkLocationData(iLocations[1]);

        iLocations = null;
    }

    private void testLocationListenerInterval_1s()
    {
        setCurrentTest("testLocationListenerInterval_1s()");

        testInterval(1, 4);
    }

    private void testLocationListenerInterval_3s()
    {
        setCurrentTest("testLocationListenerInterval_3s()");

        testInterval(3, 5);
    }

    private void testLocationListenerInterval_7s()
    {
        setCurrentTest("testLocationListenerInterval_7s()");

        testInterval(7, 3);
    }

    private void testLocationListenerTimeout()
    {
    }

    private void testLocationListenerMaxAge()
    {
    }

    private void testChangeListener()
    {
        setCurrentTest("testChangeListener()");
        providerSetUp(null);
        assertTrue(iLP != null, "LocationProvider is null");

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

        tearDown();
    }

    private void tearDown()
    {
        // Remove this class as listener
        try
        {
            if (iLP != null)
            {
                iLP.setLocationListener(null, -1, -1, -1);
            }
        }
        catch (Exception e)
        {
            echo("PeriodicUpdateTest: Could not remove listener!");
            echo("PeriodicUpdateTest: Exception thrown: " + e);
        }
    }

    public void providerStateChanged(LocationProvider l, int event)
    {
    }

    public void locationUpdated(LocationProvider l, Location fix)
    {
        iLocUpdateCallbackTime[iNumUpdates] = System.currentTimeMillis();

        echo("PeriodicUpdateTest.locationUpdated() called");
        iLocations[iNumUpdates] = fix;
        ++iNumUpdates;
        assertTrue(l != null, "LocationProvider is null");
    }

    // ------------------------ Helper methods -----------------------

    private void assertBad(int aInterval, int aTimeout, int aMaxAge)
    {
        try
        {
            iLP.setLocationListener(this, aInterval, aTimeout, aMaxAge);
            // Oops, no exception thrown, remove listener
            iLP.setLocationListener(null, -1, -1, -1);
            assertTrue(false, "Bad arguments should throw exception: "
                       + aInterval + "," + aTimeout + "," + aMaxAge);
        }
        catch (IllegalArgumentException iae)
        {
            // OK
            assertNoMessage(iae);
        }
        catch (Exception e)
        {
            assertTrue(false, "Unknown error: " + e);
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
            assertTrue(false, "setLocationListener(" + aListener + ","
                       + aInterval + "," + aTimeout + "," + aMaxAge
                       + ") threw exception:\n" + iae);
        }

        // remove listener
        tearDown();
    }

    private void testInterval(int aInterval, int aNumberOfFixes)
    {
        assertTrue(aNumberOfFixes > 0, "Must test with at least one fix!");

        final int TIMETOFIX = 100; // 100 ms
        final int TIMEOUT = 1000 + 2 * TIMETOFIX;
        final int MAXTIME = 1000 * (aNumberOfFixes * aInterval) + TIMEOUT;

        Criteria criteria = new Criteria();
        criteria.setPreferredResponseTime(TIMETOFIX);

        providerSetUp(criteria);
        assertTrue(iLP != null, "LocationProvider is null");

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
            assertTrue(timeDiff > (aInterval - TIMEOUT) * 1000,
                       "Period difference is less than allowed for fix" + i + ": "
                       + timeDiff);
            assertTrue(timeDiff < (aInterval + TIMEOUT) * 1000,
                       "Period difference is more than allowed for fix" + i + ": "
                       + timeDiff);
            assertTrue((timeOfFix - startTime) < ((i + 1) * aInterval * 1000),
                       "Timestamp too late for fix" + i);
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
                echo("Timeout (updates = " + iNumUpdates + ")");
                timeout = true;
                break;
            }
        }

        tearDown();

        assertTrue(!timeout, "Got a timeout");
    }

}
